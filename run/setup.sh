#!/bin/bash
# ライブラリを取得するスクリプト
set -e

# スクリプトの場所を基準に実行
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

libs=(googletest spdlog di)
for lib in "${libs[@]}"; do
    if [ ! -e "external/$lib/.git" ]; then
        echo "$lib を取得します..."
        git submodule update --init "external/$lib"
    else
        echo "$lib は既に取得済みです"
    fi
done

echo "セットアップが完了しました"
echo "hi"
