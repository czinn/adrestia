import tensorflow as tf
import keras
from keras import layers, optimizers
import matplotlib.pyplot as plt
import numpy as np
import json

def load_file(filename):
	f = open(filename, 'r')
	xs = []
	ys = []
	for line in f:
		data = json.loads(line)
		xs.append(data[0])
		ys.append(data[1])
	f.close()
	return (np.array(xs), np.array(ys))

train_data, train_labels = load_file('trains.txt')
test_data, test_labels = load_file('tests.txt')

def score_model(model):
	test_predictions = model.predict(test_data).flatten()
	correct, incorrect = 0, 0
	for x, y in zip(test_labels, test_predictions):
		if x == 0:
			continue
		is_correct = (x > 0) == (y > 0)
		correct += is_correct
		incorrect += not is_correct

	return correct / (correct + incorrect)

class NaiveModel:
	def __init__(self, weights):
		self.weights = np.array([weights]).transpose()
	
	def predict(self, test_data):
		return test_data @ self.weights

naive_model = NaiveModel([25 * 5, 10 * 1, 10 * 2, 0, 10, 5, 0, 25 * -5, 10 * -1, 10 * -2, 0, -10, -5, 0])
print(score_model(naive_model))

naiver_model = NaiveModel([25 * 5, 0, 0, 0, 0, 0, 0, 25 * -5, 0, 0, 0, 0, 0, 0])
print(score_model(naiver_model))

def build_model():
	model = keras.Sequential([
		layers.Dense(64, activation='relu', input_shape=[train_data.shape[1]]),
		layers.Dense(64, activation='relu'),
		layers.Dense(1)
	])

	optimizer = optimizers.RMSprop(0.001)

	model.compile(loss='mse',
								optimizer=optimizer,
								metrics=['mae', 'mse'])
	return model

model = build_model()

# Display training progress by printing a single dot for each completed epoch
class PrintDot(keras.callbacks.Callback):
	def on_epoch_end(self, epoch, logs):
		if epoch % 100 == 0: print('')
		print('.', end='')

EPOCHS = 200

history = model.fit(
	train_data, train_labels,
	epochs=EPOCHS, validation_split = 0.2, verbose=0,
	callbacks=[PrintDot()])

def plot_history(history):
	hist = history.history
	plt.figure()
	plt.xlabel('Epoch')
	plt.ylabel('Mean Abs Error')
	plt.plot(history.epoch, hist['mean_absolute_error'],
					 label='Train Error')
	plt.plot(history.epoch, hist['val_mean_absolute_error'],
					 label = 'Val Error')
	plt.legend()
	plt.ylim([0,1])
	plt.show()
	
	plt.figure()
	plt.xlabel('Epoch')
	plt.ylabel('Mean Square Error')
	plt.plot(history.epoch, hist['mean_squared_error'],
					 label='Train Error')
	plt.plot(history.epoch, hist['val_mean_squared_error'],
					 label = 'Val Error')
	plt.legend()
	plt.ylim([0,1])
	plt.show()

plot_history(history)

print(score_model(model))

model.save('model.h5', include_optimizer=False)
