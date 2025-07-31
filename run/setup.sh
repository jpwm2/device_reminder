#!/usr/bin/env bash
set -e

# 基本パッケージのインストール
sudo apt-get update
sudo apt-get install -y build-essential cmake gdb valgrind libgpiod-dev git

# googletest の取得（事前に必要な外部依存を手動でclone）
mkdir -p external
cd external
if [ ! -d googletest ]; then
  git clone --depth=1 https://github.com/google/googletest.git
fi
cd ..
