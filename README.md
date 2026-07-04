# EchoDrop

A low-cost device that listens to water pipes and finds hidden leaks.

Tunisia loses close to a third of its drinking water to leaks in old underground
pipes, and a leak stays invisible until a pipe bursts or a bill jumps. EchoDrop is
a small sensor that clips onto a pipe and listens. A leak makes a faint, steady
sound inside the pipe that people can't hear. EchoDrop picks it up, tells a leak
apart from normal water use, and sends an alert so it gets fixed before the water
is gone.

I'm from Tunisia, where water is scarce and a lot of it is lost this way, so this
is the problem I wanted to work on.

## How it works

Three parts: a contact microphone on the pipe, an ESP32 that runs the detection,
and an alert (a buzzer or a WiFi notification). The hard part is the software that
decides "leak" or "normal" from the sound. It looks at three things: how continuous
the sound is, how much its energy varies over time, and where its energy sits in
frequency. A leak is steady and broadband; normal use is short and bursty.

## The detection demo

`echodrop_demo.py` builds simulated pipe signals (a steady leak hiss, normal taps,
and a steady appliance hum as a hard case), pulls out those features, and trains a
small classifier. On this simulated set it reaches an F1 around 0.88. Most of the
errors are the appliance hum mistaken for a leak, which is the real challenge and
the next thing to solve.

Run it:

```bash
pip install numpy matplotlib
python3 echodrop_demo.py
```

It writes `echodrop_signaux.png` (what the sounds look like) and
`echodrop_detection.png` (how the two separate, plus the confusion matrix).

These are simulated signals to show the method works, not field recordings. Real
pipe audio will be noisier, and validating on it is the next step.

## The device

- `echodrop_model.scad`: parametric 3D model of the casing and clip. Open in
  OpenSCAD, export STL.
- `echodrop_modele_onshape.md`: step by step to build the CAD model in Onshape.
- `echodrop_blender.py`: builds the model and renders it in Blender.
- `echodrop_render.png`: a render of the device on a pipe.
- `echodrop_dispositif.svg`, `echodrop_comment_ca_marche.svg`: diagrams.

## Limitations

Early prototype. The detection runs on simulated audio, and the physical device
isn't built yet. Next steps: record real leak and non-leak audio, build a first
unit on an ESP32, and test it on a real pipe.

## License

MIT. See `LICENSE`.
