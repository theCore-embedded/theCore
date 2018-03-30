.. _theCore_DeveloperDocs:

Community and developer guidelines
==================================

.. _theCore_Roadmap:

Roadmap
-------

theCore aims to provide a rich and consistent embedded environment for a range
of different devices. Such complexity requires from users an advanced knowledge
of build tools, toolchains and a board layout even in simple cases like a LED
blinking.

Thus the main milestones are directed to improve the initial user experience.
Such goals are:

* Provide all-in-one scripts for controlling a development flow, similar to
  what ``mbed_cli`` does.

  These scripts must be able to:

  * Download required theCore revision.
  * Initialize development environment, by optionally installing and running Nix.
  * Detect and inform about missing dependencies.
  * Bootstrap a new project based on theCore, optionally using one of
    supported boards as a base.
  * Launch a project build.
  * Detect connected devices and be able to flash a project binary.
  * Provide a ncurses-like (at least) interface to configure the project settings,
    that otherwise will require a manual editing of a target JSON file.

* Make theCore IoT-ready.

  It is not a surprise that a modern embedded project must include a some
  kind of connectivity support. Most required technologies that missing in
  theCore are:

  * lwIP integration.
  * MQTT/COAP libraries.
  * ESP8266 support (as external modem at least).
  * Ethernet/WiFi drivers.

* Add more multi-target examples where possible.

  Much of theCore's API is portable. Examples should demonstrate an usage of
  many development boards with the same application code.
  Existing examples should be also converted to support many boards.

* Complete support for at least one development board for each supported platform.

  Support for a dev board includes:

  * Drivers for each device soldered on the board.
  * Periphery drivers (SPI, UART, I2C, etc.) that are required by the devices,
    soldered on the board.
  * A default JSON file that provides a base clock, periphery and external
    devices configuration. User can change that JSON file to suit his needs.
  * Platform and device capabiilities - similar to what ``Kconfig``
    in Linux does.

.. _theCore_Branching:

Branching and release model
---------------------------

There are two main branches - ``develop`` and ``master``.
Both branches are protected against force-pushing unless it is really-really
required (almost never happen).

As names state, all the development (normally) happens in ``develop`` branch,
while ``master`` contains stable versions. ``master`` branch is tagged according
to `the semantic versioning approach`_.

Full explanation of branching model, accepted in theCore you can find in `nvie git article`_.

Code style
----------

.. highlight:: cpp

.. note:: The source formatting script called ``srcformat.sh`` in ``scripts`` directory,
          though it is still under development.

Whitespaces and indentation
~~~~~~~~~~~~~~~~~~~~~~~~~~~

#. Do not use tabs. Only spaces are allowed.
#. Set indent to 4 spaces per indent.
#. Indent ``public``, ``private``, ``protected`` with the same indent as
   a ``class`` block.
#. Indent ``class`` block with the same indent as a namespace::

     // OK
     namespace foo
     {

     class entity
     {
     public:
         /* */
     private:
         /* */
     protected:
         /* */
     };

     } // namespace foo

#. Indent switch blocks so that the ``case X:`` statements are indented in the switch block::

     // OK
     switch (foo) {
         case 1:
             a += 1;
             break;

         case 2: {
             a += 2;
             break;
         }
     }

#. Do not indent labels.
#. Do not indent preprocessor block.
#. Insert spaces around operators, except ``++``, ``--``, ``!``, ``.``, ``->``, ``*`` and ``&``.
#. Insert space padding between ``if``, ``for``, ``while`` and the following bracket.
#. **Do not leave trailing whitespaces.**

Bracket placement
~~~~~~~~~~~~~~~~~

#. Use `One True Bracket Style`_.
#. Place brackets in namespaces, classes, extern-"C" and functions on the next line, e.g::

     // Wrong
     namespace foo {

     }

     // OK
     namespace bar
     {

     }

#. Add brackets to unbracketed one line conditional statements (e.g. ``if``, ``for``, ``while``...).
#. Do not put whitespace between angle brackets in template definitions::

     // Good
     foo<int, bar<int>> obj;

     // Bad
     foo<int, bar<int> > obj;

Code length
~~~~~~~~~~~

#. Prefer 80 character code line length. 100 characters is a maximum allowed line length.
#. Prefer to keep functions and methods with not more than 100 lines of code.

Naming
~~~~~~

#. Use `snake_case`_ for identifiers.
#. Use ``m_`` prefix when naming private and protected class members.
#. Use CamelCase style for template parameters only::

     template<typename CrazyAllocator>
     class my_strange_container;

Comments
~~~~~~~~

#. **Do not leave commented out code.**
#. Prefer not keeping ``TODO`` in code without corresponding issue.
   Place issue number next to the ``TODO`` with a short explanation why it is there::

     // TODO #123: replace foo() with something that exposes smaller IRQ latency.
     foo();

#. Use doxygen for commenting the function prototypes and class methods.
#. Use C++ comment lines for doxygen commenting::

     //! This function does something.
     //! \details Also it calls 911.
     void foo();

#. Do not use ``\brief`` doxygen directive. Brief tag is set automatically for first sentence (until the first dot).
   See also `autobrief docs`_.

#. Do not add redundant Doxygen comments, like::

     //! Set the size.
     //! \param size The size.
     void set_size(size_t size);

Commit and PR style
-------------------

.. highlight:: none

#. Try to follow common `git guidelines`_.
#. **Do not keep commits named just** ``FIX`` **,** ``Review`` **,** ``tmp`` **, etc.**
#. Prefer placing issue number at the start of the summary line, prefixed with ``>#``::

     >#911 Implement calling police officer

   This will be recognized by GitHub as issue reference.
   Note that simply putting ``#`` at the beginning will not do the trick.
   Since a line with the ``#`` at the start is interpreted as the commentary.

#. **Do not** include issue number in the pull request name.
#. Use ``[WIP]`` (Work In Progress) prefix for PRs that are still in progress or review
   and should not be merged right away.

.. _the semantic versioning approach: http://semver.org/
.. _nvie git article: http://nvie.com/posts/a-successful-git-branching-model/
.. _One True Bracket Style: https://en.wikipedia.org/wiki/Indent_style#Variant:_1TBS_.28OTBS.29
.. _`snake_case`: https://en.wikipedia.org/wiki/Snake_case
.. _`git guidelines`: https://chris.beams.io/posts/git-commit/
.. _`autobrief docs`: https://www.stack.nl/~dimitri/doxygen/manual/config.html#cfg_javadoc_autobrief
