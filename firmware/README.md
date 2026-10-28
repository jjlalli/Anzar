# Firmware

The application code is in [`app/`](app). It drops into a standard STM32CubeIDE
project for a Nucleo board, so the project files CubeMX generates are not kept here.

| File | What it does |
|---|---|
| `anzar_config.h` | every setting: build mode, window size, class IDs, thresholds |
| `anzar_sensor.c` | LSM6DSO setup and window sampling (the only shield-specific file) |
| `anzar_model.c` | the NanoEdge calls (the only NanoEdge-specific file) |
| `anzar_detector.c` | vote smoothing and the leak / tap-left-running timers |
| `anzar_io.c` | serial port, LED, buzzer |
| `anzar_app.c` | ties it together: `anzar_app_init()` and `anzar_app_loop()` |

## You need

- STM32CubeIDE
- a NUCLEO-F401RE board and an X-NUCLEO-IKS01A3 shield
- ST's LSM6DSO driver, `lsm6dso_reg.c` and `lsm6dso_reg.h`, from the `lsm6dso-pid`
  repository on STMicroelectronics' GitHub (BSD-3-Clause)

## Set up the project

1. **File > New > STM32 Project**, open the **Board Selector** and pick
   **NUCLEO-F401RE**. Answer yes to initialising all peripherals in their default
   mode. This sets up USART2 (the ST-LINK serial port) and the LD2 user LED.
2. In the `.ioc` pinout, enable **I2C1** in Fast Mode (400 kHz) on **PB8 (SCL)** and
   **PB9 (SDA)**, the Arduino D15/D14 pins the shield uses. CubeMX sometimes puts
   I2C1 on PB6/PB7, so check.
3. Optional buzzer: set a free pin as GPIO output, give it the user label `BUZZER`,
   and set `ANZAR_USE_BUZZER` to 1.
4. Generate the code.
5. Copy `app/*.c` and the two driver `.c` files into `Core/Src`, and `app/*.h` and
   `lsm6dso_reg.h` into `Core/Inc`.
6. In `Core/Src/main.c`, inside the user-code blocks:

```c
/* USER CODE BEGIN Includes */
#include "anzar_app.h"
/* USER CODE END Includes */

  /* USER CODE BEGIN 2 */
  anzar_app_init();
  /* USER CODE END 2 */

    /* USER CODE BEGIN 3 */
    anzar_app_loop();
    /* USER CODE END 3 */
```

7. Build and run over USB. Open the ST-LINK serial port at 115200 baud: you should
   see `# Anzar`, `# mode: datalog`, then long lines of numbers, one per window.

While flashing and recording training data the board is on USB. Deployed, it runs
from the battery pack: see [docs/hardware.md](../docs/hardware.md#power).

## Two builds

Choose with `ANZAR_MODE` in `anzar_config.h`, or define `ANZAR_MODE=1` in the
project's preprocessor settings.

- **Datalog** (`ANZAR_MODE_DATALOG`, the default) streams raw windows for training.
  No model needed.
- **Detect** (`ANZAR_MODE_DETECT`) runs the NanoEdge library and the alerts. Train a
  model first: [docs/training.md](../docs/training.md).

## Adding the NanoEdge library

Put the files from the Studio's deployment zip in [`nanoedge/`](nanoedge), then in
the project properties add that folder to the **include paths**, and under the linker
**libraries** add `neai` with that folder as a **library search path**. The library
must be compiled in the Studio for the same MCU as your Nucleo.

## Reading the board

Serial port in detect mode:

```
# window=leak p=87% state=leak alert=none
# ALERT leak
```

LED:

| LED | Meaning |
|---|---|
| off | nothing to report |
| steady | leak |
| blinking | tap left running |
| fast blinking | the accelerometer or the library failed; the serial port says which |
