import numpy as np
import matplotlib.pyplot as plt
from distortion import applyThresholdDistortionInt32
from lowpassfilter import low_pass_filter

def generate_sin_wave():
    cycles = 2
    resolution = 48000
    length = np.pi * cycles * 2
    return np.sin(np.arange(0, length, length / resolution))

mywave = generate_sin_wave().tolist()

for i in range(0, len(mywave)):
    mywave[i] = round(mywave[i], 4)

plt.plot(mywave)
plt.ylabel("normal data")

mywave = low_pass_filter(mywave)

plt.plot(mywave)
plt.ylabel("normal data")
plt.show()
