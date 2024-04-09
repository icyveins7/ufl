from pyufl import PyuflDouble
import numpy as np
import scipy.signal as sps


# # This should fail
# ftaps = taps.astype(np.float32)
# ufl.set_up_taps(ftaps)

# Make simple qpsk signal
qpsk = np.array([1, 1j, -1, -1j], dtype=np.complex128)

# Create short signal at baud rate
T = 0.01
symbols = np.array([0, 1, 3, 2, 3], dtype=np.int32)
signal = qpsk[symbols]

# Upsample manually
up = 4
signal_up = np.zeros(up * signal.size, dtype=signal.dtype)
signal_up[::up] = signal

# Filter manually
taps = sps.firwin(8, 0.25)
filtered = sps.lfilter(taps, 1, signal_up)

# Interpolate manually
t_orig = np.arange(filtered.size) * T/up
print("Upsampled time vector: ")
print(t_orig)
print("\n")
t = np.array([-0.0001, 0.001, 0.012, 0.041, 0.05])
y = np.interp(t, t_orig, filtered, left=0, right=0)
print("\n".join(["%s, %s" % (str(i.real), str(i.imag)) for i in y]))

# Test our extension
ufl = PyuflDouble()
ufl.set_up_rate(up)
ufl.set_threads(2)
ufl.set_up_taps(taps)
print("Threads: %d" % ufl.get_threads())
print("Upsample rate: %d" % ufl.get_up_rate())
uflout = np.zeros_like(y)
ufl.interpolate_numpy(
    signal,
    T,
    t,
    uflout
)
