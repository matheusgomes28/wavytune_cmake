#! /usr/bin/env bash

# This script finds all the shell scripts
# ans uses shellcheck on all of them.

set -euo pipefail

PROJECT_DIR=$(cd "$(dirname "$0")/.." && pwd)
SHELL_FILES=$(find "$PROJECT_DIR" -name "*.sh")
echo "$SHELL_FILES" | xargs shellcheck
