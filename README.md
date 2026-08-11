# stm32f411-template

Minimal template for STM32F411 development. Includes a basic `Makefile`, startup code, CMSIS headers, and linker script for building and flashing firmware using `arm-none-eabi-gcc` and OpenOCD.

### Hardware requirements

- STM32F411 (e.g. Black Pill) with a **25 MHz HSE crystal** — `common/src/osc.c` configures the PLL assuming this exact input frequency (PLLM=25 divides it down to a 1 MHz VCO input). A board with a different crystal needs different PLL settings.
- **USB**: PA11/PA12 wired to USB D-/D+ (OTG FS). VBUS sensing is left enabled (default HAL config), so **PA9 must be connected to VBUS** or the USB peripheral will never detect a session and enumeration will hang.
- **ST-Link/SWD**: SWCLK, SWDIO, GND, and 3V3 connected to the target for `make flash`.
- PA8 is optionally available as an MCO1 debug clock output (`osc_hse_debug()` in `osc.c`), if you want to probe the system clock with a scope/logic analyzer.

### Build and Flash (Firmware)

```bash
cd app
make        # build ELF, HEX, BIN
make flash  # program via ST-Link
make clean  # remove build artifacts
```

### Template structure

```
stm32f411-template/
├── app/
│   ├── src/            # source files, e.g. startup assembly
│   ├── include/        # project-specific headers
│   ├── Makefile        # build system
│   └── linker.ld       # linker script
└── common/
    ├── src/            # shared source files
    ├── include/        # shared headers
    └── cmsis/          # CMSIS headers
```

