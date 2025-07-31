# device_reminder

このプロジェクトではサブモジュールとして GoogleTest などの外部ライブラリを利用しています。
クローン直後にビルドを行う場合は、以下のコマンドでサブモジュールを取得してください。

```bash
git submodule update --init --recursive
```

または `run/setup.sh` を実行すると、必要なパッケージのインストールとサブモジュールの更新をまとめて行えます。
