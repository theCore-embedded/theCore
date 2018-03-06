.. _theCore_SDSPI:

SDSPI driver
------------

.. image:: https://pre00.deviantart.net/39c5/th/pre/f/2010/225/8/5/micro_sd_card_up_by_tash78.png
  :alt: SD card with SD SPI driver
  :height: 450
  :width: 450

(`Image by tash78 @ devianart`_)

theCore SDPSI driver allows to communicate with SD cards using platforms that
support SPI.

Specification
~~~~~~~~~~~~~

Simplified specification can be downloaded for free from `SD card association website`_.

Source code
~~~~~~~~~~~

SDSPI driver implementation is located under ``dev/sdspi/`` directory, relative to
theCore root.

Driver documentation
~~~~~~~~~~~~~~~~~~~~

:doxy_url:`Click here to open SDSPI Doxygen docs<group__sdspi.html>`.

SDSPI usage example
~~~~~~~~~~~~~~~~~~~

.. note:: This section is under construction.

Known limitations
~~~~~~~~~~~~~~~~~

SDSPI driver does not support SPI dynamic clock changing after initialization.
SDSPI driver, by definition, works only in SPI mode, not SDIO.

Verified hardware
~~~~~~~~~~~~~~~~~

SDSPI driver is tested with `Catalex micro-SD card adapter/module`_ and
`high-capacity, class 10 micro SD card`_. For the sake of fair testing,
:ref:`theCore_FAT_filesystem` were used on top of SDSPI driver.

Additional references
~~~~~~~~~~~~~~~~~~~~~

* `SD card association main page`_
* `Speed classes of SD cards`_
* `ELEM-chan article on using MMC/SDC in SPI mode`_

.. _`Image by tash78 @ devianart`: https://tash78.deviantart.com/art/Micro-SD-Card-UP-175156797
.. _`Catalex micro-SD card adapter/module`: http://www.aessmart.com/product/673/a531-micro-sd-card-module-adaptercatalex
.. _`high-capacity, class 10 micro SD card`: http://bit.ly/2HU5yr7
.. _`SD card association website`: https://www.sdcard.org/downloads/pls/

.. _`SD card association main page`: https://www.sdcard.org/
.. _`Speed classes of SD cards`: https://www.sdcard.org/developers/overview/speed_class/
.. _`ELEM-chan article on using MMC/SDC in SPI mode`: http://elm-chan.org/docs/mmc/mmc_e.html