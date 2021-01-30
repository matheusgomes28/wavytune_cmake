#!/usr/bin/env bash

# Script to call the installation command
# for conan. Here we use the debug Ubuntu
# profile

set -euox

PROJECT_ROOT=$(cd "$(dirname "$0")/.." && pwd)
PROFILE_DIR="${PROJECT_DIR}/helpers/conan/profiles"
