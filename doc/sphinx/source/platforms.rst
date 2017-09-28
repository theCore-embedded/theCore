.. _theCore_Platforms:

Supported platforms
===================

STM32
-----

The STM32 is a family of 32-bit Flash microcontrollers based on the
ARM® Cortex®-M processor. theCore supports two families of STM32: STM32F4 and STM32L1

.. important:: The platform is configured by theCore configurator.
    To make sure you are familiar with it, check the :ref:`theCore_Configurator`
    section before going any further.

Minimal configuration JSON file for the STM32 MCU must contain the platform
object with following fields:

  * ``name`` - platform name (simply ``stm32``)
  * ``device`` - desired device ID. Depends on MCU.
  * ``clock`` - the clock configuration:

    * ``source`` - the clock source. Depends on board capabilities and MCU.
    * ``speed`` - the clock speed in Hz.

.. important:: Currently, only ``HSE`` clock source is supported.

For example, STM32F4 Discovery board the basic configuration can look like that:

.. code-block:: json
   :linenos:

   {
        "platform": {
            "name": "stm32",
            "device": "STM32F407VG",
            "clock": {
                "source": "HSE",
                "speed": 8000000
            }
        }
    }

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
    * `STM32L1 RM`_

Periphery overview
~~~~~~~~~~~~~~~~~~

STM32 MCU peripheries, can be configured within the same JSON file.
Layout of the platform configuration object is described by JSON schema,
placed under ``platform/stm32/schemas/stm32.schema.json``:

.. literalinclude:: ../../../platform/stm32/schemas/stm32.schema.json
    :language: json
    :linenos:

Each periphery configuration placed under a property with a relevant name.
The example below illustrates UART periphery being added to JSON config:

.. code-block:: json
   :emphasize-lines: 9-15
   :linenos:

    {
        "platform": {
            "name": "stm32",
            "device": "STM32F407VG",
            "clock": {
                "source": "HSE",
                "speed": 8000000
            },
            "uart": [
                {
                    "id": "USART3",
                    "baud": 115200,
                    "alias": "test_uart"
                }
            ]
        }
    }

Note that to use any of periphery, corresponding pins must be configured too.
**Do not forget to include pin multiplexing configuration for each desired periphery.**
Proceed to the `STM32 Multiplexing`_ for more details.

U(S)ART
~~~~~~~

:Driver source:     ``platform/stm32/export/aux/usart_bus.hpp``
:Template file:     ``platform/stm32/templates/uart_cfg.in.hpp``

The instance of the driver, generated during the configuration step, can
be used directly by its API or indirectly, as underlying device for theCore
console.

Check :ref:`the console configuration section<STM32 Console>` to get
information about selecting particular UART as a console output.

STM32 theCore UART supports only IRQ mode, where interrupt is generated after
each byte transmission. DMA mode is not yet implemented.

Schema:

.. literalinclude:: ../../../platform/stm32/schemas/uart.schema.json
    :language: json
    :linenos:

Properties:

* ``id`` - UART periphery ID, named after identifier found in :ref:`RM <STM32 RM>`.
  Examples are: ``UART1`` or ``USART4``. Depends on MCU family.

* ``baud`` - desired baud rate of the selected UART.

* ``alias`` - user-defined typename, which can be used as a reference to an
  concrete UART driver instance, both in configuration JSON and C++.
  Must be valid C++ identifier. See :ref:`the UART example<STM32 UART example>`
  below for more details.

* ``comment`` - user-defined string, describes driver purpose. Must be valid
  string, suitable for C/C++ comments.

.. _STM32 UART example:

Example configuration:

.. literalinclude:: platform/stm32/uart_example.json
    :language: json
    :linenos:

Example output for STM32F4 family (user-defined aliases are highlighted):

.. literalinclude:: _static/generated/stm32/uart_example.hpp
    :language: cpp
    :linenos:
    :lines: 10-49,57
    :emphasize-lines: 22,39

`Full STM32 UART example header <_static/generated/stm32/uart_example.hpp>`_

Usage:

.. note:: This section is under construction

ADC and channels
~~~~~~~~~~~~~~~~~~~~

.. warning:: ADC was tested only with STM32F4 family.

:Driver sources:    ``platform/stm32/export/aux/adc.hpp``
                    ``platform/stm32/family/f4xx/export/stm32f4xx_adc_wrap.hpp``
:Template file:     ``platform/stm32/templates/adc_cfg.in.hpp``

ADC configuration split onto two entities. First is configuration of the ADC
itself, second is the configuration for particular channels. In such way,
it is possible to have different set of ADC channels used with the same ADC.

ADC schema:

.. literalinclude:: ../../../platform/stm32/schemas/adc.schema.json
    :language: json
    :linenos:

