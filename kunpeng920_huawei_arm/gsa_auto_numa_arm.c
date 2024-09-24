#include "gsa_auto_numa_arm.h"

static int sleep_time = 1*TIME_SECOND;     /* Profile by sleep_time useconds chunks */
int SYS_NUMA_BALANCING_STATE = 0; // 全局变量：存储系统当前numa balancing状态
FILE *NUMA_BALANCING_CTL_FILE_POINTER = NULL; // 全局变量：指向/proc/sys/kernel/numa_balancing文件
FILE *NUMA_BALANING_FLAG_STATISTIC_FILE_POINTER = NULL; // 全局变量：numa_flag_statistic.csv文件
FILE *STATISTICAL_RESULT_FILE_POINTER = NULL; // 全局变量：statisticalResult.csv

// 移动平均算法实现变量
#define WINDOW_SIZE 6 // 窗口大小
typedef struct {
    float *window;
    int size;
    int index;
    float sum;
} MovingAverage;
MovingAverage *initMovingAverage(int size) {
    MovingAverage *ma = malloc(sizeof(MovingAverage));
    ma->window = malloc(size * sizeof(float));
    ma->size = size;
    ma->index = 0;
    ma->sum = 0.0;
    for (int i = 0; i < size; ++i) {
        ma->window[i] = 0.0; // 初始时，启动auto numa_balancing
    }
    return ma;
}
void updateMovingAverage(MovingAverage *ma, float newValue) {
    ma->sum -= ma->window[ma->index % ma->size]; // 移除一个数据
    ma->sum += newValue; // 增加一个数据
    ma->window[ma->index % ma->size] = newValue; // 增加一个数据
    ma->index++;
}
float getMovingAverage(MovingAverage *ma) {
    int count = ma->index < ma->size ? ma->index : ma->size;
    return ma->sum / count;
}
void freeMovingAverage(MovingAverage *ma) {
    free(ma->window);
    free(ma);
}

/*
 * 配置相关性能事件属性
 * Events :
 * - PERF_TYPE_RAW: raw counters. The value must be 0xz0040yyzz.
 *      For 'z-zz' values, see AMD reference manual (eg. 076h = CPU_CLK_UNHALTED).
 *      'yy' is the Unitmask.
 *      The '4' is 0100b = event is enabled (can also be enable/disabled via ioctl).
 *      The '0' before yy indicate which level to monitor (User or OS).
 *              It is modified by the event_attr when .exclude_[user/kernel] == 0.
 *              When it is the case the bits 16 or 17 of .config are set to 1 (== monitor user/kernel).
 *              If this is set to anything else than '0', it can be confusing since the kernel does not modify it when .exclude_xxx is set.
 *
 * - PERF_TYPE_HARDWARE: predefined values of HW counters in Linux (eg PERF_COUNT_HW_CPU_CYCLES = CPU_CLK_UNHALTED).
 *
 * - leader = -1 : the event is a group leader
 *   leader = x!=-1 : the event is only scheduled when its group leader is scheduled
 */
