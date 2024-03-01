import numpy as np
import matplotlib.pyplot as plt
import sys

y1 = np.load(sys.argv[1])
y2 = np.load(sys.argv[2])
t = np.load("TIME.npy")


plt.plot(t, y1, label=sys.argv[1])
plt.plot(t, y2, labe l=sys.argv[2])
plt.grid()
plt.legend()
plt.show()
