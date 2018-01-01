TI TM4C123G LaunchPad blinky
----------------------------

:Location:          ``examples/tm4c_led``
:Target:            Tiva C Series TM4C123G LaunchPadEvaluation Board
:External HW:       none
:Toolchain:         GNU arm-none-eabi v.5.2 or newer
:Additional SW:     UniFlash, minicom

Wiring
~~~~~~

.. image:: ../_images/tm4c_led.jpg
   :alt: TI TM4C123G USB connection

No special wiring is required. Just connect the board to the powered USB port.

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

#. Flash firmware via UniFlash_. The UniFlash interface is straightforward.

#. Launch ``minicom`` with device associated with the board.
   (``/dev/ttyACM0`` here used as an example)::

     # From new terminal
     minicom -D /dev/ttyACM0

Expected output
~~~~~~~~~~~~~~~

In ``minicom`` you should be able to see::

  Welcome to theCore
  Starting TM4C led demo...

Target JSON config for the TM4C LED example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../examples/tm4c_led/tm4c_led.json

On-board LED will blink with the different (RGB) colors.

.. _UniFlash: http://processors.wiki.ti.com/index.php/Category:CCS_UniFlash
