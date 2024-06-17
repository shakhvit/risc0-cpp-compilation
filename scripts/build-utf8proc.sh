#!/bin/bash
#
#


set -e # exit on error
set -o pipefail # exit on fail in pipe
# set -u # exit on unset variable

# keep track of the last executed command
trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG
# echo an error message before exiting
trap 'echo "\"${last_command}\" command executed with exit code $?."' EXIT

ROOT_DIR=$(pwd)

. scripts/install-toolchain.sh # install the toolchain and source the path to env var



export CC=${RISCV32_TOOLCHAIN}/riscv32-unknown-elf-gcc
export CXX=${RISCV32_TOOLCHAIN}/riscv32-unknown-elf-g++
export AR=${RISCV32_TOOLCHAIN}/riscv32-unknown-elf-ar
export AS=${RISCV32_TOOLCHAIN}/riscv32-unknown-elf-as
export LD=${RISCV32_TOOLCHAIN}/riscv32-unknown-elf-ld
export RANLIB=${RISCV32_TOOLCHAIN}/riscv32-unknown-elf-ranlib
export STRIP=${RISCV32_TOOLCHAIN}/riscv32-unknown-elf-strip
export OBJDUMP=${RISCV32_TOOLCHAIN}/riscv32-unknown-elf-objdump
export OBJCOPY=${RISCV32_TOOLCHAIN}/riscv32-unknown-elf-objcopy


mkdir -p external/libs/libutf8proc/lib
mkdir -p external/libs/libutf8proc/include

cd external/utf8proc
rm -rf build
mkdir build
cmake -S . -B build
cmake --build build
cp build/libutf8proc.a ${ROOT_DIR}/external/libs/libutf8proc/lib/
cp utf8proc.h ${ROOT_DIR}/external/libs/libutf8proc/include/
rm -rf build