static event_t default_events[] = {
   { //ddrc
      .name    = "hisi_sccl1_ddrc0/flux_wcmd/", // #0
      .type    = 0x2d,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl1_ddrc0/flux_rcmd/", // #1
      .type    = 0x2d,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 0,
   },
   { 
      .name    = "hisi_sccl1_ddrc1/flux_wcmd/", // #2
      .type    = 0x2e,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl1_ddrc1/flux_rcmd/", // #3
      .type    = 0x2e,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 0,
   },
   { 
      .name    = "hisi_sccl1_ddrc2/flux_wcmd/", // #4
      .type    = 0x2f,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl1_ddrc2/flux_rcmd/", // #5
      .type    = 0x2f,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 0,
   },
   { 
      .name    = "hisi_sccl1_ddrc3/flux_wcmd/", // #6
      .type    = 0x30,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl1_ddrc3/flux_rcmd/", // #7
      .type    = 0x30,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl3_ddrc0/flux_wcmd/", // #8
      .type    = 0x29,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_ddrc0/flux_rcmd/", // #9
      .type    = 0x29,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_ddrc1/flux_wcmd/", // #10
      .type    = 0x2a,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_ddrc1/flux_rcmd/", // #11
      .type    = 0x2a,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_ddrc2/flux_wcmd/", // #12
      .type    = 0x2b,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_ddrc2/flux_rcmd/", // #13
      .type    = 0x2b,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_ddrc3/flux_wcmd/", // #14
      .type    = 0x2c,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_ddrc3/flux_rcmd/", // #15
      .type    = 0x2c,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl5_ddrc0/flux_wcmd/", // #16
      .type    = 0x35,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_ddrc0/flux_rcmd/", // #17
      .type    = 0x35,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_ddrc1/flux_wcmd/", // #18
      .type    = 0x36,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_ddrc1/flux_rcmd/", // #19
      .type    = 0x36,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_ddrc2/flux_wcmd/", // #20
      .type    = 0x37,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_ddrc2/flux_rcmd/", // #21
      .type    = 0x37,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_ddrc3/flux_wcmd/", // #22
      .type    = 0x38,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_ddrc3/flux_rcmd/", // #23
      .type    = 0x38,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl7_ddrc0/flux_wcmd/", // #24
      .type    = 0x31,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_ddrc0/flux_rcmd/", // #25
      .type    = 0x31,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_ddrc1/flux_wcmd/", // #26
      .type    = 0x32,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_ddrc1/flux_rcmd/", // #27
      .type    = 0x32,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_ddrc2/flux_wcmd/", // #28
      .type    = 0x33,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_ddrc2/flux_rcmd/", // #29
      .type    = 0x33,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_ddrc3/flux_wcmd/", // #30
      .type    = 0x34,
      .config  = 0x2,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_ddrc3/flux_rcmd/", // #31
      .type    = 0x34,
      .config  = 0x3,
      .leader  = -1,
      .cpuid   = 72,
   },
   {// hha
      .name    = "hisi_sccl1_hha2/rx_ops_num/", // #32   
      .type    = 0x23,
      .config  = 0x0,
      /* 
      * 领导者事件代表着事件组的领导者，其他事件加入到这个领导者事件的组中。
      * 事件组允许将相关的事件一起统计，**领导者事件的计数将包括组中所有事件的计数值**。
      *
      * 通过创建事件组，可以方便地对相关事件进行联合监控，并在进行性能分析时同时考虑这些相关事件的数据。
      * 这有助于更全面地了解系统或应用程序的性能特征，找出可能的性能瓶颈并进行优化。
      */
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl1_hha2/rx_outer/", // #33
      .type    = 0x23,
      .config  = 0x1,
      //.leader  = 32,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl1_hha2/rx_sccl/", // #34
      .type    = 0x23,
      .config  = 0x2,
      //.leader  = 32,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl1_hha3/rx_ops_num/", // #35
      .type    = 0x24,
      .config  = 0x0,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl1_hha3/rx_outer/", // #36
      .type    = 0x24,
      .config  = 0x1,
      // .leader  = 35,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl1_hha3/rx_sccl/", // #37
      .type    = 0x24,
      .config  = 0x2,
      // .leader  = 35,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl3_hha0/rx_ops_num/", // #38
      .type    = 0x21,
      .config  = 0x0,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_hha0/rx_outer/", // #39
      .type    = 0x21,
      .config  = 0x1,
      // .leader  = 38,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_hha0/rx_sccl/", // #40
      .type    = 0x21,
      .config  = 0x2,
      // .leader  = 38,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_hha1/rx_ops_num/", // #41
      .type    = 0x22,
      .config  = 0x0,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_hha1/rx_outer/", // #42
      .type    = 0x22,
      .config  = 0x1,
      // .leader  = 41,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_hha1/rx_sccl/", // #43
      .type    = 0x22,
      .config  = 0x2,
      // .leader  = 41,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl5_hha6/rx_ops_num/", // #44
      .type    = 0x27,
      .config  = 0x0,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_hha6/rx_outer/", // #45
      .type    = 0x27,
      .config  = 0x1,
      // .leader  = 44,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_hha6/rx_sccl/", // #46
      .type    = 0x27,
      .config  = 0x2,
      // .leader  = 44,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_hha7/rx_ops_num/", // #47
      .type    = 0x28,
      .config  = 0x0,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_hha7/rx_outer/", // #48
      .type    = 0x28,
      .config  = 0x1,
      // .leader  = 47,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_hha7/rx_sccl/", // #49
      .type    = 0x28,
      .config  = 0x2,
      // .leader  = 47,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl7_hha4/rx_ops_num/", // #50
      .type    = 0x25,
      .config  = 0x0,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_hha4/rx_outer/", // #51
      .type    = 0x25,
      .config  = 0x1,
      // .leader  = 50,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_hha4/rx_sccl/", // #52
      .type    = 0x25,
      .config  = 0x2,
      // .leader  = 50,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_hha5/rx_ops_num/", // #53
      .type    = 0x26,
      .config  = 0x0,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_hha5/rx_outer/", // #54
      .type    = 0x26,
      .config  = 0x1,
      // .leader  = 53,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_hha5/rx_sccl/", // #55
      .type    = 0x26,
      .config  = 0x2,
      // .leader  = 53,
      .leader  = -1,
      .cpuid   = 72,
   },
   // L3 cached read
   {
      .name    = "hisi_sccl1_l3c8/rd_cpipe", // #56
      .type    = 0x0F,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl1_l3c8/wr_cpipe", // #57
      .type    = 0x0F,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl1_l3c8/rd_hit_cpipe", // #58
      .type    = 0x0F,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl1_l3c8/wr_hit_cpipe", // #59
      .type    = 0x0F,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl1_l3c8/victim_num", // #60
      .type    = 0x0F,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "hisi_sccl1_l3c9/rd_cpipe", // #61
      .type    = 0x10,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 4,
   },
   {
      .name    = "hisi_sccl1_l3c9/wr_cpipe", // #62
      .type    = 0x10,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 4,
   },
   {
      .name    = "hisi_sccl1_l3c9/rd_hit_cpipe", // #63
      .type    = 0x10,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 4,
   },
   {
      .name    = "hisi_sccl1_l3c9/wr_hit_cpipe", // #64
      .type    = 0x10,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 4,
   },
   {
      .name    = "hisi_sccl1_l3c9/victim_num", // #65
      .type    = 0x10,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 4,
   },
   {
      .name    = "hisi_sccl1_l3c10/rd_cpipe", // #66
      .type    = 0x11,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 8,
   },
   {
      .name    = "hisi_sccl1_l3c10/wr_cpipe", // #67
      .type    = 0x11,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 8,
   },
   {
      .name    = "hisi_sccl1_l3c10/rd_hit_cpipe", // #68
      .type    = 0x11,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 8,
   },
   {
      .name    = "hisi_sccl1_l3c10/wr_hit_cpipe", // #69
      .type    = 0x11,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 8,
   },
   {
      .name    = "hisi_sccl1_l3c10/victim_num", // #70
      .type    = 0x11,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 8,
   },
   {
      .name    = "hisi_sccl1_l3c11/rd_cpipe", // #71
      .type    = 0x12,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 12,
   },
   {
      .name    = "hisi_sccl1_l3c11/wr_cpipe", // #72
      .type    = 0x12,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 12,
   },
   {
      .name    = "hisi_sccl1_l3c11/rd_hit_cpipe", // #73
      .type    = 0x12,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 12,
   },
   {
      .name    = "hisi_sccl1_l3c11/wr_hit_cpipe", // #74
      .type    = 0x12,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 12,
   },
   {
      .name    = "hisi_sccl1_l3c11/victim_num", // #75
      .type    = 0x12,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 12,
   },
   {
      .name    = "hisi_sccl1_l3c12/rd_cpipe", // #76
      .type    = 0x13,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 16,
   },
   {
      .name    = "hisi_sccl1_l3c12/wr_cpipe", // #77
      .type    = 0x13,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 16,
   },
   {
      .name    = "hisi_sccl1_l3c12/rd_hit_cpipe", // #78
      .type    = 0x13,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 16,
   },
   {
      .name    = "hisi_sccl1_l3c12/wr_hit_cpipe", // #79
      .type    = 0x13,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 16,
   },
   {
      .name    = "hisi_sccl1_l3c12/victim_num", // #80
      .type    = 0x13,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 16,
   },
   {
      .name    = "hisi_sccl1_l3c13/rd_cpipe", // #81
      .type    = 0x14,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 20,
   },
   {
      .name    = "hisi_sccl1_l3c13/wr_cpipe", // #82
      .type    = 0x14,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 20,
   },
   {
      .name    = "hisi_sccl1_l3c13/rd_hit_cpipe", // #83
      .type    = 0x14,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 20,
   },
   {
      .name    = "hisi_sccl1_l3c13/wr_hit_cpipe", // #84
      .type    = 0x14,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 20,
   },
   {
      .name    = "hisi_sccl1_l3c13/victim_num", // #85
      .type    = 0x14,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 20,
   },
   {
      .name    = "hisi_sccl3_l3c0/rd_cpipe", // #86
      .type    = 0x09,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_l3c0/wr_cpipe", // #87
      .type    = 0x09,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_l3c0/rd_hit_cpipe", // #88
      .type    = 0x09,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_l3c0/wr_hit_cpipe", // #89
      .type    = 0x09,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_l3c0/victim_num", // #90
      .type    = 0x09,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 24,
   },
   {
      .name    = "hisi_sccl3_l3c1/rd_cpipe", // #91
      .type    = 0x0A,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 28,
   },
   {
      .name    = "hisi_sccl3_l3c1/wr_cpipe", // #92
      .type    = 0x0A,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 28,
   },
   {
      .name    = "hisi_sccl3_l3c1/rd_hit_cpipe", // #93
      .type    = 0x0A,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 28,
   },
   {
      .name    = "hisi_sccl3_l3c1/wr_hit_cpipe", // #94
      .type    = 0x0A,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 28,
   },
   {
      .name    = "hisi_sccl3_l3c1/victim_num", // #95
      .type    = 0x0A,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 28,
   },
   {
      .name    = "hisi_sccl3_l3c2/rd_cpipe", // #96
      .type    = 0x0B,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 32,
   },
   {
      .name    = "hisi_sccl3_l3c2/wr_cpipe", // #97
      .type    = 0x0B,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 32,
   },
   {
      .name    = "hisi_sccl3_l3c2/rd_hit_cpipe", // #98
      .type    = 0x0B,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 32,
   },
   {
      .name    = "hisi_sccl3_l3c2/wr_hit_cpipe", // #99
      .type    = 0x0B,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 32,
   },
   {
      .name    = "hisi_sccl3_l3c2/victim_num", // #100
      .type    = 0x0B,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 32,
   },
   {
      .name    = "hisi_sccl3_l3c3/rd_cpipe", // #101
      .type    = 0x0C,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 36,
   },
   {
      .name    = "hisi_sccl3_l3c3/wr_cpipe", // #102
      .type    = 0x0C,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 36,
   },
   {
      .name    = "hisi_sccl3_l3c3/rd_hit_cpipe", // #103
      .type    = 0x0C,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 36,
   },
   {
      .name    = "hisi_sccl3_l3c3/wr_hit_cpipe", // #104
      .type    = 0x0C,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 36,
   },
   {
      .name    = "hisi_sccl3_l3c3/victim_num", // #105
      .type    = 0x0C,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 36,
   },
   {
      .name    = "hisi_sccl3_l3c4/rd_cpipe", // #106
      .type    = 0x0D,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 40,
   },
   {
      .name    = "hisi_sccl3_l3c4/wr_cpipe", // #107
      .type    = 0x0D,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 40,
   },
   {
      .name    = "hisi_sccl3_l3c4/rd_hit_cpipe", // #108
      .type    = 0x0D,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 40,
   },
   {
      .name    = "hisi_sccl3_l3c4/wr_hit_cpipe", // #109
      .type    = 0x0D,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 40,
   },
   {
      .name    = "hisi_sccl3_l3c4/victim_num", // #110
      .type    = 0x0D,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 40,
   },
   {
      .name    = "hisi_sccl3_l3c5/rd_cpipe", // #111
      .type    = 0x0E,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 44,
   },
   {
      .name    = "hisi_sccl3_l3c5/wr_cpipe", // #112
      .type    = 0x0E,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 44,
   },
   {
      .name    = "hisi_sccl3_l3c5/rd_hit_cpipe", // #113
      .type    = 0x0E,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 44,
   },
   {
      .name    = "hisi_sccl3_l3c5/wr_hit_cpipe", // #114
      .type    = 0x0E,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 44,
   },
   {
      .name    = "hisi_sccl3_l3c5/victim_num", // #115
      .type    = 0x0E,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 44,
   },
   {
      .name    = "hisi_sccl5_l3c24/rd_cpipe", // #116
      .type    = 0x1B,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_l3c24/wr_cpipe", // #117
      .type    = 0x1B,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_l3c24/rd_hit_cpipe", // #118
      .type    = 0x1B,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_l3c24/wr_hit_cpipe", // #119
      .type    = 0x1B,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_l3c24/victim_num", // #120
      .type    = 0x1B,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 48,
   },
   {
      .name    = "hisi_sccl5_l3c25/rd_cpipe", // #121
      .type    = 0x1C,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 52,
   },
   {
      .name    = "hisi_sccl5_l3c25/wr_cpipe", // #122
      .type    = 0x1C,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 52,
   },
   {
      .name    = "hisi_sccl5_l3c25/rd_hit_cpipe", // #123
      .type    = 0x1C,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 52,
   },
   {
      .name    = "hisi_sccl5_l3c25/wr_hit_cpipe", // #124
      .type    = 0x1C,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 52,
   },
   {
      .name    = "hisi_sccl5_l3c25/victim_num", // #125
      .type    = 0x1C,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 52,
   },
   {
      .name    = "hisi_sccl5_l3c26/rd_cpipe", // #126
      .type    = 0x1D,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 56,
   },
   {
      .name    = "hisi_sccl5_l3c26/wr_cpipe", // #127
      .type    = 0x1D,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 56,
   },
   {
      .name    = "hisi_sccl5_l3c26/rd_hit_cpipe", // #128
      .type    = 0x1D,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 56,
   },
   {
      .name    = "hisi_sccl5_l3c26/wr_hit_cpipe", // #129
      .type    = 0x1D,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 56,
   },
   {
      .name    = "hisi_sccl5_l3c26/victim_num", // #130
      .type    = 0x1D,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 56,
   },
   {
      .name    = "hisi_sccl5_l3c27/rd_cpipe", // #131
      .type    = 0x1E,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 60,
   },
   {
      .name    = "hisi_sccl5_l3c27/wr_cpipe", // #132
      .type    = 0x1E,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 60,
   },
   {
      .name    = "hisi_sccl5_l3c27/rd_hit_cpipe", // #133
      .type    = 0x1E,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 60,
   },
   {
      .name    = "hisi_sccl5_l3c27/wr_hit_cpipe", // #134
      .type    = 0x1E,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 60,
   },
   {
      .name    = "hisi_sccl5_l3c27/victim_num", // #135
      .type    = 0x1E,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 60,
   },
   {
      .name    = "hisi_sccl5_l3c28/rd_cpipe", // #136
      .type    = 0x1F,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 64,
   },
   {
      .name    = "hisi_sccl5_l3c28/wr_cpipe", // #137
      .type    = 0x1F,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 64,
   },
   {
      .name    = "hisi_sccl5_l3c28/rd_hit_cpipe", // #138
      .type    = 0x1F,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 64,
   },
   {
      .name    = "hisi_sccl5_l3c28/wr_hit_cpipe", // #139
      .type    = 0x1F,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 64,
   },
   {
      .name    = "hisi_sccl5_l3c28/victim_num", // #140
      .type    = 0x1F,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 64,
   },
   {
      .name    = "hisi_sccl5_l3c29/rd_cpipe", // #141
      .type    = 0x20,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 68,
   },
   {
      .name    = "hisi_sccl5_l3c29/wr_cpipe", // #142
      .type    = 0x20,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 68,
   },
   {
      .name    = "hisi_sccl5_l3c29/rd_hit_cpipe", // #143
      .type    = 0x20,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 68,
   },
   {
      .name    = "hisi_sccl5_l3c29/wr_hit_cpipe", // #144
      .type    = 0x20,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 68,
   },
   {
      .name    = "hisi_sccl5_l3c29/victim_num", // #145
      .type    = 0x20,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 68,
   },
   {
      .name    = "hisi_sccl7_l3c16/rd_cpipe", // #146
      .type    = 0x15,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_l3c16/wr_cpipe", // #147
      .type    = 0x15,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_l3c16/rd_hit_cpipe", // #148
      .type    = 0x15,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_l3c16/wr_hit_cpipe", // #149
      .type    = 0x15,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_l3c16/victim_num", // #150
      .type    = 0x15,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 72,
   },
   {
      .name    = "hisi_sccl7_l3c17/rd_cpipe", // #151
      .type    = 0x16,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 76,
   },
   {
      .name    = "hisi_sccl7_l3c17/wr_cpipe", // #152
      .type    = 0x16,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 76,
   },
   {
      .name    = "hisi_sccl7_l3c17/rd_hit_cpipe", // #153
      .type    = 0x16,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 76,
   },
   {
      .name    = "hisi_sccl7_l3c17/wr_hit_cpipe", // #154
      .type    = 0x16,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 76,
   },
   {
      .name    = "hisi_sccl7_l3c17/victim_num", // #155
      .type    = 0x16,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 76,
   },
   {
      .name    = "hisi_sccl7_l3c18/rd_cpipe", // #156
      .type    = 0x17,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 80,
   },
   {
      .name    = "hisi_sccl7_l3c18/wr_cpipe", // #157
      .type    = 0x17,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 80,
   },
   {
      .name    = "hisi_sccl7_l3c18/rd_hit_cpipe", // #158
      .type    = 0x17,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 80,
   },
   {
      .name    = "hisi_sccl7_l3c18/wr_hit_cpipe", // #159
      .type    = 0x17,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 80,
   },
   {
      .name    = "hisi_sccl7_l3c18/victim_num", // #160
      .type    = 0x17,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 80,
   },
   {
      .name    = "hisi_sccl7_l3c19/rd_cpipe", // #161
      .type    = 0x18,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 84,
   },
   {
      .name    = "hisi_sccl7_l3c19/wr_cpipe", // #162
      .type    = 0x18,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 84,
   },
   {
      .name    = "hisi_sccl7_l3c19/rd_hit_cpipe", // #163
      .type    = 0x18,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 84,
   },
   {
      .name    = "hisi_sccl7_l3c19/wr_hit_cpipe", // #164
      .type    = 0x18,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 84,
   },
   {
      .name    = "hisi_sccl7_l3c19/victim_num", // #165
      .type    = 0x18,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 84,
   },
   {
      .name    = "hisi_sccl7_l3c20/rd_cpipe", // #166
      .type    = 0x19,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 88,
   },
   {
      .name    = "hisi_sccl7_l3c20/wr_cpipe", // #167
      .type    = 0x19,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 88,
   },
   {
      .name    = "hisi_sccl7_l3c20/rd_hit_cpipe", // #168
      .type    = 0x19,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 88,
   },
   {
      .name    = "hisi_sccl7_l3c20/wr_hit_cpipe", // #169
      .type    = 0x19,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 88,
   },
   {
      .name    = "hisi_sccl7_l3c20/victim_num", // #170
      .type    = 0x19,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 88,
   },
   {
      .name    = "hisi_sccl7_l3c21/rd_cpipe", // #171
      .type    = 0x1A,
      .config  = 0x00,
      .leader  = -1,
      .cpuid   = 92,
   },
   {
      .name    = "hisi_sccl7_l3c21/wr_cpipe", // #172
      .type    = 0x1A,
      .config  = 0x01,
      .leader  = -1,
      .cpuid   = 92,
   },
   {
      .name    = "hisi_sccl7_l3c21/rd_hit_cpipe", // #173
      .type    = 0x1A,
      .config  = 0x02,
      .leader  = -1,
      .cpuid   = 92,
   },
   {
      .name    = "hisi_sccl7_l3c21/wr_hit_cpipe", // #174
      .type    = 0x1A,
      .config  = 0x03,
      .leader  = -1,
      .cpuid   = 92,
   },
   {
      .name    = "hisi_sccl7_l3c21/victim_num", // #175
      .type    = 0x1A,
      .config  = 0x04,
      .leader  = -1,
      .cpuid   = 92,
   },
};
static int nb_events = sizeof(default_events)/sizeof(*default_events); // 计算事件的个数
static event_t *events = default_events; // 新的指针指向数组，减少函数调用的时候，数组被复制

