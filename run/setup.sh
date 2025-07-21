#!/usr/bin/env bash
set -e

sudo apt-get update
sudo apt-get install -y build-essential cmake gdb valgrind libgpiod-dev
git submodule update --init --recursive
