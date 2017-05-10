# The Core _HTU21D temperature and humidity sensor_ example

 ```
 target:            stm32f4 Discovery board
 external HW:       UART-to-USB converter attached to the USART2, HTU21D sensor
 toolchain:         GNU arm-none-eabi v.5.2 or greather
 additional SW:     openocd
 ```

## Wiring

 - [HTU21D](https://cdn-shop.adafruit.com/datasheets/1899_HTU21D.pdf) sensor connected to I2C1.
   ```
   PB6 (I2C SCL) - sensor's SCL
   PB9 (I2C SDA) - sensor's SDA
   +3.3V - sensor's VDD
   GND - sensor's GND
   ```

 - Attach any preferable UART-to-USB converter ([such as this](http://www.geekfactory.mx/wp-content/uploads/2013/06/converdidor_usb_ttl_rs232_pl_2303hx_01.jpg))
module according to following pinout:
   ```
   PD5 (USART RX) - module's TX
   PD6 (USART TX) - module's RX
   ```

Do not forget ground.

## Building

Toolchain is required to build this application. The Core already has one suitable for this target.

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
    arm-none-eabi-gdb htu21d_example

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
    Loading section .text, size 0x7678 lma 0x8000000
    Loading section .data, size 0x3c lma 0x8007678
    Start address 0x800477c, load size 30388
    Transfer rate: 18 KB/sec, 10129 bytes/write.
    ```

1. Start new firmware.

    ```
    (gdb) continue
    Continuing.
    ```

1. Watch console output.

## Expected output

In console:
```
Welcome to theCore
Starting HTU21D sensor...
Reset done
Temperature: 21.722 Celsius   Humidity: 40.882%
Temperature: 21.712 Celsius   Humidity: 40.859%
Temperature: 21.701 Celsius   Humidity: 40.852%
...
```
