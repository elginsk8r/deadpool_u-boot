#!/bin/bash

exec_name=$0

set -o errtrace
trap 'echo Fatal error: script ${exec_name} aborting at line $LINENO, command \"$BASH_COMMAND\" returned $?; exit 1' ERR

cpu_num=$(grep -c processor /proc/cpuinfo)

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo DIR:$DIR

function usage(){
  echo "Usage: ${exec_name} <board> [workspace path]"
  echo "supported boards: spencer-p0, mars-p0"
}

function building_uboot(){
  soc_family_name=$1
  local_name=$2
  rev=$3
  board_name=$4
  cfg_suffix=$6

  config=${local_name}_${rev}${cfg_suffix}
  echo "building u-boot for ${board}"

  ./mk ${config} --board_name $board_name --bl2 fip/${soc_family_name}/bl2.bin --bl30 fip/${soc_family_name}/bl30.bin --bl31 fip/${soc_family_name}/bl31.img --bl32 fip/${soc_family_name}/bl32.img $5

  # make T=1 to use latest git commit time as build timestamp.

  echo "mk done\n"
  product=`echo ${board} | cut -d "-" -f1`
  if [ ! -z $workspace_path ]; then
    mkdir -p ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/
    if [ "$product" == "spencer" ] || \
       [ "$product" == "mars" ]; then
      # Copy bl2 and bl3x images for bootloader signing under eureka source.
      cp fip/build/bl2_new.bin \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/bl2_new.bin.${board}
      if [[ "$product" == "spencer" || "$product" == "mars" ]] ; then
        cp fip/c1/bl30_new.fake \
          ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/bl30_new.bin.${board}
      else
        cp fip/build/bl30_new.bin \
          ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/bl30_new.bin.${board}
      fi
      cp fip/build/bl31.img \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/bl31.img.${board}
      cp fip/build/bl32.img \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/bl32.img.${board}
      cp fip/build/bl33.bin \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/bl33.bin.${board}

      # Copy ddr bin for bootloader signing under eureka source.
      # TODO(ljchen): Remove hard code of ddr files under vendor/amlogic.
      cp fip/${soc_family_name}/ddr4_1d.fw \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/
      cp fip/${soc_family_name}/ddr4_2d.fw \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/
      cp fip/${soc_family_name}/ddr3_1d.fw \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/
      cp fip/${soc_family_name}/piei.fw \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/
      cp fip/${soc_family_name}/lpddr4_1d.fw \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/
      cp fip/${soc_family_name}/lpddr4_2d.fw \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/
      cp fip/${soc_family_name}/diag_lpddr4.fw \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/
      cp fip/${soc_family_name}/aml_ddr.fw \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/aml_ddr.fw.${board}

    else
      cp fip/${soc_family_name}/u-boot.bin.usb.bl2 \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/u-boot.bin.usb.bl2
      cp fip/${soc_family_name}/u-boot.bin.usb.tpl \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/u-boot.bin.usb.tpl
      cp fip/${soc_family_name}/u-boot.bin \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/u-boot.bin
    fi
  fi
}

if (( $# < 1 ))
then
  usage
  exit 2
fi

pushd $DIR

readonly board=$1
readonly workspace_path=$2
readonly cross_compile=$DIR/../amlogic/linaro/gcc-linaro-7.3.1-2018.05-i686_aarch64-elf/bin/aarch64-elf-
readonly cross_compile_t32=$DIR/../amlogic/linaro/gcc-arm-none-eabi-6-2017-q2-update/bin/arm-none-eabi-
readonly vendor_amlogic=$DIR/../vendor/amlogic

dbg_flag="debug"
zircon_cfg=""

if [ "$3" = "release" -o "$4" = "release" ]; then
	dbg_flag="release"
elif [ "$4" = "zircon" -o "$5" = "zircon" ]; then
	zircon_cfg="_zircon"
fi


export ENABLE_UBOOT_UPDATE=1

case $board in
  spencer-p0)
    # Spencer mock board based on nq-p2
    building_uboot c1 c1_nq p2 $board $dbg_flag
    ;;
  mars-p0)
    # Mars mock board based on gq-p2
    building_uboot c1 c1_gq p2 $board $dbg_flag
    ;;
  *)
    echo "unknown board: $board"
    exit 1
esac
popd
