#!/bin/sh
# Builds and runs the host tests for the decision logic (no board needed).
set -e
cd "$(dirname "$0")"
cc -std=c11 -Wall -Wextra -Werror -I../firmware/app \
   ../firmware/app/anzar_detector.c test_detector.c -o test_detector
./test_detector
