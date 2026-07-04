import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib import mlab

# Simulated pipe-acoustic signals. A leak is a steady broadband hiss;
# normal flow is quiet with short, occasional bursts (a tap being used).
# This demonstrates the detection method, not field data.

FS = 8000
DUR = 2.0
N = int(FS * DUR)
t = np.arange(N) / FS
rng = np.random.default_rng(1)

def bandpass(x, lo, hi):
    X = np.fft.rfft(x)
    f = np.fft.rfftfreq(len(x), 1 / FS)
    X[(f < lo) | (f > hi)] = 0
    return np.fft.irfft(X, n=len(x))

def leak_signal():
    lo = rng.uniform(400, 700); hi = rng.uniform(1600, 2200)
    x = bandpass(rng.standard_normal(N), lo, hi)
    amp = rng.uniform(0.015, 0.04) if rng.random() < 0.15 else rng.uniform(0.12, 0.6)  # some leaks near the noise floor
    x *= amp / (np.std(x) + 1e-9)
    x *= 1.0 + 0.08 * rng.standard_normal(N)
    return x + 0.02 * rng.standard_normal(N)

def normal_signal():
    x = 0.02 * rng.standard_normal(N)
    r = rng.random()
    if r < 0.22:                                # steady appliance/pump hum: a real false-alarm trap
        hum = bandpass(rng.standard_normal(N), 600, 1500)
        return x + hum * (rng.uniform(0.1, 0.3) / (np.std(hum) + 1e-9))
    if r < 0.55:                                # a tap left running most of the window
        s = rng.integers(0, N // 3); e = rng.integers(2 * N // 3, N)
        burst = bandpass(rng.standard_normal(e - s), 300, 3000)
        x[s:e] += burst * (0.5 / (np.std(burst) + 1e-9))
    else:
        for _ in range(rng.integers(0, 3)):     # short taps
            s = rng.integers(0, N - 1200)
            e = min(N, s + rng.integers(400, 2500))
            burst = bandpass(rng.standard_normal(e - s), 300, 3000)
            x[s:e] += burst * (0.5 / (np.std(burst) + 1e-9))
    return x

def features(x):
    S, f, _ = mlab.specgram(x, NFFT=256, Fs=FS, noverlap=128)
    fe = S.sum(axis=0)
    fe = fe / (fe.max() + 1e-12)
    duty = np.mean(fe > 0.1)                  # part of time the sound is "on"
    variab = np.std(fe)                        # steady leak -> low, bursts -> high
    band = (f >= 500) & (f <= 2000)
    band_ratio = S[band].sum() / (S.sum() + 1e-12)
    centroid = (f[:, None] * S).sum() / (S.sum() + 1e-12)
    return [duty, variab, band_ratio, centroid, np.sqrt(np.mean(x ** 2))]

X, y, ex = [], [], {}
for i in range(250):
    xl, xn = leak_signal(), normal_signal()
    X.append(features(xl)); y.append(1)
    X.append(features(xn)); y.append(0)
    if i == 0:
        ex = {"leak": xl, "normal": xn}
X, y = np.array(X), np.array(y, float)

idx = rng.permutation(len(X))
cut = int(0.7 * len(X))
tr, te = idx[:cut], idx[cut:]
mu, sd = X[tr].mean(0), X[tr].std(0) + 1e-9
Xs = (X - mu) / sd

def fit(Xt, yt, lr=0.3, epochs=4000):
    Xb = np.hstack([np.ones((len(Xt), 1)), Xt])
    w = np.zeros(Xb.shape[1])
    for _ in range(epochs):
        p = 1 / (1 + np.exp(-Xb @ w))
        w -= lr * Xb.T @ (p - yt) / len(yt)
    return w

w = fit(Xs[tr], y[tr])
pred = ((1 / (1 + np.exp(-np.hstack([np.ones((len(te),1)), Xs[te]]) @ w))) >= 0.5).astype(int)
yte = y[te].astype(int)
tp = np.sum((pred==1)&(yte==1)); fp = np.sum((pred==1)&(yte==0))
fn = np.sum((pred==0)&(yte==1)); tn = np.sum((pred==0)&(yte==0))
f1 = 2*tp / (2*tp + fp + fn + 1e-9)
cm = np.array([[tn, fp], [fn, tp]])
print(f"F1 = {f1:.3f}")
print("confusion:\n", cm)

fig, ax = plt.subplots(2, 2, figsize=(11, 6))
ax[0,0].plot(t, ex["leak"], lw=0.5, color="#1C48B5")
ax[0,0].set_title("Fuite - signal"); ax[0,0].set_xlabel("Temps (s)")
ax[0,1].plot(t, ex["normal"], lw=0.5, color="#444")
ax[0,1].set_title("Normal - signal"); ax[0,1].set_xlabel("Temps (s)")
for a_, key, ttl in [(ax[1,0],"leak","Fuite - spectrogramme"),(ax[1,1],"normal","Normal - spectrogramme")]:
    a_.specgram(ex[key], NFFT=256, Fs=FS, noverlap=128, cmap="magma")
    a_.set_title(ttl); a_.set_xlabel("Temps (s)"); a_.set_ylabel("Fréquence (Hz)")
plt.tight_layout(); plt.savefig("echodrop_signaux.png", dpi=130); plt.close()

fig, ax = plt.subplots(1, 2, figsize=(11, 4.5))
m = y == 1
ax[0].scatter(X[~m,0], X[~m,1], s=14, c="#888", label="Normal")
ax[0].scatter(X[m,0], X[m,1], s=14, c="#1C48B5", label="Fuite")
ax[0].set_xlabel("Continuité du son (part du temps actif)")
ax[0].set_ylabel("Variabilité de l'énergie")
ax[0].set_title("Deux mondes séparés par le son"); ax[0].legend()
ax[1].imshow(cm, cmap="Blues")
ax[1].set_xticks([0,1]); ax[1].set_yticks([0,1])
ax[1].set_xticklabels(["Normal","Fuite"]); ax[1].set_yticklabels(["Normal","Fuite"])
ax[1].set_xlabel("Predit"); ax[1].set_ylabel("Reel")
ax[1].set_title(f"Detection (F1 = {f1:.2f})")
for (i,j),v in np.ndenumerate(cm):
    ax[1].text(j, i, str(int(v)), ha="center", va="center",
               color="white" if v>cm.max()/2 else "black", fontsize=13)
plt.tight_layout(); plt.savefig("echodrop_detection.png", dpi=130); plt.close()
print("figures written")
