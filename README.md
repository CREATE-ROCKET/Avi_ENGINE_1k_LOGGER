# GSE電装 ロガー

# ファイル構成
CAD: Eagle,KiCadファイル
analysis: ログ解析プログラム
src: 基板書き込みプログラム

# アナログ回路の詳細

## 推力測定

1つのセンサーに対応

[使用するロードセル](https://www.te.com/usa-en/product-FC2311-0000-0250-L.html)

センサー出力 → 作動増幅回路 (ゲイン:47) → ローパスフィルタ (500Hz, R = 3.3 kΩ, C = 0.1 μF) → ADC

## 圧力測定

4つのセンサーに対応

[使用するセンサー](https://www.bdsensors.de/fileadmin/user_upload/Download/Datenblaetter_datasheets/DB_DMK331P_E.pdf)

センサー出力 → 分圧回路 (ゲイン: 0.5) → ボルテージフォロワ → ローパスフィルタ (500Hz, R = 3.3 kΩ, C = 0.1 μF) → ADC

## 圧力測定 (以前使っていたもの、参考)

[DMK331Pのリンク](https://www.bdsensors.de/fileadmin/user_upload/Download/Datenblaetter_datasheets/DB_DMK331P_E.pdf)

本機は1つの測定用回路を搭載、**ただしこの回路は電源電圧が不足しており、上のPU5402用の回路で運用する**

電源電圧は12V, センサー出力は0-10V で運用

センサー出力 → 分圧回路 (ゲイン: 0.5) → ボルテージフォロワ → ローパスフィルタ (500Hz, R = 3.3 kΩ, C = 0.1 μF) → ADC

## 圧力測定 (より以前使っていたもの、参考)

[FSV-010MPのリンク](https://www.sayama.com/products/index.php/item?cell003=%E5%9C%A7%E5%8A%9B%E3%82%BB%E3%83%B3%E3%82%B5&cell004=%E3%82%B2%E3%83%BC%E3%82%B8%E5%9C%A7%EF%BC%88%E6%B6%B2%E4%BD%93%EF%BC%89&name=%E3%82%B2%E3%83%BC%E3%82%B8%E5%9C%A7%E8%A8%88+%E5%9C%A7%E5%8A%9B%E3%82%BB%E3%83%B3%E3%82%B5++FSV-010MP&id=3137&label=1)

本機は非搭載

電源電圧は12V, センサー出力は0-5V で運用

センサー出力 → ローパスフィルタ (500Hz, R = 3.3 kΩ, C = 0.1 μF) → ADC

## 温度測定

熱電対により測定、高温用が2つ、低温用が3つ

[使用するセンサー](https://akizukidenshi.com/catalog/g/gP-10160/)

| 温度(℃) | 温度(K) | 抵抗値(Ω) |
|---------|---------|-----------|
| 0       | 273.15  | 2.87E+04  |
| 30      | 303.15  | 8.27E+03  |
| 60      | 333.15  | 2.98E+03  |
| 90      | 363.15  | 1.27E+03  |

高温用の分圧抵抗は10kΩ、低温用の分圧抵抗は1kΩ 

## ADC
[MCP3208-BI/SL](https://www.digikey.jp/en/products/detail/microchip-technology/MCP3208-BI-SL/319444)を使用

# 解析プログラム

`analysis/analysis.py` が解析プログラム。`analysis/analysis.py`が存在するディレクトリにて `python analysis.py <inputfile.csv> <outputfile.csv>`を実行。

colabなどのnotebookで実行できるようにした「燃焼実験ログをグラフに.ipynb」も同梱しました（宮田）