// 一些中断信号处理
static void sig_handler(int signal) {
   printf("#signal caught: %d\n", signal);
   fflush(NUMA_BALANCING_CTL_FILE_POINTER);
   fflush(NUMA_BALANING_FLAG_STATISTIC_FILE_POINTER);
   fflush(STATISTICAL_RESULT_FILE_POINTER);
   fflush(NULL);
   exit(0);
}

// 读取性能参数的调用函数包装
static long sys_perf_counter_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags) {
   int ret = syscall(__NR_perf_counter_open, hw_event, pid, cpu, group_fd, flags);
#  if defined(__x86_64__) || defined(__i386__)
   if (ret < 0 && ret > -4096) {
      errno = -ret;
      ret = -1;
   }
#  endif
   return ret;
}

// 获得NUAM节点 i L3C_victim的值
static uint64_t get_L3C_node_victim(struct perf_read_ev *last_counts, struct perf_read_ev *last_counts_prev, int node){
   uint64_t L3C_node_victim = 0;
   for(int i = 0; i < 6; i++){
      L3C_node_victim += last_counts[60 + node * 30 + i * 5].value - last_counts_prev[60 + node * 30 + i * 5].value;
   }

   return L3C_node_victim;
}

// 获得NUMA节点 i L3C_hit_percent的值
static float get_L3C_node_hit_percent(struct perf_read_ev *last_counts, struct perf_read_ev *last_counts_prev, int node){
   float L3C_node_hit_percent = 0;
   uint64_t L3C_node_rd_cpipe = 0;
   uint64_t L3C_node_wr_cpipe = 0;
   uint64_t L3C_node_rd_hit_cpipe = 0;
   uint64_t L3C_node_wr_hit_cpipe = 0;
   for(int i = 0; i < 6; i++){
      L3C_node_rd_cpipe += last_counts[56 + node * 30 + i * 5].value - last_counts_prev[56 + node * 30 + i * 5].value;
      L3C_node_wr_cpipe += last_counts[57 + node * 30 + i * 5].value - last_counts_prev[57 + node * 30 + i * 5].value;
      L3C_node_rd_hit_cpipe += last_counts[58 + node * 30 + i * 5].value - last_counts_prev[58 + node * 30 + i * 5].value;
      L3C_node_wr_hit_cpipe += last_counts[59 + node * 30 + i * 5].value - last_counts_prev[59 + node * 30 + i * 5].value;
   }

   return L3C_node_hit_percent = ((L3C_node_rd_hit_cpipe + L3C_node_wr_hit_cpipe) / (float)(L3C_node_rd_cpipe + L3C_node_wr_cpipe)) * 100;
}

