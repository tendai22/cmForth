CHAPTER II.  BUILDING A NC4000 COMPUTER
 
 
1.  Commerical Products Using NC4000 Chip
       One of the major design goals Chuck Moore wanted to achieve with the NC4000 chip was ease in constructing a high performance computer with minimal parts and resources.  All the necessary signals are brought out to the pins on the chip package, making it very easy to attach external memory and control circuitry to form a complete computer system.  There are several computers already being built and are available commercially, based on the NC4000 chip.  Many others have been custom built to solve specific problems.
       When the first wafer of NC4000 was diced at Mostek, about 80 good chips passed the functional tests.  Chuck Moore was the first person to take delivery from this batch.  He built a PC board to host these first chips and called the resulting computer 'Alpha Board' for obvious reasons.  Chuck used the Alpha Board to demonstrate the performance of NC4000 and developed the first package of software for it.  Alpha Board was Chuck's personal computer and was not distributed commercially.  The board is about 6" by 4" in size, containing about 8K words of memory, 2K words for two stacks, a few glue chips, and a clock.  It ran at 7 MHz maximum.  Chuck was able to generate color CRT displays with this board in many of his demonstrations.
       Novix took the rest of the batch from Mostek and built Beta Boards with them.  The Beta Boards were sold by Novix as development tools for NC4000 chips.  The boards measured 10" by 14".  It used high speed RAM's and ROM's and was specified to run at 7-8 MHz.  64K words of memory are on board, as well as two RS-232 serial ports and a SCSI disk interface.  The software delivered with the Beta Board was a version of poly-FORTH developed for Novix by Forth, Inc.  There are two versions of the Beta Board system, one using an IBM PC as a host computer and the other a stand alone system with its own terminal and disk drives.
       The second batch of NC4000 chips were made by Mostek and delivered to Novix in early 1986.  This batch used the same mask as the first batch, but the pin out and the package were modified.  Novix will build and sell Beta Boards using the new NC4000 chip.  Novix is also advertising complete NC4000 development systems with hard disk drives and floppy disk drives using Beta Boards as the brain.
       Chuck Moore updated the design of Alpha Board for the new NC4000 chips and is distributing a 'Gamma Board' kit, which consists of a NC4000 chip, a 6" by 4" PC board, and a pair of 2732's with the cmFORTH firmware.  This kit is sold through his company Computer Cowboys in Woodside, California.  The kit also contains 360 Augat Holtit press-fit sockets for mounting the chips.  User must supply four 6264 CMOS memory chips, a 74HC132 NAND gate chip, a 4 MHz clock, and a few resistors and capacitors to populate the board.  To use the board, one only has to supply 5 V to Vdd and hook a terminal to the pseudo RS-232 port on board.
       Software Composers, a company in Palo Alto, California, is also building a single board computer based on NC4000 chip.  This single board computer is called 'Delta Board' with a code name SC1000C.  It is very similar to Gamma Board in design but with a different layout.  The memory bus and the I/O bus are brought to a 72-finger edge connector with pertinent timing signals.  The bus structure makes it easy to add memory and peripheral devices to the single board computer.  It has 8K words of RAM, 4K of ROM, two stacks, and a serial port.  It also uses cmFORTH as its operating system.  The Delta Board is available both in assembled form or in kits. Software Composers is also producing supporting accessories such as power supply, back plane, expansion memory board, etc.
       Since it is rather straightforward to use the NC4000 chip to build a computer, we will surely see more NC4000 products and more applications in the near future.
.new
.5
　
　
2.   Build Your Own NC4000 Computer
       What I want to do here is to describe a typical design of NC4000 based computer.  By providing you with enough essential information on this design, you should be able to build a small computer using the NC4000 chip, or incorporate the design into your system to suit your application.
       This design is very similar to that of the Gamma Board and that of the Delta Board, as Chuck Moore provided the basic information in helping us develop our prototype board.  The schematics in the following sections are thus useful for users of either board.  The design is broken down into three major sections: the CPU, the stacks, and the memory sections.  Each section will be discussed in detail.
 
 
