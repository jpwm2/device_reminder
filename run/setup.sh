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
if sudo apt-get update && sudo apt-get install -y build-essential cmake gdb valgrind libgpiod-dev git; then
  echo -e "\e[1;32m[SUCCESS]\e[0m パッケージのインストール完了"
else
  echo -e "\e[1;31m[FAILED]\e[0m パッケージのインストールに失敗しました" >&2
  exit 1
fi

# 外部依存を手動で取得
echo -e "\n\e[1;34m[INFO]\e[0m 外部ライブラリを取得中..."
mkdir -p external
cd external

# リポジトリクローン関数
clone_repo() {
  local name=$1
  local url=$2

  if [ ! -d "$name" ]; then
    echo -e "\e[1;32m[CLONE]\e[0m $name をクローン中..."
    if git clone "$url" "$name"; then
      echo -e "\e[1;32m[SUCCESS]\e[0m $name のクローン成功"
    else
      echo -e "\e[1;31m[FAILED]\e[0m $name のクローンに失敗しました" >&2
      exit 1
    fi
  else
    echo -e "\e[1;36m[SKIP]\e[0m $name は既に存在します"
  fi
}

# 各ライブラリの取得
clone_repo googletest https://github.com/google/googletest.git
clone_repo spdlog https://github.com/gabime/spdlog.git
clone_repo di https://github.com/boost-ext/di.git

cd ..

# 完了メッセージ
echo -e "\n\e[1;32m✅ 環境セットアップが正常に完了しました！\e[0m"
