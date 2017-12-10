Particle Electron
-----------------

Particle electron is a tiny development kit for creating 3G cellular
connected products.

Particle electron uses its own SDK. theCore provides wrappers for I2C and Serial
peripherals.

Particle electron platform can be configured via JSON. Configuration file
must conform to a schema below:

.. literalinclude:: ../../../../platform/particle_electron/schemas/electron.schema.json
  :language: json
  :linenos:

Following sections provide configuration reference for each periphery.

Serial
~~~~~~

Serial is used for communication between the Electron and a computer or
other devices. The Electron has four hardware (USART) serial channels and
two USB serial channels.

theCore allows to enable particular Serial by its number on default baud rate.
Desired serial number must put into array with ``serial`` name.

Example configuration
+++++++++++++++++++++

.. literalinclude:: particle_electron/serial_example.json
    :language: json
    :linenos:

Example output
++++++++++++++

.. literalinclude:: ../_static/generated/particle_electron/serial_example.hpp
    :language: cpp
    :linenos:
    :lines: 12-20,35

`Full Particle Electron Serial example header <../_static/generated/particle_electron/serial_example.hpp>`_

Console
+++++++

To enable console in theCore, set ``use_console`` flag and enable ``Serial0``
via JSON:

.. code-block:: json
    :linenos:

    {
        "platform": {
            "name":         "particle_electron",
            "serial":       [ 0 ],
            "console":      true,
        }
    }

Check the :ref:`theCore_Console` section for more details about theCore console
library.

Related references
++++++++++++++++++

* `Official Serial documentation <https://docs.particle.io/reference/firmware/electron/#serial>`_

Wire (I2C)
~~~~~~~~~~

.. note:: theCore allows to configure only one Wire/I2C instance with a pinout:

   * ``SCL`` => ``D1``
   * ``SDA`` => ``D0``

Example configuration
+++++++++++++++++++++

.. literalinclude:: particle_electron/i2c_example.json
    :language: json
    :linenos:

Example output
++++++++++++++

.. literalinclude:: ../_static/generated/particle_electron/i2c_example.hpp
    :language: cpp
    :linenos:
    :lines: 12-27

`Full Particle Electron I2C/Wire example header <../_static/generated/particle_electron/i2c_example.hpp>`_

Properties
++++++++++

* ``speed`` - `I2C clock speed`_. Required field.
* ``stretch_clk`` - `I2C clock stretching`_.

Timer allocation
~~~~~~~~~~~~~~~~

To save power and improve performance, theCore uses one of Particle timers internally.
Find more details of Timers in the `Particle Electron documentation for software timers`_

Flashing firmware
~~~~~~~~~~~~~~~~~

To flash resulting binary to the board, follow next sequence:

* Put device into the `DFU mode`_
* Launch ``scripts/electron_dfu.sh`` script with a path to a binary as an argument.

.. _`DFU mode`: https://docs.particle.io/guide/getting-started/modes/electron/#dfu-mode-device-firmware-upgrade-
.. _`I2C clock speed`: https://docs.particle.io/reference/firmware/electron/#setspeed-
.. _`I2C clock stretching`: https://docs.particle.io/reference/firmware/electron/#stretchclock-
.. _`Particle Electron documentation for software timers`: https://docs.particle.io/reference/firmware/electron/#software-timers
