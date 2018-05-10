Cache common thirdparties
=========================

By default theCore will not download any of thirdparty repository, unless it is
really required. For example, if bare-metal application is built, there is no need
to download FreeRTOS.

However, when multiple applications are built on top of theCore, thirdparties
will be downloaded separately per each app. To avoid re-downloading external projects
it is possible to specify cache directory, where additional and optional
build dependencies will be stored.

Two variables can be set to control caching:

* ``THECORE_THIRDPARTY_DIR`` - directory where to download dependencies
* ``THECORE_BUILD_THIRDPARTY_DIR`` - worktree and build directory for thirdparties

Variables can be passed via an environment or CMake command-line arguments.

Using environment variables
---------------------------

#. Set variables::

        export THECORE_THIRDPARTY_DIR=~/.thecore_thirdparty
        # Note that if you build applications concurrently, you need THECORE_BUILD_THIRDPARTY_DIR
        # to be unique for every application.
        export THECORE_BUILD_THIRDPARTY_DIR=~/.thecore_thirdparty_worktree

#. Run CMake as usual::

        cmake /path/to/application

Using CMake command line
------------------------

::

    # Note that if you build applications concurrently, you need THECORE_BUILD_THIRDPARTY_DIR
    # to be unique for every application.
    cmake -DTHECORE_THIRDPARTY_DIR=~/.thecore_thirdparty -DTHECORE_BUILD_THIRDPARTY_DIR=~/.thecore_thirdparty_worktree /path/to/application
