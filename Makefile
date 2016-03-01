#
# Kurzanleitung
# =============
#
# make		-- Baut den Kernel.
# make all
#
# make install	-- Baut den Kernel und transferiert ihn auf den Server.
# 		   Das Board holt sich diesen Kernel beim nächsten Reset.
#
# make clean	-- Löscht alle erzeugten Dateien.
#


#
# Quellen
#
LSCRIPT = kernel.lds

OBJ = start.o 
OBJ += init.o 
OBJ += sys/sys.o 
OBJ += sys/exception.o 
OBJ += sys/systemIRQCntrl.o 
OBJ += drv/timer.o 
OBJ += lib/str.o 
OBJ += lib/triv-printf.o  
OBJ += drv/serial_drv.o 
OBJ += sys/except.o 
OBJ += main.o
OBJ += idleThread.o
OBJ += lib/queue.o 
OBJ += sys/threadCntrl.o 
OBJ += util/utilIRQ.o  
OBJ += sys/sleep.o 
OBJ += sys/syscall.o 
OBJ += sys/kprintf.o
OBJ += sys/mmu.o
OBJ += sys/mmu_asm.o


#
# Konfiguration
#
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -Wall -Wextra -ffreestanding -mcpu=cortex-a7 -O2
#LIBGCC := $(shell $(CC) -print-libgcc-file-name)

DEP = $(OBJ:.o=.d)

#
# Regeln
#
.PHONY: all 
all: kernel

-include $(DEP)

%.o: %.S
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

kernel: $(LSCRIPT) $(OBJ)
	$(LD) -T$(LSCRIPT) -o $@ $(OBJ) $(LIBGCC)

kernel.bin: kernel
	$(OBJCOPY) -Obinary --set-section-flags .bss=contents,alloc,load,data $< $@

kernel.img: kernel.bin
	mkimage -A arm -T standalone -C none -a 0x00100000 -d $< $@

.PHONY: install
install: kernel.img
	 cp $< /srv/tftp

.PHONY: clean
clean:
	rm -f kernel kernel.bin kernel.img
	rm -f $(OBJ)
	rm -f $(DEP)
