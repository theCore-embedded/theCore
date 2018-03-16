Filesystem support
------------------

.. image:: http://www.xs-labs.com/uploads/image/filesystem/icon-circle.png
    :alt: Filesystem interface

(image by `xs-labs`_)

theCore provides filesystem classes, somewhat similar to, and inspired by
the `Virtual File System pattern`_. In that way, theCore virtual filesystem
can work with any "real" filesystem.

Right now, only :ref:`theCore_FAT_filesystem` is supported as underlying
filesystem in theCore.

Source code
~~~~~~~~~~~

The main filesystem utility located under ``lib/fs/`` directory, relative to
theCore root.
The FATFS code is placed in ``lib/fs/fat/`` directory, relative to
theCore root.

Doxygen documentation
~~~~~~~~~~~~~~~~~~~~~

:doxy_url:`Click here to open filesystem Doxygen documentation<group__fs__iface.html>`.

.. _theCore_FAT_filesystem:

FAT filesystem
~~~~~~~~~~~~~~

The FATFS support is based on `Petite FAT from ELM`_. This FATFS implementation
is device-agnostic, meaning it can work above any driver that
can provide read/write/seek interface.

Compatible low-level device drivers
+++++++++++++++++++++++++++++++++++

* :ref:`theCore_SDSPI`.

FATFS usage example
+++++++++++++++++++

.. note:: This section is under construction.

Verified hardware
+++++++++++++++++

FATFS was tested along with :ref:`theCore_SDSPI` and
`Catalex micro-SD card adapter/module`_ on `high-capacity, class 10 micro SD card`_.

Known limitations
+++++++++++++++++

* theCore FATFS module can only work with FAT32 filesystems. FAT16 and FAT12
  are pending.
* FATFS module must be configured similarly for every underlying device:
  i.e. there is no way to have 2 SD cards with 2 FAT, configured one as readonly,
  an one as read-write. Both of them must be the same type.

Filesystem example configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ./fs/fatfs_example.json
    :language: cpp
    :linenos:

Example output
~~~~~~~~~~~~~~

Filesystem configuration header:

.. literalinclude:: ../_static/generated/fs/fs_cfg_example.hpp
    :lines: 23-41
    :language: cpp
    :linenos:

`Full filesystem configuration header <../_static/generated/fs/fs_cfg_example.hpp>`_

Filesystem definitions header:

.. literalinclude:: ../_static/generated/fs/fs_defines_example.h
    :lines: 13-15
    :language: cpp
    :linenos:

`Full filesystem definitions header <../_static/generated/fs/fs_defines_example.h>`_

Filesystem configuration schema
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../lib/fs/fs.schema.json
    :language: json
    :linenos:

Additional references
~~~~~~~~~~~~~~~~~~~~~

* `FATFS WiKi article`_
* `Petite FATFS mirror repo, used by theCore`_

.. _`xs-labs`: http://www.xs-labs.com/en/projects/filesystem/overview/
.. _`Virtual File System pattern`: https://en.wikipedia.org/wiki/Virtual_file_system
.. _Petite FAT from ELM: http://www.elm-chan.org/fsw/ff/00index_p.html
.. _`Catalex micro-SD card adapter/module`: http://www.aessmart.com/product/673/a531-micro-sd-card-module-adaptercatalex
.. _`high-capacity, class 10 micro SD card`: http://bit.ly/2HU5yr7

.. _`FATFS WiKi article`: https://en.wikipedia.org/wiki/File_Allocation_Table
.. _`Petite FATFS mirror repo, used by theCore`: https://github.com/forGGe/petite-fat