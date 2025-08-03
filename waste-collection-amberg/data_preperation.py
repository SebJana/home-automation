import pandas as pd

df_01_06 = pd.read_csv("waste-collection-01_06.csv")
df_07_12 = pd.read_csv("waste-collection-07_12.csv")

df = pd.concat([df_01_06, df_07_12], ignore_index=True)
print(df.head(372))