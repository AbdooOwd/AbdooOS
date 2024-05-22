TOOLCHAIN=~/opt/cross/bin

QEMU=qemu-system-i386

ASM?=nasm
CC16?=$(TOOLCHAIN)/i686-elf-gcc
LD16?=$(TOOLCHAIN)/i686-elf-ld

C_FLAGS += -ffreestanding -c

ROOTFS_DIR=rootfs
BUILD_DIR=bin
SRC_DIR=src

OS_FILENAME=OS.bin


# functions ?
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))


C_SOURCES := $(call rwildcard,$(SRC_DIR),*.c)
C_OBJECTS := $(patsubst %.c,%.o,$(C_SOURCES))

H_SOURCES := $(call rwildcard,$(SRC_DIR),*.h)

OBJECTS := $(call rwildcard,$(SRC_DIR),*.o)

# Lazniess
ENTRY_OBJECT := $(SRC_DIR)/core/kernel/entry.o


.PHONY: all always build run clear

all: build run
build: os-image

os-image: $(BUILD_DIR)/$(OS_FILENAME)
bootloader: stage1 stage2

stage1: $(BUILD_DIR)/stage1.bin
stage2: $(BUILD_DIR)/stage2.bin

kernel: $(BUILD_DIR)/kernel.bin


$(BUILD_DIR)/$(OS_FILENAME): always bootloader kernel
	dd if=/dev/zero of=$@ bs=512 count=2880
	mkfs.fat -F 12 -n "ABOS" $@
	dd if=$(BUILD_DIR)/stage1.bin of=$@ bs=512 count=1 conv=notrunc
	mcopy -i $@ $(BUILD_DIR)/stage2.bin "::STAGE2.SYS"
	mcopy -i $@ $(ROOTFS_DIR)/test.txt	"::test.txt"
	mcopy -i $@ $(BUILD_DIR)/kernel.bin	"::KERNEL.SYS"


$(BUILD_DIR)/stage1.bin: $(SRC_DIR)/boot/stage1.asm
	$(ASM) $< -f bin -o $@

$(BUILD_DIR)/stage2.bin: $(SRC_DIR)/boot/stage2.asm
	$(ASM) $< -f bin -o $@

#$(BUILD_DIR)/kernel.bin: $(SRC_DIR)/core/kernel/kernel.asm
#	$(ASM) $< -f bin -o $@

$(BUILD_DIR)/kernel.bin: $(ENTRY_OBJECT) $(C_OBJECTS) | always
	$(LD16) -o $@ -Ttext 0x100000 $^ --oformat binary -Map $(BUILD_DIR)/info/linked.map


#
# 	Objects
#

# %.o: %.asm
# 	$(ASM) $< -f obj -o $@

$(ENTRY_OBJECT): $(SRC_DIR)/core/kernel/entry.c
	$(CC16) $(C_FLAGS) $< -o $@

%.o: %.c $(H_SOURCES)
	$(CC16) $(C_FLAGS) $< -o $@



# Processes

run: os-image
	$(QEMU) -fda $(BUILD_DIR)/$(OS_FILENAME)
# Organization stuff

always:
	mkdir -p $(BUILD_DIR) 
	mkdir -p $(BUILD_DIR)/info

clear clean:
	rm -rf $(BUILD_DIR)/*
	rm -rf $(OBJECTS)