// 获得NUAM节点 i HHA_ops_num的值
static uint64_t get_HHA_node_rx_ops_num(struct perf_read_ev *last_counts, struct perf_read_ev *last_counts_prev, int node){
   uint64_t HHA_node_rx_ops_num = 0;
   for(int i = 0; i < 2; i++){
      HHA_node_rx_ops_num += last_counts[32 + node * 6 + i * 3].value - last_counts_prev[32 + node * 6 + i * 3].value;
   }

   return HHA_node_rx_ops_num;
}

// 获得NUMA节点 i HHA_node_rx_out_percent的值
static float get_HHA_node_rx_out_percent(struct perf_read_ev *last_counts, struct perf_read_ev *last_counts_prev, int node){
   float HHA_node_rx_out_percent = 0;
   uint64_t HHA_node_rx_ops_num = 0;
   uint64_t HHA_node_rx_outer = 0;
   uint64_t HHA_node_rx_sccl = 0;
   for(int i = 0; i < 2; i++){
      HHA_node_rx_ops_num += last_counts[32 + node * 6 + i * 3].value - last_counts_prev[32 + node * 6 + i * 3].value;
      HHA_node_rx_outer += last_counts[33 + node * 6 + i * 3].value - last_counts_prev[33 + node * 6 + i * 3].value;
      HHA_node_rx_sccl += last_counts[34 + node * 6 + i * 3].value - last_counts_prev[34 + node * 6 + i * 3].value;
   }

   return HHA_node_rx_out_percent = ((HHA_node_rx_outer + HHA_node_rx_sccl) / (float)(HHA_node_rx_ops_num)) * 100;
}

