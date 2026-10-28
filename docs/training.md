# Training the model

Anzar's model is a NanoEdge AI n-class classification library, trained on
recordings from the pipe it will watch. Train on your own installation: the same
leak feels different on PVC and on copper, next to a pump or far from it.

## The three classes

| Class | What to record |
|---|---|
| `no_flow` | everything closed, the pipe still |
| `use` | every normal flow: taps, shower, a toilet refilling, a washing machine, the water heater |
| `leak` | a thin, continuous flow: a valve cracked slightly open, a dripping joint |

There is no "tap left running" class. A tap left open vibrates exactly like a tap
in use; the difference is that it never stops. So the model only names the flow,
and the firmware turns 20 minutes of uninterrupted `use` into a tap-left-running
alert.

Put every legitimate flow in `use`, including the faint or steady ones like a
cistern refilling or the water heater. Those are the flows most easily mistaken
for a leak, so the model needs to have seen them.

## 1. Record

Mount the unit exactly where it will stay, flash the datalog build
([firmware/README.md](../firmware/README.md)), then record each class:

```bash
pip install -r tools/requirements.txt
python tools/capture.py --port /dev/tty.usbmodem1103 --label no_flow --count 300
python tools/capture.py --port /dev/tty.usbmodem1103 --label use --count 300
python tools/capture.py --port /dev/tty.usbmodem1103 --label leak --count 300
```

The port is `ls /dev/tty.usbmodem*` on macOS, or a `COM` port on Windows. Each run
appends to `data/<class>.csv`, one window per line. At 115200 baud a window takes
roughly a second, so 300 windows take roughly five minutes.

Spread each class over several sessions: different taps, different strengths,
different times of day. Many short varied sessions beat one long one.

## 2. Build the library in NanoEdge AI Studio

1. Create an **N-Class Classification** project. Target: your Nucleo's MCU.
   Sensor: accelerometer, 3 axes.
2. Import each file from `data/` as its own class, named `no_flow`, `use` and
   `leak`. Each line is one signal of 512 samples x 3 axes, values interleaved
   x, y, z, separated by commas.
3. Run the benchmark, then check the result on a fresh recording session that was
   not used for training.
4. In Deployment, compile the library and download it. Note the ID the Studio
   gives each class.

## 3. Put it on the board

Copy `NanoEdgeAI.h` and `libneai.a` (plus `knowledge.h` if your version produced
one) into `firmware/nanoedge/`, then set these in
[`firmware/app/anzar_config.h`](../firmware/app/anzar_config.h):

| Setting | Value |
|---|---|
| `ANZAR_NEAI_V5` | `1` if there is no `knowledge.h`, otherwise `0` |
| `ANZAR_CLASS_ID_NO_FLOW`, `_USE`, `_LEAK` | the IDs from the Studio |
| `ANZAR_SIGNAL_LENGTH`, `ANZAR_AXES`, `ANZAR_NUM_CLASSES` | must match the library (512, 3, 3) |
| `ANZAR_MODE` | `ANZAR_MODE_DETECT` |

Build, flash, and watch the status lines on the serial port while you open and
close taps.

## Tuning

| Setting | Default | Notes |
|---|---|---|
| `ANZAR_TAP_RUNNING_MS` | 20 min | Fine for a home. On a farm, where irrigation runs for hours, raise it or every irrigation run is flagged. |
| `ANZAR_LEAK_CONFIRM_MS` | 30 min | Shorter catches leaks sooner and raises more false alarms. |
| `ANZAR_MIN_CONFIDENCE` | 0.60 | Windows the model is less sure about are ignored. |
| `ANZAR_VOTE_WINDOW` | 5 | Windows in the majority vote. |

The trained library is not committed to this repository. It is built from one
installation's recordings, so each installation should train its own.
