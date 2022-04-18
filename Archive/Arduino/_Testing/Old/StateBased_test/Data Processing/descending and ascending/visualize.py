import plotly.express as px
import pandas as pd

eulerfile = "ascending_euler.csv"
quatfile = "ascending_quaternions.csv"

# import the csv
df = pd.read_csv(eulerfile, header=None, names=['Roll', 'Pitch', 'Yaw'])
fig = px.scatter(df)
fig.show()

df = pd.read_csv(quatfile, header=None)
fig = px.scatter(df)
fig.show()
