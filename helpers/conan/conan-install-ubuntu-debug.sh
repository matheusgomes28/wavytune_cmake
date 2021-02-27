#! /usr/bin/env bash

# This script calls the conan command 
# with the x86_64 Debug profile for
# ubuntu to pull the dependencies

set -euo pipefail

PROJECT_DIR=$(cd "$(dirname "$0")/../../" && pwd)
PROFILES_DIR="${PROJECT_DIR}/helpers/conan/profiles"

conan install -pr "${PROFILES_DIR}/x86_64_gcc7_ubuntu_debug" --build=missing "${PROJECT_DIR}"
