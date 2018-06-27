.. _theCore_htu21d_example:

HTU21D temperature and humidity sensor example
----------------------------------------------

:Location:          https://github.com/theCore-embedded/example_sensor_htu21d
:Target:            STM32F4 Discovery board
:External HW:       UART-to-USB converter attached to the USART2, HTU21D sensor

HTU21D example shows how to read humidity and temperature samples using theCore.
For more information about what this sensor does see the :ref:`theCore_HTU21D` page.

.. STARTOF COMMON SECTION MARKER

Supported targets (boards)
~~~~~~~~~~~~~~~~~~~~~~~~~~

+---------------------+--------------------------+-----------------------------------+
| Target name         | Configuration file       | Description                       |
+=====================+==========================+===================================+
| stm32f4_disc        | stm32f4_discovery.json   | STM32F4 discovery board           |
+---------------------+--------------------------+-----------------------------------+

Wiring
~~~~~~

STM32F4 Discovery board
++++++++++++++++++++++++

.. image:: https://i.imgur.com/vX2uOb5.jpg
   :alt: UART and HTU21D example wiring for stm32f4discovery board

#. Connect HTU21D_ to I2C1 on the Discovery board using following pins:

   +-------------------+-----------------+
   | PB6 (I2C1 SCL)    | sensor's SCL    |
   +-------------------+-----------------+
   | PB9 (I2C1 SDA)    | sensor's SDA    |
   +-------------------+-----------------+
   | +3.3V             | sensor's VDD    |
   +-------------------+-----------------+
   | GND               | sensor's GND    |
   +-------------------+-----------------+

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

      tcore init --remote https://github.com/theCore-embedded/example_sensor_htu21d
      cd example_sensor_htu21d

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
   |GET_DISCOVERY_REVISION|

#. Launch ``flash`` command in separate terminal, as shown below.

   For old STM32F407G-DISC boards, with STLINK/V2:

   ::

     tcore flash --sudo


   For new STM32F407G-DISC1 boards, with STLINK/V2.1:

   ::

     tcore flash --sudo --debugger-config stlink-v2.1

Expected output
~~~~~~~~~~~~~~~

Observe console output in the minicom::

        Welcome to theCore
        the_core v0.3.0.321 90f4894-dirty
        Starting HTU21D sensor...
        Reset done
        Temperature: 25.362 Celsius   Humidity: 45.162%
        Temperature: 25.362 Celsius   Humidity: 45.170%
        Temperature: 25.362 Celsius   Humidity: 45.155%
        Temperature: 25.362 Celsius   Humidity: 45.147%
        Temperature: 25.351 Celsius   Humidity: 45.155%

.. _such as this: http://www.geekfactory.mx/wp-content/uploads/2013/06/converdidor_usb_ttl_rs232_pl_2303hx_01.jpg
.. _HTU21D: https://cdn-shop.adafruit.com/datasheets/1899_HTU21D.pdf

.. ENDOF COMMON SECTION MARKER

.. |GET_DISCOVERY_REVISION| replace:: :ref:`theCore_STM32F4_Discovery_rev`.
