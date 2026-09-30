CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m3 -mthumb -ffreestanding -O0
LDFLAGS = -mcpu=cortex-m3 -mthumb -nostdlib -T stm32f103.ld

all: firmware.bin

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

firmware.elf: startup.o main.o
	$(CC) $(LDFLAGS) startup.o main.o -o firmware.elf

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary firmware.elf firmware.bin

clean:
	rm -f main.o firmware.elf firmware.bin
