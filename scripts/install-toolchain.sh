#!/bin/bash

set -e # exit on error

# Determine machine type based on OS and processor
UNAME_S=$(uname -s)
UNAME_P=$(uname -p)
MACHINE=""

case "$UNAME_S" in
    Linux)
        MACHINE="linux"
        ;;
    Darwin)
        MACHINE="osx"
        ;;
esac

case "$UNAME_P" in
    x86_64)
        MACHINE="${MACHINE}-x86_64"
        ;;
esac

if [[ "$UNAME_P" == arm* ]]; then
    MACHINE="${MACHINE}-arm64"
fi

if [[ ! -d "./riscv32im-${MACHINE}" ]]; then
    curl -L https://github.com/risc0/toolchain/releases/download/2024.01.05/riscv32im-${MACHINE}.tar.xz | tar xvJ -C ./
else
    echo "riscv32 toolchain already exists, skipping step"
fi


export RISCV32_TOOLCHAIN="$(pwd)/riscv32im-${MACHINE}/bin"
