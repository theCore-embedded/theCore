.. _theCore_blinky:

Simple LED blink example
------------------------

:Location:          https://github.com/theCore-embedded/example_blinky
:External HW:       none

This example flashes available LEDs on supported boards, one by one.

Supported targets (boards)
~~~~~~~~~~~~~~~~~~~~~~~~~~

+---------------------+--------------------------+-----------------------------------+
|     Target name     |    Configuration file    |            Description            |
+=====================+==========================+===================================+
| stm32f4_disc        | stm32f4_discovery.json   | STM32F4 discovery board           |
+---------------------+--------------------------+-----------------------------------+
| tiva_tm4c_launchpad | tiva_tm4c_launchpad.json | TM4C123G LaunchPad Evaluation Kit |
+---------------------+--------------------------+-----------------------------------+

Wiring
~~~~~~

No special wiring is required. Just connect the board to the powered USB port.

Preparing
~~~~~~~~~

#. Install theCore (this may take a while)::

        pip3 install tcore
        tcore bootstrap

#. Download this example::

        tcore init --remote https://github.com/theCore-embedded/example_blinky

#. Step into the project directory::

        cd example_blinky

Building
~~~~~~~~

* For STM32 Discovery board::

        tcore compile --target stm32f4_disc

* For Tiva TM4C LaunchPad::

        tcore compile --target tiva_tm4c_launchpad

Running
~~~~~~~

#. Connect your board.

#. Run:

   * For TivaC launchpad::

        tcore --sudo flash

   * For old STM32F407G-DISC boards, with STLINK/V2::

        tcore --sudo flash

   * For new STM32F407G-DISC1 boards, with STLINK/V2.1::

        tcore --sudo flash --debugger-config stlink-v2.1

Expected output
~~~~~~~~~~~~~~~

On-board LEDs will blink with the different (RGB) colors.
