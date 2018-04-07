.. _theCore_STM32F4_HM-10_example:

STM32F4 discovery and HM-10 Bluetooth LE (4.0) wireless module
--------------------------------------------------------------

:Location:          ``examples/hm10``
:Target:            STM32F4 Discovery board
:External HW:       UART-to-USB converter attached to the USART2, HM-10 module, Android device
:Toolchain:         GNU arm-none-eabi v.5.2 or newer
:Additional SW:     openocd, minicom, `BLE Terminal`_ Android app


This example demonstrates using of :ref:`theCore_HM-10` using theCore.
With Bluetooth application on Android device it is possible to establish
communication link between the smartphone and any embedded device with
HM-10 attached.

It this particular example, STM32F4 Discovery board is used.

Wiring
~~~~~~

.. image:: https://i.imgur.com/ZlNgnD5.jpg
  :alt: UART, HM-10 and stm32f4discovery board wiring

#. Connect USART2 and power to HM-10_  on the Discovery board using following pins:

   +-------------------+-----------------+
   | PA2 (USART2 TX)   | module's RX     |
   +-------------------+-----------------+
   | PA3 (USART2 RX)   | module's TX     |
   +-------------------+-----------------+
   | +3.3V             | module's VDD    |
   +-------------------+-----------------+
   | GND               | module's GND    |
   +-------------------+-----------------+

#. Attach any preferable UART-to-USB converter `such as this`_ module according to following pinout:

   +-------------------+-----------------+
   | PD8 (USART3 TX)   | module's RX     |
   +-------------------+-----------------+
   | PD9 (USART3 RX)   | module's TX     |
   +-------------------+-----------------+
   | GND               | module's GND    |
   +-------------------+-----------------+

Installing additional software on Android smartphone
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. image:: https://i.imgur.com/ag4DVY3.jpg
  :alt: BLE Terminal download app

To be able to communicate with HM-10 module via Android smartphone, a special
application must be installed.

Open Play Store and find `BLE Terminal`_ app. Install it as usual.

Building
~~~~~~~~

#. Complete :ref:`theCore_examples_initial_setup` section.
#. Execute build commands.
   The CMake Toolchain file is required to build this application.
   theCore already has one suitable for this target.

   ::

     cd examples/hm10
     mkdir build
     cd build
     cmake -DCMAKE_TOOLCHAIN_FILE=../../../toolchains/arm-cm4-gnu.cmake ..
     make

Running
~~~~~~~

Firmware will be flashed via ``openocd`` and ``gdb``.

#. Connect stm32f4 Discovery board to USB cable and connect USB <-> UART
   converter to the PC.

#. Launch ``minicom`` with device associated with USB <-> UART converter.
   (``/dev/ttyUSB0`` here used as an example)::

     # From new terminal
     minicom -D /dev/ttyUSB0

#. Determine stm32f4discovery board revision.

   If you don't remember your board revision, check FAQ section
   :ref:`theCore_STM32F4_Discovery_rev`.

#. Launch ``openocd`` and flash image using script from openocd installation.

   For old STM32F407G-DISC boards, with STLINK/V2:

   ::

     sudo $(which openocd) -f board/stm32f4discovery.cfg -c 'init; reset halt; flash write_image erase hm10_example.bin 0x08000000; reset run; exit'

   For new STM32F407G-DISC1 boards, with STLINK/V2.1:

   ::

     sudo $(which openocd) -f board/stm32f429disc1.cfg -c 'init; reset halt; flash write_image erase hm10_example.bin 0x08000000; reset run; exit'

   See :ref:`theCore_SudoOpenOCD_Nix` section to get insight why ``which openocd``
   is important.

   .. note::

     Optionally, complete the :ref:`theCore_OpenOCD_NoRoot` guide, to be able
     run OpenOCD without superuser permissions.

#. Check ``minicom`` terminal. You should be able to see::

    Welcome to theCore
    the_core v0.3.236 b9c05be-dirty
    Starting HM-10 Bluetooth example...
    Bytes sent: 16
    Bytes received: 0
    Bytes sent: 16
    Bytes received: 0
    Bytes sent: 16
    Bytes received: 0

#. Open `BLE Terminal`_ on your smartphone. You must be able to see nearby BT devices:

   .. image:: https://i.imgur.com/YT1Ggju.png
    :alt: BLE Terminal app main window

#. Select HM-10 device.

#. You will be presented with a terminal window where the data sent from device is displayed

#. Now, type something to the terminal prompt in BLE Terminal app and tap "Send ASCII" button:

   .. image:: https://i.imgur.com/68eWX1n.png
    :alt: BLE Terminal and HM-10 output window

#. Check ``minicom`` terminal again::

    Bytes received: 0
    Bytes sent: 16
    Bytes received: 16
    data in return

Target JSON config for the Bluetooth example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../examples/hm10/stm32f4_hm10.json

.. _such as this: http://www.geekfactory.mx/wp-content/uploads/2013/06/converdidor_usb_ttl_rs232_pl_2303hx_01.jpg
.. _HM-10: https://cdn-shop.adafruit.com/datasheets/1899_HTU21D.pdf
.. _`BLE Terminal`: https://play.google.com/store/apps/details?id=com.mightyit.gops.bleterminal&hl=en
