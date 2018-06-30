.. _theCore_STM32:

STM32
-----

The STM32 is a family of 32-bit Flash microcontrollers based on the
ARM Cortex-M processor. theCore supports two families of STM32: STM32F4 and STM32L1

.. important:: The platform is configured by theCore configurator.
    To make sure you are familiar with it, check the :ref:`theCore_Configurator`
    section before going any further.

.. important:: Currently, only ``STM32F407VG`` device is supported for STM32 platform.

To import all generated definitions into the application code, simply add following
line to your source:

.. code-block:: cpp

    #include <aux/generated.hpp>

For more JSON configuration examples for STM32 platform, refer to
the :ref:`theCore_Examples` page.

Following sections provide in-depth description of peripheral configuration.
Arm yourself with the Reference Manual from ST before going any further.

.. _STM32 RM:

.. hint:: Hereinafter **RM** stands for *Reference Manual* from MCU vendor.
    Note that some STM32 families has more than one RM per family.
    Pick appropriate, based on your concrete MCU model.
    Below are links to RMs for each supported family.

    * `STM32F4 RM`_
..    * `STM32L1 RM`_

Available examples
~~~~~~~~~~~~~~~~~~

* :ref:`theCore_blinky`
* :ref:`theCore_hello_world`
* :ref:`theCore_stm32f4_cs43l22`
* :ref:`theCore_HM-10_example`
* :ref:`theCore_htu21d_example`

Periphery overview
~~~~~~~~~~~~~~~~~~

STM32 MCU peripheries, can be configured trough theCore configurator.

Note that to use any of periphery, corresponding pins must be configured too.
**Do not forget to include pin multiplexing configuration for each desired periphery.**
Proceed to the `STM32 Multiplexing`_ section for more details.

Timer allocation
~~~~~~~~~~~~~~~~

:Driver sources:    ``platform/stm32/export/aux/execution.hpp``

The system timer is a periphery that can provide fixed frequency ticks for
whole application. The system timer can be configured from the
"Systimer" submenu.

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

U(S)ART
~~~~~~~

:Driver source:     ``platform/stm32/export/aux/usart_bus.hpp``
:Template file:     ``platform/stm32/templates/uart_cfg.in.hpp``

The UART configuration resides in the "U(S)ART" submenu.

The instance of the driver, generated during the configuration step, can
be used directly by its API or indirectly, as underlying device for theCore
console.

Check :ref:`the console configuration section<STM32 Console>` to get
information about selecting particular UART as a console output.

Available options
+++++++++++++++++

:channel:

  Placed under "Enabled UART channel" selector. UART periphery to use.

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

* Only 1150200 and 9600 bauds are supported.
* Following configuration is hard-coded and cannot be changed (yet):

  * Stop bits: 1
  * Data length: 8 bits
  * Parity: none

* STM32 theCore UART supports only IRQ mode, where interrupt is generated after
  each byte transmission. DMA mode is not yet implemented.

Usage
+++++

.. note:: This section is under construction

ADC and channels
~~~~~~~~~~~~~~~~

.. .. warning:: ADC was tested only with STM32F4 family.

:Driver sources:    ``platform/stm32/export/aux/adc.hpp``
                    ``platform/stm32/family/f4xx/export/stm32f4xx_adc_wrap.hpp``
:Template file:     ``platform/stm32/templates/adc_cfg.in.hpp``

ADC configuration split onto two entities. First is configuration of the ADC
itself, second is the configuration for particular channels. In such way,
it is possible to have different set of ADC channels used with the same ADC.

ADC options are available from "ADC" menu. ADC channel options can be found in
"ADC channels" menu.

Available ADC options
+++++++++++++++++++++

:module:

  ADC module to use.

:mode:

  ADC mode of operation. Can be set to ``IRQ`` or ``DMA``. In IRQ mode, each
  ADC conversion will result in IRQ event generated. In DMA mode, all conversions
  are held by DMA and only last event is generated.

  IRQ is easier to use. DMA is faster, but requires deeper configuration.

:DMA module:

  The DMA module used for getting ADC data. Available only if mode is set to DMA.
  See section 10.3.3 Channel selection of `STM32 RM`_ to get insights of DMA
  module mapping.

:DMA stream:

  The DMA stream used for getting ADC data. Available only if mode is set to DMA.
  See section 10.3.3 Channel selection of `STM32 RM`_ to get insights of DMA
  stream mapping.

:DMA channel:

  The DMA stream used for getting ADC data. Available only if mode is set to DMA.
  See section 10.3.3 Channel selection of `STM32 RM`_ to get insights of DMA
  channel mapping.

:alias:

  Driver C++ alias that will be created. Alias can be used in the user code
  to access given ADC.

:comment:

  C++ comment string that will be placed next to the driver alias in
  auto-generated code.

Known limitations
+++++++++++++++++

* Continuous mode is not implemented. See `#200`_.

Available ADC channel options
+++++++++++++++++++++++++++++

:channel group name:

  The ADC channel group is a named array of channels. theCore configurator
  allows to add or delete a group of channels. To configure a new group, first
  create an empty group.

:channels:

  Selection of channels, included in the group. All 16 available channels
  can be grouped in user-defined way. See `STM32 RM`_ for a mapping between
  channels and pins.

:trigger:

  Trigger for ADC conversion:

  * ``software`` - ADC conversion started only when software request it.

Known limitations
+++++++++++++++++

