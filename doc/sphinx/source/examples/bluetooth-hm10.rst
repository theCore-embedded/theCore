.. _theCore_HM-10_example:

HM-10 Bluetooth LE (4.0) wireless module example
------------------------------------------------

:Location:          https://github.com/theCore-embedded/example_hm10_bluetooth
:External HW:       UART-to-USB converter attached to the STM32F4 Discovery,
                    HM-10 module, Android device

This example demonstrates using of :ref:`theCore_HM-10` using theCore.
With Bluetooth application on Android device it is possible to establish
communication link between the smartphone and any embedded device with
HM-10 attached.

.. STARTOF COMMON SECTION MARKER

Supported targets (boards)
--------------------------

+---------------------+--------------------------+-----------------------------------+
|     Target name     |    Configuration file    |            Description            |
+=====================+==========================+===================================+
| stm32f4_disc        | stm32f4_discovery.json   | STM32F4 discovery board           |
+---------------------+--------------------------+-----------------------------------+

Wiring
~~~~~~

STM32F4 Discovery board
+++++++++++++++++++++++

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
  :align: center
  :width: 400

To be able to communicate with HM-10 module via Android smartphone, a special
application must be installed.

Open Play Store and find `BLE Terminal`_ app. Install it as usual.

Preparing
~~~~~~~~~

#. Install and initialize theCore (if not done previously)::

    pip3 install tcore
    tcore bootstrap

#. Download the example::

    tcore init --remote https://github.com/theCore-embedded/example_hm10_bluetooth

#. Step into the project directory::

    cd example_hm10_bluetooth

Building
~~~~~~~~

::

  tcore compile --target stm32f4_disc

Running
~~~~~~~

#. Launch `minicom` with device associated with USB <-> UART converter.
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
    :align: center
    :width: 400

#. Select HM-10 device.

#. You will be presented with a terminal window where the data sent from device is displayed

#. Now, type something to the terminal prompt in BLE Terminal app and tap "Send ASCII" button:

   .. image:: https://i.imgur.com/68eWX1n.png
    :alt: BLE Terminal and HM-10 output window
    :align: center
    :width: 400

#. Check ``minicom`` terminal again::

    Bytes received: 0
    Bytes sent: 16
    Bytes received: 16
    data in return

.. _such as this: http://www.geekfactory.mx/wp-content/uploads/2013/06/converdidor_usb_ttl_rs232_pl_2303hx_01.jpg
.. _HM-10: https://cdn-shop.adafruit.com/datasheets/1899_HTU21D.pdf
.. _`BLE Terminal`: https://play.google.com/store/apps/details?id=com.mightyit.gops.bleterminal&hl=en

.. ENDOF COMMON SECTION MARKER

.. |GET_DISCOVERY_REVISION| replace:: :ref:`theCore_STM32F4_Discovery_rev`.
