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