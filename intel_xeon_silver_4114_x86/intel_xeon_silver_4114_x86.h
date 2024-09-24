#ifndef PROFILER_H_
#define PROFILER_H_ // 防止多次引入头文件

#define _GNU_SOURCE
#include <stdio.h> // 标准输入输出
#include <signal.h> // signal()函数
#include <stdlib.h> // exit(0)函数
#include <numaif.h> // numa_max_node()
#include <numa.h> // numa库
#include <linux/perf_event.h> //struct perf_event_attr的头文件
#include <asm/unistd.h> //__NR_perf_event_open的头文件
#include <unistd.h> // syscall()的头文件
#include <sys/ioctl.h> // ioctl()的头文件
#include <stdint.h>
#include <assert.h>
#include <sched.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/syscall.h>
#define __EXPORTED_HEADERS__
#include <sys/sysinfo.h>
#undef __EXPORTED_HEADERS__
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <dirent.h>

#undef __NR_perf_counter_open
#ifdef __powerpc__
#define __NR_perf_counter_open  319
#elif defined(__x86_64__)
#define __NR_perf_counter_open  298
#elif defined(__i386__)
#define __NR_perf_counter_open  336
#elif defined(__aarch64__)
#define __NR_perf_counter_open  241 //arm架构下
#endif

typedef struct _event {
   uint64_t type;
   uint64_t config;
   uint64_t config1;
   uint64_t exclude_kernel;
   uint64_t exclude_user;
   const char* name;
   long leader;
   int cpuid;
} event_t; //将_event结构体转化为外部结构体

struct perf_read_ev {
   uint64_t value;
   uint64_t time_enabled;
   uint64_t time_running;
};

#define TIME_SECOND             1000000

#endif /* PROFILER_H_ */