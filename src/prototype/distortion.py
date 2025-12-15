import numpy as np

def applyThresholdDistortionInt32(data: list[np.int32]):
    threshold = 0.3
    for i in range(0, len(data)):
        if data[i] > threshold:
            data[i] = threshold
        elif data[i] < -threshold:
            data[i] = -threshold