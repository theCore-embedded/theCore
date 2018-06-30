.. _theCore_TIVAC_TM4C123G:

Texas Instruments Tiva C TM4C123G
---------------------------------

.. image:: https://uge-one.com/image/cache/catalog/catalog/0-TM4C123G-500x375.jpg
   :align: center
   :width: 700px

Tiva C TM4C123G is a Cortex-M4 based microcontroller from Texas Instruments.

:Module location:
    ``platform/tm4c``
:Supported devices:
    TM4C123GH6PM
:Data sheets/Reference manuals:
    `TM4C123GH6PM datasheet`_

.. important:: The platform is configured by theCore configurator.
    To make sure you are familiar with it, check the :ref:`theCore_Configurator`
    section before going any further.

.. important:: Currently, only ``TM4C123GH6PM`` device is supported.

To import all generated definitions into the application code, simply add following
line to your source:

.. code-block:: cpp

    #include <aux/generated.hpp>

Available examples
~~~~~~~~~~~~~~~~~~

* :ref:`theCore_FATFS_SDSPI`
* :ref:`theCore_blinky`
* :ref:`theCore_hello_world`

Periphery overview
~~~~~~~~~~~~~~~~~~

TM4C MCU peripheries can be configured through theCore configurator.

Note that to use any of periphery, corresponding pins must be configured too.
**Do not forget to include pin multiplexing configuration for each desired periphery.**
Proceed to the `TM4C Multiplexing`_ section for more details.

System timer
~~~~~~~~~~~~

:Driver sources:    ``platform/tm4c/export/aux/execution.hpp``

The system timer is a periphery that can provide fixed frequency ticks for
whole application. The system timer can be configured from the
"Systimer configuration" submenu.

Available options
+++++++++++++++++

:frequency:

  Timer frequency in Hz.

:source:

  Timer source. Only ``systick`` is available for now.

:owner:

  Timer owner. If ``user`` is selected, then theCore will configure the timer,
  but will not use it inside its internal machinery. In such case, the user can
  decide when to stop or start the system timer.

  If `thecore` is selected, then the systimer will be both configured and
  managed by internal modules of theCore. For example, timer can be started
  or stopped in delay routines inside theCore. Trying to start or stop the timer
  directly by user will lead to undefined behaviour.

Known limitations
+++++++++++++++++

* Only SysTick can be used as a timer source.
* No dynamic change in frequency is allowed. This is by design.

Usage
+++++

.. note:: This section is under construction

UART
~~~~

:Driver sources:    ``platform/tm4c/export/aux/uart.hpp``

The UART configuration resides in the "UART configuration" submenu.

Available options
+++++++++++++++++

:channel:

  UART periphery to use. In TM4C, 7 UARTs are available.

:baud:

  Baud rate of UART.

:alias:

  Driver C++ alias that will be created. Alias can be used in the user code
  to access given UART.

:comment:

  C++ comment string that will be placed next to the driver alias in
  auto-generated code.

Known limitations
+++++++++++++++++

* Only 115200 baud is supported.
* Following configuration is hard-coded and cannot be changed (yet):

  * Stop bits: 1
  * Data length: 8 bits
  * Parity: none

Console
+++++++

To enable console in TM4C platform, change the ``console`` option field to
desired UART channel. The channel must be first enabled via UART menu.

Check the :ref:`theCore_Console` section for more details about theCore console
library.

Usage
+++++

.. note:: This section is under construction

SSI / SPI
~~~~~~~~~

:Driver sources:    ``platform/tm4c/export/aux/spi.hpp``

The SSI stands for Serial Synchronous Interface. In TM4C MCUs it is analogous
to SPI. The SSI/SPI configuration is placed under the "SSI (SPI) config" submenu.

Available options
+++++++++++++++++

:channel:

  The SPI channel to enable.

:type:

  SPI type. Only ``master`` is supported.

:CPOL:

  SPI clock polarity.

:CPHA:

  SPI clock phase.

:System clock divider for SPI:

  The SPI clock is configured trough divider of the system clock.
  Say, the system clock is 100000 Hz. Setting divider to 4 will configure
  SPI clock to value of 25000 Hz.

Known limitations
+++++++++++++++++

* theCore SPI driver for TM4C can work only in master mode.
  See `issue #361`_.
* SPI clock is not yet configurable in the driver and it is set to a
  fraction of the system clock: ``spi_clock = system_clock / 4``.
  See `issue #360`_.
* Only Motorola SPI modes are supported, though datasheet lists more than that.
  See `issue #362`_.

.. _TM4C Multiplexing:

Pin multiplexing
~~~~~~~~~~~~~~~~

:Driver sources:    ``platform/tm4c/export/platform/pin_cfg.hpp``
                    ``platform/tm4c/export/platform/gpio_device.hpp``

Pins can be configured from the "I/O pin configuration" submenu.

Available options
+++++++++++++++++

:channel:

  Channel is an actual pin that should be configured.

:direction:

  Direction of pin - ``output``, ``input`` or ``af``.
  ``af`` stands for Alternate Function. Selecting AF allows to use the pin
  for desired periphery. List of supported peripheries is different for each
  pin. See below.

:type:

  Possible pin types are:

  * ``standard`` - no push/pull resistor.
  * ``push`` - push to VCC resistor.
  * ``pull`` - pull to GND resistor.
  * ``open drain`` - open drain configuration.
  * ``analog`` - analog input.
  * ``wake high`` and ``wake low`` - pin configuration for wakeup MCU functionality.

:alias:

  Comma-separated driver C++ aliases that will be created.
  Each alias can be used in the user code to access given pin.

:comment:

  C++ comment string that will be placed next to the driver alias in
  auto-generated code.

Usage
+++++

.. note:: This section is under construction

External interrupts
~~~~~~~~~~~~~~~~~~~

.. note:: This section is under construction

.. _`TM4C123GH6PM datasheet`: http://www.ti.com/lit/ds/spms376e/spms376e.pdf
.. _`issue #362`: https://github.com/forGGe/theCore/issues/362
.. _`issue #361`: https://github.com/forGGe/theCore/issues/361
.. _`issue #360`: https://github.com/forGGe/theCore/issues/360
