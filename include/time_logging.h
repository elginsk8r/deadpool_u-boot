#ifndef __TIME_LOGGING_H_
#define __TIME_LOGGING_H_ 1

#include <timer.h>

#define LOG_UBOOT_NUM 0xfff62840
#define LOG_UBOOT_ENTRY 0xfff62844
#define LOG_BEFORE_LOAD 0xfff62848
#define LOG_AFTER_LOAD 0xfff6284C
#define LOG_BEFORE_SEC 0xfff62850
#define LOG_AFTER_SEC 0xfff62854
#define LOG_BEFORE_DECOMP 0xfff62858
#define LOG_AFTER_DECOMP 0xfff6285C
#define LOG_KERN_JUMP 0xfff62860

static inline void logging_set_num(uint32_t num)
{
	*(uint32_t *)LOG_UBOOT_NUM = num;
}

static inline void logging_set_entry(uint64_t entry)
{
	*(uint32_t *)entry = get_time();
}

#endif /* __TIME_LOGGING_H_ */
