import numpy as np
import pylab as pl


if __name__ == "__main__":
	
	signal = np.genfromtxt('example.txt', delimiter = ',')
	
	print signal
	
	signal = signal[:-1]
	
	
	window = 15
	
	newsig = []
	
	for i in range(0, len(signal)-window):
		newsig.append(np.sum(signal[i:i+window]))
	
	for i in range(0,window):
		newsig.append(0)
	
	
	pl.plot(signal)
	pl.figure()
	pl.plot(newsig)
	pl.show()
	