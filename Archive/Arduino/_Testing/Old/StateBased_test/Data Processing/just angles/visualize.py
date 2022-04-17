import plotly.express as px
import pandas as pd
import math

eulerfile = "angles_euler.csv"
quatfile = "angles_quaternions.csv"

# import the csv
df_euler = pd.read_csv(eulerfile, header=None, names=['Roll', 'Pitch', 'Yaw'])
df_quat = pd.read_csv(quatfile, header=None)

fig1 = px.scatter(df_euler*180/math.pi)
fig2 = px.scatter(df_quat)
fig1.show()
fig2.show()

# fig = px.line(x=["a","b","c"], y=[1,3,2], title="sample figure")
# print(fig)
# fig.show()
