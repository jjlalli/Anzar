# Hardware

| Part | Role |
|---|---|
| STM32 NUCLEO-F401RE | runs the firmware and the NanoEdge model |
| X-NUCLEO-IKS01A3 MEMS shield | LSM6DSO 3-axis accelerometer, read over I2C |
| Battery pack, 7-12 V | powers the board through VIN and GND |
| Enclosure | holds the boards |
| Clamps, straps or zip ties | hold the enclosure tight against the pipe |
| USB cable | flashing the firmware and recording training data |
| Buzzer (optional) | audible alert, on any free GPIO |

![Exploded view](anzar_exploded.svg)

## Assembly

The shield plugs straight onto the Nucleo's Arduino headers. Leave its jumpers in
their factory positions. The firmware reads the LSM6DSO through I2C1 on the
Arduino D14/D15 pins.

Using a different shield or accelerometer only changes
[`firmware/app/anzar_sensor.c`](../firmware/app/anzar_sensor.c).

## Power

In use, the board runs from the battery pack wired to VIN and GND. Powering the
board from VIN needs the power-source jumper set for an external supply: follow the
power supply section of ST's Nucleo-64 user manual (UM1724) for your board
revision, including what to do before plugging in USB at the same time.

## Mounting

Anzar feels the pipe through the enclosure, so the mechanical contact matters more
than anything else in the build:

- fix the enclosure firmly enough that it cannot rattle or slide
- keep the boards flat and fixed inside the enclosure so the shield moves with the
  pipe
- record the training data with the unit in its final position, and record again
  if you move it

![Mounted on a pipe](anzar_assembled.svg)
