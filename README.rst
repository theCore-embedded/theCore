theCore: C++ Embedded Framework
===============================

|Build Status|
|Join the chat at https://t.me/joinchat/HQF-SEgtMQXoNOq_D71pSg|
|Follow us at https://t.me/theCoreEmbedded|
|Join the chat at https://gitter.im/forGGe/theCore|

theCore is the C++ embedded framework for various microcontroller applications,
with or without RTOS (bare-metal).

theCore tries to provide all that is necessary to create portable application code:

* Startup, initialization and periphery code for each supported platform.
* Build system support for different platform and target configurations.
* Platform-independent device drivers, like temperature sensors or displays.
* Optional OS-related abstractions: semaphores, mutexes and threads.
* Easy-to-deploy development environment.
* Different libraries and utilities.

Documentation
-------------

theCore documentation hosted on GitHub pages and available in two formats:
`multi-page HTML`_ for ease of navigation and `single-page HTML`_ for ease
of manual searching.

|Doxygen documentation| is also available (trough it is not complete).

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

Project links
-------------

Here a couple of additional references that you might be interest in:

* `Mailing list`_. For large discussions and announcements.
* `theCore Telegram group`_. For discussions.
* `theCore Telegram channel`_. For announcements.
* `theCore Twitter`_. You can check some announcements there too.
* `OpenHub page`_. Pretty good and explanatory.

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
leave message at `theCore Telegram group`_, `mailing list`_, gitter_, contact me
directly at forgge@gmail.com, or simply check out `a list of theCore issues`_.

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
.. _OpenHub page: https://www.openhub.net/p/theCoreEmbedded
.. _theCore Twitter: https://twitter.com/theCoreEmbedded
.. _Mailing list: https://mailmanlists.eu/mailman/listinfo/thecore
.. _theCore Telegram group: https://t.me/joinchat/HQF-SEgtMQXoNOq_D71pSg
.. _theCore Telegram channel: https://t.me/theCoreEmbedded
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
.. |Join the chat at https://t.me/joinchat/HQF-SEgtMQXoNOq_D71pSg| image:: https://img.shields.io/badge/chat-telegram-blue.svg
   :target: https://t.me/joinchat/HQF-SEgtMQXoNOq_D71pSg
.. |Follow us at https://t.me/theCoreEmbedded| image:: https://img.shields.io/badge/follow-telegram-blue.svg
   :target: https://t.me/theCoreEmbedded

.. LINKS TO BE AUTOMATICALLY SUBSTITUTED BY SPHINX.
.. DO NOT MOVE THIS LABEL.

.. |Doxygen documentation| replace:: `Doxygen documentation`_
.. |Roadmap| replace:: Roadmap_
.. |Branching| replace:: Branching_
.. |Guides| replace:: Guides_
.. |Examples| replace:: Examples_
.. |Platforms| replace:: Platforms_
.. |Credits| replace:: Credits_
.. |DeveloperDocs| replace:: `Community guidelines`_

.. _Doxygen documentation: https://forgge.github.io/theCore/doxygen/
.. _Roadmap: https://forgge.github.io/theCore/community.html#roadmap
.. _Branching: https://forgge.github.io/theCore/community.html#brancing-model
.. _Guides: https://forgge.github.io/theCore/guides/index.html
.. _Examples: https://forgge.github.io/theCore/examples/index.html
.. _Platforms: https://forgge.github.io/theCore/platform/index.html
.. _Credits: https://forgge.github.io/theCore/credits.html
.. _Community guidelines: https://forgge.github.io/theCore/community.html
