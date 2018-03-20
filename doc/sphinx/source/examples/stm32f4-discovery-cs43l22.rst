.. _theCore_stm32f4_cs43l22:

STM32F4 discovery audio example with CS43L22 audio DAC
------------------------------------------------------

:Location:          ``examples/cs43l22_audio``
:Target:            STM32F4 Discovery board
:External HW:       UART-to-USB converter attached to the USART3, headphones
:Toolchain:         GNU arm-none-eabi v.5.2 or newer
:Additional SW:     openocd, minicom

Wiring
~~~~~~

.. image:: https://i.imgur.com/fhWxQ4Z.jpg
   :alt: CS43L22 stm32f4discovery wiring

#. Attach any preferable UART-to-USB converter `such as this`_ module according to following pinout:

   +-------------------+-----------------+
   | PD8 (USART3 TX)   | module's RX     |
   +-------------------+-----------------+
   | PD9 (USART3 RX)   | module's TX     |
   +-------------------+-----------------+
   | GND               | module's GND    |
   +-------------------+-----------------+

Building
~~~~~~~~

#. Complete :ref:`theCore_examples_initial_setup` section.
#. Execute build commands.
   The CMake Toolchain file is required to build this application.
   theCore already has one suitable for this target.

   ::

     cd examples/cs43l22_audio
     mkdir build
     cd build
     cmake -DCMAKE_TOOLCHAIN_FILE=../../../toolchains/arm-cm4-gnu.cmake ..
     make

Running
~~~~~~~

Firmware will be flashed via ``openocd`` and ``gdb``.

#. Connect stm32f4 Discovery board to USB cable and connect USB <-> UART converter to the PC.

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

     sudo $(which openocd) -f board/stm32f4discovery.cfg -c 'init; reset halt; flash write_image erase audio.bin 0x08000000; reset run; exit'

   For new STM32F407G-DISC1 boards, with STLINK/V2.1:

   ::

     sudo $(which openocd) -f board/stm32f429disc1.cfg -c 'init; reset halt; flash write_image erase audio.bin 0x08000000; reset run; exit'

   See :ref:`theCore_SudoOpenOCD_Nix` section to get insight why ``which openocd``
   is important.

   .. note::

     Optionally, complete the :ref:`theCore_OpenOCD_NoRoot` guide, to be able
     run OpenOCD without superuser permissions.

#. Attach headphones to the audio jack on Discovery board.

Expected output
~~~~~~~~~~~~~~~

In ``minicom`` you should be able to see::

  Welcome to theCore
  Playing audio sample...


In headphones you should hear cool 8-bit sound.

Target JSON config for the audio example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../examples/cs43l22_audio/stm32f4_cs43l22.json


.. _such as this: http://www.geekfactory.mx/wp-content/uploads/2013/06/converdidor_usb_ttl_rs232_pl_2303hx_01.jpg
