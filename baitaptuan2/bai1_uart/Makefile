TARGET = main

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CPU = -mcpu=cortex-m3 -mthumb

CFLAGS = $(CPU) -Wall -O2 -g -std=c11
CFLAGS += -DSTM32F103xB -DUSE_HAL_DRIVER
CFLAGS += -Iinc -IInc -ISrc
CFLAGS += -IDrivers/STM32F1xx_HAL_Driver/Inc

LDFLAGS = $(CPU) -T STM32F103XB_FLASH.ld -specs=nano.specs -specs=nosys.specs -Wl,--gc-sections

SRCS = Src/main.c
SRCS += Src/system_stm32f1xx.c
SRCS += Src/stm32f1xx_it.c
SRCS += Startup/startup_stm32f103xb.s
SRCS += $(filter-out %_template.c, $(wildcard Drivers/STM32F1xx_HAL_Driver/Src/*.c))

OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:.s=.o)

all: $(TARGET).elf $(TARGET).bin

$(TARGET).elf: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash: $(TARGET).elf
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(TARGET).elf verify reset exit"

clean:
	rm -f $(TARGET).elf $(TARGET).bin

.PHONY: all flash clean