* Only software trigger is implemented. For other triggers, see `#199`_.

Usage
+++++

.. note:: This section is under construction

I2C
~~~

:Driver source:     ``platform/stm32/export/aux/i2c_bus.hpp``
:Template file:     ``platform/stm32/templates/i2c_cfg.in.hpp``

Available options
+++++++++++++++++

:channel:

  Placed under "Enabled I2C channel" selector. I2C periphery to use.

:mode:

  Mode of operation. IRQ is only mode that is supported.

:speed:

  I2C clock speed, in Hz.

:duty cycle:

  I2C duty cycle.

:ack:

  Enables or disables the I2C acknowledgement.

:ack address bits:

  Specifies if 7-bit or 10-bit address is acknowledged.

:own address:

  Specifies the first device own address. This parameter can be a 7-bit or 10-bit address.

:alias:

  Driver C++ alias that will be created. Alias can be used in the user code
  to access given I2C.

:comment:

  C++ comment string that will be placed next to the driver alias in
  auto-generated code.

Usage
+++++

.. note:: This section is under construction

I2S and SPI
~~~~~~~~~~~

:Driver source:     ``platform/stm32/export/aux/spi_i2s_bus.hpp``
:Template file:     ``platform/stm32/templates/spi_i2s_cfg.in.hpp``

In STM32F4 the I2S is multiplexed with SPI. Pay attention to not use
the same periphery for both SPI and I2S. The configuration is located
under "SPI and I2S" menu.

Available options for I2S
+++++++++++++++++++++++++

:channel:

  I2S periphery to use.

:standard:

  Specifies the standard used for the I2S communication. Refer to
  :ref:`Reference Manual <STM32 RM>` to get explanation about each. Values are:

  * ``phillips``
  * ``MSB``
  * ``LSB``
  * ``PCMShort``
  * ``PCMLong``

:master clock:

  Specifies whether the I2S MCLK output is enabled or not.

:data bits:

  Specifies the data format for the I2S communication.

:audio frequency:

  Specifies the frequency selected for the I2S communication, in kHz.

:clock polarity:

  Specifies the idle state of the I2S clock.

:DMA module:

  The DMA module used for transferring I2S data.
  See section 10.3.3 Channel selection of `STM32 RM`_ to get insights of DMA
  module mapping.

:DMA stream:

  The DMA stream used for transferring I2S data.
  See section 10.3.3 Channel selection of `STM32 RM`_ to get insights of DMA
  stream mapping.

:DMA channel:

  The DMA stream used for transferring I2S data.
  See section 10.3.3 Channel selection of `STM32 RM`_ to get insights of DMA
  channel mapping.

:alias:

  Driver C++ alias that will be created. Alias can be used in the user code
  to access given I2S.

:comment:

  C++ comment string that will be placed next to the driver alias in
  auto-generated code.

Available options for SPI
+++++++++++++++++++++++++

:channel:

  The SPI channel to enable.

:type:

  SPI type. Only ``master`` is supported.

:CPOL:

  SPI clock polarity.

:CPHA:

  SPI clock phase.

Known limitations
+++++++++++++++++

* theCore SPI driver for STM32 can work only in master mode.
* theCore generator is not yet supports SPI, see `#284`_.

Usage
+++++

.. note:: This section is under construction

.. _STM32 Multiplexing:

Pin multiplexing
~~~~~~~~~~~~~~~~

:Driver sources:    ``platform/stm32/export/platform/gpio_device.hpp``

Pins can be configured from the "I/O pin configuration" submenu.

Available options
+++++++++++++++++

:channel:

  Channel is an actual pin that should be configured.

:mode:

  Pin modes:

  * ``output``
  * ``input``
  * ``af`` - AF stands for Alternate Function. Selecting AF allows to use the pin
    for desired periphery. List of supported peripheries is different for each
    pin. See below.
  * ``analog`` - in analog mode, pin can be used as input for ADC.

:push/pull:

  Possible push/pull options are:

  * ``no`` - no push/pull resistor.
  * ``push down`` - push to VCC resistor.
  * ``pull up`` - pull to GND resistor.
  * ``open drain`` - open drain configuration.
  * ``analog`` - analog input.
  * ``wake high`` and ``wake low`` - pin configuration for wakeup MCU functionality.

:type:

  * ``push-pull`` - push/pull configuration enabled.
  * ``open drain`` - open drain configuration.

:speed:

  Pin clock in MHz.

:GPIO alias:

  Driver C++ alias that will be created for accessing pin trough GPIO interface.
  Such alias can be used in the user code for controlling pin states.

:comment:

  C++ comment string that will be placed next to the driver alias in
  auto-generated code.

:alternate function:

  The alternate function, used for this pin. Available if the pin mode is set to
  ``af``.

EXTI
~~~~

.. note:: This section is under construction

Miscellaneous configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. _STM32 Console:

Console
+++++++

.. note:: This section is under construction. For now, all examples in the
          :ref:`theCore_Examples` section leverage the console. Use them as
          a guidance when enabling console for your application.

.. _STM32F4 RM: https://goo.gl/Xn1DRB
.. _STM32L1 RM: https://goo.gl/sML2mi
.. _`#199`: https://github.com/forGGe/theCore/issues/199
.. _`#200`: https://github.com/forGGe/theCore/issues/200
.. _`#284`: https://github.com/forGGe/theCore/issues/284
