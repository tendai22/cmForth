 
## 5.3.   I/O and Memory Instructions

The I/O and memory instructions in NC4000 are characterized by the fact that the two most significant bits in the instructions are set and that the least significant 5 bits often contain a literal value.  The other 9 bits in between are decoded to perform ALU, data stack, and return operations.  The general format of this class of instructions is as follows:
```
I/O and Memory Instructions   1 1 X !  ALU  Y CSA ;  literal
```
However, there are many special cases causing the I/O and memory instructions to appear as if they are governed by random logic.  The best one can do is to present the entire table of valid I/O and memory instructions as shown in Table 1.8.

Based on the table, we can make a few observations which might guide the user in understanding these rather complicated instructions.

The store bit, bit 12, will always be controlling read and write to/from the memory or registers.  If this bit is zero, the instruction is a fetch operation; otherwise, it must be a store instruction.

The ALU field is also always predictable, as it specifies what kind of ALU operation shall be performed on the operands.  In most cases, the ALU operation can be performed on the operand while the I/O or memory operation is being processed.  However, it is not always obvious as to which operands are used in the ALU operation.

Bit 6 is very close in function to the SA bit in the ALU instructions.  If it is zero, the data stack depth is not changed and all operations are performed on the T and N registers.  these two registers will contain the results when the instruction is completed.  The C bit, bit 7, is similar to the lower bit in the Y field of the ALU instruction.  It selects the N register as the input to the Y-port of the ALU unit.  If it is set, the carry bit is also used in the ALU operation; otherwise, only the N register is used without carry.
.new
 
 
       Table 1.8.   Valid I/O and Memory Instructions
``` 
Code     a=7 (Pass Y)        a=0 (Pass T)        a= Arith/Logic
 
 
14a0nn   ---                 ---                 nn @ a-
14a1nn   nn @                ---                 ---
14a2nn   ---                 ---                 nn @ c-
14a3nn   nn I@               ---                 ---
 
14a400   ---                 ---                 n a-
14a500   n                   ---                 ---
14a600   ---                 ---                 n c-
14a7nn   ---                 ---                 nn I@ a-
 
15a0nn   nn !                ---                 DUP nn ! a
15a1nn   ---                 ---                 ---
15a2nn   nn I!               ---                 DUP nn I! c
15a3nn   ---                 DUP nn I!           ---
 
15a4nn   ---                 ---                 nn a-
15a5nn   nn                  ---                 ---
15a6nn   ---                 ---                 nn c-
15a7nn   nn I@!              ---                 ---
 
16a000   ---                 ---                 @ a-
16a100   @                   ---                 ---
16a200   ---                 ---                 @ c-
16a300   ---                 ---                 ---
 
16a4nn   ---                 ---                 nn X@ a-
16a5nn   nn X@               ---                 ---
16a6nn   ---                 ---                 nn X@ c-
16a7nn   ---                 ---                 nn @a-
 
17a000   !                   ---                 ---
17a100   ---                 ---                 ---
17a200   ---                 ---                 ---
17a300   OVER SWAP !         ---                 ---
 
17a4nn   nn X!               ---                 ---
17a5nn   DUP nn X!           ---                 ---
17a6nn   ---                 ---                 ---
17a7nn   ---                 ---                 nn !a-
```
         Special Return Stack Instructions
```
140721   R> DROP             157201   >R
147301   R@                  157221   TIMES
147321   R>                  157701   R> SWAP >R
 
         a-: SWAP -     c-: SWAP -c
```
.new
 
Bit 8 selects alternate ways of accessing different types of memory or I/O.  In case of literal fetch instructions, setting bit 8 would cause a fetch of the 16 bit literal value following the instruction.  A zero in bit 8 would fetch the short literal embedded in the instruction itself.  Extended memory fetch and store instructions (16xxxx and 17xxxx types) are invoked by setting bit 8 to indicate that the extended memory addressing mode is selected.

When bits 6 to 9 are all set (1xx7xx type), the instruction refers to some internal registers, whose register number is encoded in the 5 bit literal field.  There are 17 addressible registers in the NC4000.  Their register numbers, assigned names and functions are listed in Table 1.9.
 
 
       Table 1.9.   NC4000 Internal Registers
``` 
       Name     Number       Function
       J/K              0              Data/Return Stack Pointers
       I                  1              Return Index Register
       P                 2              Program Counter
       -1                3             True Register
       MD            4/5           Multiplier/Divisor Register
       SR              6/7           Square-root Register
       B                  8            B-Port Data Register
       B Mask         9            B-Port Mask Register
       B I/O           10           B-Port Direction register
       B Tristate     11           B-Port Tristate Register
       X                 12           X-Port Data Register
       X Mask        13           X-Port Mask Register
       X I/O           14           X-Port Direction Register
       X Tristate     15           X-Port Tristate Register
       #Times         17          I as TIMES Counter
```
MD and SR registers are used for special purposes, i.e., to hold necessary parameters for doing more complicated arithmetic operations such as multiply, divide, and square-root.  However, if they are not used by these specialized instructions, these registers are available for storing temporary data for convenient retrieval.

In performing I/O functions and communication with the outside world through the B-port and X-port, one only has to read or write the B or X data registers and the data will be read from the input pins or be written to the output pins.  Before the actual I/O operations, the pins must be initialized and assigned appropriate functions.  The function of each I/O pin can be programmed via the I/O, Mask, and Tristate Registers in the B- and X-ports.  The exact function of bits in these registers are shown in the following table:

.new
 
 
       Table 1.10.   Function of Bits in the I/O Registers
``` 
       Register              Bit function if set
 
                             Input               Output
 
 
       Data                  Set comparison-     ---
                             latch
 
       Mask                  ---                 Inhibit writing
 
       I/O                   Set for output      Set for output
 
       Tristate              ---                 Set to tristate
                                                 after write cycle.
```
