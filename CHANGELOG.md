# Change Log

## [v0.3.1](https://github.com/forGGe/theCore/tree/v0.3.1) (2017-12-25)
[Full Changelog](https://github.com/forGGe/theCore/compare/v0.3.0...v0.3.1)

**Merged pull requests:**

- Fix clang wrapper issue on Nix [\#337](https://github.com/forGGe/theCore/pull/337) ([forGGe](https://github.com/forGGe))
- Fix readme file links and build instructions [\#223](https://github.com/forGGe/theCore/pull/223) ([forGGe](https://github.com/forGGe))

## [v0.3.0](https://github.com/forGGe/theCore/tree/v0.3.0) (2016-12-10)
[Full Changelog](https://github.com/forGGe/theCore/compare/v0.2.1...v0.3.0)

**Closed issues:**

- cs43l22 example does not produce any sound [\#192](https://github.com/forGGe/theCore/issues/192)
- Attempt to run board flasher under nix shell results in openocd error [\#190](https://github.com/forGGe/theCore/issues/190)
- Enable EXTI manager test for stm32f4 discovery suite [\#179](https://github.com/forGGe/theCore/issues/179)
- Console driver interfers with uart\_bat test in platform\_bat suite [\#176](https://github.com/forGGe/theCore/issues/176)
- stm32, l1: review and fix DMA wrapper in a light of get\_size\_div\(\) changes [\#167](https://github.com/forGGe/theCore/issues/167)
- Use stm32f4x.cfg instead of stm32f4x\_stlink.cfg [\#165](https://github.com/forGGe/theCore/issues/165)
- Enable console support [\#164](https://github.com/forGGe/theCore/issues/164)
- Implement EXTI platform driver [\#163](https://github.com/forGGe/theCore/issues/163)
- Add notion of examples and on-device tests into README file [\#160](https://github.com/forGGe/theCore/issues/160)
- Improve tuple iteration using C++14 [\#149](https://github.com/forGGe/theCore/issues/149)
- undefined variable ‘gcc-arm-embedded-5\_2’ when running nix on master [\#144](https://github.com/forGGe/theCore/issues/144)
- Hello World test case: loop output with delay [\#141](https://github.com/forGGe/theCore/issues/141)
- Replace CMAKE\_FORCE\_C\_COMPILER since it is depreceated [\#139](https://github.com/forGGe/theCore/issues/139)
- Platform defaults to host, even if it is not expected [\#136](https://github.com/forGGe/theCore/issues/136)
- Find a way to produce objects instead of library archives and integrate it into theCore [\#133](https://github.com/forGGe/theCore/issues/133)
- Move Petite FAT to the separate submodule [\#98](https://github.com/forGGe/theCore/issues/98)
- Create CS43L22 usage example [\#94](https://github.com/forGGe/theCore/issues/94)
- Abort build for stm32f4xx platform if cross-compiler wasn't set [\#87](https://github.com/forGGe/theCore/issues/87)
- TI TM4C12x support [\#52](https://github.com/forGGe/theCore/issues/52)
- Create code style guide [\#41](https://github.com/forGGe/theCore/issues/41)
- stm32l1xx support [\#38](https://github.com/forGGe/theCore/issues/38)
- Clang toolchain file for cortex-m4 [\#14](https://github.com/forGGe/theCore/issues/14)

**Merged pull requests:**

- 0.3.0 release [\#194](https://github.com/forGGe/theCore/pull/194) ([forGGe](https://github.com/forGGe))

## [v0.2.1](https://github.com/forGGe/theCore/tree/v0.2.1) (2016-10-12)
[Full Changelog](https://github.com/forGGe/theCore/compare/v0.2.0...v0.2.1)

**Closed issues:**

- Thread library: change semaphore try\_wait\(\) API  [\#135](https://github.com/forGGe/theCore/issues/135)
- create or use existing hardware register access library [\#130](https://github.com/forGGe/theCore/issues/130)
- stm32 platform: make sure it works with FreeRTOS [\#124](https://github.com/forGGe/theCore/issues/124)
- stm32f4xx -\> stm32: port EXTI driver from stm32f4xx [\#122](https://github.com/forGGe/theCore/issues/122)
- Support of building for different platforms and configuraions on travis [\#6](https://github.com/forGGe/theCore/issues/6)

**Merged pull requests:**

- \[\#144\] Use proper Nix compiler package [\#154](https://github.com/forGGe/theCore/pull/154) ([forGGe](https://github.com/forGGe))

## [v0.2.0](https://github.com/forGGe/theCore/tree/v0.2.0) (2016-07-14)
[Full Changelog](https://github.com/forGGe/theCore/compare/v0.1.1...v0.2.0)

**Closed issues:**

- stm32f4xx: gcc v6.0 can't build theCore [\#93](https://github.com/forGGe/theCore/issues/93)
- stm32f4xx:  this\_thread namespace for non-os environments is missing [\#70](https://github.com/forGGe/theCore/issues/70)
- Add demo examples into separate directory inside the Core [\#65](https://github.com/forGGe/theCore/issues/65)
- Create WiKi page for EXTI driver usage [\#57](https://github.com/forGGe/theCore/issues/57)
- Consolidate common platform definitions  [\#50](https://github.com/forGGe/theCore/issues/50)
- Implement driver for digital light sensor BH1750 [\#49](https://github.com/forGGe/theCore/issues/49)
- Implement CS43L22 driver [\#46](https://github.com/forGGe/theCore/issues/46)
- Binary semaphroe exists for FreeRTOS thread, but missing in default thread library [\#44](https://github.com/forGGe/theCore/issues/44)
- Implement intrusive list [\#39](https://github.com/forGGe/theCore/issues/39)
- the bus driver: support xfer\(\) while in IRQ  [\#36](https://github.com/forGGe/theCore/issues/36)
- stm32f4xx: EXTI driver [\#35](https://github.com/forGGe/theCore/issues/35)
- stm32f4xx: configurable usart module [\#34](https://github.com/forGGe/theCore/issues/34)
- Protect build from non-existing CppUTest library [\#32](https://github.com/forGGe/theCore/issues/32)
- Implement platform-level/IRQ debug console and asserts [\#27](https://github.com/forGGe/theCore/issues/27)
- Disable assertions in Release and MinSizeRel modes [\#25](https://github.com/forGGe/theCore/issues/25)
- Merge irq platform code and irq\_manager [\#22](https://github.com/forGGe/theCore/issues/22)
- Add I2S bus implementation  [\#20](https://github.com/forGGe/theCore/issues/20)
- Provide reliable semaphore based on FreeRTOS implementation [\#11](https://github.com/forGGe/theCore/issues/11)

**Merged pull requests:**

- 0.2.0 release [\#115](https://github.com/forGGe/theCore/pull/115) ([forGGe](https://github.com/forGGe))
- Fix Nix shell command [\#69](https://github.com/forGGe/theCore/pull/69) ([rasendubi](https://github.com/rasendubi))
- Main Page Update [\#21](https://github.com/forGGe/theCore/pull/21) ([InnusiG](https://github.com/InnusiG))
- Add a Gitter chat badge to README.md [\#10](https://github.com/forGGe/theCore/pull/10) ([gitter-badger](https://github.com/gitter-badger))

## [v0.1.1](https://github.com/forGGe/theCore/tree/v0.1.1) (2016-04-14)
[Full Changelog](https://github.com/forGGe/theCore/compare/v0.1.0...v0.1.1)

**Merged pull requests:**

- Patch-fix PR [\#5](https://github.com/forGGe/theCore/pull/5) ([forGGe](https://github.com/forGGe))

## [v0.1.0](https://github.com/forGGe/theCore/tree/v0.1.0) (2016-04-12)
**Merged pull requests:**

- Filter out branches from Travis CI [\#3](https://github.com/forGGe/theCore/pull/3) ([forGGe](https://github.com/forGGe))
- Travis integration [\#2](https://github.com/forGGe/theCore/pull/2) ([forGGe](https://github.com/forGGe))
- Add more ports to smt32f4xx platform [\#1](https://github.com/forGGe/theCore/pull/1) ([RostakaGmfun](https://github.com/RostakaGmfun))



\* *This Change Log was automatically generated by [github_changelog_generator](https://github.com/skywinder/Github-Changelog-Generator)*
