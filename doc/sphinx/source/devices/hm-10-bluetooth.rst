HM-10 Bluetooth BTLE module
---------------------------

.. image:: https://cdn.itead.cc/media/catalog/product/h/m/hm_10_03.jpg
  :alt: HM-10 Bluetooth Module BTLE

HM-10 is a BLE module for embedded system to get BLE wireless communication
with BLE capable devices (e.g. iPhone and iPad).
It is fully configurable by a rich and well documented AT command-set and
allows transparent data communication via serial UART (default baudrate 9600bps).

Datasheet
~~~~~~~~~

Datasheet v524 can be found at `fab.cba.mit.edu website`_

Source code
~~~~~~~~~~~

HM-10 driver implementation is located under ``dev/hm10/`` directory, relative to
theCore root.

Driver documentation
~~~~~~~~~~~~~~~~~~~~

`Click here to open HM-10 Doxygen docs`_.

HM-10 usage example
~~~~~~~~~~~~~~~~~~~

Refer to :ref:`theCore_STM32F4_HM-10_example` page for usage example.

Known limitations
~~~~~~~~~~~~~~~~~

theCore HM-10 driver operates only in slave mode.
For master mode, see `"Implement HM-10 master mode" GitHub issue`_.

References
~~~~~~~~~~

* `Martyn Currey's "HM-10 Bluetooth 4 BLE Modules" guide`_
* `Instructable's "HOW TO USE BLUETOOTH 4.0 HM10"`_

.. _fab.cba.mit.edu website: http://fab.cba.mit.edu/classes/863.15/doc/tutorials/programming/bluetooth/bluetooth40_en.pdf
.. _`Click here to open HM-10 Doxygen docs`: ../doxygen/group__hm10.html
.. _`"Implement HM-10 master mode" GitHub issue`: https://github.com/forGGe/theCore/issues/354
.. _`Martyn Currey's "HM-10 Bluetooth 4 BLE Modules" guide`: http://www.martyncurrey.com/hm-10-bluetooth-4ble-modules/
.. _`Instructable's "HOW TO USE BLUETOOTH 4.0 HM10"`: http://www.instructables.com/id/How-to-Use-Bluetooth-40-HM10/
