 
8.   Optimizing Compiler
       The compiler in a regular Forth system is very simple.  It only has to search the dictionary, find the words and compile their execution addresses.  Each word represents one function.  The only complication is to build control structures in a definition, which requires additional actions during compilation.  The compiler for NC4000 machine is much more complicated due to following reasons:
.  The compiler must absorb the functions of an assembler for assembling machine instructions in addition to compiling high level words or subroutine calls.
.  More than one function may be performed by a single machine instruction.  The compiler must be able to recognize such a sequence of functions and combine them into a single machine instruction.
.  There are three memory spaces to be dealt with: the main memory, the local memory, and the registers.
.  Deficiency in the prototype chip precluding certain combinations of bit patterns.
       In this version of cmFORTH, Chuck Moore chose a very simple and quite effective approach towards optimizing the assembly of machine instructions.  He picked three critical points to exercise code optimization: at the end of a definition when ; is executed, whenever a binary ALU code is assembled, and when a shift code is assembled.  These three cases cover most situations where optimization is effective.  Other situations can be optimized by explicitly hand coding special machine instructions.
       An important variable ?CODE is used to control the optimizing process.  Whenever a multi-function machine code is compiled, its address is stored in ?CODE so that the smart compiler can work on it.  When a high level word (subroutine call), a conditional or unconditional branch, or a loop instruction is compiled, ?CODE is set to zero, in effect turning the smart compiler off for that instruction.
 
　
　
8.1.   The Smart ; Compiler
       The subroutine call in NC4000 is a one cycle instruction and the subroutine return is an one bit field which can be embedded in many other NC4000 machine instructions.  Obviously, if we can recognize the conditions when the return bit can be inserted into the last instruction in a definition, we can always save a machine cycle.  Most of the colon definitions can be treated this way by the smart ; compiler.
 
OCTAL                We want to see the bit patterns
                             in machine instructions.  Octal
                             is the most natural representation.
 
: PACK                       ( a n -- )
                             Pack the return bit into the machine
                             instruction in address a if possible.
                             Otherwise, compile an explicit
                             return instruction.  Terminate
                             the calling word by discarding
                             top of return address.
       160257 AND            These bits are relevant bits which
                                          must be examined.
       140201 XOR            If bits match this pattern, it is
                                       a memory instruction and the return
                                       bit cannot be packed into it.
       IF                    Bit pattern does not match 140201,
         40 SWAP +!          pack the return bit into a.
       ELSE                  Pattern matches with 140201,
         DROP                Discard address a.
         100040 ,            Compile an explicit return instruction.
       THEN
       R>DROP                Work is done.  Exit the EXIT routine
                                     immediately.
       ;
 
: EXIT                       ( -- )
                             Look through all the possible
                             patterns where the return bit
                             can be packed and pack it.
       ?CODE @ DUP           Last instruction a machine code?
       IF                    Yes.  Go work on it.
         0 ?CODE !           First re-initialize ?CODE.
         DUP @               Fetch the machine code.
         DUP 0<              Is the bit 15 set?
         IF                  Yes.  It looks like a machine
                             code.
           DUP 170000 AND 100000 =
                             Is it an ALU instruction?
           IF PACK THEN      Yes.  Pack the return bit.
           DUP 170300 AND 140300 =
                             Is it a register fetch instruction?
           IF PACK THEN      Yes.  Pack the return bit.
           DUP 170000 AND 150000 =
                             Is it a short literal store instruction?
           IF                Yes.
             DUP 170600 AND 150000 XOR
                             15x6xx cannot be a valid instruction.
             IF PACK THEN    If not 15x6xx, pack the return
                             bit.
           THEN DROP         End of multi-function code processing.
         ELSE                Last instruction is not a multi-function
                             machine code.  However, if it is a call
                             instruction, it can be substituted by
                             a jump instruction to save an explicit
                             return instruction.
           DUP H @ dA @ - XOR  Compare the address in ?CODE with
                             the current dictionary pointer.
           170000 AND 0=     Are they in the same 4K cell page?
           IF                Yes.
             7777 AND        Isolate the 12 bit address field.
             130000 XOR      Tag the unconditional jump field.
             SWAP !          Store it in the address pointed
                             by ?CODE.
             EXIT            Terminate here immediately.
           THEN
           DROP              Discard contents of ?CODE.
         THEN
       THEN DROP             Discard ?CODE.
       100040 ,              Compile explicit return instruction.
                             Not possible to optimize.
       ;
 
 
: ;                          ( -- )
                             The optimizing ; compiler.
       [COMPILE] RECURSIVE   Reset the smudge bit in the name
                             field of the new definition, making
                             it available for searching.
       R>DROP                Exit the compiler loop at the
                             end of this word (;).
       [COMPILE] EXIT        EXIT was made immediate.  Force
                             its compilation.
       ;
　
　
8.2.   The Smart ALU Function Compiler
       The ALU instructions are the most complicated type of instructions in the NC4000 chip, because all the fields and bits are functional code.  Thus a large variety of instructions can be constructed, doing many things in single machine cycles.  A smart compiler would have to be able to recognize all these conditions in order to combine the maximum number of functions into a single machine instruction.
       The elementary ALU functions like + , - , SWAP- , AND , OR , and XOR are defined by the smart compiler BINARY.  BINARY will examine the instruction previously compiled to see if these ALU functions can be incorporated into that instruction and do so whenever possible.
 
 
