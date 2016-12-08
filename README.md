# theCore: C++ Embedded Framework

[![Build Status](https://travis-ci.org/forGGe/theCore.svg?branch=master)](https://travis-ci.org/forGGe/theCore)
[![Join the chat at https://gitter.im/forGGe/theCore](https://badges.gitter.im/forGGe/theCore.svg)](https://gitter.im/forGGe/theCore?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## Description

theCore is highly configurable and straightforward C++ framework aimed to reduce development efforts for embedded applications.
The main idea of theCore is to make as much as possible in the compile time.
This affects the way how the device drivers and peripherals are represented.
In fact, each driver is the template class that is instantiated with all the dependencies in a distinct type by user of theCore in the compile time.

You can check out more directly in the code of [the SPI driver](platform/stm32/export/aux/spi_bus.hpp) or  [the USART bus](platform/stm32/export/aux/usart_bus.hpp).

The project is at the initial stage. But something is already in there:
- CMake-based build-system
- Dependency management through [nix](https://nixos.org/nix/)
- Unit tests using [CppUTest](http://cpputest.github.io/)
- Filesystem [Petite FAT](http://elm-chan.org/fsw/ff/00index_p.html)
- Thread, allocators, devices abstractions
- Platform-level drivers, the IRQ manager, console, startup code
- Useful libraries
- Doxygen documentation

## Getting started

If you are interested in the simplest, ready-made demo project based on theCore, refer to [the blinky example project](https://github.com/forGGe/theCore-blinky).

You may also check [examples within theCore source tree](examples/).

In order to proceed with theCore itself, start cloning this repository:
```
git clone https://github.com/forGGe/theCore.git
```

When the code is received, you have to download and install all the dependencies.
The process can take some time.
To make it easier, I’d strongly recommend to download and install `nix`.
The installation guide can be found [on the official website](https://nixos.org/nixos/manual/index.html#ch-installation) or [Domenkozar's blog](https://www.domenkozar.com/2014/01/02/getting-started-with-nix-package-manager/).

After `nix` installation, run:
```bash
nix-shell --pure
...
```

## Examples

Examples reside in the [`examples`](examples) dir. Each example contains instructions how to compile and use the example.

To build all examples at once you can use nix and specially crafted [superproject](examples/CMakeLists.txt).
To do that, [enter the nix shell](#getting-started) and proceed as follows.

```bash
cd examples
mkdir build
cd build
cmake ..
make
```

Resulting binaries will be placed under corresponding directories.
I.e. the [cs43l22 example](examples/cs43l22_audio) if built that way, will be placed under `build/stm32f4_cs43l22_audio-prefix/src/stm32f4_cs43l22_audio-build/` directory.
You can refer to the [external project CMake documetation](https://cmake.org/cmake/help/v3.4/module/ExternalProject.html) and [the superproject CMake list file](examples/CMakeLists.txt) to figure out where binaries will be placed.
Instructions describing how to flash binary onto the device placed in the readme file inside each example.

## Unit tests

theCore's unit tests can be built and run on the host machine.
For this you have to run the nix-shell and trigger a build:

```bash
cd /path/to/the/core
nix-shell . --pure
mkdir build
cd build
cmake ..
make
```

Then, you should run the unit tests:
```bash
ctest . --output-on-failure
```

## On-device tests

theCore comes with tests that can be executed on the device itself.
Refer to [the theCore testing documentation](tests/README.md) to figure out how to build, use and manage on-device theCore tests.
To build all tests, [enter the nix shell](#getting-started) and proceed as follows:

```bash
cd tests
mkdir build
cd build
cmake ..
make
```

## Supported platforms

| Short name | Full name | List of supported MCUs | Path in project |
|-----------|------------|------------------------|-----------------|
| stm32f4xx | STM32 F4 series | STM32F407VG | [stm32 multiplatform](platform/stm32) |
| stm32l1xx | STM32 L1 series | STM32L152RE | [stm32 multiplatform](platform/stm32) |
| tm4c      | Texas Instrument TM4C |TM4C123GH6PM | [platform/tm4c](platform/tm4c) |
| host      | Host platform | - | [platform/host](platform/host) |

## Documentation

The current documentation of theCore is completely stated in the code (as doxygen comments).
To generate the HTML pages from Doxygen is quite enough to run:

```bash
make core_doc
```

in the build directory (see [unit tests section](#unit-tests)).

## License

The source code is distributed under LGPL v3.0 License.


## Contribution & Using

The project is on its very beginning, so any help is more than welcome and highly appreciated.
If you'd like to take a part in the project growth and (or) have any questions – feel free to email me at forgge@gmail.com, leave message at [gitter](https://gitter.im/forGGe/theCore) or simply check out [a list of issues](https://github.com/forGGe/theCore/issues).
If you have any suggestions on theCore improvement or just like it how it is, don’t keep silence! I’ll be happy to read your reviews.

When making pull requests, make sure that code conforms to [theCore code style guide](style.md).

You are welcome to use theCore in your own projects.
If there is something that it’s not enough for this, please let me know via email or open some issues and we will do our best to handle this ASAP.
Good luck!
