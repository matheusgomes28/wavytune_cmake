#!/usr/bin/env bass

# This script calls the conan command 
# with the x86_64 Debug profile for
# ubuntu to pull the dependencies

set -euox

PROJECT_DIR=$(cd "$(dirname "$0")/.." && pwd)
PROFILES_DIR="${PROJECT_DIR}/helpers/conan/profiles"

conan install -pr "${PROFILES_DIR}/x86_64_ubuntu_debug" --build=missing ${PROJECT_DIR}
