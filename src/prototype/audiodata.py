import numpy as np
import matplotlib.pyplot as plt
from distortion import applyThresholdDistortionInt32
from lowpassfilter import low_pass_filter
import random

def generate_sin_wave(cycles: int = 2, resolution: int = 48000):
    length = np.pi * cycles * 2
    return np.sin(np.arange(0, length, length / resolution))

mywave = generate_sin_wave(2, 48000).tolist()


def create_noise(wave: list[int], max_noise_level: float, only_every_third: bool):
    noised_wave = []
    for idx in range(0, len(wave)):
        if only_every_third:
            if idx % 75 == 0:
                noised_wave.append(wave[idx] + random.uniform(-max_noise_level, max_noise_level))
            else:
                noised_wave.append(wave[idx])
        else:
            noised_wave.append(wave[idx] + random.uniform(-max_noise_level, max_noise_level))


    return noised_wave

for i in range(0, len(mywave)):
    mywave[i] = round(mywave[i], 4)

mywave = create_noise(mywave, 0.5, True)

plt.plot(mywave)
plt.ylabel("normal data")

mywave = low_pass_filter(mywave, 0.01)

plt.plot(mywave)
plt.show()