// 获得NUAM节点 i DDRC read and write comend的值
static uint64_t get_DDRC_node_w_r_cmd(struct perf_read_ev *last_counts, struct perf_read_ev *last_counts_prev, int node){
   uint64_t DDRC_node_w_r_cmd = 0;
   for(int i = 0; i < 8; i++){
      DDRC_node_w_r_cmd += last_counts[0 + node * 8 + i].value - last_counts_prev[0 + node * 8 + i].value;
   }

   return DDRC_node_w_r_cmd;
}

// 获得NUAM节点 i DDRC read percent的值
static float get_DDRC_node_r_percent(struct perf_read_ev *last_counts, struct perf_read_ev *last_counts_prev, int node){
   float DDRC_node_r_percent = 0;
   uint64_t DDRC_node_w_cmd = 0;
   uint64_t DDRC_node_r_cmd = 0;
   uint64_t DDRC_node_w_r_cmd = 0;

   for(int i = 0; i < 4; i++){
      DDRC_node_w_cmd += last_counts[0 + node * 8 + i * 2].value - last_counts_prev[0 + node * 8 + i * 2].value;
      DDRC_node_r_cmd += last_counts[1 + node * 8 + i * 2].value - last_counts_prev[1 + node * 8 + i * 2].value;
   }

   return DDRC_node_r_percent = (DDRC_node_r_cmd / (float)(DDRC_node_w_cmd + DDRC_node_r_cmd)) * 100;
}

