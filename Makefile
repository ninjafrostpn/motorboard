FW_VER=3

TOOLCHAIN_DIR = ./libopencm3
PREFIX = arm-none-eabi
CC = $(PREFIX)-gcc
LD = $(PREFIX)-gcc
SIZE = $(PREFIX)-size
GDB = $(PREFIX)-gdb
OBJCOPY = $(PREFIX)-objcopy
OOCD = openocd

LDSCRIPT = stm32-mcv4.ld
OOCD_BOARD = oocd/mcv4.cfg

CFLAGS += -mcpu=cortex-m3 -mthumb -msoft-float -DSTM32F1 \
	  -Wall -Wextra -O0 -std=gnu99 -g -fno-common \
	  -I$(TOOLCHAIN_DIR)/include -DFW_VER=$(FW_VER)
LDFLAGS += -lc -lm -L$(TOOLCHAIN_DIR)/lib/thumb/cortex-m3 -L$(TOOLCHAIN_DIR)/lib \
	   -L$(TOOLCHAIN_DIR)/lib/stm32/f1 -lnosys -T$(LDSCRIPT) \
	   -nostartfiles -Wl,--gc-sections,-Map=mcv4.map -mcpu=cortex-m3 \
	   -mthumb -march=armv7-m -mfix-cortex-m3-ldrd -msoft-float

O_FILES = main.o led.o output.o usart.o analogue.o fw_ver.o
TEST_O_FILES = test.o led.o output.o

all: mcv4.bin mcv4_test.bin

test: mcv4_test.bin

include depend

mcv4.elf: $(O_FILES) $(LD_SCRIPT) $(TOOLCHAIN_DIR)/lib/libopencm3_stm32f1.a
	$(LD) -o $@ $(O_FILES) $(LDFLAGS) -lopencm3_stm32f1
	$(SIZE) $@

mcv4_test.elf: $(TEST_O_FILES) $(LD_SCRIPT) $(TOOLCHAIN_DIR)/lib/libopencm3_stm32f1.a
	$(LD) -o $@ $(TEST_O_FILES) $(LDFLAGS) -lopencm3_stm32f1
	$(SIZE) $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

depend: *.c
	rm -f depend
	for file in $^; do \
		$(CC) $(CFLAGS) -MM $$file -o - >> $@ ; \
	done ;

.PHONY: all test clean flash

flash: mcv4.elf
	$(OOCD) -f "$(OOCD_BOARD)" \
	        -c "init" \
	        -c "reset init" \
	        -c "stm32f1x mass_erase 0" \
	        -c "flash write_image $<" \
	        -c "reset" \
	        -c "shutdown"

debug: mcv4.elf
	$(OOCD) -f "$(OOCD_BOARD)" \
	        -c "init" \
	      #  -c "reset halt" &
	$(GDB)  mcv4.elf

clean:
	-rm -f mcv4.elf depend *.o
