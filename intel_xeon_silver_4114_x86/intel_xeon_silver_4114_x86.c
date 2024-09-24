#include "intel_xeon_silver_4114_x86.h"

static int sleep_time = 1*TIME_SECOND;     /* Profile by sleep_time useconds chunks */
FILE *STATISTICAL_RESULT_FILE_POINTER = NULL; // 全局变量：statisticalResult.csv

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
   // IMC, node 0
   { //imc_0
      .name    = "node0/uncore_imc_0/CAS_COUNT/RD", // #0
      .type    = 0x0D,
      .config  = 0x04 | (0x03 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_imc_0/CAS_COUNT/WR", // #1
      .type    = 0x0D,
      .config  = 0x04 | (0x0c << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   { //imc_1
      .name    = "node0/uncore_imc_1/CAS_COUNT/RD", // #2
      .type    = 0x0E,
      .config  = 0x04 | (0x03 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_imc_1/CAS_COUNT/WR", // #3
      .type    = 0x0E,
      .config  = 0x04 | (0x0c << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   { //imc_2
      .name    = "node0/uncore_imc_2/CAS_COUNT/RD", // #4
      .type    = 0x0F,
      .config  = 0x04 | (0x03 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_imc_2/CAS_COUNT/WR", // #5
      .type    = 0x0F,
      .config  = 0x04 | (0x0c << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   { //imc_3
      .name    = "node0/uncore_imc_3/CAS_COUNT/RD", // #6
      .type    = 0x10,
      .config  = 0x04 | (0x03 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_imc_3/CAS_COUNT/WR", // #7
      .type    = 0x10,
      .config  = 0x04 | (0x0c << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   { //imc_4
      .name    = "node0/uncore_imc_4/CAS_COUNT/RD", // #8
      .type    = 0x11,
      .config  = 0x04 | (0x03 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_imc_4/CAS_COUNT/WR", // #9
      .type    = 0x11,
      .config  = 0x04 | (0x0c << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   { //imc_5
      .name    = "node0/uncore_imc_5/CAS_COUNT/RD", // #10
      .type    = 0x12,
      .config  = 0x04 | (0x03 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_imc_5/CAS_COUNT/WR", // #11
      .type    = 0x12,
      .config  = 0x04 | (0x0c << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   // IMC, node 1
   { //imc_0
      .name    = "node1/uncore_imc_0/CAS_COUNT/RD", // #12
      .type    = 0x0D,
      .config  = 0x04 | (0x03 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_imc_0/CAS_COUNT/WR", // #13
      .type    = 0x0D,
      .config  = 0x04 | (0x0c << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   { //imc_1
      .name    = "node1/uncore_imc_1/CAS_COUNT/RD", // #14
      .type    = 0x0E,
      .config  = 0x04 | (0x03 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_imc_1/CAS_COUNT/WR", // #15
      .type    = 0x0E,
      .config  = 0x04 | (0x0c << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   { //imc_2
      .name    = "node1/uncore_imc_2/CAS_COUNT/RD", // #16
      .type    = 0x0F,
      .config  = 0x04 | (0x03 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_imc_2/CAS_COUNT/WR", // #17
      .type    = 0x0F,
      .config  = 0x04 | (0x0c << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   { //imc_3
      .name    = "node1/uncore_imc_3/CAS_COUNT/RD", // #18
      .type    = 0x10,
      .config  = 0x04 | (0x03 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_imc_3/CAS_COUNT/WR", // #19
      .type    = 0x10,
      .config  = 0x04 | (0x0c << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   { //imc_4
      .name    = "node1/uncore_imc_4/CAS_COUNT/RD", // #20
      .type    = 0x11,
      .config  = 0x04 | (0x03 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_imc_4/CAS_COUNT/WR", // #21
      .type    = 0x11,
      .config  = 0x04 | (0x0c << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   { //imc_5
      .name    = "node1/uncore_imc_5/CAS_COUNT/RD", // #22
      .type    = 0x12,
      .config  = 0x04 | (0x03 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_imc_5/CAS_COUNT/WR", // #23
      .type    = 0x12,
      .config  = 0x04 | (0x0c << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },

   //////////////////////////////////////////////////////////

   // UPI, node 0
   { // UPI 0
      .name    = "node0/uncore_upi_0/TxL_FLITS", // #24
      .type    = 0x13,
      .config  = 0x02 | (0x0F << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_upi_0/RxL_FLITS", // #25
      .type    = 0x13,
      .config  = 0x03 | (0x0F << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   { // UPI 1
      .name    = "node0/uncore_upi_1/TxL_FLITS", // #26
      .type    = 0x14,
      .config  = 0x02 | (0x0F << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_upi_1/RxL_FLITS", // #27
      .type    = 0x14,
      .config  = 0x03 | (0x0F << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   // UPI, node 1
   { // UPI 0
      .name    = "node1/uncore_upi_0/TxL_FLITS", // #28
      .type    = 0x13,
      .config  = 0x02 | (0x0F << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_upi_0/RxL_FLITS", // #29
      .type    = 0x13,
      .config  = 0x03 | (0x0F << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   { // UPI 1
      .name    = "node1/uncore_upi_1/TxL_FLITS", // #30
      .type    = 0x14,
      .config  = 0x02 | (0x0F << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_upi_1/RxL_FLITS", // #31
      .type    = 0x14,
      .config  = 0x03 | (0x0F << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },

   /////////////////////////////////////////////////////////////////

   // CHA, node 0
   // cha0
   {
      .name    = "node0/uncore_cha_0/LLC_LOOKUP/ANY", // #
      .type    = 0x18,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_0/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x18,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_0/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x18,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_0/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x18,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_0/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x18,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_0/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x18,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_0/LLC_LOOKUP/WRITE", // #
      .type    = 0x18,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xFF << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_0/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x18,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xFE << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_0/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x18,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_0/REQUESTS/READS_LOCAL", // #38
      .type    = 0x18,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_0/REQUESTS/READS_REMOTE", // #39
      .type    = 0x18,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_0/REQUESTS/WRITES_LOCAL", // #40
      .type    = 0x18,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_0/REQUESTS/WRITES_REMOTE", // #41
      .type    = 0x18,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   // cha 1
   {
      .name    = "node0/uncore_cha_1/LLC_LOOKUP/ANY", // #
      .type    = 0x19,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_1/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x19,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_1/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x19,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_1/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x19,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_1/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x19,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_1/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x19,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_1/LLC_LOOKUP/WRITE", // #
      .type    = 0x19,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_1/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x19,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_1/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x19,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_1/REQUESTS/READS_LOCAL", // #48
      .type    = 0x19,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_1/REQUESTS/READS_REMOTE", // #49
      .type    = 0x19,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_1/REQUESTS/WRITES_LOCAL", // #50
      .type    = 0x19,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_1/REQUESTS/WRITES_REMOTE", // #51
      .type    = 0x19,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   // cha 2
   {
      .name    = "node0/uncore_cha_2/LLC_LOOKUP/ANY", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_2/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_2/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_2/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_2/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_2/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_2/LLC_LOOKUP/WRITE", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_2/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_2/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_2/REQUESTS/READS_LOCAL", // #58
      .type    = 0x1A,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_2/REQUESTS/READS_REMOTE", // #59
      .type    = 0x1A,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_2/REQUESTS/WRITES_LOCAL", // #60
      .type    = 0x1A,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_2/REQUESTS/WRITES_REMOTE", // #61
      .type    = 0x1A,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   // cha 3
   {
      .name    = "node0/uncore_cha_3/LLC_LOOKUP/ANY", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_3/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_3/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_3/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_3/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_3/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_3/LLC_LOOKUP/WRITE", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_3/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_3/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_3/REQUESTS/READS_LOCAL", // #68
      .type    = 0x1B,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_3/REQUESTS/READS_REMOTE", // #69
      .type    = 0x1B,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_3/REQUESTS/WRITES_LOCAL", // #70
      .type    = 0x1B,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_3/REQUESTS/WRITES_REMOTE", // #71
      .type    = 0x1B,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   // cha 4
   {
      .name    = "node0/uncore_cha_4/LLC_LOOKUP/ANY", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_4/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_4/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_4/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_4/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_4/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_4/LLC_LOOKUP/WRITE", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_4/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_4/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_4/REQUESTS/READS_LOCAL", // #78
      .type    = 0x1C,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_4/REQUESTS/READS_REMOTE", // #79
      .type    = 0x1C,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_4/REQUESTS/WRITES_LOCAL", // #80
      .type    = 0x1C,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_4/REQUESTS/WRITES_REMOTE", // #81
      .type    = 0x1C,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   // cha 5
   {
      .name    = "node0/uncore_cha_5/LLC_LOOKUP/ANY", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_5/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_5/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_5/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_5/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_5/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_5/LLC_LOOKUP/WRITE", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_5/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_5/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_5/REQUESTS/READS_LOCAL", // #88
      .type    = 0x1D,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_5/REQUESTS/READS_REMOTE", // #89
      .type    = 0x1D,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_5/REQUESTS/WRITES_LOCAL", // #90
      .type    = 0x1D,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_5/REQUESTS/WRITES_REMOTE", // #91
      .type    = 0x1D,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   // cha 6
   {
      .name    = "node0/uncore_cha_6/LLC_LOOKUP/ANY", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_6/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_6/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_6/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_6/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_6/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_6/LLC_LOOKUP/WRITE", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_6/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_6/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_6/REQUESTS/READS_LOCAL", // #98
      .type    = 0x1E,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_6/REQUESTS/READS_REMOTE", // #99
      .type    = 0x1E,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_6/REQUESTS/WRITES_LOCAL", // #100
      .type    = 0x1E,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_6/REQUESTS/WRITES_REMOTE", // #101
      .type    = 0x1E,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   // cha 7
   {
      .name    = "node0/uncore_cha_7/LLC_LOOKUP/ANY", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_7/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_7/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_7/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_7/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_7/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_7/LLC_LOOKUP/WRITE", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_7/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_7/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_7/REQUESTS/READS_LOCAL", // #108
      .type    = 0x1F,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_7/REQUESTS/READS_REMOTE", // #109
      .type    = 0x1F,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_7/REQUESTS/WRITES_LOCAL", // #110
      .type    = 0x1F,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_7/REQUESTS/WRITES_REMOTE", // #111
      .type    = 0x1F,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   // cha 8
   {
      .name    = "node0/uncore_cha_8/LLC_LOOKUP/ANY", // #
      .type    = 0x20,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_8/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x20,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_8/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x20,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_8/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x20,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_8/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x20,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_8/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x20,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_8/LLC_LOOKUP/WRITE", // #
      .type    = 0x20,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_8/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x20,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_8/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x20,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_8/REQUESTS/READS_LOCAL", // #118
      .type    = 0x20,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_8/REQUESTS/READS_REMOTE", // #119
      .type    = 0x20,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_8/REQUESTS/WRITES_LOCAL", // #120
      .type    = 0x20,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_8/REQUESTS/WRITES_REMOTE", // #121
      .type    = 0x20,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   // cha 9
   {
      .name    = "node0/uncore_cha_9/LLC_LOOKUP/ANY", // #
      .type    = 0x21,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_9/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x21,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_9/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x21,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_9/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x21,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_9/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x21,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_9/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x21,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_9/LLC_LOOKUP/WRITE", // #
      .type    = 0x21,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_9/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x21,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_9/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x21,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_9/REQUESTS/READS_LOCAL", // #128
      .type    = 0x21,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_9/REQUESTS/READS_REMOTE", // #129
      .type    = 0x21,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_9/REQUESTS/WRITES_LOCAL", // #130
      .type    = 0x21,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   {
      .name    = "node0/uncore_cha_9/REQUESTS/WRITES_REMOTE", // #131
      .type    = 0x21,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 0,
   },
   // TOR_INSERTS node 1
   // cha0
   {
      .name    = "node1/uncore_cha_0/LLC_LOOKUP/ANY", // #
      .type    = 0x18,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_0/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x18,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_0/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x18,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_0/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x18,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_0/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x18,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_0/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x18,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_0/LLC_LOOKUP/WRITE", // #
      .type    = 0x18,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_0/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x18,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_0/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x18,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_0/REQUESTS/READS_LOCAL", // #138
      .type    = 0x18,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_0/REQUESTS/READS_REMOTE", // #139
      .type    = 0x18,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_0/REQUESTS/WRITES_LOCAL", // #140
      .type    = 0x18,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_0/REQUESTS/WRITES_REMOTE", // #141
      .type    = 0x18,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   // cha 1
   {
      .name    = "node1/uncore_cha_1/LLC_LOOKUP/ANY", // #
      .type    = 0x19,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_1/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x19,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_1/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x19,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_1/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x19,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_1/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x19,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_1/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x19,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_1/LLC_LOOKUP/WRITE", // #
      .type    = 0x19,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_1/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x19,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_1/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x19,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_1/REQUESTS/READS_LOCAL", // #148
      .type    = 0x19,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_1/REQUESTS/READS_REMOTE", // #149
      .type    = 0x19,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_1/REQUESTS/WRITES_LOCAL", // #150
      .type    = 0x19,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_1/REQUESTS/WRITES_REMOTE", // #151
      .type    = 0x19,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   // cha 2
   {
      .name    = "node1/uncore_cha_2/LLC_LOOKUP/ANY", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_2/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_2/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_2/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_2/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_2/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_2/LLC_LOOKUP/WRITE", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_2/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_2/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x1A,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_2/REQUESTS/READS_LOCAL", // #158
      .type    = 0x1A,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_2/REQUESTS/READS_REMOTE", // #159
      .type    = 0x1A,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_2/REQUESTS/WRITES_LOCAL", // #160
      .type    = 0x1A,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_2/REQUESTS/WRITES_REMOTE", // #161
      .type    = 0x1A,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   // cha 3
   {
      .name    = "node1/uncore_cha_3/LLC_LOOKUP/ANY", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_3/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_3/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_3/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_3/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_3/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_3/LLC_LOOKUP/WRITE", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_3/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_3/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x1B,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_3/REQUESTS/READS_LOCAL", // #168
      .type    = 0x1B,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_3/REQUESTS/READS_REMOTE", // #169
      .type    = 0x1B,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_3/REQUESTS/WRITES_LOCAL", // #170
      .type    = 0x1B,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_3/REQUESTS/WRITES_REMOTE", // #171
      .type    = 0x1B,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   // cha 4
   {
      .name    = "node1/uncore_cha_4/LLC_LOOKUP/ANY", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_4/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_4/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_4/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_4/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_4/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_4/LLC_LOOKUP/WRITE", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_4/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_4/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x1C,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_4/REQUESTS/READS_LOCAL", // #178
      .type    = 0x1C,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_4/REQUESTS/READS_REMOTE", // #179
      .type    = 0x1C,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_4/REQUESTS/WRITES_LOCAL", // #180
      .type    = 0x1C,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_4/REQUESTS/WRITES_REMOTE", // #181
      .type    = 0x1C,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   // cha 5
   {
      .name    = "node1/uncore_cha_5/LLC_LOOKUP/ANY", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_5/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_5/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_5/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_5/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_5/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_5/LLC_LOOKUP/WRITE", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_5/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_5/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x1D,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_5/REQUESTS/READS_LOCAL", // #188
      .type    = 0x1D,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_5/REQUESTS/READS_REMOTE", // #189
      .type    = 0x1D,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_5/REQUESTS/WRITES_LOCAL", // #190
      .type    = 0x1D,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_5/REQUESTS/WRITES_REMOTE", // #191
      .type    = 0x1D,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   // cha 6
   {
      .name    = "node1/uncore_cha_6/LLC_LOOKUP/ANY", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_6/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_6/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_6/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_6/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_6/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_6/LLC_LOOKUP/WRITE", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_6/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_6/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x1E,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_6/REQUESTS/READS_LOCAL", // #198
      .type    = 0x1E,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_6/REQUESTS/READS_REMOTE", // #199
      .type    = 0x1E,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_6/REQUESTS/WRITES_LOCAL", // #200
      .type    = 0x1E,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_6/REQUESTS/WRITES_REMOTE", // #201
      .type    = 0x1E,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   // cha 7
   {
      .name    = "node1/uncore_cha_7/LLC_LOOKUP/ANY", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_7/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_7/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_7/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_7/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_7/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_7/LLC_LOOKUP/WRITE", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_7/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_7/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x1F,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_7/REQUESTS/READS_LOCAL", // #208
      .type    = 0x1F,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_7/REQUESTS/READS_REMOTE", // #209
      .type    = 0x1F,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_7/REQUESTS/WRITES_LOCAL", // #210
      .type    = 0x1F,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_7/REQUESTS/WRITES_REMOTE", // #211
      .type    = 0x1F,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   // cha 8
   {
      .name    = "node1/uncore_cha_8/LLC_LOOKUP/ANY", // #
      .type    = 0x20,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_8/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x20,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_8/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x20,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_8/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x20,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_8/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x20,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_8/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x20,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_8/LLC_LOOKUP/WRITE", // #
      .type    = 0x20,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_8/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x20,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_8/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x20,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_8/REQUESTS/READS_LOCAL", // #218
      .type    = 0x20,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_8/REQUESTS/READS_REMOTE", // #219
      .type    = 0x20,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_8/REQUESTS/WRITES_LOCAL", // #220
      .type    = 0x20,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_8/REQUESTS/WRITES_REMOTE", // #221
      .type    = 0x20,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   // cha 9
   {
      .name    = "node1/uncore_cha_9/LLC_LOOKUP/ANY", // #
      .type    = 0x21,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_9/LLC_LOOKUP/ANY/HIT", // #
      .type    = 0x21,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_9/LLC_LOOKUP/ANY/MISS", // #
      .type    = 0x21,
      .config  = 0x34 | (0x11 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_9/LLC_LOOKUP/DATA_READ", // #
      .type    = 0x21,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_9/LLC_LOOKUP/DATA_READ/HIT", // #
      .type    = 0x21,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_9/LLC_LOOKUP/DATA_READ/MISS", // #
      .type    = 0x21,
      .config  = 0x34 | (0x03 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_9/LLC_LOOKUP/WRITE", // #
      .type    = 0x21,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF1 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_9/LLC_LOOKUP/WRITE/HIT", // #
      .type    = 0x21,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0xF0 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_9/LLC_LOOKUP/WRITE/MISS", // #
      .type    = 0x21,
      .config  = 0x34 | (0x05 << 8),
      .config1  = 0x01 << 17,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_9/REQUESTS/READS_LOCAL", // #228
      .type    = 0x21,
      .config  = 0x50 | (0x01 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_9/REQUESTS/READS_REMOTE", // #229
      .type    = 0x21,
      .config  = 0x50 | (0x02 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_9/REQUESTS/WRITES_LOCAL", // #230
      .type    = 0x21,
      .config  = 0x50 | (0x04 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
   {
      .name    = "node1/uncore_cha_9/REQUESTS/WRITES_REMOTE", // #231
      .type    = 0x21,
      .config  = 0x50 | (0x08 << 8),
      .config1  = -1,
      .leader  = -1,
      .cpuid   = 1,
   },
};
static int nb_events = sizeof(default_events)/sizeof(*default_events); // 计算事件的个数
static event_t *events = default_events; // 新的指针指向数组，减少函数调用的时候，数组被复制

// 一些中断信号处理
static void sig_handler(int signal) {
   printf("#signal caught: %d\n", signal);
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
      if(events[j].config1 != -1){
         events_attr[j].config1 = events[j].config1;
      }
      events_attr[j].exclude_kernel = events[j].exclude_kernel;
      events_attr[j].exclude_user = events[j].exclude_user;
      //PERF_FORMAT_TOTAL_TIME_ENABLED：表示在读取性能计数时，将返回事件被启用的总时间。PERF_FORMAT_TOTAL_TIME_RUNNING：表示在读取性能计数时，将返回事件正在运行的总时间。
      events_attr[j].read_format = PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING;
      fd[j] = sys_perf_counter_open(&events_attr[j], -1, events[j].cpuid, (events[j].leader==-1)?-1:fd[events[j].leader], 0);
      if (fd[j] < 0) {
         fprintf(stdout, "sys_perf_counter_open failed: %s\n", strerror(errno));
         return;
      }
      ioctl(fd[j], PERF_EVENT_IOC_RESET, 0); // 重置计数器的值
      ioctl(fd[j], PERF_EVENT_IOC_ENABLE, 0); // 寄存器开始计数
   }
   fprintf(STATISTICAL_RESULT_FILE_POINTER, "\n"); // 统计信息表格题目设置好后，换行
	
   // 定义临时存储结果的变量
   struct perf_read_ev single_count;
   struct perf_read_ev *last_counts = calloc(nb_events, sizeof(*last_counts));
   struct perf_read_ev *last_counts_prev = calloc(nb_events, sizeof(*last_counts_prev));

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
         // fprintf(stdout, "No %d: Registering event %s, value is: %ld\n", j, events[j].name, last_counts[j].value-last_counts_prev[j].value); // 调试用，输出性能事件的名字及值
         last_counts_prev[j] = last_counts[j];
      }
      fprintf(STATISTICAL_RESULT_FILE_POINTER, "\n"); // 统计信息表格本次事件读取完毕后，换行

      // 记录上一次统计结果
      // for(int j=0; j < nb_events; j++){
      //    last_counts_prev[j] = last_counts[j];
      // }
   }

   // 关闭打开的文件
   for (int j = 0; j < nb_events; j++) {
      close(fd[j]);
   }

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

   thread_loop(); // 循环执行，统计全局系统状态

   fclose(STATISTICAL_RESULT_FILE_POINTER); //关闭打开的文件
   return 0;
}