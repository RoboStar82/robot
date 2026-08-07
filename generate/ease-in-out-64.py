#! /usr/bin/env python3

def cubicBezier1(t, p):
    return p[0] * (1 - t) * (1 - t) * (1 - t) + p[1] * 3 * (1 - t) * (1 - t) * t + p[2] * 3 * (1 - t) * t * t + p[3] * t * t * t

def easeInOutX(t):
    return cubicBezier1(t, [ 0.0, 0.42, 0.58, 1.0 ])

def easeInOutY(t):
    return cubicBezier1(t, [ 0.0, 0.0, 1.0, 1.0 ])

easeInOutValues = []
easeInOutCounts = []
easeInOutTable = []

for i in range(0, 128):
    easeInOutValues.append(0.00)
    easeInOutCounts.append(0)
    easeInOutTable.append(0)

for t in range(0, 10001):
    x = int(round(easeInOutX(t / 10000) * 127))
    y = easeInOutY(t / 10000)
    easeInOutValues[x] += y
    easeInOutCounts[x] += 1

for i in range(0, 128):
    easeInOutTable[i] = (easeInOutValues[i] / easeInOutCounts[i])

h = []

for x in range(0, 256):
    if x < 128:
        i = x - 127
        r = -easeInOutTable[-i]
    else:
        i = x - 128
        r = easeInOutTable[i]
    q = int(round(r * 64))
    # print('' + str(x) + '\t' + str(i) + '\t' + str(r) + '\t' + str(q))
    h.append(str(q))

print('{' + ', '.join(h) + '}')
