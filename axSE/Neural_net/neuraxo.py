# first neural network with keras tutorial
from numpy import loadtxt
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Add
from tensorflow.keras.layers import Layer
import sys
sys.path.append('../util')
import stats

DATA = r'processed.csv'
N_FEATURES = 21
EPOCHS = 3 #30
BATCH_SIZE = 5 #5
ACTIVATION = 'sigmoid'
TRAIN = 20000

LAYERS = [21, 10, 10, 10, 5] #[100, 50, 25, 25, 12] 

# load the dataset
dataset = loadtxt(DATA, delimiter=',')
# split into input (X) and output (y) variables
X = dataset[:, 1:N_FEATURES + 1] # CURRENTLY INPUTTING TRIVIAL PROBLEM DATA.....
y = dataset[:,N_FEATURES + 1]
# define the keras model
model = Sequential()
model.add(Dense(LAYERS[0], input_shape=(N_FEATURES,), activation=ACTIVATION))
for i in LAYERS[1:]:
    model.add(Dense(i, activation=ACTIVATION))
# model.add(Add())
model.add(Dense(1, activation=ACTIVATION))
# compile the keras model
model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])
# fit the keras model on the dataset

model.fit(X[:TRAIN], y[:TRAIN], epochs=EPOCHS, batch_size=BATCH_SIZE)

error = 0
tols = [0.01, 0.05, 0.075, 0.1, 0.20]
wrong = [0] * len(tols)
y_pred = model.predict(X[TRAIN:])
for why, yp in zip(y[TRAIN:], y_pred):
    if (why < 0.5) != (yp[0] < 0.5):
        error += 1

print(error / (len(y_pred)))
print(error)

# stats.get_stats(y_pred, y[TRAIN:], tols=tols, max=1)