import math

def dft(data: list[float], n: int):
    angle = (2 * math.pi) / n
    Wn = complex(math.cos(angle), math.sin(angle))
    dftData = []

    for k in range(0, n):
        dftData.append(0)
        for i in range(0, n):
            dftData[k] += (data[i] * pow(Wn, (-1 * k * i)))
    return dftData