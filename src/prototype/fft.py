import math
import numpy as np
import time

def radix2CooleyTukey(data: list[float], n: int):
    if n == 1:
        return
    
    split1 = list()
    split2 = list()

    for idx in range(0, len(data)):
        if idx % 2 == 0:
            split1.append(data[idx])
        else:
            split2.append(data[idx])
        
    radix2CooleyTukey(split1, n / 2)
    radix2CooleyTukey(split2, n / 2)

    ang: float = (-2 * np.pi) / n
    w = complex(1, 0)
    wn = complex(np.cos(ang), np.sin(ang))

    i = 0
    size = n / 2
    while i < size:
        t = w * split2[i]
        data[i] = split1[i] + t
        data[i + int(n/2)] = split1[i] - t
        w *= wn
        i+=1
    return data

test_data = [-70, 56, -21, -30, -64, 69, -53, -40, 75, -67, 1, -14, -73, -6, -42, -92, 83, 86, -96, -31, 88, 53, 24, -26, -85, -16, 68, 42, -73, 27, -55, 19, -64, 98, 25, -12, -97, 64, -19, 87, 94, -27, -34, 47, 7, -8, 27, 79, 72, 82]
t1 = time.perf_counter_ns()
test_data = radix2CooleyTukey(test_data, 8)
t2 = time.perf_counter_ns()

print(t2-t1)
print(test_data)