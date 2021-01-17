# anyfile_to_image
[English](README_en.md)
## 入力されたファイルをデータとするpnm画像ファイルを作成
### 使い方
```
picgen [オプション] 入力ファイル名 出力ファイル名
```
#### オプション
|オプション|機能|
|:---|:---|
|-h,--help|ヘルプを表示して終了する|
|-X, --X_ratio NUM             |出力画像の横幅の比率|
|-Y, --Y_ratio NUM             |出力画像の縦の比率|
|-x, --X_absolute NUM          |出力画像の横の絶対サイズ|
|-y, --Y_absolute NUM          |出力画像の縦の絶対サイズ|
|-m, --mode (MONO\|GRAY\|COLOR)  |どの形式で出力するか（デフォルトはcolor）|
|--no_extension                |拡張子を付け足さない|
### 説明
このプログラムは、入力ファイルをデータとするpnm画像を生成します。
実際には、適切な縦と横のピクセル数を算出し、それをもとにpnmヘッダを出力し、その後に入力ファイルをコピーし、データが足りなかったら0で埋める、ということを行っています。
