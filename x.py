import numpy as np
import matplotlib.pyplot as plt
import math


def get_LDR_resistance(v_mv):
    v = v_mv / 1000.0
    if v == 0:
        return 10000.0
    return (3.3 * 10000 / v) - 10000


def mv_to_lux(mv):
    R = get_LDR_resistance(mv)
    b = 6.12
    m = -0.8
    exp = (math.log10(R) - b) / m
    return 10**exp


SKIP = 10

t = np.load("logs/t.npy")
n_gropus = len(t) // SKIP
last = n_gropus * SKIP

t = t[:last:SKIP]
t -= t[0]

measured = (
    np.mean((np.load("logs/measured.npy")[:last].reshape(-1, SKIP)), axis=1).astype(int)
    + 0.5
)
target = np.mean(np.load("logs/target.npy")[:last].reshape(-1, SKIP), axis=1)
u = np.mean(np.load("logs/u.npy")[:last].reshape(-1, SKIP), axis=1).astype(int)


measured = [mv_to_lux(m) for m in measured]
target = [mv_to_lux(m) for m in target]

_, (ax1, ax2) = plt.subplots(2, 1, figsize=(9, 6))
ax1.plot(t, measured, label="measured")
ax1.plot(t, target, label="target")
ax1.set_xlabel("t (ms)")
ax1.set_ylabel("lux")
# ax1.set_ylim(6.5, 6.8)
ax1.legend()
ax1.grid()

ax2.plot(t, u, label="controller output")
ax2.set_xlabel("t (ms)")
ax2.set_ylabel("PWM signal (0-4096)")
# ax2.set_ylim(1100, 1200)
ax2.legend()
ax2.grid()

plt.show()
