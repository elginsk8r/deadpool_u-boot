#!/bin/bash

exec_name=$0

set -o errtrace
trap 'echo Fatal error: script ${exec_name} aborting at line $LINENO, command \"$BASH_COMMAND\" returned $?; exit 1' ERR

cpu_num=$(grep -c processor /proc/cpuinfo)

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo DIR:$DIR

function usage(){
  echo "Usage: ${exec_name} <board> [workspace path]"
  echo "supported boards: spencer-p1/p2/b1/b3/b4, venus-p1, korlan-p0/p1/b1/b3/b4"
}

readonly fsi_folder="bootloader"
readonly fct_folder="factory/bootloader"
readonly fsi="$fsi_folder:"
readonly fct="$fct_folder:"

function building_uboot(){
  soc_family_name=$1
  local_name=$2
  rev=$3
  board_name=$4
  cfg_suffix=$6

  config=${local_name}_${rev}${cfg_suffix}

  for cfg in "$fsi" "$fct"; do
    local folder="${cfg%:*}"

    echo "building u-boot for ${board} in ${folder}"

    ./mk ${config} --board_name $board_name --bl2 fip/${soc_family_name}/bl2.bin --bl30 fip/${soc_family_name}/bl30.bin --bl31 fip/${soc_family_name}/bl31.img --bl32 fip/${soc_family_name}/bl32.img $5

    # make T=1 to use latest git commit time as build timestamp.

    echo "mk done\n"
    local product=`echo ${board} | cut -d "-" -f1`
    local bootloader_path=${workspace_path}/vendor/amlogic/${product}/prebuilt/${folder}
    if [ ! -z $workspace_path ]; then
      mkdir -p ${bootloader_path}
      if [ "$product" == "spencer" ] || \
        [ "$product" == "korlan" ] || \
        [ "$product" == "venus" ]; then
        # Copy bl2 and bl3x images for bootloader signing under eureka source.
        cp fip/build/bl2_new.bin ${bootloader_path}/bl2_new.bin.${board}
        cp fip/build/bl31.img ${bootloader_path}/bl31.img.${board}
        cp fip/build/bl32.img ${bootloader_path}/bl32.img.${board}
        cp fip/build/bl33.bin ${bootloader_path}/bl33.bin.${board}

        # Copy ddr bin for bootloader signing under eureka source.
        # TODO(ljchen): Remove hard code of ddr files under vendor/amlogic.
        cp fip/${soc_family_name}/aml_ddr.fw ${bootloader_path}

      else
        cp fip/${soc_family_name}/u-boot.bin.usb.bl2 ${bootloader_path}/u-boot.bin.usb.bl2
        cp fip/${soc_family_name}/u-boot.bin.usb.tpl ${bootloader_path}/u-boot.bin.usb.tpl
        cp fip/${soc_family_name}/u-boot.bin ${bootloader_path}/u-boot.bin
      fi
    fi
  done
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
  spencer-p1)
    building_uboot c2 c2_spencer p1 $board $dbg_flag
    ;;
  spencer-p2)
    building_uboot c2 c2_spencer p2 $board $dbg_flag
    ;;
  spencer-b1|spencer-b3)
    building_uboot c2 c2_spencer bx $board $dbg_flag
    ;;
  spencer-b4)
    export ENABLE_UBOOT_UPDATE=0
    building_uboot c2 c2_spencer bx $board $dbg_flag
    ;;
  venus-p1)
    building_uboot c2 c2_venus p1 $board $dbg_flag
    ;;
  korlan-p0)
    building_uboot a1 a1_korlan p0 $board $dbg_flag
    ;;
  korlan-p1)
    building_uboot a1 a1_korlan p1 $board $dbg_flag
    ;;
  korlan-p2)
    building_uboot a1 a1_korlan p2 $board $dbg_flag
    ;;
  korlan-b1)
    building_uboot a1 a1_korlan b1 $board $dbg_flag
    ;;
  korlan-b3|korlan-b4)
    building_uboot a1 a1_korlan bx $board $dbg_flag
    ;;
  *)
    echo "unknown board: $board"
    exit 1
esac
popd
