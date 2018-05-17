.. _theCore_hello_world:

Simple host hello world example
-------------------------------

:Location:          https://github.com/theCore-embedded/example_hello_world
:External HW:       UART-to-USB converter for STM32F4Discovery board

Supported targets (boards)
~~~~~~~~~~~~~~~~~~~~~~~~~~

+---------------------+--------------------------+-----------------------------------+
| Target name         | Configuration file       | Description                       |
+=====================+==========================+===================================+
| host                | host.json                | Host target build                 |
+---------------------+--------------------------+-----------------------------------+
| stm32f4_disc        | stm32f4_discovery.json   | STM32F4 discovery board           |
+---------------------+--------------------------+-----------------------------------+
| tiva_tm4c_launchpad | tiva_tm4c_launchpad.json | TM4C123G LaunchPad Evaluation Kit |
+---------------------+--------------------------+-----------------------------------+

Wiring
~~~~~~

* If you have Tiva TM4C Launchpad - simply connect it to the USB.

* If you have STM32F4 Discovery board:

  #. Attach any preferable UART-to-USB
     converter module (`such as this`_) according to following pinout:

     +-------------------+-----------------+
     | PD8 (USART3 TX)   | module's RX     |
     +-------------------+-----------------+
     | PD9 (USART3 RX)   | module's TX     |
     +-------------------+-----------------+
     | GND               | module's GND    |
     +-------------------+-----------------+

  #. Connect your STM32 Discovery board to the PC.

Preparing
~~~~~~~~~

#. Install and initialize theCore (if not done previously)::

    pip3 install tcore
    tcore bootstrap

#. Download the example::

    tcore init --remote https://github.com/theCore-embedded/example_hello_world

#. Step into the project directory::

    cd example_hello_world

Building
~~~~~~~~

* For STM32 Discovery board::

    tcore compile --target stm32f4_disc

* For Tiva TM4C LaunchPad::

    tcore compile --target tiva_tm4c_launchpad

* For host::

    tcore compile --target host

Running
~~~~~~~

#. If you wish to run the example on the embedded device, launch `minicom`
   with device associated with USB <-> UART converter.
   (``/dev/ttyUSB0`` here used as an example)::

        # From new terminal
        tcore runenv "minicom -D /dev/ttyUSB0"

   Or the same, but with superuser permissions::

        # From new terminal
        tcore runenv --sudo "minicom -D /dev/ttyUSB0"

#. Run:

   * On TivaC launchpad::

        tcore --sudo flash

   * For old STM32F407G-DISC boards, with STLINK/V2::

        tcore --sudo flash

   * For new STM32F407G-DISC1 boards, with STLINK/V2.1::

        tcore --sudo flash --debugger-config stlink-v2.1

   * For host target, execute::

        ./build/host/hello_world

Expected output
~~~~~~~~~~~~~~~

Observe console output (either in current shell if running on host, or using
   minicom if running on the embedded device)::

        Welcome to theCore
        the_core v0.3.0.307 9ff344b-dirty
        Hello World!

        Starting delay demonstration...

        [--] Waiting for 2 seconds
        [--] Done waiting 2 seconds

        [----] Waiting for 4 seconds
        [----] Done waiting 4 seconds

        [--------] Waiting for 8 seconds
        [--------] Done waiting 8 seconds

.. _such as this: http://www.geekfactory.mx/wp-content/uploads/2013/06/converdidor_usb_ttl_rs232_pl_2303hx_01.jpg
