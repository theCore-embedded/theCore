.. _theCore_FATFS_SDSPI:

SD-card demo using FATFS and SDSPI
----------------------------------

:Location:          https://github.com/theCore-embedded/example_fatfs
:External HW:       `Catalex micro-SD card adapter/module`_

The example shows how to use :ref:`theCore_FAT_filesystem` over
:ref:`theCore_SDSPI` in theCore framework.

Hardware in question is `a regular SD card`_ connected to `the Catalex micro-SD adapter`_.

.. STARTOF COMMON SECTION MARKER

Supported targets (boards)
~~~~~~~~~~~~~~~~~~~~~~~~~~

+---------------------+--------------------------+-----------------------------------+
| Target name         | Configuration file       | Description                       |
+=====================+==========================+===================================+
| tiva_tm4c_launchpad | tiva_tm4c_launchpad.json | TM4C123G LaunchPad Evaluation Kit |
+---------------------+--------------------------+-----------------------------------+

Wiring
~~~~~~

.. image:: https://i.imgur.com/y27nRhg.png
    :alt: TI TM4C123G Launchpad with Micro SD card adapter attached

Connect SPI3 (SSI3) and power to `the Catalex micro-SD adapter`_
on the LaunchPad board using following pins:

.. important::

   **The Catalex module must be powered from +5V source**. Pay attention to
   wiring. Otherwise, if connected to +3.3v, the card adapter may misbehave.

+-------------------+---------------------+
| PD0               | module's SPI CLK    |
+-------------------+---------------------+
| PD1               | module's SPI CS     |
+-------------------+---------------------+
| PD2               | module's SPI MISO   |
+-------------------+---------------------+
| PD3               | module's SPI MOSI   |
+-------------------+---------------------+
| Vbus (+5V)        | module's VCC        |
+-------------------+---------------------+
| GND               | module's GND        |
+-------------------+---------------------+

Preparing
~~~~~~~~~

#. Install and initialize theCore (if not done previously)::

    pip3 install tcore
    tcore bootstrap

#. Download the example::

    tcore init --remote https://github.com/theCore-embedded/example_fatfs

#. Step into the project directory::

    cd example_fatfs

Building
~~~~~~~~

::

  tcore compile --target tiva_tm4c_launchpad


Running
~~~~~~~

#. Launch `minicom` (``/dev/ttyACM0`` used here as an example)::

        # From new terminal
        tcore runenv "minicom -D /dev/ttyACM0"

   Or the same, but with superuser permissions::

        # From new terminal
        tcore runenv --sudo "minicom -D /dev/ttyACM0"

#. Flash to the board::

    tcore flash --sudo

Expected output
~~~~~~~~~~~~~~~

#. In ``minicom`` you should be able to see contents of the SD card root directory.
   For example, it can look like this::

     Welcome to theCore
     the_core v0.3.287 5045e04-dirty
     Starting FATFS example...
     #. 0 type: dir  name : HOME
     #. 1 type: dir  name : VAR
     #. 2 type: file name : TEST_FILE.TXT
     #. 3 type: file name : ANOTHER_FILE.TXT
     Which file to open?

#. Select a file to print into the console. File contents then will appear
   on the screen.

.. _`Catalex`: http://www.aessmart.com/product/673/a531-micro-sd-card-module-adaptercatalex
.. _`Catalex micro-SD card adapter/module`: Catalex_
.. _`the Catalex micro-SD adapter`: Catalex_
.. _`a regular SD card`: http://bit.ly/2HU5yr7

.. ENDOF COMMON SECTION MARKER
