#ifndef _AMLOGIC_AVB_H_
#define _AMLOGIC_AVB_H_

#include <../lib/libavb/libavb.h>

int avb_verify(AvbSlotVerifyData** out_data);
int is_device_unlocked(void);

#endif /* _AMLOGIC_AVB_H_ */
