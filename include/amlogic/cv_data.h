#ifndef _CV_DATA_H_
#define _CV_DATA_H_

#ifdef __cplusplus
extern "C" {
#endif

/* cv_data configuration */
#define CV_DATA_BASE_ADDR 0xfff60000
#define CV_DATA_CUR_VER 1
#define CV_DATA_VER_ADDR (CV_DATA_BASE_ADDR + 0x1FF)
#define CV_RODATA_BASE_ADDR CV_DATA_BASE_ADDR
#define CV_RWDATA_BASE_ADDR (CV_DATA_BASE_ADDR + 0x200)
#define CV_DATA_SIZE_LIMIT 512

/* rtos_status configuration */
#define RTOS_STATUS_BASE_ADDR 0xfff62000
#define RTOS_STATUS_MAGIC 0xABBACDDC

struct CvRodata {
	u16 size;
	u8 led[50];
	u8 crc8;
} __packed;

struct CvRwdata {
	u16 size;
	u8 rtos_enable;
	u8 oobe_status;
	u8 camera_enable;
	u8 camera_mode;
	u8 camera_vflip;
	u8 led_mode;
	u8 audio_enable;
	u8 crc8;
} __packed;

struct CameraPara {
	u8 is_exposure_valid;
	u8 ir_mode_enabled;
	u8 ir_power_setting;
	u32 last_optimal_exposure_value;
	u32 last_optimal_short_expo_lines;
	u32 last_optimal_gain;
	u32 expo_ratio;
	u32 last_histogram_mean;
	u32 delay_frames;
	u8 is_hdr_enabled;
} __packed;

struct RtosStatus {
	u32 magic;
	u8 corrupted;
	u16 wakeup_reasons;
	u8 day_night;
	u32 first_frame_te;
	struct CameraPara camera_data;
	u8 crc8;
} __packed;

#ifdef __cplusplus
}
#endif
#endif
