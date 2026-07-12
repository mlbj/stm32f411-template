# stm32f411-template

Minimal template for STM32F411 development. Includes a basic `Makefile`, startup code, CMSIS headers, and linker script for building and flashing firmware using `arm-none-eabi-gcc` and OpenOCD.

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

