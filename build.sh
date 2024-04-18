#!/bin/bash

set -ex

code_dir=${PWD}
mkdir -p Build

pushd Build
    g++ \
        -Wall \
        -Werror \
        -fsanitize=address \
        -std=c++17 \
        -D DQN_UNIT_TESTS_WITH_MAIN \
        -D DQN_UNIT_TESTS_WITH_KECCAK \
        -x ${code_dir}/dqn.cpp \
        -g \
        -o dqn_unit_tests
popd
