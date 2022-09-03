# GSE電装 統合新型ロガー

# アナログ回路の詳細

## 推力測定

1つのセンサーに対応

[使用するロードセル](https://www.te.com/usa-en/product-FC2311-0000-0250-L.html)

作動増幅回路 (ゲイン:49.9) → ローパスフィルタ (R = 820 Ω, C = 0.1 μF ) → ADC

## 圧力測定 1

2つのセンサーに対応

[使用するセンサー](https://www.bdsensors.de/fileadmin/user_upload/Download/Datenblaetter_datasheets/DB_DMK331P_E.pdf)

分圧回路 (ゲイン: 0.5) → ボルテージフォロワ → ローパスフィルタ (R = 820 Ω, C = 0.1 μF ) → ADC

## 圧力測定 2

1つのセンサーに対応

[使用するセンサー](https://www.sayama.com/products/index.php/item?cell003=%E5%9C%A7%E5%8A%9B%E3%82%BB%E3%83%B3%E3%82%B5&cell004=%E3%82%B2%E3%83%BC%E3%82%B8%E5%9C%A7%EF%BC%88%E6%B6%B2%E4%BD%93%EF%BC%89&name=%E3%82%B2%E3%83%BC%E3%82%B8%E5%9C%A7%E8%A8%88+%E5%9C%A7%E5%8A%9B%E3%82%BB%E3%83%B3%E3%82%B5++FSV-010MP&id=3137&label=1)

ローパスフィルタ (R = 820 Ω, C = 0.1 μF ) → ADC

## 温度測定

熱電対により測定、高温用が2つ、低温用が3つ

## ADC
[MCP3208-BI/SL](https://www.digikey.jp/en/products/detail/microchip-technology/MCP3208-BI-SL/319444)を使用