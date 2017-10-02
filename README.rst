theCore: C++ Embedded Framework
===============================

|Build Status| |Join the chat at https://gitter.im/forGGe/theCore|

Description
-----------

theCore is the open source, highly configurable and straightforward C++ embedded
framework aimed to reduce development, testing and hardware migration
efforts for a wide range of bare-metal embedded applications.

theCore tries to provide all that is necessary to create portable application code:

* Startup, initialization and periphery code for each supported platform.
* Build system support for different platform and target configurations.
* Platform-independent device drivers, like temperature sensors or displays.
* Optional OS-related abstractions: semaphores, mutexes and threads.
* Easy-to-deploy development environment.
* Different libraries and utilities.

theCore uses powerful type system based on modern C++ and code generation that
greatly reduces error rate and debugging time. Shifting different sanity checks
to compile time rather than runtime not only makes code more performant, but
also secure and easier to extend.

The project is at the initial stage. You can check |Roadmap| section
in the documentation or `github issues`_ to get insights on what happens
right now and where project is moving.

Documentation
-------------

theCore documentation hosted on GitHub pages and available in two formats:
`multi-page HTML`_ for ease of navigation and `single-page HTML`_ for ease
of manual searching.

Where to get theCore
--------------------

theCore is now available only from source and hosted solely on GitHub_.
There are different branches for development and stable versions. Check |Branching|
section for information about branches purpose.

Getting started
---------------

You can start using theCore by completing guides and tutorials in the |Guides|
section or check the source code of `the standalone blinky example project`_.

It is also worth to look at examples that are placed in `examples` dir and
read the |Examples| section to understand how to build and launch them.

Supported platforms
-------------------

Supported platform list along with information about each platform is located
in |Platforms| section.

License
-------

The source code is distributed under `MPL v2.0 License`_ (Mozilla Public License
Version 2.0).

MPL is like LGPL, but with static linking exception.

For many practical cases it means that **you can use theCore in your proprietary
embedded applications without disclosing the application source code**.

Keep it private, if needed. MPL allows you to do that.

Pay note though, that MPL is still a copy-left license. So you are obligated
to distribute any changes of theCore itself if there were any.

It is strongly recommended to read `MPL 2.0 FAQ`_ to get more familiar with it.

Technologies and projects used
------------------------------

Most notable are:

* C++11 and C++14 - as a main programming language standards.
* Python 3 and cog_ - code generation.
* Doxygen_ and Sphinx_ - in-source and project-wide documentation.
* CMake_ - build-system.
* Nix_ - development environment management.
* CppUTest_ and Unity_ - unit and on-device testing.
* Travis_ - continuos integration.

Check the |Credits| section for more.

Contribution & Using
--------------------

The project is on its very beginning, so any help is more than welcome
and highly appreciated. If you’d like to take a part in the project
growth and (or) have any questions – take a look at |DeveloperDocs| section,
leave message at gitter_, contact me directly at forgge@gmail.com,
or simply check out `a list of theCore issues`_.

If you have any suggestions on theCore improvement or just
like it how it is, don’t keep silence! I’ll be happy to read your
reviews.

You are welcome to use theCore in your own projects. If there is
something that it’s not enough for this, please let me know via email or
open some issues and we will do our best to handle this ASAP. Good luck!

.. _StubLink: http://google.com

.. _GitHub: https://github.com/forGGe/theCore/
.. _github issues: https://github.com/forGGe/theCore/issues
.. _multi-page HTML: https://forgge.github.io/theCore/
.. _single-page HTML: https://forgge.github.io/theCore/singlehtml/contents.html
.. _the standalone blinky example project: https://github.com/forGGe/theCore-blinky
.. _MPL v2.0 License: https://www.mozilla.org/en-US/MPL/
.. _MPL 2.0 FAQ: https://www.mozilla.org/en-US/MPL/2.0/FAQ/
.. _cog: https://nedbatchelder.com/code/cog/
.. _Doxygen: http://www.stack.nl/~dimitri/doxygen/
.. _Sphinx: http://www.sphinx-doc.org/
.. _CMake: https://cmake.org/
.. _Nix: https://nixos.org/nix/
.. _CppUTest: http://cpputest.github.io/
.. _Unity: http://www.throwtheswitch.org/unity/
.. _Travis: https://travis-ci.org/forGGe/theCore/
.. _gitter: https://gitter.im/forGGe/theCore
.. _a list of theCore issues: https://github.com/forGGe/theCore/issues

.. |Build Status| image:: https://travis-ci.org/forGGe/theCore.svg?branch=master
   :target: https://travis-ci.org/forGGe/theCore
.. |Join the chat at https://gitter.im/forGGe/theCore| image:: https://badges.gitter.im/forGGe/theCore.svg
   :target: https://gitter.im/forGGe/theCore?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge

.. LINKS TO BE AUTOMATICALLY SUBSTITUTED BY SPHINX.
.. DO NOT MOVE THIS LABEL.

.. |Roadmap| replace:: Roadmap_
.. |Branching| replace:: Branching_
.. |Guides| replace:: Guides_
.. |Examples| replace:: Examples_
.. |Platforms| replace:: Platforms_
.. |Credits| replace:: Credits_
.. |DeveloperDocs| replace:: DeveloperDocs_

.. _Roadmap: https://forgge.github.io/theCore/community.html#roadmap
.. _Branching: https://forgge.github.io/theCore/community.html#brancing-model
.. _Guides: https://forgge.github.io/theCore/guides.html
.. _Examples: https://forgge.github.io/theCore/examples.html
.. _Platforms: https://forgge.github.io/theCore/platforms.html
.. _Credits: https://forgge.github.io/theCore/credits.html
.. _DeveloperDocs: https://forgge.github.io/theCore/community.html
