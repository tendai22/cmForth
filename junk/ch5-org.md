## 5.   The Instruction Set of NC4000

Let's think of how a conventional computer interacts with its users and the layers the computer has to go through between accepting a command from the user and actually performing a function inside the computer.  The command is issued to the operating system, which calls a compiled program into memory for execution.  The program would usually be constructed from a set of statements written in either a high level language or in assembly language.  This program would have to be compiled or assembled into a set of machine instructions.  When the machine instructions are executed, microcode inside the CPU are invoked which actually do the dirty work of operating the gates and shuffling the bits.  Thus there are at least 5 levels of interpretation.  The NC4000 architecture reduces this type of interpretation to only two levels, user commands and machine instructions.  The greatly reduced complexity between the user and the actual action of the computer is the most important reason for the speed and the versatility of NC4000 chip.

NC4000 is a 16 bit microprocessor.  Its basic data elements and instructions are all in 16 bit words or cells.  The instructions are sometimes called 'external microcode' in the sense that the instruction decoder would take individual bits in the instruction and perform individually assigned functions in parallel.  It does not need another layer of microcode to perform the designed functions of an instruction.  The side benefits are that many Forth instructions can be performed in a single machine cycle.

There has been many projects implementing Forth engines in hardware.  All of these designs have attempted to encode individual Forth words into single machine instructions.  They were shown to be much faster than Forth engines implemented in software because of the reduced overhead in NEXT, NEST, and UNNEST instructions and in the operation of the stacks.  Chuck Moore went far beyond them in NC4000.  What he attempted was to find the simplest way to controll the stacks and perform the operations while only using the top-most elements on the stack.  He discovered that many of these functions can be performed independent of one another.  Pushing or popping the stacks, arithmetic/logic operations, accessing main memory, and input/output are operations in different domains of the microprocessor.  They do not have to be performed serially.  These distinct, almost independent domains can be controlled by the very limited number of bits in a 16 bit instruction similar to bits in microcode.  Thus it is possible to perform many functions in a single machine cycle, instead of using many machine cycles to perform one function, such as in all conventional microcode based microprocessors.

.new
 
 
## 5.1.   Classes of NC4000 Instructions

There are four major classes of instructions in NC4000:

the subroutine calls, the I/O and memory instructions, the branch and loop instructions and the ALU instructions.  The class of any instruction is decoded in the most significant four bits of the machine instruction, as shown in Fig. 1.5.
 
Fig. 1.5.  Decoding of NC4000 Instructions.
``` 
Call                          0 a a a a a a a a a a a a a a a
ALU                           1 0 0 0 x x x x x x x x x x x x 
Branch                        1 0 b b a a a a a a a a a a a a
Memory                        1 1 x x x x x x x x l l l l l l
       a: address    b: branch     l: literal    x: control
```
 
Bit 15 is truly the most significant bit in the NC4000 machine instruction.  If it is zero, the instruction is a subroutine call and the rest of the instruction contains a 15 bit subroutine address.  Zero in this bit position triggers the subroutine threading mechanism in NC4000.  The program counter is pushed onto the return stack, i.e., copied into the I register.  The 15 bit address in the instruction is moved into the address multiplexer A.  At the beginning of the next machine cycle, the instruction stored at that address will be fetched for execution.  The program counter P will be pointing at the next instruction in that subroutine.

Using bit 15 to encode a subroutine call has only one drawback--it can only call subroutines in the lower 32K word memory.  The upper 32K word addressible memory cannot be used to store executable programs.  It was a very serious trade-off in the design of the chip.  The most important argument to support this trade-off is that Forth programs written for NC4000 can be extremely compact because of the single cycle subroutine calls and the condensation of many functions into a single instruction.  Many large programs are needed to fill up the 32K word program memory.  By the time memory requirements exceed 32K words, we will probably have a 32 bit Novix chip to take care of those stupendous programs that result from lazy-minds or uncommunicative programming teams.

When bit 15 is set, bit 14 is used to distinguish IO/memory instructions from ALU/branch instructions.  When bit 14 is zero, the next two bits are used to decide whether the rest of the instruction is to be decoded as an ALU instruction or used as a 12 bit branch address.  When bit 14 is one, then the rest of the instruction will be decoded to determine the type of I/O or memory instruction and how the I/O or memory is to be accessed.
 
 
       Fig. 1.6.   NC4000 Instruction Formats
