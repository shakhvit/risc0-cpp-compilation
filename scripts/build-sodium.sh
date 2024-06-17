#!/bin/bash
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

export PATH=$RISCV32_TOOLCHAIN:$PATH # add toolchain path to the PATH

rm -rf ${ROOT_DIR}/external/libs/libsodium
mkdir -p ${ROOT_DIR}/external/libs/libsodium

cd external/libsodium

make clean
export CFLAGS="-march=rv32im -mabi=ilp32"
./configure --host=riscv32-unknown-elf --disable-pie --prefix=${ROOT_DIR}/external/libs/libsodium/
make && make install

cd -