Properties:

* ``id`` - ADC periphery ID, named after identifier found in :ref:`RM <STM32 RM>`.
  Examples are: ``ADC1`` or ``ADC2``.
  Depends on MCU family. See :ref:`RM <STM32 RM>` to get valid ID.

* ``mode`` - desired mode of operation. Can be either ``DMA`` or ``IRQ``.
  Mode affects how conversions are managed. In DMA mode, conversions are
  handled by DMA and interrupt is generated when conversion is finished
  or when half of channels are converted.
  In IRQ mode, each channel conversion produces interrupt.

* ``alias`` - user-defined typename, which can be used as a reference to an
  concrete UART driver instance, both in configuration JSON and C++.
  Must be valid C++ identifier. See :ref:`the configuration example<STM32 UART example>`
  below for more details.

* ``comment`` - user-defined string, describes driver purpose. Must be valid
  string, suitable for C/C++ comments.

Channels schema:

.. literalinclude:: ../../../platform/stm32/schemas/adc_channels.schema.json
    :language: json
    :linenos:

* ``trigger`` - trigger to start conversion on selected channels:

  * ``SW`` - software trigger. Only trigger supported at this moment.

* ``nums`` - array with channel numbers. Refer to :ref:`RM <STM32 RM>` to get
  possible channels and their mapping.

* ``alias`` - user-defined typename, which can be used as a reference to an
  channel set in C++. Must be valid C++ identifier.

* ``comment`` - user-defined string, describes driver purpose. Must be valid
  string, suitable for C/C++ comments.

Example configuration (IRQ mode):

.. literalinclude:: platform/stm32/adc_example.json
    :language: json
    :linenos:

Example output for STM32F4 family:

.. literalinclude:: _static/generated/stm32/adc_example.hpp
    :language: cpp
    :linenos:
    :lines: 9-37

`Full STM32 ADC example header <_static/generated/stm32/adc_example.hpp>`_

Usage:

.. note:: This section is under construction

I2C
~~~

:Driver source:     ``platform/stm32/export/aux/i2c_bus.hpp``
:Template file:     ``platform/stm32/templates/i2c_cfg.in.hpp``

JSON schema:

.. literalinclude:: ../../../platform/stm32/schemas/i2c.schema.json
    :language: json
    :linenos:

Properties:

* ``id`` - I2C periphery ID, named after identifier found in :ref:`RM <STM32 RM>`.
  Examples are: ``I2C1`` or ``I2C2``.
  Depends on MCU family. See :ref:`RM <STM32 RM>` to get valid ID.

* ``mode`` - data transfer mode. Only ``IRQ`` is supported. In IRQ mode, each
  byte transfer results in interrupt generation. The software driver responds
  to such interrupt with requesting next byte transmission.

* ``speed`` - specifies the clock frequency in Hz. Refer to the :ref:`RM <STM32 RM>`
  to check clock speed limitations.

* ``duty_cycle`` - duty cycle of selected I2C bus
  (ratio of T\ :sub:`low` \/T\ :sub:`high` \ ). Can be either ``2/1`` or ``16/9``.

* ``ack`` - enables or disables the acknowledgement.

* ``ack_addr_bit`` - specifies if 7-bit or 10-bit address is acknowledged.

* ``own_addr`` - specifies the first device own address.

* ``alias`` - user-defined typename, which can be used as a reference to I2C
  driver instance in C++. Must be valid C++ identifier.

* ``comment`` - user-defined string, describes a driver purpose. Must be valid
  string, suitable for C/C++ comments.

Example configuration:

.. literalinclude:: platform/stm32/i2c_example.json
    :language: json
    :linenos:

Example output:

.. literalinclude:: _static/generated/stm32/i2c_example.hpp
    :language: cpp
    :linenos:
    :lines: 9-30

`Full STM32 I2C example header <_static/generated/stm32/i2c_example.hpp>`_

Usage:

.. note:: This section is under construction

I2S
~~~

JSON schema:

.. literalinclude:: ../../../platform/stm32/schemas/i2s.schema.json
    :language: json
    :linenos:

Properties:

* ``id`` - I2S periphery ID, named after identifier found in :ref:`RM <STM32 RM>`.
  Examples are: ``I2S`` or ``I2S``.
  Depends on MCU family. See :ref:`RM <STM32 RM>` to get valid ID.

* ``dma_tx`` - DMA id to use for TX transactions. See :ref:`the configuration example <STM32 I2S example>`
  and the :ref:`DMA<STM32 DMA>` section for more details.

* ``dma_rx`` - DMA id to use for RX transactions. See :ref:`the configuration example <STM32 I2S example>`
  and the :ref:`DMA<STM32 DMA>` section for more details.

