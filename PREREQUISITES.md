
# Prerequisites

In order to build RVS from source, you need to have several packages on your
system:

- GCC 5.4.0
- ROCm 1.8
- CMake 3.5.0
- libpci-dev
- Doxygen 1.8.11

For Ubuntu:
---------------

## GCC
GCC 5.4.0 or later should already be installed on your system.
If not, refere to GCC installation instructions.

## ROCm > 1.8

Please install following ROCm installation instructions. Get the latest ROCM

## CMake

If CMake is not installed on your system you may install it like this:

sudo yum in    sudo apt-get install software-properties-common
    sudo add-apt-repository ppa:george-edison55/cmake-3.x
    sudo apt-get update
    sudo apt-get install cmake

## libpci-dev

First install `libpci3.s0`:
    sudo apt-get update
    sudo apt-get install libpci3

Then install header files:
    sudo apt-get install libpci-dev

## Doxygen
    sudo apt-get update
    sudo apt-get install doxygen

For Cent OS:
---------------

## GCC
GCC 5.4.0 or later should already be installed on your system.
If not, refere to GCC installation instructions.

## ROCm > 1.8

Please install following ROCm installation instructions. Get the latest ROCM

## CMake

If CMake is not installed on your system you may install it like this:

    sudo yum install cmake3

## libpci-dev
    sudo yum install pciutils-devel.x86_64

## Doxygen
    sudo yum install doxygen

For SLE :
---------------

## GCC
GCC 5.4.0 or later should already be installed on your system.
If not, refere to GCC installation instructions.

## ROCm > 1.8

Please install following ROCm installation instructions. Get the latest ROCM

## CMake

If CMake is not installed on your system you may install it like this:

    sudo zypper install cmake

## libpci-dev
    sudo zypper install pciutils-devel.x86_64

## Doxygen
    sudo zypper install doxygen





