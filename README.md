# Anzar

Acoustic leak detection for water pipes. A low-cost device that listens to water
pipes and finds hidden leaks.

*Anzar (ⴰⵏⵣⴰⵔ) is the Amazigh word for rain, and the name of the rain deity
invoked in North African drought rituals.*

Tunisia loses close to a third of its drinking water to leaks in old underground
pipes, and a leak stays invisible until a pipe bursts or a bill jumps. Anzar is
a small sensor that clips onto a pipe and listens. A leak makes a faint, steady
sound inside the pipe that people can't hear. Anzar picks it up, tells a leak
apart from normal water use, and sends an alert so it gets fixed before the water
is gone.

I'm from Tunisia, where water is scarce and a lot of it is lost this way, so this
is the problem I wanted to work on.

## How it works

Three parts: a contact microphone clamped to the pipe, a microcontroller that runs
the detection on-device, and a local alert. No cloud, no connectivity required —
the classification happens on the MCU itself, which is what makes the unit cheap
enough to put on a pipe and forget about.

The hard part is the software that decides "leak" or "normal" from the sound. It
looks at three things: how continuous the sound is, how much its energy varies over
time, and where its energy sits in frequency. A leak is steady and broadband; normal
use is short and bursty.

## Hardware

The signal path was prototyped on an ESP32 with a piezo contact microphone. For the
bench version I'm specifying an STM32 + MEMS microphone stack so inference runs on
the microcontroller with industrial-grade sensing, and evaluating ST's edge-AI
tooling for the on-device model. Sensor selection and the mechanical coupling to the
pipe are the open questions.

## The detection demo

`anzar_demo.py` builds simulated pipe signals (a steady leak hiss, normal taps,
and a steady appliance hum as a hard case), pulls out those features, and trains a
small classifier. On this simulated set it reaches an F1 around 0.88. Most of the
errors are the appliance hum mistaken for a leak, which is the real challenge and
the next thing to solve.

Run it:

```bash
pip install numpy matplotlib
python3 anzar_demo.py
```

It writes `anzar_signaux.png` (what the sounds look like) and
`anzar_detection.png` (how the two separate, plus the confusion matrix).

These are simulated signals to show the method works, not field recordings. Real
pipe audio will be noisier, and validating on it is the next step.

## Limitations

Early prototype, and I'd rather be precise about what that means. The detection runs
on **simulated** audio, not field recordings. No physical unit has been built yet, so
nothing here has met a real pipe. The F1 figure describes the method on synthetic
signals and should not be read as device performance.

Next steps, in order: record real leak and non-leak audio from a controlled rig,
finalise the sensor and microcontroller, build the first unit, and validate on a live
pipe.

## Note on the name

This project was previously called EchoDrop; it was renamed to Anzar in July 2026.

## License

MIT. See `LICENSE`.