2.1.   The CPU Section
       The NC4000 chip, its timing, and I/O connections are shown in Fig. 2.1.  The memory interface to the main memory and two stacks will be elaborated later.  Here we shall only be concerned with the immediate control signals passed to the NC4000 chip.
       The CLK input is driven by a single phase CMOS clock.  The frequency of this clock depends on the memory speed and the maximum speed of the chip.  A general requirement of the clock is that the high period of the clock must be longer than 65 ns to allow NC4000 enough time to generate correct memory and stack addresses.  The low period of the system clock must be long enough for the memory to send data back on the data lines.  If we wanted the chip to run at its full speed, all memory chips should have an access speed less than 60 ns.  Using slower but cheaper memory chips with access time of about 100 ns, the maximum clock rate would be limited to less than 5 MHz.  4 MHz is a good choice with 150 ns memory chips.
       A CMOS crystal clock provides stable and accurate timing for most applications.  A simple RC oscillator can also be used as clock source.  However, because the clock is used to control the baud rate of the RS-232 serial port, a CMOS crystal clock might be more appropriate.  The duty cycle of the clock can vary from 40 to 60%.
       The clock signal is distributed throughout the entire computer, synchronizing other components with NC4000.  The low period of this clock is used to enable memory read or write.  At the rising edge of the clock, most input signals to NC4000 are latched by NC4000.  The memory and I/O write enable signals, i. e., WED, WER, WES and WEB, have different timing characteristics.  However, when the clock is low, these enable signals are assured to be valid.  The falling edge of the clock can thus be used to latch these enable signals into the memory or I/O device.
       The RST (reset) input is connected through a NAND gate to an RC network.  The RC network generates a reset sequence during power-up by holding RST input pin low for about 100 ms after 5 V power is applied to the chip.  When RST is released to 5 Volts, NC4000 will execute the RESET word stored in ROM memory at address 1000H.  The reset sequence assures that NC4000 is initialized properly and then enters into the text interpreter loop.
       The INT (interrupt) input is   normally pulled to 5 V through a pull-up resistor.  When this input is grounded and then released to 5 V, an interrupt request flip-flop is set inside NC4000.  If interrupt is enabled, NC4000 will make a subroutine call to memory location 32 where an interrupt service routine must reside.  Return from this subroutine call will then reset the interrupt flip-flop for the next interrupt.  When the interrupt request flip-flop is set by an external interrupt signal and the interrupt is disabled, the subroutine call to location 32 is suppressed but the flip-flop will remain set until interrupt is enabled and serviced.  Further interrupts before the flip-flop is reset will then be lost.
       Bit 8 (100H) in the Tristate Register of X-port (register 15) is the interrupt enable bit.
       In the NC4000P prototype chip, the use of interrupt is severely limited because interrupt must not occur during a two cycle memory instruction or during a jump instruction.  Interrupt will destroy the memory address in the address multiplexer and the interrupt service routine will lose its proper return address.  Interrupt can only be enabled during a sequence of single cycle, non-jump instructions.
.new
.5
       Fig. 2.1.   CPU Section of a NC4000 Computer
.new
 
 
 
2.2.  I/O Ports
       All sixteen B-port I/O lines and three of the X-port I/O lines--X1, x2 and X3-- are configured by the reset routine to be output lines and are pulled to ground.  As a result it is safe to leave all these I/O lines open.  Input lines to NC4000 must not be left floating because NC4000 tends to overheat if it finds unterminated inputs.
       Each of the output lines thus configured can draw 16 mA from the device connected to it.  If you are going to connect other devices to these ports, be sure that your device can withstand this abuse.  To use any of these lines as input to NC4000, you will have to modify the RESET routine in cmFORTH so that NC4000 will be configured correctly upon power-up or reset.
       X0 and X4 in the X-port are used to implement a serial communication port in this design.  This serial port allows the NC4000 chip to talk to a standard RS-232 terminal.  It is not a true RS-232 port because the voltage level is between 0 and 5 Volts.  However, it does communicate correctly with all standard RS-232 equipment.  X0 is the transmitter and X4 is the receiver.  X0 can drive the receiver of a RS-232 device directly.  X4 cannot be connected directly to a RS-232 transmitter because the transmitter swings to -9 volts.  The negative swing must be limited to protect the diode in NC4000.  The simplest solution is to put two 3K current limiting resistors between these two ports and the external RS-232 device.
 
 
 