* ``standart`` - specifies the standard used for the I2S communication. Allowed values are:
  ``phillips``, ``MSB``, ``LSB``, ``PCMShort``, ``PCMLong``. Refer to :ref:`Reference Manual <STM32 RM>`
  to get explanation about each.

* ``data_bits`` - specifies the data format for the I2S communication.

* ``master_clk`` - specifies whether the I2S MCLK output is enabled or not.

* ``audio_freq`` - specifies the frequency selected for the I2S communication.

* ``CPOL`` - specifies the idle state of the I2S clock.

* ``alias`` - user-defined typename, which can be used as a reference to I2S
  driver instance in C++. Must be valid C++ identifier.

* ``comment`` - user-defined string, describes a driver purpose. Must be valid
  string, suitable for C/C++ comments.

.. _STM32 I2S example:

Example configuration:

.. literalinclude:: platform/stm32/i2s_example.json
    :language: json
    :linenos:
    :emphasize-lines: 8,14,20,21

DMA aliases and their usage in I2S config are highlighted.

Example output (**DMA configuration header is omitted for clarity**):

.. literalinclude:: _static/generated/stm32/i2s_example.hpp
    :language: cpp
    :linenos:
    :lines: 17-42

`Full STM32 I2S example header <_static/generated/stm32/i2s_example.hpp>`_

Usage:

.. note:: This section is under construction

Related references:

* :ref:`theCore_stm32f4_cs43l22`
* :ref:`DMA documentation <STM32 DMA>`
* :ref:`theCore_CS43L22`

SPI
~~~

.. note:: This section is under construction

.. _STM32 DMA:

DMA
~~~

:Driver sources:    ``platform/stm32/family/export/stm32_dma_wrap_interface.hpp``
                    ``platform/stm32/family/f4xx/export/stm32_dma_wrap.hpp``
:Template file:     ``platform/stm32/family/f4xx/templates/dma_cfg.in.hpp``

Direct memory access (DMA) is used in order to provide high-speed data transfer between
peripherals and memory and between memory and memory. Data can be quickly moved by
DMA without any CPU action. This keeps CPU resources free for other operations.

.. warning:: DMA configuration via JSON is supported only for STM32F4 family.

In most cases, DMA is used internally by the platform-level drivers.
Platform drivers, like I2C or I2S can be linked with particular DMA by using
its alias.

Each driver with DMA support will include corresponding DMA example.

Configuration parameters (stream, channel, etc.) heavily depends on MCU model.
Refer to the :ref:`RM <STM32 RM>` for allowed DMA configuration for selected periphery.

* STM32F4 MCU family

  Schema:

  .. literalinclude:: ../../../platform/stm32/family/f4xx/schemas/dma.schema.json
      :language: json
      :linenos:

  Properties:

  * ``num`` - DMA number. There are 2 DMAs on STM32F4. There are two DMA controllers in STM32F4.
    Refer to the section *10. DMA controller (DMA)* of the :ref:`RM <STM32 RM>`
    to get more information.

  * ``stream`` - DMA stream number. There are 8 streams for each DMA controller,
    numbered from 0 to 7. See section *10.3.5 DMA streams* in the :ref:`RM <STM32 RM>`
    for more details.

  * ``channel`` - DMA channel number. There are up to 8 channels (requests) per
    each DMA stream, numbered from 0 to 7.
    See section *10.3.3 Channel selection* in the :ref:`RM <STM32 RM>` for more details.

  * ``alias`` - user-defined typename, which can be used as a reference to an
    concrete DMA driver instance in configuration JSON.
    Must be valid C++ identifier. Refer to individual periphery documentation
    for examples.

  * ``comment`` - user-defined string, describes DMA purpose. Must be valid
    string, suitable for C/C++ comments.

  Example configuration:

  .. literalinclude:: platform/stm32/f4xx/dma_example.json
      :language: json
      :linenos:

  Example output:

  .. literalinclude:: _static/generated/stm32/f4xx/dma_example.hpp
      :language: cpp
      :linenos:
      :lines: 8-18

  `Full STM32F4 DMA example header <_static/generated/stm32/f4xx/dma_example.hpp>`_

* STM32L1 MCU family

  .. note:: This section is under construction.

Usage:

.. note:: This section is under construction

.. _STM32 Multiplexing:

Pin multiplexing
~~~~~~~~~~~~~~~~

.. note:: This section is under construction

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

Kinetis KE02Z
-------------

.. note:: This section is under construction.

Particle Electron
-----------------

.. note:: This section is under construction.

Tiva C series TM4C123G
----------------------

.. note:: This section is under construction.

.. _STM32F4 RM: https://goo.gl/Xn1DRB
.. _STM32L1 RM: https://goo.gl/sML2mi
