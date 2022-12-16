DATA = r'pdata.csv'
N_FEATURES = 14

LAYERS = [12, 6, 3] #[30, 50, 70, 30, 10]

# first neural network with keras tutorial
from numpy import loadtxt
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Add
from tensorflow.keras.layers import Layer


# load the dataset
dataset = loadtxt(DATA, delimiter=',')
print(dataset)
print('askdflkjsaldf')
# split into input (X) and output (y) variables
X = dataset[:, 1:N_FEATURES + 1] # CURRENTLY INPUTTING TRIVIAL PROBLEM DATA.....
y = dataset[:,N_FEATURES + 1]
print(X)
# define the keras model
model = Sequential()
model.add(Dense(LAYERS[0], input_shape=(N_FEATURES,), activation='relu'))
for i in LAYERS[1:]:
    model.add(Dense(i, activation='relu'))
# model.add(Add())
model.add(Dense(1, activation='sigmoid'))
# compile the keras model
model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])
# fit the keras model on the dataset
model.fit(X[:20000], y[:20000], epochs=1, batch_size=1)
# evaluate the keras model
# _, accuracy = model.evaluate(X, y)
# print('Accuracy: %.2f' % (accuracy*100))
print(len(X))
error = 0
wrong, tol = 0, 0.075
y_pred = model.predict(X[20000:])
for i, yp in enumerate(y_pred):
    error += abs(y[20000 + i] - yp)
    if abs(y[20000 + i] - yp) >= tol:
        wrong += 1

# print(y[:200], y_pred[:200])
print('Off by (avg):',error**0.5 / len(y))
print('Accuracy:', 1 - wrong / len(y))

# input1 = tf.keras.layers.Input(shape=(16,))
# x1 = tf.keras.layers.Dense(8, activation='relu')(input1)
# input2 = tf.keras.layers.Input(shape=(32,))
# x2 = tf.keras.layers.Dense(8, activation='relu')(input2)
# added = tf.keras.layers.arcTan([x1, x2]) ?????
# out = tf.keras.layers.Dense(1)(added)
# model = tf.keras.models.Model(inputs=[input1, input2], outputs=out)

# can we just do model.add(arcTan())?
# checkout the lambda layer (you can specify your own function...)