2.3.   Main Memory
       In the design of a small computer with NC4000 as the central processing unit, there are two important constraints in the arrangement of memory.  One is that the reset routine must begin at location 1000H and some ROM memory must be put in the neighborhood of 1000H for a self booting system.  The other is that memory location 0 to 31 are local memory to NC4000, which can be accessed by single cell local memory instructions.  cmFORTH uses many of these local memory cells to store system variables for fast access.  Therefore, memory around location 0 must be RAM memory.
       If memory chips came in 4K byte sizes, the memory design would be straightforward.  We would decode memory in 4K pages and arrange ROM's and RAM's accordingly.  However, low cost, static CMOS RAM's are available either in 2K or 8K byte sizes.  The choice is either using many small chips or wasting space in large chips.
       Chuck Moore suggested the following decoding scheme which would fully utilize a pair of 8K byte RAM chips with a pair of 4K byte PROM chips by partially decoding the RAM memory space.  This decoding scheme is shown in Fig. 2.2.  A12 address line is inverted by a NAND gate.  The negated A12 signal is used to drive the positive chip select CE line of 6264 RAM chips and the negative output select ~OE of 2732 PROM chips.  Address line A13 is connected to the A12 pins on 6264 chips.  This allows the RAM's to respond to addresses from 0 to 0FFFH and from 2000H to 2FFFH, while the PROM's reside between 1000H and 1FFFH.
       This partial decoding method fully utilizes the 8K byte 6264 RAM chips.  The problem is that it would not allow more than 8K words of RAM in the system.  It is quite suitable for very small systems, but will make memory expansion very difficult.
       For a system which must use more than 8K words of RAM memory, a conventional decoding scheme shown in Fig. 2.3 is more appropriate.
       A 74HC138 1 of 8 decoder chip is used to select memory pages of 8K word size.  Address lines A13, A14, and A15 generate address select signals to enable memory pages.  In the lowest memory space or page 0, RAM must occupy addresses from 0 to FFFH and ROM must occupy addresses from 1000H to 1FFFH.  This is achieved by using negated A12 to enable ROM via ~CE and using A12 to select RAM via ~OE.  RAM chips above 2000H are selected by the 74138 decoder directly.
       In this decoding method, half of the 8K RAM in page 0 is wasted.  However, this system can accommodate 64K words of memory for a full blown NC4000 computer.
       It is important that the chip select (CS) pins of the memory chips must always be enabled by tying them to ground, because the time delay in memory chips from chip select to data available is too long to be useed with NC4000.  Using the chip enable (CE) and output enable (OE) to select appropriate chips allows these slow and inexpensive ROM and RAM chips to run at a rate much higher than that specified in the data sheets.
.new
.5
       Fig. 2.2.   Memory Decoding for a Small NC4000 Computer
.new
.5
       Fig. 2.3.   Memory Decoding for a Large NC4000 Computer
.new
 
 
 
2.4.   The Data Stack and the Return Stack
       NC4000 supports two external stacks, one for subroutine return addresses and one for data to be passed between subroutines or words.  Since these stacks have data paths independent of the main memory bus and I/O bus, NC4000 can access all these data buses simultaneously in a single clock cycle.  The most significant result is that a subroutine call can be performed in a single clock cycle.
       The data path to either stack includes a 16 bit wide data bus and an 8 bit wide address bus, in addition to the respective write enable line and the common clock signal.  The 8 bit width of the address bus limits the depth of the external stacks to 256 words.  For most application, two stacks of 256 words deep are more than adequate.  However, it is difficult to find cheap static memory of this depth.  Currently, the most readily available static RAM memory chips are either 2K bytes (6116) or 8K bytes (6264 or 6265).  It seems to be a great waste to use only 256 bytes in them, but that's life.
       In Fig. 2.4 the wiring of both the data and return stacks are shown schematically.  We use 6264 chips as an example, because they are also used for the main memory.  The circuit for smaller 6116 is almost identical and can be inferred easily.  Using either type of RAM chips, the timing and control are similar.  The chip select and output enable lines are always enabled by tying either to 5 V or to ground.  The chip enable lines (~CE) are enabled by the main clock during the low half of the clock period.  The write enable lines (WES and WER) are connected to the write enable lines (~WE) of the respective chips.
       Since NC4000 only supplies 8 address lines to either stack, the extra address lines on the RAM chips must be either pulled to 5 V or grounded.  If you absolutely must have more than 256 words for a stack, you can use the I/O lines in B-port or X-port to control the most significant address lines and swap the stacks in pages of 256 words.
.new
.5
       Fig. 2.4.   Data and Return Stacks of a NC4000 Computer
.new
 
 
 
3.   Circuit Board for NC4000 Computer
       From the above description, a single board computer using NC4000 as its central processing unit is very simple, with a chip count of about 10.  A 6" by 4" PC board is more than enough to host these chips.  For those who can handle wirewrap guns or tools comfortably, constructing such a computer will be a one-day project.  To avoid wiring errors, a printed circuit board is a much better way to go.  Since both Chuck Moore's Computer Cowboys and Software Composers are supplying PC boards with the NC4000 chip, it is worth the extra costs to buy their kits and do the assembly yourself.  You can order the kits Yrom from them directly:
 
       Novix Inc.
       10590 N. Tantau Ave.
       Cupertino, CA  95014
       (408) 253-6930
 
       Computer Cowboys
       410 Star Hill Road
       Woodside, CA  94062
       (415) 851-4362
 
       Software Composers
       210 California St., Suite F
       Palo Alto, CA  94306
       (415) 322-8763
 
       A final note on the power supply.  The NC4000 computer as described consumes about 200 mA at 5 V, with a 4 MHz clock.  The voltage of the power supply is not very critical.  It can vary from 4 to 6 Volts.  A small regulated 5 V power supply of any kind is adequate.  A wall mount 6 Volt power supply for video games should work well, too.  Chuck tried the ultimate power supply: 3 or 4 alkaline D cells.  He estimated that 3 D cells could last 30 hours and 4 cells, 50 hours.  If you had a Radio Shack 100 portable computer to substitute for a terminal, you would have a truly portable and powerful computer in a briefcase.
　