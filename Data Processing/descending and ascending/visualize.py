import plotly.express as px
import pandas as pd

csvfile = "ascending_euler.csv"

# import the csv
df = pd.read_csv(csvfile, header=None, names=['Roll', 'Pitch', 'Yaw'])
print(df)

fig = px.scatter(df)
fig.show()
