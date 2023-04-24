 
2.   Features of NC4000 Chip
 
       The Novix NC4000 is a super high-speed processing engine which is designed to directly execute high level Forth instructions.  The single chip microprocessor, NC4000, gains its remarkable performance by eliminating both the ordinary assembly language and internal microcode which, in most conventional processors, intervene between the high level application and the hardware.
       A number of distinguishing features of this Forth engine on silicon are summarized as follows: 
.  16 bit high speed, HCMOS single chip Forth engine.
.  Direct execution of most Forth primitives in a single machine cycle without internal microcode.
.  One cycle subroutine calls with mostly zero cycle returns.
.  Supports 64K word memory, or 4M bytes with address extension port (the X-port).
.  Fully static operation permiting very low power consumption suitable for battery powered applications.
.  One cycle structured IF, ELSE, and LOOP instructions.
.  Multiplication, division, and square-root step instructions.
.  TIMES instruction allowing any instruction, including auto-incrementing/decrementing memory access, to be repeated once per cycle.
.  single instruction fetch and store from/to the local memory.
.  One cycle generation of hex FFFF.
.  257 element 16 bit hardware return stack with the top element in an on-chip I register.
.  258 element 16 bit hardware data stack with top two elements in on-chip T and N registers.
.  Two versatile I/O ports, both of which are bidirectional, maskable, auto-comparable, and programmable for either latched or tristate output.
.  Simultaneous access of return stack, data stack, main memory, and I/O port, concurrent with operation of ALU and shifter.
.  Execution of multiple Forth words in a single cycle instruction, e.g. "OVER + ;", yielding over 180 available instruction combinations, not including permutations of register addressing.
 
 
3.   External Data Paths
       NC4000 chip is housed in a 121 pin pin-grid package.  The pin layout is show is Fig. 1.1.  The names and function of the pin groups are shown in Table 1.1.
       The external data paths spawn by the large number of pins can be shown schematically in Fig. 1.2.  The pins can be grouped into five different functional groups: Main memory data and address, data stack data and address, return stack data and address, I/O ports, and timing/control.
 
 
       Table 1.1.   NC4000 Pin Names and Functions.
 
       A0-A15                Main Memory Address Bus
       B0-B15                I/O Port Bus
       CLK                   Processor Clock Input
       INT                   External Interrupt
       J0-J7                 Return Stack Address Bus
       K0-K7                 Data Stack Address Bus
       D0-D15                Main Memory Data Bus
       R0-R15                Return Stack Data Bus
       RST                   Processor Reset
       S0-S15                Data Stack Data Bus
       VDD                   Power Supply
       VSS                   Ground
       WEB                   I/O Port Write Enable
       WED                   Main Memory Write Enable
       WER                   Return Stack Write Enable
       WES                   Data Stack Write Enable
       X0-X4                 Address Extension Port
.new
.5
       Fig. 1.1.   NC4000 Pin Layout.
 
.new
.5
       Fig. 1.2.   External Data Paths of NC4000.
.new
 
 
3.1.  Main Memory
       The NC4000 controls and communicates with the main memory through 16 address lines, 16 data lines, and a write-enable line WED.  The memory addressing space is thus 64K words or 128K bytes.  The timing of the memory is synchronized by a single phase clock signal.  At the rising edge of the clock, data from the main memory is latched into the data memory port.  At the failing edge of the clock, memory address lines are stablized and addresses are available.  The main memory must put the requested data on the data lines before the rising edge of the clock.  The speed of the clock is thus dependent on the time required by the NC4000 chip to calculate the next address, which determines the length of the high period of the clock, and the time required by the memory to put valid data on the data lines, which determines the length of the low period of the clock.  The high period, as required by NC4000 is 65 ns at the minimum, and the low period depends on the memory used in the system.  Using high speed CMOS RAM with 50 ns access time, the clock speed can be pushed to about 8 MHz.  Using low cost CMOS static RAM with 150-200 ns access time, 4 MHz would be more appropriate.
       There are a few restrictions on the use of memory.  Although the chip can address 64K words of memory, only the lower 32K can be used as program memory because the MSB bit of an instruction is used to indicate a subroutine call.  However, the top 32K words can be addressed as data memory.  Since the hardware reset causes the chip to start executing the instruction located at memory location 1000H, it is mandatory that the bootstrap routine be programmed into this and the subsequent memory.  Thus ROM memory must occupy a block of memory space starting at 1000H.  Memory location 0 to 31 are special, in that these memory locations can be accessed by the NC4000 with single word instructions, while other memory locations can only be accessed by two word instructions.  Hence the memory starting at 0 is preferrably RAM memory if the software is able to take advantage of this hardware function.
       Whenever new data are to be written to the main memory, the WED (memory write enable) line will be brought low to coincide with the low period of the system clock.  This line should be tied with the write enable lines of the RAM memory so that new data can be written into RAM memory.
       This chip is intended to operate with static RAM memory chips that do not require a complicated memory refresh process.
       The memory space can be greatly enlarged if the 5 I/O lines of the X-port (Extension Port) are used as extra address lines to control the main memory.  In this manner the addressable memory can be expanded to 2M words or 4M bytes.
 
 
 