// 判断是否应该关闭auto NUMA
static int determineWhetherToTurnOffAutoNUMA(struct perf_read_ev *last_counts, struct perf_read_ev *last_counts_prev, \
      MovingAverage *ma_node_L3C_hit_percent[], MovingAverage *ma_node_HHA_rx_out_percent[], MovingAverage *ma_node_DDRC_read_percent[]){
   // 计算NUMA节点的个数
   // int max_node = numa_max_node() + 1;
   int max_node = 4; // numa节点个数
   int numa_balancing_flag = 0; // numa balance标志位，默认不开启Auto numa

   // 默认阈值大小
   uint64_t L3C_node_victim_threshold = 2000000; // L3C_victim阈值（counts per second）
   float L3C_node_hit_percent_threshold = 10; // L3C_hit_percent阈值（%）(10%和20%纠结中)
   uint64_t HHA_node_ops_num_threshold = 2000000; // HHA_ops_num阈值
   float HHA_rx_out_percent_threshold = 50; // HHA_out_percent阈值(%)
   uint64_t DDRC_node_flux_w_r_cmd_threshold = 2000000; // DDRC read and write comend阈值
   float DDRC_node_r_percent_threshold = 50; // DDRC read percent阈值

   // 对本次的比例值进行移动平均移动算法更新
   for(int i=0; i<max_node; i++){
      updateMovingAverage(ma_node_L3C_hit_percent[i], get_L3C_node_hit_percent(last_counts, last_counts_prev, i));
      updateMovingAverage(ma_node_HHA_rx_out_percent[i], get_HHA_node_rx_out_percent(last_counts, last_counts_prev, i));
      updateMovingAverage(ma_node_DDRC_read_percent[i], get_DDRC_node_r_percent(last_counts, last_counts_prev, i));
   }

   // 判断是否应该开启auto NUMA 的逻辑
   for (int i = 0; i < max_node; i++){
      // 判断当前节点DDRC_read_and_write是否达到阈值
      // printf("Node %d:\nDDRC_node_w_r_cmd: %ld, ", i, get_DDRC_node_w_r_cmd(last_counts, last_counts_prev, i)); // 测试用
      if(get_DDRC_node_w_r_cmd(last_counts, last_counts_prev, i) > DDRC_node_flux_w_r_cmd_threshold){
         // 判断当前DDRC的读比例是否达到阈值
         // printf("DDRC_node_r_percent: %f, ", get_DDRC_node_r_percent(last_counts, last_counts_prev, j)); // 测试用
         // printf("DDRC_node_r_percent: %f, ", getMovingAverage(ma_node_DDRC_read_percent[i])); // 测试用
         if(getMovingAverage(ma_node_DDRC_read_percent[i]) > DDRC_node_r_percent_threshold){
            // 判断节点的HHA_ops_num是否达到阈值
            // printf("HHA_node_rx_ops_num: %ld, ", get_HHA_node_rx_ops_num(last_counts, last_counts_prev, i)); // 测试用
            if(get_HHA_node_rx_ops_num(last_counts, last_counts_prev, i) > HHA_node_ops_num_threshold){
               // 判断当前节点HHA_out_percent是否达到阈值
               // printf("HHA_node_rx_out_percent: %f, ", get_HHA_node_rx_out_percent(last_counts, last_counts_prev, i)); // 测试用
               // printf("HHA_node_rx_out_percent: %f, ", getMovingAverage(ma_node_HHA_rx_out_percent[i])); // 测试用
               if(getMovingAverage(ma_node_HHA_rx_out_percent[i]) > HHA_rx_out_percent_threshold){
                  for (int j = 0; j < max_node; j++){
                     if(i == j){
                        continue;
                     }
                     // 判断L3C_victim是否达到阈值
                     // printf("L3C_node_victim: %ld, ", get_L3C_node_victim(last_counts, last_counts_prev, j)); // 测试用
                     if(get_L3C_node_victim(last_counts, last_counts_prev, j) > L3C_node_victim_threshold){
                        // 判断L3C_hit_percent是否达到阈值
                        // printf("%f, ", get_L3C_node_hit_percent(last_counts, last_counts_prev, j)); // 测试用
                        // printf("L3C_hit_percent: %f, ", getMovingAverage(ma_node_L3C_hit_percent[j])); // 测试用
                        if(getMovingAverage(ma_node_L3C_hit_percent[j]) < L3C_node_hit_percent_threshold){
                           numa_balancing_flag = 1;
                           break;
                        }
                     }
                  }
               }
            }
         }
      }
      if(numa_balancing_flag == 1){
         break;
      }
      // printf("\n-------------------------------\n"); // 测试用
   }

   return numa_balancing_flag;
}

