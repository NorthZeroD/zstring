#!/usr/bin/env bash

set -e

selfdir="$(dirname "${BASH_SOURCE[0]}")"

./${selfdir}/scripts/cmake_clang_build.sh

for i in {1..9}; do
    if ./${selfdir}/build/tests/${i}.* \
        | diff --color -u ${selfdir}/tests/ans/${i}*.txt -;
    then
        echo -e '\033[32m================PASS================\033[0m'
    else
        echo -e '\033[31m================FAIL================\033[0m'
    fi
done
echo -e '\033[33mDone.\033[0m'