3.2.   The Data stack and the Return stack.
       A Forth engines requires at least two stacks, one to store return addresses for unfinished subroutines and the other to store parameters passed between subroutines.  Since the gate array with 4000 gates cannot support the necessary memory to host two stacks, the data and address lines of these two stacks are brought off the chip.  Each stack uses 16 data lines, 8 address lines, and a write enable line.  Since the address lines are only 8 bit wide, the depth of the stacks is limited to 256 words in the external stack memory.  If more than 256 words are pushed on to the external stack, the stack would wrap around like a circular buffer, and the data stored 256 words before the the current word would be lost.
       The depth of the stack, does not seem to be that restrictive because most programs use a depth of only 10 words each, on the return or data stacks.  The depth of the stacks will be a serious concern only when a recursive procedure is used.  There, one has to be careful that the depth of the stacks is not exceeded.
       The timing requirements for the stacks, are almost identical to those of the main memory.  Stack data are latched into the chip when the system clock makes a low to high transition.  The addresses to the stacks are stablized when the clock goes from high to low and the stack memory must put the data requested on the stack data lines before the clock goes from low to high.  Thus the same type of memory used in the main memory can be used for stacks.  There is little advantage to use higher speed memory for the stacks.
       The write enable lines WES and WER to the stack memories are low for the low period of the system clock when data are to be written to the stack memory.
       Since most commercial static CMOS memory chips have capacity larger than 256 bytes, it seems to be rather wasteful to use these chips for the stack without being able to fully utilize their capacity.  One way to take advantage of the extra stack space is to use the lines in the X-port to perform bank switching of the stacks.  This is very useful in supporting a multi-task system, in which each task has its own data and return stacks.  Task switching in this environment will be extremely fast since the operating conditions of each task are fully preserved in their individual stack space.
 
 
 
3.3.   B-Port and X-Port
       Two input/output ports are supported by the NC4000 chip: a 16 bit B-port (B for bus) and a 5 bit X-port (X for extension).  These two ports are fully programmable by the user through 4 internal registers for each port: a direction register to specify individual bits to be input or output, a mask register to protect individual bits from being written to, a tristate register to tristate output bits, and a data register to read from pins and write to pins.  Both ports can do I/O operations in single machine cycles as data registers are read or written.  These 21 programmable, high speed I/O lines make NC4000 chip an extremely versatile controller chip for all types of high throughput, real time control applications.
       The B-port write enable (WEB) line is low for the duration of the low period of the system clock when the output of the I/O ports is stablized.  Data on the input lines are latched into the data register at the rising edge of the system clock as usual.  Output data are available on the output pins about 100 ns after the rising edge of the system clock.
       An interesting behavior of the I/O port is that a set of output data latches in the data register can be written to even when the bits are assigned as input.  The data on the input pins are XOR with the bits in the output latches when read by the CPU.  If the data latch were loaded with ones, we can invert the input data on the fly as they are read through the data register.  This extra logic operation is programmable for each individual I/O pin.
 
 
 
3.4.   System Timing and Control
       The NC4000 is an asynchronous CPU chip which requires a single phase master clock.  All internal registers are static and the information held in them remain indefinitely while the clock is held low.  The higher limit of clock rate is set by the time required by the internal logic to calculate the address of the next instruction during the high period of the clock, which is about 65 ns in the prototype chip.  Using a symmetric crystal oscillator for the master clock, this limits the speed of NC4000P to about 8 MHz.  The low period of the clock is used mainly to wait for the external memory to put their date on the data lines.  If one uses slower memory chips, the low period of the clock must be stretched accordingly.
       The external clock signal is brought in via the CLK line.  As long as the low and high periods satisfy the above requirements, the rate and the duty cycle of the clock are not critical.  Either crystal oscillator or simple RC timing circuits can be used to generate the system clock signal.
       The reset signal RST if brought low will stop all internal operations in the chip.  When RST is brought back to high, NC4000 will jump to the reset memory location at 1000H and start executing the instruction fetched from that location.  The software bootstrap routine must be placed in that location for the system to work properly.
       There is also an interrupt input pin named INT.  When the INT pin is brought low, NC4000 will execute a call instruction to location 20H, where a service subroutine must be placed.  In the prototype chip, the use of this interrupt facility is severelly restricted because the interrupt can be serviced correctly only when a single cycle instruction is being executed.  The interrupt will lose its return address if it occurs during the first cycle of a two cycle instruction.  If precaution is taken to enable the interrupt only during a sequence of single cycle instructions, interrupt can be serviced correctly.
       Being a CMOS gate array, the power supply voltage to the VDD pins can range from 0 to 7 volts.  Nominal operation voltage is 5.0 Volts and typical current during operation is 10 mA.  The supply voltage might have to be higher than 5 volts if it is to be used with a higher clock rate of 7 MHz or higher.
.new
 
       Fig. 1.3.   Timing Diagrams of NC4000.
 
 
　
