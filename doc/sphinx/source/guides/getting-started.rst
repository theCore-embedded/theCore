.. _theCore_GettingStarted:

Getting started
---------------

Downloading theCore
~~~~~~~~~~~~~~~~~~~

First thing first you need to download theCore and setup development environment.
To do that, use dedicated `tcore` script, written in Python 3:

#. Install Python 3 and dependencies, by using `DigitalOcean guide`_.

#. Install ``tcore`` script::

     sudo pip3 install tcore

#. Install ``curl``. For example, in Ubuntu, run::

     sudo apt-get install curl

#. Download theCore and development environment. Choose one option from below:

   * With script asking root permissions (see
     `Details of the development environment`_ section to understand why)::

       tcore bootstrap

   * With script **not asking** any root permissions::

       sudo mkdir /nix
       sudo chown $(whoami) /nix
       tcore bootstrap

Details of the development environment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All additional packages, like toolchains, build dependencies and some
additional host utilities are manager trough `Nix`_. Nix helps
to deploy everything that you need during development of the embedded application.

The installiation process of Nix is curated by the ``tcore`` script.
More information about it you can find in the :ref:`theCore_tcore` section.

Nix is installed under `/nix` directory. If it does not exist, Nix installation
script will ask for permissions. That's why running bare `tcore bootstrap`
may result in a sudo password request.

Some examples of packages, contaned in ``/nix`` directory are:

* Git
* CMake
* toolchains: GCC, Clang
* OpenOCD
* GDB

Check the ``default.nix`` file in the root of theCore project for full list
of packages requested from Nix.

After Nix is deployed, theCore itself is downloaded into ``~/.theCore`` directory.
``tcore`` script uses Git under Nix environment to retrieve theCore from a remote,
so no need to install Git into the host machine.

Moving forward
~~~~~~~~~~~~~~

From now on, you are ready for creating embedded applications using theCore.
Check :ref:`theCore_Examples` section to understand how such applications
are built.

More info about Nix
~~~~~~~~~~~~~~~~~~~

If you want to get yourself more familiar with Nix, check out `the Domenkozar's blog`_
or read the `official documentation`_.

.. _`DigitalOcean guide`: https://www.digitalocean.com/community/tutorials/how-to-install-python-3-and-set-up-a-local-programming-environment-on-ubuntu-16-04
.. _Nix: https://nixos.org/nix/
.. _`the Domenkozar's blog`: https://www.domenkozar.com/2014/01/02/getting-started-with-nix-package-manager/
.. _`official documentation`: https://nixos.org/nix/manual/
