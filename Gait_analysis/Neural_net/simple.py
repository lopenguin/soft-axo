# first neural network with keras tutorial
from numpy import loadtxt
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Add
from tensorflow.keras.layers import Layer

DATA = r'simple_data.csv'
N_FEATURES = 2

LAYERS = [100]

# load the dataset
dataset = loadtxt(DATA, delimiter=',')
# split into input (X) and output (y) variables
X = dataset[:,0:N_FEATURES]
y = dataset[:,N_FEATURES]
# define the keras model
model = Sequential()
model.add(Dense(LAYERS[0], input_shape=(N_FEATURES,), activation='relu'))
for i in LAYERS[1:]:
    model.add(Dense(i, activation='relu'))
model.add(Add())
model.add(Dense(1, activation='sigmoid'))
# compile the keras model
model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])
# fit the keras model on the dataset
model.fit(X, y, epochs=10, batch_size=10)
# evaluate the keras model
_, accuracy = model.evaluate(X, y)
print('Accuracy: %.2f' % (accuracy*100))
