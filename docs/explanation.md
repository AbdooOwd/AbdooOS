# Explanation

## Introduction

I'll try explaining with **every detail** how the OS works.
How it boots, sets up and works.

## Boot Sector

The OS starts in **Real Mode**, which is a 16 bit processor mode.
It has many problems tho, like:
-   **Its limitations:** We can only use up to 1Mb of memory.
-   **No Memory Protection:** Memory can mess with herself
    which is not good for the OS.

But it still has good stuff, like the **BIOS** and its interrupts.
The BIOS, standing for Basic Input Output System, is a little 
program that is preinstalled on your ROM. It is copied to
the memory and executed. it is responsible for looking
for an OS to run, and it provides useful interrupts.
Interrupts are like signals, or functions we can use to do
cool stuff like printing text, getting keyboard input,
and a bunch of other stuff.

Let's go back to our boot sector. The boot sector has a `start`
label, which is the start point of our booting. From it we 
do a far jump to the `main` label, which is where all the booting
magic happens.

We do this far jump, because between those 3 bytes of instruction
and that `main` label, is the "BIOS Parameter Block" and
the "Extended Boot Record". That BPB and EBR are datas
and informations for the filesystem (because we use the FAT12 
file system in our OS), the BIOS and the boot sector.
I'll explain why: the boot sector is the first thing the BIOS
should execute. Knowing that the boot sector must be 512 bytes
long, we can't put a lot of stuff there--it wouldn't fit!
So instead we use the "Multistage Booting" method.
It consists of booting from a 512 bytes long boot sector, but SIKE!
That sector isn't actually **ALL** of the boot sector, this was
the **stage 1** of the boot sector. The **2nd stage** is in another
file, and we search and execute that file, `STAGE2.SYS`,
using the file system (FAT12).