```　
Call                          0          address
ALU                           1 0 0 0  ALU   Y Tn ; SA D %SLSR
IF (conditional branch)       1 0 0 1        address
LOOP                          1 0 1 0        address
ELSE (unconditional branch)   1 0 1 1        address
Literal Fetch                 1 1 0 0  ALU   Y Tn ;  literal
Literal Store                 1 1 0 1  ALU   Y Tn ;  literal 
Memory Fetch                  1 1 1 0  ALU   Y Tn ;  literal
Memory Store                  1 1 1 1  ALU   Y Tn ;  literal
```
Another way of classifying the NC4000 instruction set is shown in Fig. 1.6.  In this figure, the non-subroutine call instructions are classified according to the instruction type field, bits 12-14.  In this figure, all the bits which perform specific functions are named and placed in their respective bit positions.  We shall discuss each of these instruction types in great detail in later sections.  Only a few general comments will be made here, as an overview of this instruction set.

In all I/O, memory, and ALU instructions, types 4-6 and type 0, bit 5 is called the return bit.  This bit, when set, will cause a subroutine return, in addition to whatever the instruction may otherwise do.  Therefore, a subroutine return in NC4000 can be a zero cycle operation, since it gets a free ride if the last instruction in a subroutine is an IO/memory or ALU instruction.  It would be very difficult to optimize a subroutine return instruction any further.

In the prototype NC4000P chip, however, this return bit should not be tagged to a two cycle memory instruction, because the return operation will interfere with the memory fetching.  The memory address will be replaced by the return address from the return stack.  This is not a problem with single cycle memory instructions as there is no conflict in the use of the address multiplexer.

In IO/memory and ALU instructions, bits 9 to 11 in the ALU field determines the function of the ALU section on the chip.  Thus a free ALU operation can be tagged on to an IO/memory operation.

An ALU operation requires two operands.  One of the operands is always taken from the Top register T and the other operand is usually selected from an internal register, specified by a two bit field Y, bits 7 and 8.  In most instances, the Y field is zero, which selects the Next register N as its operand.

External data stacks are controlled by two bits: Tn bit at bit 6 and SA bit at bit 4 in an ALU instruction, or bit 14 in a memory instruction.  Tn bit, if set, copies the contents of T into N at the beginning of an instruction.  The SA, stack active bit, signals the external stack to perform a push or a pop operation.  If both Tn and SA bits are set, old T register is copied into N register and the contents of the N register is pushed on the external data stack.  If Tn is zero and SA is set, then the top element on the data stack is popped back into the N register and the data in N register is lost.  The combination of control bits in the ALU field, the Y selector, and the Tn and SA bits allow the NC4000 to perform most Forth ALU operations and stack operations, and as well as combinations of the ALU and stack operations.

In a memory instruction, the least significant 5 bits constitute a literal field, which contains a small integer from 0 to 31.  This literal is used to represent different types of information needed by the memory instruction.  In a short literal instruction, the small literal is pushed on the data stack as an integer.  In an internal register accessing instruction, it selects one of the registers to be accessed.  In a local memory instruction, it is the address of a local memory (the first 32 words in the main memory).  These local memory words can thus be accessed by a single instruction.  In the extended memory instructions, it supplies the page number to be put out on the X-port, to select one of the 32 memory pages for the memory instruction to access.  In many instructions, this field is not needed and must be cleared to zero.

In a branch instruction, bits 12 and 13 determine which type of branch it is and the lower 12 bits supply the target address.  The 12 bit address field specifies an absolute address within the 4K word page which contains the branch instruction.  It is thus impossible to branch across a 4K word boundary.  The programmer must be aware of this property in the branch instructions when he is close to a page boundary.

A 1 in the two bit field (bits 12 to 13) indicates an IF instruction, which does a conditional branch to the following 12 bit address.  The branch condition is taken from the Top register T.  If T register is zero, the IF instruction becomes a DROP.  A 3 in this field  indicates an unconditional branch or an ELSE instruction.  The 12 bit address in the address field is always taken as the address of the next instruction.  If a 2 is in this two bit field, the instruction is a NEXT instruction, which will decrement the I register--containing the loop index--and will branch to the 12 bit address if I is not 0.  When I is decremented to zero, the conditional end of loop, the NEXT instruction pops the index off the return stack and terminates the loop.

