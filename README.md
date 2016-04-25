# The Core: C++ Embedded Framework

[![Build Status](https://travis-ci.org/forGGe/theCore.svg?branch=master)](https://travis-ci.org/forGGe/theCore)
[![Join the chat at https://gitter.im/forGGe/theCore](https://badges.gitter.im/forGGe/theCore.svg)](https://gitter.im/forGGe/theCore?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## Description

The Core is highly configurable, straightforward C++ framework with minimum set of development dependencies. It aimed to reduce development efforts for embedded applications.
The main idea of The Core is to make as much as possible in the compile time. This affects the way how the device drivers and peripherals are represented. In fact, each driver is the template class that is instantiated with all the dependencies in a distinct type by user of The Core in compile-time.

You can check out more directly in the code of [the SPI driver](/blob/master/platform/stm32f4xx/export/platform/spi_bus.hpp) or  [the USART bus](/blob/master/platform/stm32f4xx/export/platform/usart_bus.hpp).

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

If you are interested in the simplest, ready-made demo project based on The Core,
refer to [the blinky example project](https://github.com/forGGe/theCore-blinky).

In order to proceed with The Core itself, start cloning this repository:
```
git clone https://github.com/forGGe/theCore.git
```

When the code is received, you have to download and install all the dependencies.
The process can take some time.
To make it easier, I’d strongly recommend to download and install `nix`.
The installation guide could be found
[on the official website](https://nixos.org/nixos/manual/index.html#ch-installation) or
[Domenkozar's blog](https://www.domenkozar.com/2014/01/02/getting-started-with-nix-package-manager/)
.

After `nix` installation, run:
```bash
nix-shell core --pure
...
```

## Unit tests

The Core's unit tests can be built and run on the host machine.
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

## Supported platforms

Currently, the stm32f4xx platform is supported and available for further development, as well as the host-platform, used for unit tests.

## Documentation

The current documentation of The Core is completely stated in the code (as doxygen comments). To generate the HTML pages from Doxygen is quite enough to run:

```bash
make core_doc
```

in the build directory (see [units](#Unit tests)).

## License

The source code is distributed under LGPL v3.0 License.


## Contribution & Using

The project is on its very beginning, so any help is more than welcome and highly appreciated. If you'd like to take a part in the project growth and (or) have any questions – feel free to email me at forgge@gmail.com, leave message at [gitter](https://gitter.im/forGGe/theCore) or simply check out [a list of issues](/issues). If you have any suggestions on The Core improvement or just like it how it is, don’t keep silence! I’ll be happy to read your reviews. 

You are welcome to use The Core in your own projects. If there is something that it’s not enough for this, please let me know via email or open some issues and we will do our best to handle this ASAP. Good luck!

