#!/bin/bash

set -x
set -e

CRC_LEN=4
CRC_BLOCK_LEN=38
DEFAULT_SHA_256=0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
MOD_INFO_SUFFIX_LEN=2800
MOD_INFO_SUFFIX=${DEFAULT_SHA_256}${MOD_INFO_SUFFIX_LEN}
CRC_BLOCK_CONTENTS=${MOD_INFO_SUFFIX}78563412
IN_FILE=$1
OUT_PRECRC=${IN_FILE}.pre_crc
OUT_NOCRC=${IN_FILE}.no_crc
OUT_FILE=${IN_FILE}.dfu
OUT_CRCBLOCK=${IN_FILE}.blk_crc

arm-none-eabi-objcopy -O binary "$IN_FILE" "$OUT_PRECRC"

FLZ=$(( $(stat -c '%s' "$OUT_PRECRC" ) - $CRC_LEN))
FLZ_BLK=$(( $( stat -c '%s' "$OUT_PRECRC" ) - $CRC_BLOCK_LEN))

head -c $FLZ_BLK "$OUT_PRECRC" > "$OUT_NOCRC"
tail -c $CRC_BLOCK_LEN "$OUT_PRECRC" > "$OUT_CRCBLOCK"
test "$CRC_BLOCK_CONTENTS" = "$(xxd -p -c 500 $OUT_CRCBLOCK)"

sha256sum "$OUT_NOCRC" | cut -c 1-65 | xxd -r -p | dd bs=1 of="$OUT_PRECRC" seek=$FLZ_BLK conv=notrunc

head -c $FLZ "$OUT_PRECRC" > "$OUT_NOCRC"

crc32 "$OUT_NOCRC" | cut -c 1-10 | xxd -r -p | dd bs=1 of="$OUT_PRECRC" seek=$FLZ conv=notrunc
cp "$OUT_PRECRC" "$OUT_FILE"

dfu-suffix -v 2B04 -p D00A -a "$OUT_FILE"
dfu-util -d 0x2B04:0xD00A -a 0 -s 0x8080000:leave -D "$OUT_FILE"
