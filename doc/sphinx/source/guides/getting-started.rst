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

You can obtain the environment using few ways:

* By `using Nix`_.
* By `using Docker`_.
* Install everything manually. See :ref:`theCore_NoNixInstall` section for more
  details on that.

You can either do it manually and spend decent time on that or ask
package manager called Nix_ to setup all the development packages for you.

If you prefer to manually install dependencies, refer to :ref:`theCore_NoNixInstall`
section for more details.

Using Docker
============

#. Install docker.
   Installation depends on your Linux distributions, but here are some insights:

   * `Docker installation on Ubuntu 16.04`_
   * `Docker installation on ArchLinux`_

#. After Docker is installed and Docker daemon is running, you can launch
   theCore environment image. Execute::

     # Pulls Docker image from dockerhub
     # https://hub.docker.com/r/thecoreembedded/thecore/
     docker pull thecoreembedded/thecore

     # Run shell in the docker with all environment deployed
     docker run -v $(pwd):/root/thecore -w /root/thecore -it thecoreembedded/thecore

Using Nix
=========

#. Install Nix.
   On Linux and Mac OS X, the easiest way to install Nix is to run
   the following shell command (as a user other than root)::

     curl https://nixos.org/nix/install | sh

#. Run Nix shell to get all environment required::

     # Run from theCore directory
     nix-shell .

Moving forward
~~~~~~~~~~~~~~

From now on, you are ready for creating embedded applications using theCore.
Check :ref:`theCore_Examples` section to understand how such applications
are built.

More info about Nix
~~~~~~~~~~~~~~~~~~~

If you want to get yourself more familiar with Nix, check out `the Domenkozar's blog`_
or read the `official documentation`_.


.. _Nix: https://nixos.org/nix/
.. _`the Domenkozar's blog`: https://www.domenkozar.com/2014/01/02/getting-started-with-nix-package-manager/
.. _`official documentation`: https://nixos.org/nix/manual/
.. _`Docker installation on Ubuntu 16.04`: https://www.digitalocean.com/community/tutorials/how-to-install-and-use-docker-on-ubuntu-16-04
.. _`Docker installation on ArchLinux`: https://wiki.archlinux.org/index.php/Docker
