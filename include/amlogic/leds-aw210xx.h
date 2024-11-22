#ifndef LEDS_AW2026_H_
#define LEDS_AW2026_H_

#include <common.h>

#ifdef CONFIG_SYS_I2C_MESON
#include <i2c.h>
#include <dt-bindings/i2c/meson-i2c.h>
#include <fs.h>
#endif

#define N_CAL_SETTINGS 6
#define I2C_BUS_NUM 4
#define I2C_LED_REG 0x20

/*****************************************************
* register about led mode
*****************************************************/
#define AW210XX_REG_GCR						(0x20)
#define AW210XX_REG_BR00L					(0x21)
#define AW210XX_REG_BR00H					(0x22)
#define AW210XX_REG_BR01L					(0x23)
#define AW210XX_REG_BR01H					(0x24)
#define AW210XX_REG_BR02L					(0x25)
#define AW210XX_REG_BR02H					(0x26)
#define AW210XX_REG_BR03L					(0x27)
#define AW210XX_REG_BR03H					(0x28)
#define AW210XX_REG_BR04L					(0x29)
#define AW210XX_REG_BR04H					(0x2A)
#define AW210XX_REG_BR05L					(0x2B)
#define AW210XX_REG_BR05H					(0x2C)
#define AW210XX_REG_BR06L					(0x2D)
#define AW210XX_REG_BR06H					(0x2E)
#define AW210XX_REG_BR07L					(0x2F)
#define AW210XX_REG_BR07H					(0x30)
#define AW210XX_REG_BR08L					(0x31)
#define AW210XX_REG_BR08H					(0x32)
#define AW210XX_REG_BR09L					(0x33)
#define AW210XX_REG_BR09H					(0x34)
#define AW210XX_REG_BR10L					(0x35)
#define AW210XX_REG_BR10H					(0x36)
#define AW210XX_REG_BR11L					(0x37)
#define AW210XX_REG_BR11H					(0x38)
#define AW210XX_REG_BR12L					(0x39)
#define AW210XX_REG_BR12H					(0x3A)
#define AW210XX_REG_BR13L					(0x3B)
#define AW210XX_REG_BR13H					(0x3C)
#define AW210XX_REG_BR14L					(0x3D)
#define AW210XX_REG_BR14H					(0x3E)
#define AW210XX_REG_BR15L					(0x3F)
#define AW210XX_REG_BR15H					(0x40)
#define AW210XX_REG_BR16L					(0x41)
#define AW210XX_REG_BR16H					(0x42)
#define AW210XX_REG_BR17L					(0x43)
#define AW210XX_REG_BR17H					(0x44)
#define AW210XX_REG_UPDATE					(0x45)
#define AW210XX_REG_SL00					(0x46)
#define AW210XX_REG_SL01					(0x47)
#define AW210XX_REG_SL02					(0x48)
#define AW210XX_REG_SL03					(0x49)
#define AW210XX_REG_SL04					(0x4A)
#define AW210XX_REG_SL05					(0x4B)
#define AW210XX_REG_SL06					(0x4C)
#define AW210XX_REG_SL07					(0x4D)
#define AW210XX_REG_SL09					(0x4F)
#define AW210XX_REG_SL10					(0x50)
#define AW210XX_REG_SL11					(0x51)
#define AW210XX_REG_SL12					(0x52)
#define AW210XX_REG_SL13					(0x53)
#define AW210XX_REG_SL14					(0x54)
#define AW210XX_REG_SL15					(0x55)
#define AW210XX_REG_SL16					(0x56)
#define AW210XX_REG_SL17					(0x57)
#define AW210XX_REG_GCCR					(0x58)
#define AW210XX_REG_PHCR					(0x59)
#define AW210XX_REG_OSDCR					(0x5A)
#define AW210XX_REG_OSST0					(0x5B)
#define AW210XX_REG_OSST1					(0x5C)
#define AW210XX_REG_OSST2					(0x5D)
#define AW210XX_REG_OTCR					(0x5E)
#define AW210XX_REG_SSCR					(0x5F)
#define AW210XX_REG_UVCR					(0x60)
#define AW210XX_REG_GCR2					(0x61)
#define AW210XX_REG_GCR3					(0x62)
#define AW210XX_REG_RESET					(0x70)
#define AW210XX_REG_ABMCFG					(0x80)
#define AW210XX_REG_ABMGO					(0x81)
#define AW210XX_REG_ABMT0					(0x82)
#define AW210XX_REG_ABMT1					(0x83)
#define AW210XX_REG_ABMT2					(0x84)
#define AW210XX_REG_ABMT3					(0x85)
#define AW210XX_REG_GBRH					(0x86)
#define AW210XX_REG_GBRL					(0x87)
#define AW210XX_REG_GSLR					(0x88)
#define AW210XX_REG_GSLG					(0x89)
#define AW210XX_REG_GSLB					(0x8A)
#define AW210XX_REG_GCFG					(0x8B)

