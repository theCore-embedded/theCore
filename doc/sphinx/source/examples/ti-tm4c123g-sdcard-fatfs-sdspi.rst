.. _theCore_TM4C_FATFS_SDSPI:

TI TM4C123G SD-card demo using FATFS and SDSPI
----------------------------------------------

:Location:          ``examples/tm4c_fatfs``
:Target:            Tiva C Series TM4C123G LaunchPadEvaluation Board
:External HW:       `Catalex micro-SD card adapter/module`_
:Toolchain:         GNU arm-none-eabi v.5.2 or newer
:Additional SW:     UniFlash, minicom, OpenOCD

The example shows how to use :ref:`theCore_FAT_filesystem` over
:ref:`theCore_SDSPI` on :ref:`TM4C123G microcontroller<theCore_TIVAC_TM4C123G>`.

Hardware in question is `a regular SD card`_ connected to
`the Catalex micro-SD adapter`_ and `TM4C123G LaunchPad™ Evaluation Kit`_.

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

Building
~~~~~~~~

#. Complete :ref:`theCore_examples_initial_setup` section.

#. Execute build commands.
   The CMake Toolchain file is required to build this application.
   theCore already has one suitable for this target.

   ::

       mkdir build
       cd build
       cmake -DCMAKE_TOOLCHAIN_FILE=../../toolchains/arm-cm4-gnu.cmake ..
       make

Running
~~~~~~~

#. Flash firmware either by using UniFlash_ or OpenOCD.
   The UniFlash interface is straightforward.

   For OpenOCD, execute::

     sudo $(which openocd) -f board/ek-lm4f120xl.cfg -c 'init; reset halt; flash write_image erase tm4c_fatfs.bin 0x0; reset run; exit'

#. Launch ``minicom`` with device associated with the board.
   (``/dev/ttyACM0`` here used as an example)::

     # From new terminal
     minicom -D /dev/ttyACM0

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

Target JSON config for the TM4C FATFS example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For your reference, the JSON configuration for this example is displayed below:

.. literalinclude:: ../../../../examples/tm4c_led/tm4c_led.json

.. _UniFlash: http://processors.wiki.ti.com/index.php/Category:CCS_UniFlash

.. _`Catalex`: http://www.aessmart.com/product/673/a531-micro-sd-card-module-adaptercatalex
.. _`Catalex micro-SD card adapter/module`: Catalex_
.. _`the Catalex micro-SD adapter`: Catalex_
.. _`a regular SD card`: http://bit.ly/2HU5yr7
.. _`TM4C123G LaunchPad™ Evaluation Kit`: http://www.ti.com/tool/EK-TM4C123GXL