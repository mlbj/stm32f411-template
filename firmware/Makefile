PROJECT  = stm32f411-template

# Directories
SRCDIR   = src
BINDIR   = bin
OBJDIR   = obj
INCDIR   = include
COMDIR   = ../common


# CPU
CPU      = cortex-m4

# Sources
SRC      = $(wildcard $(SRCDIR)/*.c) $(wildcard $(COMDIR)/src/*.c)
ASM      = $(wildcard $(SRCDIR)/*.s) $(wildcard $(COMDIR)/src/*.s)

# Objects
OBJ      = $(addprefix $(OBJDIR)/,$(notdir $(SRC:.c=.o)))
OBJ     += $(addprefix $(OBJDIR)/,$(notdir $(ASM:.s=.o)))

# Includes
INCLUDE  = -I$(INCDIR) -I$(COMDIR)/include -I$(COMDIR)/cmsis

# Linker script
LSCRIPT  = linker.ld

# Flags -g3 -fno-unwind-tables -fno-asynchronous-unwind-tables
CFLAGS   = -mcpu=$(CPU) -mthumb -std=c99 -Wall -O0 -DSTM32F411xE -DUSE_HAL_DRIVER $(INCLUDE) \
           -ffunction-sections -fdata-sections -fstack-usage -fno-exceptions \
		   -MMD -MP --static -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb 

# -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"



ASFLAGS  = -mcpu=$(CPU)   # keep it minimal, no -lgcc -lnosys  -nostartfiles
LDFLAGS  = -T$(LSCRIPT) -Wl,--gc-sections -lc --specs=nosys.specs --specs=nano.specs

# Tools
CC       = arm-none-eabi-gcc
AS       = arm-none-eabi-as
OBJCOPY  = arm-none-eabi-objcopy
SIZE     = arm-none-eabi-size --format=SysV
OBJDUMP  = arm-none-eabi-objdump
GDB      = arm-none-eabi-gdb
OCD      = openocd

# Final targets
ELF      = $(BINDIR)/$(PROJECT).elf
BIN      = $(BINDIR)/$(PROJECT).bin
HEX      = $(BINDIR)/$(PROJECT).hex

# Default target
all: $(ELF) $(BIN) $(HEX)

# Rules
$(ELF): $(OBJ) $(LSCRIPT)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@
	$(OBJDUMP) -D $@ > $(BINDIR)/$(PROJECT).lst
	$(SIZE) $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(COMDIR)/src/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.s
	@mkdir -p $(OBJDIR)
	$(AS) $(ASFLAGS) -o $@ $<

$(OBJDIR)/%.o: $(COMDIR)/src/%.s
	@mkdir -p $(OBJDIR)
	$(AS) $(ASFLAGS) -o $@ $<

# Flash
flash: $(ELF)
	$(OCD) -f interface/stlink.cfg -f target/stm32f4x.cfg \
		-c "program $< verify reset exit"

# Clean
clean:
	$(RM) -r $(OBJDIR) $(BINDIR)
