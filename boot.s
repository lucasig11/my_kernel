# set Magic number to 0x1BADB002 to be identified by the bootloader
# Magic: A fixed hexadecimal number identified by the bootloader as the 
# header(starting point) of the kernel to be loaded.
.set MAGIC,     0x1BADB002

# set flags to 0
# flags :- If bit 0 in the flags word is set, then all boot modules loaded along with 
# the operating system must be aligned on page (4KB) boundaries
.set FLAGS,     0

# set the checksum
# checksum :- which is used by special purpose by bootloader 
# and its value must be the sum of magic number and flags
.set CHECKSUM,  -(MAGIC + FLAGS)

# set multiboot enabled
.section .multiboot

# define the types to 'long' for each data defined
.long MAGIC
.long FLAGS
.long CHECKSUM

# set the stack bottom
stackBottom:

# define the maximum size of stack to 512 bytes
.skip 1024

# set the stack top which grows from higher to lower
stackTop:

.section .text
.global _start
.type _start, @function

_start:
# assign current stack pointer location to stackTop
    mov $stackTop, %esp
# call the kernel main source
    call kernel_entry

    cli

# put system in infinite loop
hltLoop:

    hlt
    jmp hltLoop
.size _start, . - _start
