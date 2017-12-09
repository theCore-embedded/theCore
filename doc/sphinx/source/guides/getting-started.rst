.. _theCore_GettingStarted:

Getting started
---------------

Downloading theCore
~~~~~~~~~~~~~~~~~~~

First thing first you need to clone and update all modules of theCore.

#. Clone repository::

     git clone https://github.com/forGGe/theCore.git

#. Update all submodules::

     git submodule update --init --recursive

Deploying development environment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Now it is time to get the development environment - toolchains,
build dependencies and some additional host utilities. Everything that
you need during development of the embedded application.

You can either do it manually and spend decent time on that or ask
package manager called Nix_ to setup all the development packages for you.

If you prefer to manually install dependencies, refer to :ref:`theCore_NoNixInstall`
section for more details.

Otherwise start with following.

#. Install Nix.
   On Linux and Mac OS X, the easiest way to install Nix is to run
   the following shell command (as a user other than root)::

     curl https://nixos.org/nix/install | sh

#. Run Nix shell to get all environment required::

     # Run from theCore directory
     nix-shell --pure

From now on, you are ready for creating embedded applications using theCore.

More info about Nix
~~~~~~~~~~~~~~~~~~~

If you want to get yourself more familiar with Nix, check out `the Domenkozar's blog`_
or read the `official documentation`_.


.. _Nix: https://nixos.org/nix/
.. _`the Domenkozar's blog`: https://www.domenkozar.com/2014/01/02/getting-started-with-nix-package-manager/
.. _`official documentation`: https://nixos.org/nix/manual/
