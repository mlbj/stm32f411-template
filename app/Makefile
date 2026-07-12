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

# Flags
CFLAGS   = -mcpu=$(CPU) -mthumb -std=c99 -Wall -O0 -g3 -DSTM32F411xE $(INCLUDE)
ASFLAGS  = -mcpu=$(CPU)
LDFLAGS  = -T$(LSCRIPT) -nostdlib -Wl,--gc-sections

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