: BINARY                     ( n1 n2 -- )
                             n2 is the code of the ALU instruction.
                             n1 is the pattern which can be XOR'ed
                             into the previous instruction to
                             install the ALU function.
                             Define a smart ALU compiler.
       CREATE                Make a new header.
       , ,                   Compile n2 and n1 into the code
                             field.
       DOES                  Now define what the new compiler
                             directive will do during compilation.
       R> 77777 AND          Pointer to the stored patterns
                             n2 and n1.
       2@                    Retrieve them.
       ?CODE @ DUP           Are we dealing with a machine
                             code?
       IF                    Yes.  Turn on the optimizer.
         @                   The machine instruction.
         DUP 117100 AND 107100 =
                             Is it of the SWAP/OVER type?
         OVER 177700 AND 157500 = OR
                             Or a short literal?
         IF                  Yes.  We can do something with
                             it now.
           DUP 107020 -      Not a DROP?
           IF                Not DROP.
             SWAP-DROP       Discard n2.
             XOR             Force the ALU code into the ALU
                             field of the previous instruction.
             DUP 700 AND 200 =
                             Test if carry must be included.
             IF 500 XOR      If so, restore the Tn bit.
             ELSE
               DUP 70000 AND 0=
                             Make sure we have an ALU instruction
                             at hand, then
               IF 20 XOR THEN
                             flip the Stack Active SA bit.
             THEN
             ?CODE @ ! EXIT  The machine code can incorporate
                             ALU code in the ALU field.  Update
                             the machine code.
           THEN
         THEN
       THEN
       DROP                  Drop the ?CODE, which is zero.
       ,C                    Compile n2 as the explicit ALU
                             machine code.
       DROP                  Discard the compare mask.
       ;
 
 
       Now, all the binary ALU code compiler can be defined
by BINARY:
 
 
6100 101020 BINARY AND
1100 102020 BINARY SWAP-
4100 103020 BINARY OR
3100 104020 BINARY +
2100 105020 BINARY XOR
5100 106020 BINARY -
 
 
 
　
　
8.3.   The Shift Compiler
       Shift functions can be appended to all ALU machine code.  code.  However, restrictions have to be imposed while programming the NC4000 prototype chip so that shifts produce the desired results.
 
 
: SHIFT                      ( n -- )
                             Define smart shift compilers.
       CREATE                Make new header.
       ,                     Save n, the shift code in the
                             code field.
       DOES                  Actual compilation action.
       R> 77777 AND          Pointer to the stored shift code.
       @                     Get the code.
       ?CODE @ DUP           Is the previous word a machine
                             instruction?
       IF                    Yes.  Do optimization.
         @                   The machine code.
         DUP 171003 AND 100000 =
                             Is it an ALU code without any
                             prior shift operation?
         IF                  Yes.
           XOR               Pack in the shift code.
           ?CODE @ !         Store it back.
           EXIT              Done and out.
         THEN
       THEN
       DROP                  Cannot optimize.  Discard the
                             code address.
       100000 XOR ,C         Compile an explicit shift machine
                             instruction.
       ;
 
 
 
       The three shift functions which can be safely packed into ALU instructions are:
 
 
2 SHIFT 2*
1 SHIFT 2/
2 SHIFT 0<
 
 
One has to be careful about 0< which has to be followed by a NOP before it can be used to do logic branching.  As shown in Screen 1, 0< must be redefined for the prototype chip:
 
: 0<                         ( n -- f )
                             Prototype 0<.
       [COMPILE] 0<
       [COMPILE] NOP         A NOP must follow 0< to allow
                             enough time for the bits to
                             propagate.
       ;
 
       The double integer shift functions cannot be packed into other ALU code due to the prototype restrictions.  They are defined as explicit single cycle instructions:
 
100012 uCODE D2*
100011 uCODE D2/
 
 
 
　
　
8.4.   Merging of DUP
       Sometimes a DUP operation can be merged into a machine code, whose stack active bit can be turned on, to accommodate the DUP function.  A single cycle DUP instruction must be compiled immediately before the machine instruction under consideration.
 
 
: DUP?                       ( -- )
                             Pack two previous instructions
                             into one if the first is a single
                             cycle DUP instruction.
       HERE 2 - @            Fetch the instruction just before
                             the one recently compiled.
       100120 =              Is it a single cycle DUP instruction?
       IF                    Yes.  Try to pack.
         HERE 1 - @          Get the most recent instruction.
         7100 XOR            Turn on Tn bit and change data
                             source to T, thus activating DUP.
         -2 ALLOT            Delete the two compiled instructions.
         ,C                  Replace them with a single instruction.
       THEN
       ;
 
 
       Not many instruction pairs can be packed this way.
The ones often used in cmFORTH are:
 
 
: I!                         ( n -- )
                             Compile a register store instruction.
       157200 SHORT          Compile a short literal instruction
                             with n as the register number.
       DUP?                  Often the data stored into a register
                             are needed for other purposes.
                             If a DUP instruction is used this
                             way, it can be packed into the
                             I! instruction.
       ;
 
: >R                         ( -- )
                             Compile a >R or a DUP >R instruction.
       157201 ,C             Compile the single >R instruction.
       DUP?                  Pack DUP if available.
       ;
 
 
　