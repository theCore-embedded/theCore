External interrupts from user button
------------------------------------

:Location:          https://github.com/theCore-embedded/example_button_interrupt
:External HW:       UART-to-USB converter

EXTI - the external interrupt.

  An external interrupt is a computer system interrupt that happens as a result
  of outside interference, whether that’s from the user, from peripherals,
  from other hardware devices or through a network. These are different
  than internal interrupts that happen automatically as the machine
  reads through program instructions.

  -- Techopedia_

In this application, one particular type of external interrupt is used:
GPIO line interrupt. When voltage level changes on the line (e.g. when button is
pressed), interrupt is generated.

.. STARTOF COMMON SECTION MARKER

Supported targets (boards)
~~~~~~~~~~~~~~~~~~~~~~~~~~

+---------------------+--------------------------+-----------------------------------+
| Target name         | Configuration file       | Description                       |
+=====================+==========================+===================================+
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

     .. image:: https://i.imgur.com/dRVRHV2.jpg
        :alt: UART wiring for stm32f4discovery EXTI example


  #. Connect your STM32 Discovery board to the PC.

Preparing
~~~~~~~~~

#. Install and initialize theCore (if not done previously)::

    pip3 install tcore
    tcore bootstrap

#. Download the example::

    tcore init --remote https://github.com/theCore-embedded/example_button_interrupt

#. Step into the project directory::

    cd example_button_interrupt

Building
~~~~~~~~

* For STM32 Discovery board::

    tcore compile --target stm32f4_disc

* For Tiva TM4C LaunchPad::

    tcore compile --target tiva_tm4c_launchpad

Running
~~~~~~~

#. Launch ``minicom`` with device associated with USB <-> UART converter.
   (``/dev/ttyUSB0`` here used as an example)::

        # From new terminal
        tcore runenv "minicom -D /dev/ttyUSB0"

   Or the same, but with superuser permissions::

        # From new terminal
        tcore runenv --sudo "minicom -D /dev/ttyUSB0"

#. Run:

   * On TivaC launchpad::

        tcore flash --sudo

   * For old STM32F407G-DISC boards, with STLINK/V2::

        tcore flash --sudo

   * For new STM32F407G-DISC1 boards, with STLINK/V2.1::

        tcore flash --sudo --debugger-config stlink-v2.1

Expected output
~~~~~~~~~~~~~~~

Observe console output using ``minicom`` if running on the embedded device)::

        Welcome to theCore
        the_core v0.3.0.307 9ff344b-dirty
        Starting EXTI (button interrupt) demo

Every time you press a button (SW1 on TivaC board, USR_BTN on STM32F4 Discovery),
following output will be displayed::

        Button pressed!

.. _such as this: http://www.geekfactory.mx/wp-content/uploads/2013/06/converdidor_usb_ttl_rs232_pl_2303hx_01.jpg
.. _Techopedia: https://www.techopedia.com/definition/7115/external-interrupt

.. ENDOF COMMON SECTION MARKER