// 持续进行性能监控
static void thread_loop() {
   int *fd = calloc(nb_events * sizeof(*fd), 1); // 执行性能事件的文件描述符
   struct perf_event_attr *events_attr = calloc(nb_events * sizeof(*events_attr), 1); // 性能事件的属性变量
   assert(events_attr != NULL);
   assert(fd);

   // 配置性能事件的属性
   for (int j = 0; j < nb_events; j++) {
      // fprintf(stdout, "No %d: Registering event %s\n", j, events[j].name); // 调试用，输出性能事件的名字
      fprintf(STATISTICAL_RESULT_FILE_POINTER, "%s,", events[j].name); // 设置统计信息表格题目
      
      // 为需要监控的性能事件设置属性，并获得性能事件的文件描述符
      events_attr[j].size = sizeof(struct perf_event_attr);
      events_attr[j].type = events[j].type;
      events_attr[j].config = events[j].config;
      events_attr[j].exclude_kernel = events[j].exclude_kernel;
      events_attr[j].exclude_user = events[j].exclude_user;
      //PERF_FORMAT_TOTAL_TIME_ENABLED：表示在读取性能计数时，将返回事件被启用的总时间。PERF_FORMAT_TOTAL_TIME_RUNNING：表示在读取性能计数时，将返回事件正在运行的总时间。
      events_attr[j].read_format = PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING;
      fd[j] = sys_perf_counter_open(&events_attr[j], -1, events[j].cpuid, (events[j].leader==-1)?-1:fd[events[j].leader], 0);
      if (fd[j] < 0) {
         fprintf(stdout, "sys_perf_counter_open failed: %s\n", strerror(errno));
         return;
      }
   }
   fprintf(STATISTICAL_RESULT_FILE_POINTER, "\n"); // 统计信息表格题目设置好后，换行
	
   // 定义临时存储结果的变量
   struct perf_read_ev single_count;
   struct perf_read_ev *last_counts = calloc(nb_events, sizeof(*last_counts));
   struct perf_read_ev *last_counts_prev = calloc(nb_events, sizeof(*last_counts_prev));

   // 定义移动平均算法需要的变量
   MovingAverage *ma_numa_balancing_flag = initMovingAverage(WINDOW_SIZE); // numa balancing flag移动平均变量
   MovingAverage *ma_node_L3C_hit_percent[4]; // L3C hit percent移动平均变量
   MovingAverage *ma_node_HHA_rx_out_percent[4]; // HHA rx out percent移动平均变量
   MovingAverage *ma_node_DDRC_read_percent[4]; // DDRC read percent移动平均变量
   for(int j=0; j<4; j++){
      ma_node_L3C_hit_percent[j] = initMovingAverage(WINDOW_SIZE); // L3C hit percent移动平均初始化
      ma_node_HHA_rx_out_percent[j] = initMovingAverage(WINDOW_SIZE); // HHA rx out percent移动平均初始化
      ma_node_DDRC_read_percent[j] = initMovingAverage(WINDOW_SIZE); // DDRC read percent移动平均初始化
   }

   // int i=0;
   // 持续进行性能监控，输出每秒的递增量，即速率
   while (1) {
      usleep(sleep_time); // 睡眠，单位是微秒（μs）
      for (int j = 0; j < nb_events; j++) {
         assert(read(fd[j], &single_count, sizeof(single_count)) == sizeof(single_count)); // 读取counters结果
         /*            printf("[%d,%d] %ld enabled %ld running %ld%%\n", i, j,
         single_count.time_enabled - last_counts[i*nb_events + j].time_enabled,
         single_count.time_running - last_counts[i*nb_events + j].time_running,
         (single_count.time_enabled-last_counts[i*nb_events + j].time_enabled)?100*(single_count.time_running-last_counts[i*nb_events + j].time_running)/(single_count.time_enabled-last_counts[i*nb_events + j].time_enabled):0); */
         last_counts[j] = single_count;

         fprintf(STATISTICAL_RESULT_FILE_POINTER, "%ld,", last_counts[j].value-last_counts_prev[j].value); // 输出所有的统计结果（本次和上一次之间的差值）
      }
      fprintf(STATISTICAL_RESULT_FILE_POINTER, "\n"); // 统计信息表格本次事件读取完毕后，换行

      // 判断是否应该开启auto NUMA
      updateMovingAverage(ma_numa_balancing_flag, \
         determineWhetherToTurnOffAutoNUMA(last_counts, last_counts_prev, \
            ma_node_L3C_hit_percent, \
               ma_node_HHA_rx_out_percent, \
                  ma_node_DDRC_read_percent)); // 根据L3C，HHA，DDRC确定是否开启auto numa，然后对判断结果进行平滑
      int ma_numa_balancing_avg = (getMovingAverage(ma_numa_balancing_flag) > 0.6); // 获得移动平衡结果
      fprintf(NUMA_BALANING_FLAG_STATISTIC_FILE_POINTER, "%d\n", ma_numa_balancing_avg); // 输出移动平衡结果

      // 设置/proc/sys/kernel/numa_balancing的值
      if(SYS_NUMA_BALANCING_STATE != ma_numa_balancing_avg){ // 如果当前状态和计算结果不符，更新numa_balancing状态
         NUMA_BALANCING_CTL_FILE_POINTER = fopen("/proc/sys/kernel/numa_balancing", "w");
         if (NUMA_BALANCING_CTL_FILE_POINTER == NULL) {
            perror("Error opening file: /proc/sys/kernel/numa_balancing");
            return;
         }
         if(fprintf(NUMA_BALANCING_CTL_FILE_POINTER, "%d", ma_numa_balancing_avg) >= 0){
            SYS_NUMA_BALANCING_STATE = ma_numa_balancing_avg;
            fclose(NUMA_BALANCING_CTL_FILE_POINTER); //关闭打开的文件
         }
         else{
            fprintf(stderr, "Error writing integer to /proc/sys/kernel/numa_balancing.\n");
            fclose(NUMA_BALANCING_CTL_FILE_POINTER); //关闭打开的文件
         }
      }

      // 记录上一次统计结果
      for(int j=0; j < nb_events; j++){
         last_counts_prev[j] = last_counts[j];
      }

      //i++;
   }

   // 关闭打开的文件
   for (int j = 0; j < nb_events; j++) {
      close(fd[j]);
   }

   freeMovingAverage(ma_numa_balancing_flag); // 释放移动平滑相关数据结构
   for(int j=0; j<4; j++){
      freeMovingAverage(ma_node_L3C_hit_percent[j]); // 释放移动平滑相关数据结构
      freeMovingAverage(ma_node_HHA_rx_out_percent[j]); // 释放移动平滑相关数据结构
      freeMovingAverage(ma_node_DDRC_read_percent[j]); // 释放移动平滑相关数据结构
   }
   free(ma_node_L3C_hit_percent);
   free(ma_node_HHA_rx_out_percent);
   free(ma_node_DDRC_read_percent);

   return;
}