The last comment about this rather complicated instruction set in NC4000 is that not all combinations of control bits can generate meaningful instructions.  Certain combinations simply do not make sense at all and other combinations cause conflicting use of the registers or data paths and the results are not always predictable.  In addition, defects in the prototype NC4000P precludes some instruction or combinations of bits and those instructions should not be used.  In Table 1.7-8, we have collected all the valid ALU instructions and instruction combinations that can be safely used in the NC4000P chip.  Many of these restriction will be removed when the production chip becomes available.
 
 
## 5.2.   The ALU Instructions

ALU instructions are the most complicated class of instructions in the NC4000 chip because all of the 12 lower bits in the instructions are decoded to perform simultaneous functions.  A firm grasp on the use of the individual bits and their interactions is essential in understanding the inner mechanism of this chip.  Understanding will lead to an appreciation for the power of these instructions which can compress several Forth words into one machine instruction.  What we hope to do in this section is to go through each field and their function A number of examples will also be given to illustrate how the field and bits can be combine to form multiple function instructions.  With this information and examples, you will probably be able to decode other valid instructions and to visualize how they would work.

A more detailed data path diagram for the ALU section of the NC4000 can be of great help in explaining the inner mechanism of the ALU instructions, as shown in Fig. 1.7.

The ALU performs arithmetic and logic operations using operands supplied to it from the T register and the Y port.  The function of the ALU is specified in the ALU field in the instruction, bits 9-11.  8 different actions can be performed on the two operands as shown in Table 1.3.
 
 
       Table 1.3.   ALU Code and Function
```　
       ALU Code              Function
          0                           Pass T
          1                         T AND Y
          2                         T - Y
          3                         T OR  Y
          4                         T + Y
          5                         T XOR Y
          6                         Y - T
          7                          Pass Y
```
.new
.5
       Fig. 1.7.   Data Paths and Registers in the ALU Section
.new
.3

The two bit Y field, bits 7 and 8, controls the multiplexer which selects one of four registers as the source for the Y port as in Table 1.4.
 
       Table 1.4.   Y-Port Selector
``` 
       Y Code        Source to Y Port
          0                  N register
          1                  N register with carry
          2                  MD Multiplier/divisor register
          3                  SR Square-root register
```
Normal operations use the N register as the Y-port operand.  The N register with carry is selected when doing extended precision arithmetics.  MD register is used to store multiplier in multiplication operations or divisor in division.  Both the MD and SR registers are involved when a square-root operation is performed.

Tn, bit 6, operates a switch which connects the N register to the output of T register.  The contents of the T register is copied into the N register at the beginning of an ALU machine cycle if Tn bit is set.  If T is passed through the ALU unit unchanged and Tn bit is set, the net result is a DUP operation.  If Tn is 0 in this case, the net result will be a NOP operation.  If N is selected as the input to the ALU unit and passed through it unchanged, a DROP DUP operation will be performed when Tn is 0 and a SWAP will be performed when Tn is 1.

The Stack Active bit SA, bit 4 in the instruction, activates the external data stack in the sense that either the contents of N register is pushed on the external stack or the top element on the external stack is popped into N register.  However, the exact involvement of the external stack in this instruction also depends upon the state of Tn bit.  The stack action can be summarized in Table 1.5.
.new
 
 
       Table 1.5.   Data Stack Code and Functions
