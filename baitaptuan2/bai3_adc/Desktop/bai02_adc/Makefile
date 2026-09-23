TARGET = uart

CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CPU = -mcpu=cortex-m3 -mthumb

CFLAGS = $(CPU) \
	-Wall \
	-O0 \
	-ffunction-sections \
	-fdata-sections \
	-DSTM32F10X_MD \
	-DUSE_STDPERIPH_DRIVER \
	-I. \
	-ICMSIS/CM3/CoreSupport \
	-ICMSIS/CM3/DeviceSupport/ST/STM32F10x \
	-ISTM32F10x_StdPeriph_Driver/inc

LDFLAGS = $(CPU) \
	-T linker.ld \
	-Wl,--gc-sections \
	-specs=nosys.specs
SRC = main.c \
      system_stm32f10x.c \
      STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
      STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
      STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c \
      STM32F10x_StdPeriph_Driver/src/stm32f10x_adc.c

OBJ = $(SRC:.c=.o) startup.o

all: $(TARGET).elf $(TARGET).bin

$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

startup.o: startup.s
	$(AS) $(CPU) -c startup.s -o startup.o

clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).bin
