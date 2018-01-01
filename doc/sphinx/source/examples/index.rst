.. _theCore_Examples:

Examples
========

.. toctree::
  :maxdepth: 2
  :includehidden:

  hello-world-host
  stm32f4-discovery-htu21d-sensor
  stm32f4-discovery-cs43l22
  stm32f4-discovery-exti
  stm32f4-discovery-blink
  stm32f4-discovery-bluetooth-hm10
  ti-tm4c123g-launchpad-blink

theCore examples are organized in a form of separate projects that can be
extracted and used as a base for custom application.

Examples are of two types. First is standalone example, placed in
the separate repository for convenience. For such, in each section you will
find a reference to the project location. Second type of examples are in-source
examples, placed under ``examples`` directory within theCore source tree.

.. _theCore_examples_initial_setup:

Initial setup
-------------

Before any example can be built, an appropriate development environment must
be configured either manually by installing packages like toolchains or
by using Nix.

Using Nix is recommended. The :ref:`theCore_GettingStarted` section contains
the explanation about how to install and run Nix.

To manually install required software without Nix (not recommended),
refer to :ref:`theCore_NoNixInstall` section.
There you will find some insights about getting hand-made development environment.



