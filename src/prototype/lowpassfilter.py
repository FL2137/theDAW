import numpy as np

def low_pass_filter(data: list[int], alpha, x0 = None):
    out = np.zeros_like(data)
    yk = data[0] if x0 is None else x0
    for i in range(0, len(out)):
        yk += alpha * (data[i] - yk)
        out[i] = yk
    return out.tolist()