# AbdooOS Devlog

I'll try making this OS for the 3rd time, but this time 
I will try understanding by myself _(thank you [osdev](https://wiki.osdev.org/) <3)_

> Format in [DD/MM/YYYY]

## Structure and working of project [17/05/2024]

Let me first structure the project (folders, files)...

-   **CHALLENGE:** Try sometimes making the project from the terminal (`nano`, `touch`...).
    It'd be funny and cool!

## Coding a boot sector [18/05/2024]

I'm following a new [blog](http://www.brokenthorn.com/Resources/OSDevIndex.html), 
more documented.

I have a working boot sector, stage 1 only. Now I'm coding the 2nd stage.

**OOF!** After hours of coding _(about +4 hours)_, I finally ~~stole~~ coded stage2, 
and some assembly include files _(`gdt.inc`, `stdio.inc`, `A20.inc`).
The boot sector boots successfully using the File System. It searches, 
finds and executes stage 2 with success. It sets up stuff with success.
And finally, it enters protected mode, with success. Only one problem tho!
And this problem is: **I don't know how any of that works...** Even tho the blog 
is very, very detailed in a very good understandable way. Which goes against
the goal I had put at the benningin of the project. I shoud look at the code
and try understanding it.

Anyway, now I gotta make myself able to use a higher-level language, 
I'll **see** about that _(pun intended)_.

## Coding stage 2, includes and... kernel? [19/05/2024]

So I coded stage 2. It loads perfectly, goes to PMode. Cool.
I fixed some stuff about it too. Now I'm coding includes,
having them all in one file isn't really great...

Now I have these includes:
-   `A20.inc`: 20th address line related stuff.
-   `floppy16.inc`: sector reading, lba to chs converting...
-   `stdio.inc`: Output mostly, for now...
-   `gdt.inc`: Global Descriptor Table stuff, for PMode.

Damn! I've done so much more than the other times, and the project is still
only 700Kb! _(700Kb cuz of the `.git` dir)_.

**AGH!** I finally coded the `fat12` driver, and was able to execute the kernel.
BUT NOW IT DOESN'T REALLY EXECUTE THE KERNEL!!!!! So, in the kernel, I set up registers,
set up the stack, blah blah blah. But I also do two things: clear the screen and print text.
Easy to do, right? Clearing the screen works, BUT PRINTING DOESN'T WORK DAMMIT!!!1!!!!1!

After deep, ***deep*** thinking and examination, I realized the mistake:
The Kernel's origin **MUST** be at address 1Mb. And I realized "but it is!"
when I saw the `org 10000`. But after ***deeper*** examination, I realized 
that it must be `org 100000` with 5 zeroes! 
_(thank you "HexInspector" by 'Mateusz Chudyk' extension from VS Code (; )_

## Coding some C includes + trying to go to C [21/05/2024]

So I coded some C includes, *\*long pause\** without having entered C yet.
So now I'll try coding the boot sector's 2nd stage to load the C kernel instead.
Wish me luck (: .

## GCC Misery [22/05/2024]

I did two things until now:
-   Made a [**LiveJournal**](https://abdooowd.livejournal.com/).
-   **RAGED ABOUT GCC!!!!!**

Let me explain, so I was able to get all the C files _(only `string.c` and `main.c`)_,
and I was also able to compile them, but then `ld` threw a depression message:
`src/core/kernel/main.o: file not recognized: file format not recognized`.
Is it even coming from `ld`? Cuz the message was on a new line. Then I told myself:
"Maybe the file was compiled in a wrong way cuz, these kind of tools don't give a crap
about the file extension. They just want the **content**".
So I checked the terminal for output and... What?! It's compiling with `cc`?!!
I told him to compile with `i686-elf-gcc`!!! What the *\*quack\** is wrong!??!!?!!

Imma do an experience, I'll try running the commands ***manually***, with `i686-elf-gcc`.
Alright, Imma go do that.

Looks like it cannot find a certain `cc1`... The same thing happened when I was using
Cygwin. I'll go search that file.

OK! This one is gonna be long, but it's happy! So I recompiled the `i686-elf`
toolchain, but it didn't fix it... So I re-tried running the commands manually.
It worked fine! So what's the problem??!!
The problem was that it didn't execute `i686-elf-gcc`, it executed `cc` which
is just `gcc` but short. The problem was with the Makfile, but here's the weird part!
The rule to compile the object files was like this:

```
%.o: %.c $(H_SOURCES)
	$(CC16) $(C_FLAGS) $< -o $@
```

When I deleted the rule's content, it still executed `cc`! So there was probably 
a "cached" Makefile. When I deleted the Makefile and rewrote its content, it worked!
So happy of that!

Here is the best part: I can now execute a C-Kernel!!! And I did it by myself!
Let me explain: The boot sector's 2nd stage executes whatever kernel is at address
0x100000 _(which is 1Mib)_. So I scratched `kernel.asm` from the Makefile and now
I can execute a C-Kernel!!! Now I gotta restructure, refactor and organize the project.
Because to make all of this work I had to f* everything up.

Ok, I got problems with linking, compiling, filenames... But now I coded 
an entry point in C instead of Assmebly. Noice, progress.