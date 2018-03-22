Frequently Asked Questions
==========================

How to fix ``libusb_open() failed with LIBUSB_ERROR_ACCESS`` when running OpenOCD?
----------------------------------------------------------------------------------

If you seeing a log like this:

.. code-block:: text
   :emphasize-lines: 13

   Open On-Chip Debugger 0.10.0
   Licensed under GNU GPL v2
   For bug reports, read
     http://openocd.org/doc/doxygen/bugs.html
   Info : The selected transport took over low-level target control. The results might differ compared to plain JTAG/SWD
   adapter speed: 2000 kHz
   adapter_nsrst_delay: 100
   none separate
   srst_only separate srst_nogate srst_open_drain connect_deassert_srst
   Info : Unable to match requested speed 2000 kHz, using 1800 kHz
   Info : Unable to match requested speed 2000 kHz, using 1800 kHz
   Info : clock speed 1800 kHz
   Error: libusb_open() failed with LIBUSB_ERROR_ACCESS
   Error: open failed
   in procedure 'init'
   in procedure 'ocd_bouncer'

Solution is to either re-run OpenOCD with ``sudo``, as described in
:ref:`theCore_SudoOpenOCD_Nix` section, or configure ``udev`` to let OpenOCD connect
without requiring root privileges, using :ref:`theCore_OpenOCD_NoRoot` guide.

I get plain ``Error: open failed`` when running OpenOCD with my STM32F4 Discovery board. What should I do?
----------------------------------------------------------------------------------------------------------

Most likely, you are running wrong OpenOCD script. Log in that case may look
like this:

.. code-block:: text
   :emphasize-lines: 14

   $ openocd -f board/stm32f4discovery.cfg
   Open On-Chip Debugger 0.9.0 (2018-01-24-01:05)
   Licensed under GNU GPL v2
   For bug reports, read
   http://openocd.org/doc/doxygen/bugs.html
   Info : The selected transport took over low-level target control. The results might differ compared to plain JTAG/SWD
   adapter speed: 2000 kHz
   adapter_nsrst_delay: 100
   none separate
   srst_only separate srst_nogate srst_open_drain connect_deassert_srst
   Info : Unable to match requested speed 2000 kHz, using 1800 kHz
   Info : Unable to match requested speed 2000 kHz, using 1800 kHz
   Info : clock speed 1800 kHz
   Error: open failed
   in procedure 'init'
   in procedure 'ocd_bouncer'

Solution is to run appropriate OpenOCD script:

#. Determine the board revision using :ref:`theCore_STM32F4_Discovery_rev` section.
#. Run correct script.

   For old STM32F407G boards, with STLINK/V2::

     openocd -f board/stm32f4discovery.cfg

   For new STM32F407G-DISC1 boards, with STLINK/V2.1::

     openocd -f board/stm32f429disc1.cfg

.. _theCore_SudoOpenOCD_Nix:

How to run OpenOCD with root privileges under Nix shell?
--------------------------------------------------------

Nix modifies the ``PATH`` environment variable. But running ``sudo``,
drops ``PATH`` environment variable, so you need a full path to
executable, installed by Nix.

Thus, instead of running::

   sudo openocd [openocd_arguments]

You should run::

   sudo $(which openocd) [openocd_arguments]

How to run arbitrary command with root privileges under Nix shell?
------------------------------------------------------------------

Approach is similar, as in :ref:`theCore_SudoOpenOCD_Nix` section.
Suppose you want to run command called ``foo``, you need to execute::

   sudo $(which foo) [foo_arguments]

How to run OpenOCD without root privileges?
-------------------------------------------

Complete the :ref:`theCore_OpenOCD_NoRoot` guide.

.. _theCore_STM32F4_Discovery_rev:

How to check STM32F4 Discovery board revision?
----------------------------------------------

Run ``lsusb`` in your console.

For old Discovery board with ST-LINK/V2, you should see::

    Bus 002 Device 125: ID 0483:3748 STMicroelectronics ST-LINK/V2

For new Discovery board with ST-LINK/V2-A (labeled as STM32F407G-DISC1)::

    Bus 001 Device 002: ID 0483:374b STMicroelectronics ST-LINK/V2.1 (Nucleo-F103RB)