```
       SA      Tn            Stack Function
       0       0             ALU result to T.  N not changed.
                               No stack action.
       0       1             ALU result to T.  Old T to N.  No
                             stack action.
       1       0             ALU result to T.  External stack
                             popped into N register.
       1       1             ALU result to T.  Old T to N.  Old
                             N pushed on external stack.
``
 
It would be more pleasing if an independent bit were assigned to specify the direction of the stack activity besides the Tn bit.  However, it was found that the limited combinations of these two bits are sufficient to implement most of the stack operations required by the Forth language, while many more can be synthesized in conjunction with other activities in the ALU section.  The fact that these two bits are not in a contiguous field sometime makes it difficult to associate the instructions with their stack effects.  These were the trade-offs the designer had to make and the users have to live with them.

Bit 5 is the almighty return bit.  When this bit is set, a return from subroutine function will be triggered even as the ALU and stack functions are performed concurrently.  The return bit undoes the subroutine call, as executed by the Call instruction.  The return address on top of the return stack or the I register is popped into the address multiplexer A.  The next instruction executed will be the instruction following the Call instruction and the execution sequence will resume.  The return bit can be tagged to any ALU instruction.  Thereby a free return is generated without an explicit return instruction.  If this return bit is zero, execution will continue with the next instruction whose address is in the program counter P.

At the bottom of the ALU unit, there is a shifter which can shift the results from the ALU right or left by one bit before storing the results into the T register.  The shifter is controlled by the three LSB bits in an ALU instruction:D, SL, and SR bits, or bits 3, 1 and 0, respectively.  The bit patterns and their functions are shown in Table 1.6.
.new
　
 
       Table 1.6.   Shift Code and Function
``` 
       D    SL   SR          Function
       0      0     0           No shift.
       0      0     1           16 bit shift right.
       0      1     0           16 bit shift left.
       0      1     1           Sign extension of N into T.
       1      0     0           Not valid.
       1      0     1           32 bit shift right.
       1      1     0           32 bit shift left.
       1      1     1           Not valid.
```
The bit pattern 100 and 111 above, are not valid in the prototype chip.  The designed function of the 100 pattern is to shift the N register left one bit.  The designed function of the 111 pattern is to shift the N register right by one bit and extend its sign into the T register.  Chip defects in the prototype cause these functions to behave erratically.

Bit 2, the % or divide bit, is used only in the three divide instructions: the divide step /', the last divide step /", and the square-root step S'.  When it is set, a conditional subtraction is performed.  If the subtraction does not generate a carry, the difference is passed to the T register.  If a carry is generated, meaning that the divide step should not be performed, the results of subtraction is not written to T register.  Division and square root can be implemented by these conditional subtraction steps.

These discussions complete the description of the fields and bits in the ALU instructions and their functions.  Because so many thing can happen simultaneously, it is rather difficult to completely understand this ALU section and the different instructions the chip can perform.  The best we can hope to do is to take some of the valid ALU instructions and analyze them to familiarize yourself with the instruction set.  On the other hand, many of the combinations of functions can be automatically resolved by an optimizing compiler. It can be made to recognize permissible and restricted Forth word sequences and compile the most compact machine instructions to fully utilize the power of the NC4000 chip.  A better understanding of the inner mechanism of this ALU would enable the user to anticipate the optimization process and thus assure the production of the most efficient code.
.new
 
 
         Table 1.7.   Valid ALU Instructions
``` 
Code     a=7 (Pass Y)        a=0 (Pass T)        a=Arith/Logic
10a000   DROP DUP            NOP                 OVER a-
10a001   ---                 2/                  OVER a- 2/
10a002   ---                 2*                  OVER a- 2*
10a003   ---                 0<                  ---
10a010   ---                 ---                 ---
10a011   ---                 D2/                 ---
10a012   ---                 D2*                 ---
10a013   ---                 ---                 ---
10a020   DROP                NIP                 a
10a021   ---                 2/ NIP              a 2/
10a022   ---                 2* NIP              a 2*
10a023   ---                 NIP 0<              ---
10a030   ---                 ---                 ---
10a031   ---                 ---                 ---
10a032   ---                 ---                 ---
10a033   ---                 ---                 ---
10a100   SWAP                NIP DUP             SWAP OVER a
10a101   ---                 NIP DUP 2/          SWAP OVER a 2/
10a102   ---                 NIP DUP 2*          SWAP OVER a 2*
10a103   ---                 NIP DUP 0<          ---
10a110   ---                 ---                 ---
10a111   ---                 ---                 ---
10a112   ---                 ---                 ---
10a113   ---                 ---                 ---
10a120   OVER                DUP                 2DUP a
10a121   ---                 DUP 2/              2DUP a 2/
10a122   ---                 DUP 2*              2DUP a 2*
10a123   ---                 DUP 0<              ---
10a130   ---                 ---                 ---
10a131   ---                 ---                 ---
10a132   ---                 ---                 ---
10a133   ---                 ---                 ---
```
 
         Special ALU Instructions
```
102411   *-                  102412   *F
102414   /"                  102416   /'
102616   S'                  104411   *'
```
 
 
