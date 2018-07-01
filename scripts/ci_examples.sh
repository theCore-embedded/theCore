#!/bin/sh

set -v # Be verbose

mkdir build_examples
cd build_examples

tcore init --remote https://github.com/theCore-embedded/example_hello_world
tcore init --remote https://github.com/theCore-embedded/example_blinky
tcore init --remote https://github.com/theCore-embedded/example_button_interrupt
tcore init --remote https://github.com/theCore-embedded/example_sensor_htu21d
tcore init --remote https://github.com/theCore-embedded/example_fatfs
tcore init --remote https://github.com/theCore-embedded/example_cs43l22_audio
tcore init --remote https://github.com/theCore-embedded/example_hm10_bluetooth

# Hello world example

tcore compile --source ./example_hello_world --target host
tcore compile --source ./example_hello_world --target stm32f4_disc
tcore compile --source ./example_hello_world --target tiva_tm4c_launchpad

# Blinky

tcore compile --source ./example_blinky --target stm32f4_disc
tcore compile --source ./example_blinky --target tiva_tm4c_launchpad

# EXTI example

tcore compile --source ./example_button_interrupt --target stm32f4_disc
tcore compile --source ./example_button_interrupt --target tiva_tm4c_launchpad

# HTU21D

tcore compile --source ./example_sensor_htu21d --target stm32f4_disc

# FATFS over SDSPI

tcore compile --source ./example_fatfs --target tiva_tm4c_launchpad

# CS43L22 on STM32F4 discovery

tcore compile --source ./example_cs43l22_audio --target stm32f4_disc

# BT using HM-10 module

tcore compile --source ./example_hm10_bluetooth --target stm32f4_disc
