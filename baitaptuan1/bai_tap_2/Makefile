CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m3 -mthumb -ffreestanding -O0
LDFLAGS = -mcpu=cortex-m3 -mthumb -nostdlib -T stm32f103.ld

all: firmware.bin

main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o main.o

startup.o: src/startup.c
	$(CC) $(CFLAGS) -c src/startup.c -o startup.o

firmware.elf: startup.o main.o
	$(CC) $(LDFLAGS) startup.o main.o -o firmware.elf

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary firmware.elf firmware.bin

clean:
	rm -f main.o startup.o firmware.elf firmware.bin firmware.map

flash: firmware.bin
	st-flash write firmware.bin 0x08000000
