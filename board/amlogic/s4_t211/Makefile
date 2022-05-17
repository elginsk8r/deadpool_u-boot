SOURCE_LOGO_FILE := res/images/logo.bmp.gz
TARGET_LOGO_FILE_C := $(src)/res_images_logo_bmp_gz.c
$(TARGET_LOGO_FILE_C): $(srctree)/$(src)/$(SOURCE_LOGO_FILE)
	cd $(srctree)/$(src); xxd -i $(SOURCE_LOGO_FILE) > $(abspath $@)

obj-y += $(BOARD).o
obj-y += $(basename $(notdir $(TARGET_LOGO_FILE_C))).o

# b/206911824: vendor public key
obj-$(CONFIG_CMD_BOOTCTOL_AVB) += avb_kpub_vendor.o

