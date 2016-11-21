# The Core _CS43L22 audio_ example

 ```

 target:            stm32f4 Discovery board
 external HW:       UART-to-USB converter attached to the USART3, headphones
 toolchain:         GNU arm-none-eabi v.5.2 or greather
 additional SW:     openocd
 ```

## Wiring

 - Headphones attached to the audio jack.

 - Attach any preferable UART-to-USB converter ([such as this](http://www.geekfactory.mx/wp-content/uploads/2013/06/converdidor_usb_ttl_rs232_pl_2303hx_01.jpg))
module according to following pinout:
   ```
   PD9 (USART RX) - module's TX
   PD8 (USART TX) - module's RX
   ```

Do not forget ground.

## Building

Toolchain is required to build this application. The Core already has one suitable
for this target.

```
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../../toolchains/arm-cm4-gnu.cmake ..
```

## Running

Flash firmware via `openocd` and `gdb`.

1. Attach stm32f4 Discovery board and USB <-> UART converter to the PC.
1. Launch openocd in separate terminal using script provided by The Core. Alternatively, you can use script from `openocd` installation.

    ```
    openocd -f ~/path/to/the/core/scripts/stm32f4discovery.cfg
    ```
1. Launch `minicom` with device associated with USB <-> UART converer
    (`ttyUSB0` here used as an example).

    ```
    minicom -D /dev/ttyUSB0
    ```

1. Launch `gdb` from build directory.

    ```
    arm-none-eabi-gdb audio

    # ... GDB will produce some output

    ```
1. Under GDB shell, connect to `openocd` and flash firmware.

    ```
    (gdb) target remote :3333
    Remote debugging using :3333
    0x00000000 in ?? ()

    (gdb) monitor reset halt
    target state: halted
    target halted due to debug-request, current mode: Thread
    xPSR: 0x01000000 pc: 0x08000188 msp: 0x20020000

    (gdb) load
    Loading section .text, size 0x4724 lma 0x8000000
    Loading section .init_array, size 0xc lma 0x8004724
    Loading section .rodata, size 0x228 lma 0x8004730
    Loading section .data, size 0x24 lma 0x8004958
    Start address 0x8000188, load size 18812
    Transfer rate: 14 KB/sec, 3762 bytes/write.

    ```

1. Start new firmware.

    ```
    (gdb) continue
    Continuing.

    ```

1. Attach headphones to the audio jack on Discovery board.

## Expected output

In console:
```
Welcome to theCore
Playing audio sample...
```

In headphones you should hear cool 8-bit sound.


