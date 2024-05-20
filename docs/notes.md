# Notes


## Assembly related (NASM)
-   I can set a register to zero by doing something like: 
    `xor ax, ax` where `ax` is the register.

-   When dividing _( `div` instruction)_, the result is in `ax`
    and the remainder in `dx`.

## OS System

### Real mode
-   In x86 assembly, the `ah` register is used to specify functions when calling interrupts. 
    For instance, with the    `0x10` interrupt, setting `ah` to `0x0e` will print a character. 
    For the `0x13` interrupt, setting `ah` to `0x00` will reset the floppy, 
    and setting `ah` to `0x02` will read sectors.

### Protected Mode
-   Looks like in PMode, the carriage return _( `0x0D` )_ gets displayed
    as a music note. For a new line, I only need a logic to take the new line feed
    _( `0x0A` )_ and go to next row.

### Hardware
-   Floppy disks have 2880 sectors, and here comes math!
    if you look here: 

    <img src="./visuals/cylinders.png" width=360>

    You can see that there are 16 tracks (16 tracks = 1 cylinder),
    but in reality there are 80 tracks in a 3.5" 1.44Mb Floppy Disk.
    Each track having 18 sectors, each "platter" having two
    sides, and each platter having 2 heads. But in a floppy disk
    we only have 1 platter _(meaning 2 sides, 2 heads)_.
    So if we do the math:

    `sectors count = number of tracks * number of sectors per track * number of heads * number of platters`

    Which translates to:

    `2880 = 80 * 18 * 2 * 1`

    So yeah, in a normal 3.5" 1.44Mb Floppy Disk, we got 2880 sectors (: .

-   **CHS to LBA:** Here is the equation:
    ```
    absolute sector 	= 	(logical sector / sectors per track) + 1
    absolute head   	= 	(logical sector / sectors per track) MOD number of heads
    absolute track 	    = 	 logical sector / (sectors per track * number of heads)
    ```