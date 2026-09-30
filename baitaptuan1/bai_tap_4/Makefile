TARGET = button_toggle

CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m3 -mthumb -Wall -O0 -ffreestanding
LDFLAGS = -T linker.ld -nostdlib

all: $(TARGET).bin

main.o: main.c stm32f10x.h
	$(CC) $(CFLAGS) -c main.c -o main.o

startup.o: startup_stm32f10x_md.s
	$(AS) $(CFLAGS) -c startup_stm32f10x_md.s -o startup.o

$(TARGET).elf: main.o startup.o
	$(CC) $(CFLAGS) $(LDFLAGS) main.o startup.o -o $(TARGET).elf

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

clean:
	rm -f *.o *.elf *.bin