int main(int argc, char**argv){
   signal(SIGPIPE, sig_handler);
   signal(SIGTERM, sig_handler);
   signal(SIGINT, sig_handler);

   // 检查程序的执行命令是否正确
   if (argc != 2) {
      printf("Usage: %s [output_filename]\n", argv[0]); // <>表示必选，[]表示可选
      // return -1; // 还可以继续执行，有默认的文件名
   }

   // 如果外面指定了文件名，则使用外面的名字，否则使用某人的文件名字
   if (argc == 2){
         STATISTICAL_RESULT_FILE_POINTER = fopen(argv[1], "w");
   }
   else{
      STATISTICAL_RESULT_FILE_POINTER = fopen("statisticalResult.csv", "w"); // 在程序所在的NUMA节点创建文件statisticalResult.csv
   }
   if (STATISTICAL_RESULT_FILE_POINTER == NULL) {
      perror("Error opening file: statisticalResult.csv");
      return -1;
   }
   
   // numa flag statistic结果输出文件，用于查看numa balancing的变化情况
   NUMA_BALANING_FLAG_STATISTIC_FILE_POINTER = fopen("numa_flag_statistic.csv", "w");
   if (NUMA_BALANING_FLAG_STATISTIC_FILE_POINTER == NULL) {
      perror("Error opening file: numa_flag_statistic.csv");
      return -1;
   }
   setvbuf(NUMA_BALANING_FLAG_STATISTIC_FILE_POINTER, NULL, _IONBF, 0); // 将文件设置为无缓冲模式

   // numa_balancing控制文件
   NUMA_BALANCING_CTL_FILE_POINTER = fopen("/proc/sys/kernel/numa_balancing", "w");
   if (NUMA_BALANCING_CTL_FILE_POINTER == NULL) {
      perror("Error opening file: /proc/sys/kernel/numa_balancing");
      return -1;
   }
   // 初始时，配置auto numa_balancing
   if(fprintf(NUMA_BALANCING_CTL_FILE_POINTER, "%d", SYS_NUMA_BALANCING_STATE) >= 0){
      fprintf(stderr, "Success initial set /proc/sys/kernel/numa_balancing.\n");
      fclose(NUMA_BALANCING_CTL_FILE_POINTER); //关闭打开的文件
   }
   else{
      fprintf(stderr, "Error initial writing /proc/sys/kernel/numa_balancing.\n");
      return -1;
   }

   thread_loop(); // 循环执行，统计全局系统状态

   fclose(STATISTICAL_RESULT_FILE_POINTER); //关闭打开的文件
   fclose(NUMA_BALANING_FLAG_STATISTIC_FILE_POINTER); //关闭打开的文件
   fclose(NUMA_BALANCING_CTL_FILE_POINTER); //关闭打开的文件
   return 0;
}