.. _theCore_stm32f4_cs43l22:

STM32F4 discovery audio example with CS43L22 audio DAC
------------------------------------------------------

:Location:          https://github.com/theCore-embedded/example_cs43l22_audio
:External HW:       UART-to-USB converter attached to the USART3, headphones

Supported targets (boards)
~~~~~~~~~~~~~~~~~~~~~~~~~~

+---------------------+--------------------------+-----------------------------------+
|     Target name     |    Configuration file    |            Description            |
+=====================+==========================+===================================+
| stm32f4_disc        | stm32f4_discovery.json   | STM32F4 discovery board           |
+---------------------+--------------------------+-----------------------------------+

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

Preparing
~~~~~~~~~

#. Install and initialize theCore (if not done previously)::

      pip3 install tcore
      # Or if python3-pip is default: pip install tcore
      tcore bootstrap


#. Download the example::

      tcore init --remote https://github.com/theCore-embedded/example_cs43l22_audio
      cd example_cs43l22_audio

Building
~~~~~~~~

Run ``compile`` command using theCore CLI::

  tcore compile --target stm32f4_disc

Running
~~~~~~~

Firmware will be flashed via ``openocd`` debugger and ``flash`` command.

#. Connect stm32f4 Discovery board to USB cable and connect USB <-> UART converter to the PC.

#. Launch ``minicom`` with device associated with USB <-> UART converter.
   (``/dev/ttyUSB0`` here used as an example)::

      # From new terminal
      tcore runenv "minicom -D /dev/ttyUSB0"

   Or the same, but with superuser permissions::

      # From new terminal
      tcore runenv --sudo "minicom -D /dev/ttyUSB0"

#. Determine stm32f4discovery board revision.

   If you don't remember your board revision, check FAQ section
   :ref:`theCore_STM32F4_Discovery_rev`.

#. Launch ``flash`` command in separate terminal, as shown below.

   For old STM32F407G-DISC boards, with STLINK/V2:

   ::

     tcore flash --sudo


   For new STM32F407G-DISC1 boards, with STLINK/V2.1:

   ::

     tcore flash --sudo --debugger-config stlink-v2.1

#. Attach headphones to the audio jack on Discovery board.

#. Wear your headphones and enjoy.

Expected output
~~~~~~~~~~~~~~~

In ``minicom`` you should be able to see::

  Welcome to theCore
  Playing audio sample...

In headphones you should hear cool 8-bit sound.

.. _such as this: http://www.geekfactory.mx/wp-content/uploads/2013/06/converdidor_usb_ttl_rs232_pl_2303hx_01.jpg
