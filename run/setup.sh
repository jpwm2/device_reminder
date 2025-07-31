#!/usr/bin/env bash
set -e

# 基本パッケージのインストール
sudo apt-get update
sudo apt-get install -y build-essential cmake gdb valgrind libgpiod-dev git

# 外部依存を手動で取得
mkdir -p external
cd external

# googletest
if [ ! -d googletest ]; then
  git clone https://github.com/google/googletest.git
fi

# spdlog
if [ ! -d spdlog ]; then
  git clone https://github.com/gabime/spdlog.git
fi

# boost.di
if [ ! -d di ]; then
  git clone https://github.com/boost-ext/di.git
fi

cd ..
