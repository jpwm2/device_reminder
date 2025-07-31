#!/usr/bin/env bash
set -e

# カラフルなバナー表示
echo -e "\e[1;33m"
echo "======================================="
echo "      🚀 環境セットアップを開始します      "
echo "======================================="
echo -e "\e[0m"

# 基本パッケージのインストール
echo -e "\e[1;34m[INFO]\e[0m 必要なパッケージをインストール中..."
sudo apt-get update
sudo apt-get install -y build-essential cmake gdb valgrind libgpiod-dev git

# 外部依存を手動で取得
echo -e "\e[1;34m[INFO]\e[0m 外部ライブラリを取得中..."
mkdir -p external
cd external

# googletest
if [ ! -d googletest ]; then
  echo -e "\e[1;32m[CLONE]\e[0m googletest をクローン中..."
  git clone https://github.com/google/googletest.git
else
  echo -e "\e[1;36m[SKIP]\e[0m googletest は既に存在します"
fi

# spdlog
if [ ! -d spdlog ]; then
  echo -e "\e[1;32m[CLONE]\e[0m spdlog をクローン中..."
  git clone https://github.com/gabime/spdlog.git
else
  echo -e "\e[1;36m[SKIP]\e[0m spdlog は既に存在します"
fi

# boost.di
if [ ! -d di ]; then
  echo -e "\e[1;32m[CLONE]\e[0m boost.di をクローン中..."
  git clone https://github.com/boost-ext/di.git
else
  echo -e "\e[1;36m[SKIP]\e[0m boost.di は既に存在します"
fi

cd ..

# 完了メッセージ
echo -e "\e[1;32m✅ 環境セットアップが完了しました！\e[0m"
