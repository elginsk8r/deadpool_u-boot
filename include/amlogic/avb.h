#ifndef _AMLOGIC_AVB_H_
#define _AMLOGIC_AVB_H_

#include <../lib/libavb/libavb.h>

int avb_verify(AvbSlotVerifyData** out_data);
int is_device_unlocked(void);
uint32_t avb_get_boot_patchlevel_from_vbmeta(AvbSlotVerifyData *data);

#endif /* _AMLOGIC_AVB_H_ */