/*****************************************************
 * define register Detail
*****************************************************/
#define AW210XX_BIT_APSE_MASK				(1 << 7)
#define AW210XX_BIT_APSE_ENABLE				(1 << 7)
#define AW210XX_BIT_APSE_DISENA				(0 << 7)
#define AW210XX_BIT_CHIPEN_MASK				(1 << 0)
#define AW210XX_BIT_CHIPEN_ENABLE			(1 << 0)
#define AW210XX_BIT_CHIPEN_DISENA			(0 << 0)

#define AW210XX_UPDATE_ENABLE 				(0x0)
#define AW210XX_UPDATE_MASK				(0xff)
#define AW210XX_PDE_ENABLE 				(1 << 7)
#define AW210XX_PDE_MASK 				(1 << 7)
#define AW210XX_GE2_SET 				(1 << 2)
#define AW210XX_GE_MASK					(0x7)
#define AW210XX_PATE_ENABLE 				(1 << 0)
#define AW210XX_PATMD_AUTO 				(1 << 1)
#define AW210XX_PATCFG_MASK 				(0x3)
#define AW210XX_PAT_RISE_TIME 				(0x0)
#define AW210XX_PAT_RISE_MASK 				(0xf << 4)
#define AW210XX_PAT_ON_TIME 				(0x6)
#define AW210XX_PAT_ON_MASK 				(0xf)
#define AW210XX_PAT_FALL_TIME 				(0x0)
#define AW210XX_PAT_FALL_MASK 				(0xf << 4)
#define AW210XX_PAT_OFF_TIME 				(0x6)
#define AW210XX_PAT_OFF_MASK 				(0xf)
#define AW210XX_PAT_RUN 				(0x1)
#define AW210XX_PAT_RUN_MASK 				(0x1)


#define AW210XX_RESET_MASK                               0xff
#define AW210XX_GLOBAL_CURRENT_MASK                      0xff
#define AW210XX_SLXX_MASK 				 0xff
#define AW210XX_BRXX_MASK 				 0xff

/*****************************************************
 * define register data
*****************************************************/
#define AW210XX_RESET_CHIP					(0x00)
#define AW210XX_UPDATE_BR_SL				        (0x00)
#define AW210XX_GROUP_ENABLE				        (0x38)
#define AW210XX_GROUP_DISABLE				        (0x40)
#define AW210XX_GLOBAL_DEFAULT_SET			        (0x66)
#define AW210XX_GBRH_DEFAULT_SET			        (0x60)
#define AW210XX_GBRL_DEFAULT_SET			        (0x00)
#define AW210XX_ABMT0_SET					(0x99)
#define AW210XX_ABMT1_SET					(0x99)
#define AW210XX_ABMT2_SET					(0x00)
#define AW210XX_ABMT3_SET					(0x00)
#define AW210XX_ABMCFG_SET					(0x03)
#define AW210XX_ABMGO_SET					(0x01)
#define AW210xx_MAX_CURRENT 					(0xff)
#define AW210xx_MAX_BRIGHTNESS 					(0xff)
#define AW210xx_MIN_BRIGHTNESS					(0x0)

enum LED_ANIMATION { WHITE, YELLOW, BLINK_YELLOW };
void sys_led_init(enum LED_ANIMATION led_animation);

#endif  // LEDS_AW2026_H_
