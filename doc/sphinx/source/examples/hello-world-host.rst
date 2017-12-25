.. _theCore_host_hello:

Simple host hello world example
-------------------------------

:Location:          ``examples/host_hello``
:Target:            host
:External HW:       none
:Toolchain:         gcc/g++ v5.2 or newer
:Additional SW:     none

This example shows theCore running on the host target.
By `host` target is understood an environment capable to both compile and
run theCore applications.

Application demonstrates a console stream usage and a simple assertion.

Building
~~~~~~~~

#. Complete :ref:`theCore_examples_initial_setup` section.
#. Execute build commands::

     cd examples/host_hello
     mkdir build
     cd build
     cmake ..
     make

Running
~~~~~~~

Execute resulting application::

  ./hello

Expected output
~~~~~~~~~~~~~~~

::

  Hello, Host World!
  This is an assert demonstration. Application will abort.
  hello: main.cpp:7: int main(): Assertion `1 == 0' failed.
  Aborted (core dumped)

Target JSON for Hello World example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../examples/host_hello/host.json
