#!/bin/bash

exec_name=$0
dbg_flag="debug"
zircon_cfg=""
prebuilt_path=""

set -o errtrace
trap 'echo Fatal error: script ${exec_name} aborting at line $LINENO, command \"$BASH_COMMAND\" returned $?; exit 1' ERR

cpu_num=$(grep -c processor /proc/cpuinfo)

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo DIR:$DIR

function usage(){
  echo "Usage: ${exec_name} <board> [workspace path] [-o (prebuilt path)]"
  echo "supported boards: spencer-p1/p2/b1/b3/b4, venus-p1/p2, a049-p0, bsv3-p1"
}

function update_prebuilt_path() {
  has_bl2_bl3x=0
  if [ -d $DIR/../bl2 ] && [ -d $DIR/../bl31 ] && [ -d $DIR/../bl32 ]; then
    has_bl2_bl3x=1
  fi

  if [ "$has_bl2_bl3x" == "0" ]; then
    default_prebuilt_path=${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/blx
    if [ -d ${default_prebuilt_path} ]; then
      prebuilt_path=${default_prebuilt_path}
      echo "use prebuilt from ${prebuilt_path}"
    fi
  fi

  if [ -z "$prebuilt_path" ]; then
    prebuilt_path=fip/${soc_family_name}
    echo "unspecified prebuilt path, will use local prebuilt ${prebuilt_path}"
  fi

  echo "current prebuilt path: ${prebuilt_path}"
}

function building_uboot(){
  soc_family_name=$1
  local_name=$2
  rev=$3
  board_name=$4
  cfg_suffix=$6

  config=${local_name}_${rev}${cfg_suffix}
  product=`echo ${board} | cut -d "-" -f1`
  echo "building u-boot for ${board}"

  update_prebuilt_path
  ./mk ${config} --board_name $board_name --bl2 ${prebuilt_path}/bl2.bin --bl30 ${prebuilt_path}/bl30.bin --bl31 ${prebuilt_path}/bl31.img --bl32 ${prebuilt_path}/bl32.img $5

  # make T=1 to use latest git commit time as build timestamp.

  echo "mk done\n"
  if [ ! -z $workspace_path ]; then
    mkdir -p ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/
    if [ "$product" == "spencer" ] || \
       [ "$product" == "venus" ] || \
       [ "$product" == "a049" ] || \
       [ "$product" == "bsv3" ] || \
       [ "$product" == "bla4" ] || \
       [ "$product" == "2rs4" ] || \
       [ "$product" == "xua4" ]; then
      # Copy bl2 and bl3x images for bootloader signing under eureka source.
      cp fip/build/bl2_new.bin \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/bl2_new.bin.${board}
      cp fip/build/bl31.img \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/bl31.img.${board}
      cp fip/build/bl32.img \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/bl32.img.${board}
      cp fip/build/bl33.bin \
        ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/bl33.bin.${board}

      # Copy ddr bin for bootloader signing under eureka source.
      # TODO(ljchen): Remove hard code of ddr files under vendor/amlogic.
      if [ "$board" == "a049-p0" ]; then 
        cp fip/${soc_family_name}/aml_ddr.fw \
          ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/aml_ddr.fw.$board
      else
        cp fip/${soc_family_name}/aml_ddr.fw \
          ${workspace_path}/vendor/amlogic/${product}/prebuilt/bootloader/
      fi

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

shift
if [ -n "$workspace_path" ]; then
  shift
fi
for arg in "$@"
do
  case $arg in
    -d)
      shift
      ;;
    release)
      dbg_flag="release"
      shift
      ;;
    zircon)
      zircon_cfg="_zircon"
      shift
      ;;
    -o)
      shift
      if [ -n "$1" ]; then
        prebuilt_path=$1
        shift
      else
        echo "Error: -o option requires an argument."
        exit 1
      fi
      ;;
  esac
done

export ENABLE_UBOOT_UPDATE=1
export ENABLE_UBOOT_CLI=1
export ENABLE_PRODUCTION_MODE=0

case $board in
  spencer-p1)
    building_uboot c2 c2_spencer p1 $board $dbg_flag
    ;;
  spencer-p2)
    building_uboot c2 c2_spencer p2 $board $dbg_flag
    ;;
  spencer-b1|spencer-b3)
    export ENABLE_UBOOT_CLI=0
    building_uboot c2 c2_spencer bx $board $dbg_flag
    ;;
  spencer-b4)
    export ENABLE_UBOOT_UPDATE=0
    export ENABLE_UBOOT_CLI=0
    export ENABLE_PRODUCTION_MODE=1
    building_uboot c2 c2_spencer bx $board $dbg_flag
    ;;
  venus-p1)
    building_uboot c2 c2_venus p1 $board $dbg_flag
    ;;
  venus-p2)
    building_uboot c2 c2_venus p2 $board $dbg_flag
    ;;
  venus-b1|venus-b3)
    building_uboot c2 c2_venus bx $board $dbg_flag
    ;;
  venus-b4)
    export ENABLE_UBOOT_UPDATE=0
    export ENABLE_UBOOT_CLI=0
    export ENABLE_PRODUCTION_MODE=1
    building_uboot c2 c2_venus bx $board $dbg_flag
    ;;
  a049-p0)
    export ENABLE_UBOOT_CLI=0
    building_uboot c2 c2_a049 p0 $board $dbg_flag
    ;;
  bsv3-p1)
    building_uboot c2 c2_bsv3 p1 $board $dbg_flag
    ;;
  bla4-p0)
    building_uboot c2 c2_bla4 p0 $board $dbg_flag
    ;;
  bla4-p1)
    building_uboot c2 c2_bla4 p1 $board $dbg_flag
    ;;
  bla4-p2)
    building_uboot c2 c2_bla4 p2 $board $dbg_flag
    ;;
  bla4-b1)
    building_uboot c2 c2_bla4 bx $board $dbg_flag
    ;;
  2rs4-p1)
    building_uboot c2 c2_2rs4 p1 $board $dbg_flag
    ;;
  2rs4-p2)
    building_uboot c2 c2_2rs4 p2 $board $dbg_flag
    ;;
  xua4-p1)
    building_uboot c2 c2_xua4 p1 $board $dbg_flag
    ;;
  xua4-b1)
    building_uboot c2 c2_xua4 bx $board $dbg_flag
    ;;
  *)
    echo "unknown board: $board"
    exit 1
esac
popd
