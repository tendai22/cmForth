 

 

HEX

 

: SAME                       ( a1 a2 -- a3 a4 f | a1 t )

                             With a string address a1 and the

                             link field address of a word in

                             dictionary, compare the string

                             with the word name.  If the name

                             matches the string, return the

                             code and link field addresses

                             of the word with a false flag.

                             Otherwise, return the string address

                             a1, link field address of the

                             next word in the linked chain

                             and a true flag.

       OVER >R               Save a copy of a1 on return stack.

       DUP                   Copy a2.

       1                     Offset to the name field.

       BEGIN

         + 1 @+              Get one cell from the name field.

         SWAP                Get the contents to top of stack.

         R>                  Address of a cell in the string.

         1 @+                Fetch one cell from there.

         R>                  Replace the string address.

         - 2*                Compare contents of the two cells,

                             ignoring bit 15.

         DUP                 Need a dummy zero when looping

                             back, to be consumed by +.

       UNTIL                 Exit if the cells are not equal.

       R>DROP                Discard the string address on

                             the return stack.

       FEFF AND              Is bit 8 of the difference set?

                             This is the terminator bit (bit 7)

                             in the last cell of a word name,

                             shifted to here by 2* above.

       IF 0 AND EXIT THEN    Exit with a false flag.  The names

                             failed to match.

       SWAP 1 + @            Get the next cell in the string buffer.

       0<                    If it is a string delimiter,

       IF @ THEN             fetch the link field address.

       SWAP                  Adjust the code field address

                             of the found word.

       ;

 

: HASH                       ( n -- a )

                             Use the vocabulary code n to find the

                             pointer to the head of thread in

                             the thread table before CONTEXT.

       CONTEXT SWAP-

       ;

 

: -FIND                      ( a1 a2 n -- a1 t | a2 f )

                             With word address a1, link address

                             a2 and vocabulary mask n, search

                             the link for the word.  If found,

                             return the code and link field addresses

                             of the word and a false flag.

                             If not found, return the word

                             address and a true flag.

       HASH                  Find the head of the thread.

       BEGIN                 Start the dictionary search.

         @ DUP               Get the next link field address.

       WHILE                 If link field address is not zero,

                             continue the search.  Otherwise,

                             the end of link chain is reached.

         SAME                Compare the name field with the

                             word pattern.

       UNTIL                 Not same.  Continue with the next

                             word in the linked chain.

       0 EXIT THEN           Find a word.  Return its address

                             and a false flag.

       -1 XOR                End of the linked chain.  Return

                             with the flag.

       ;

 
7.3.   Control Structures

       The NC4000 has three branch instructions: unconditional branch, conditional branch and loop.  A branch instruction takes a 12-bit argument to specify a branch address, within a 4K word memory page.  These instructions are used to implement various control structures in high level Forth definitions.

       The conditional branching structures are of the following two types:

       ...  IF ... ELSE ... THEN  ...

       ...  IF ... THEN ...

 

       There are several types of indefinite loops which can be constructed very easily with the conditional and unconditional branch instructions.  The ones this cmFORTH system supports are:

       ...  BEGIN ... UNTIL  ...

       ...  BEGIN ... AGAIN  ...

       ...  BEGIN ... WHILE  ... REPEAT  ...

       ...  BEGIN ... WHILE  ... UNTIL ... THEN  ...

WHILE can branch to REPEAT or to THEN.  The latter construction allows additional freedom, in that there are two distinct paths after AGAIN.

       Definite loops are constructed with FOR and NEXT:

       ...  FOR ... NEXT  ...

 

which is very similar to the DO-LOOP structure in conventional Forth we all love.  However, FOR takes only one parameter which is decremented every time through NEXT.  The loop will be terminated when this index is decremented to zero.

       The FOR-NEXT definite loop can also make use of the

WHILE-THEN conditional:

       ...  FOR ... WHILE ... NEXT ... ELSE ... THEN  ...

 

However, one will have to take care of the loop index on the return stack, when the loop is terminated through WHILE.  WHILE does not modify the return stack.

 

 

OCTAL                        For instructions and addresses.

 

: OR,                        ( a n -- )

                             OR the address a into the instruction

                             n and compile the branch instruction.

       0 ?CODE !             Start a new machine instruction.

       SWAP 7777 AND         Keep only the lower 12 bits in

                             address a.

       OR                    Include truncated address into

                             the branch instruction n.

       ,                     Compile the branch instruction.

       ;

 

: begin                      ( -- a )

                             Mark the current dictionary pointer

                             as address to be branched to.

       H @                   Push the current dictionary pointer

                             on the data stack.

       0 ?CODE !             Initialize the optimizer.

       ;

 

: BEGIN                      ( -- a )

                             Starting pointer of a indefinite

                             loop.

       begin                 Leave the current dictionary pointer

                             on stack for later resolution.

       ;                     Compiler directive must be executed

                             in a definition.

 

: UNTIL                      ( a -- )

                             Compile a conditional branch to

                             address a.

       110000                Conditional branch instruction.

       OR,                   Add address and compile it.

       ;

 

: AGAIN                      ( a -- )

                             Compile an unconditional branch

                             to address a.

       130000                Unconditional branch instruction.

       OR,                   Add address and compile.

       ;

 

: THEN                       ( a -- )

                             Resolve the branch address in

                             the branch instruction compiled

                             by IF or ELSE.

       begin                 Get the address of the current

                             instruction, as pointed to by

                             the dictionary pointer.

       7777 AND              Keep only the 12 bit part.

       SWAP +!               Add it into the 12 bit address

                             field in the IF or ELSE instruction.

       ;

 

: IF                         ( -- a )

                             Compile a conditional branch instruction

                             now and leave its address on the

                             stack so that its address field

                             can be resolved by ELSE or THEN.

       begin                 Leave the address of the unconditional

                             instruction on the stack.

       110000 ,              Compile a conditional branch instruction

                             with an unresolved address field.

       ;

 

: ELSE                       ( a1 -- a2 )

                             Resolve the conditional branch

                             instruction at a1. Compile an

                             unconditional branch instruction

                             with a 0 address field.  Leave

                             its address on the stack as a2,

                             to be used by THEN to resolve.

       begin                 Address of the current unconditional

                             branch instruction.

       130000 ,              Compile an unresolved unconditional

                             branch instruction.

       SWAP                  Get a1 to top of the stack.

       [COMPILE] THEN        Invoke THEN to resolve the conditional

                             branch instruction left by IF.

       ;

 

 

: WHILE                      ( a1 -- a2 a1 )

                             Compile an unresolved conditional

                             branch instruction.  Leave its

                             address on the stack as a2 while

                             passing the address left by BEGIN.

       [COMPILE] IF          Invoke IF to compile a conditional

                             branch.

       SWAP                  Exchange a1 and a2 so that they

                             can be used by REPEAT|AGAIN and THEN

                             to resolve the branch addresses.

       ;

 

: REPEAT                     ( a1 a2 -- )

                             Resolve the BEGIN-WHILE-REPEAT

                             structure.

       [COMPILE] AGAIN       Compile an unconditional branch

                             back to BEGIN, using address a2.

       [COMPILE] THEN        Resolve the conditional branch

                             instruction compiled by WHILE.

       ;

 

: FOR                        ( -- a )

                             Start a definite loop.

       [COMPILE] >R          Compile a to-R instruction which

                             saves the loop count in the I

                             register.  Leave the address of

                             the next instruction on stack

                             for the later NEXT instruction.

       begin                 Leave address of the next instruction

                             for NEXT to branch to.

       ;

 

: NEXT                       ( a -- )

                             Compile a loop instruction and

                             use the address a on the stack

                             for the branch address.

       120000                Code for the loop instruction.

       OR,                   Resolve the backward jump address.

       ;

 

 

 

7.4.   The NC4000 Assembler

       Assembler!?  Good grief!

       Supposedly, the NC4000 chip will speak high level Forth and we shall all be freed from the tyranny of assembler and live happily forever.  The truth is that we can program in Forth and NC4000 will run the program much faster than anything we had previously.  However, if we wish to squeeze the most out of it, we still have to deal with it by bits and pieces at the machine code level.

       The NC4000 machine instruction problems can be handled in two different ways:  Map the NC4000 instruction set onto a regular Forth instruction set and solve the problem with the regular Forth programming technique; or find ways to squeeze as many functions into a single instruction as possible in order to save both machine cycles and memory space.  Here we shall be concerned with single function NC4000 instructions.  We will show how they can be defined and how they are used to allow us to program in the usual Forth style.  In the section on the Optimizing Compiler, we will discuss how a program might be optimized by combining many functions into single instructions.

 

: uCODE                      ( n -- )

                             Define an NC4000 machine instruction

                             and give it a name.  When the

                             machine instruction is invoked

                             in a colon definition, code n

                             will be compiled.

       CREATE                Give the instruction a name.

       ,                     Compile code n in the code field.

       DOES                  Above are compiler action and

                             following are run time function.

       R>                    Get the pointer to the code field.

       77777 AND             Mask off the carry bit.

       @                     Fetch the code n stored in the

                             code field.

       C,                    Now, compile n into dictionary.

                             That is the assembler function.

       ;

 

       Most of the NC4000 machine instructions can be defined using uCODE.  Here are the words actually defined this way in cmFORTH:

 

100000 uCODE NOP             One cycle Nop.

140000 uCODE TWO             Two cycle Nop.

100020 uCODE SWAP-DROP       Delete N, next item on data stack.

140721 uCODE R>DROP          Delete I, top of return stack.

160000 uCODE @DROP           NOP with a memory cycle.

154600 uCODE 0+c             Adjust for carry.

177300 uCODE N!              Store N to where T points but

                             keep a copy of N on stack.

147303 uCODE -1              Push a true on stack.

104411 uCODE *'              Multiply step.

102411 uCODE *-              Signed multiply step.

100012 uCODE D2*             Left shift the double integer.

100011 uCODE D2/             Right shift the double integer.

102416 uCODE *F              Fractional multiply step.

102414 uCODE /'              Divide step.

102414 uCODE /''             Last divide step.

102412 uCODE *F              Fraction multiply step.

102612 uCODE S'              Square-root step.

147321 uCODE R>              To-R.

157201 uCODE >R              R-from.

147301 uCODE I               Retrieve loop index.

157221 uCODE TIMES           Repeat next instruction.

 

 

 

       Instructions which use the least significant 5 bits for short literals, internal register numbers, and memory incrementals must compile proper values into this 5 bit field.

 

 

 

: -SHORT                     ( -- f )

                             Return a true flag if the current

                             instruction under construction

                             can take a 5 bit short literal or

                             argument.

       ?CODE @ @             Obtain the current instruction

                             whose address is stored in ?CODE.

       177700 AND            Mask off the lower 6 bits.

       157500 XOR            Is it not equal to 157500, which

                             is the code to access internal

                             registers?

       ;

 

 

       Here are some examples that will compile pattern 1575xx in memory to be checked by -SHORT:

 

 

157504 uCODE MD              Multiplier/divisor register.

 

157506 uCODE SR              Square-root register.

 

 

: FIX                        ( n -- )

                             Get the 5 bit literal from the

                             instruction pointed to by ?CODE

                             and combine it with n to form

                             a new instruction.  It is then

                             stored back to where ?CODE is

                             pointing to.

       ?CODE @ @             Get the instruction pointed to

                             by ?CODE.

       77 AND                Preserve only the lower 6 bits.

       OR                    OR it into n.

       ?CODE @ !             Store the instruction back to

                             dictionary.

       ;

 

: SHORT                      ( n -- )

                             Construct an instruction with

                             short literal.  If the instruction

                             cannot accept a short literal,

                             abort with an error message.

       -SHORT                Can the instruction take a short

                             literal?

       IF                    No.

         DROP                Discard n.

         ABORT" n?"          Print error message and quit.

       THEN

       FIX                   Yes.  Include the literal into

                             n and replace the old instruction.

       ;

 

: @                          ( -- | n -- )

                             A smart @ compiler.  If the address

                             is in the local memory( <32 ),

                             compile a single cycle instruction.

                             Otherwise, compile a regular two

                             cycle memory fetch instruction.

       -SHORT                Is the address in the local

                             memory area?

       IF                    Not in local memory,

         167100 ,C           Compile a two cycle memory fetch.

       ELSE                  It is in local memory,

         147100 FIX          Compile a short memory fetch with

                             address as a short literal.

       THEN

       ;                     This is a compiler directive,

                             not a regular Forth @ word.

 

: !                          ( -- | n -- )

                             A smart ! compiler similar to

                             @?

       -SHORT                Local memory?

       IF                    No.

         177000 ,C           Compile long memory store.

       ELSE                  Yes.

         157000 FIX          Compile a short memory store.

       THEN

       ;

 

 

       The NC4000 machine instructions, which must take short literals as arguments, are compiled directly using SHORT.  Since these instructions are compiler directives, their arguments or the short literal, must be known at compile time.  You cannot change the literal or register numbers dynamically at run time.  In fact, the compiler will abort if you forget to give the proper argument in the definition.

 

: I@                         ( n -- )

                             Compile a register fetch instruction

                             to fetch register n at run time.

       147300 SHORT

       ;

 

: I!                         ( n -- )

                             Compile a register store instruction

                             to store top of stack into register

                             n at run time.

       157200 SHORT

       ;

 

: @+                         ( n -- )

                             Compile a increment fetch instruction

                             which increments the address by

                             n.

       164700 SHORT

       ;

 

: !+                         ( n -- )

                             Compile a increment store instruction.

       174700 SHORT

       ;

 

: !-                         ( n -- )

                             Compile a decrement store instruction.

       172700 SHORT

       ;

 

: I@!                        ( n -- )

                             Compile a register exchange instruction

                             which swaps contents between T

                             and register n.

       157700 SHORT

       ;

 

 

 

7.5.   The Compiler Vocabulary

       To program in this environment, you will have to be aware of the difference between the compiler directives and regular Forth words, which can be compiled and interpreted.  They appear syntactically identical in a colon word but behave very differently.  The compiler directives can only be used in colon definitions and should not be executed outside of a definition.  For this reason, all the compiler directives are placed in a special vocabulary named COMPILER and all the regular Forth words are placed in the FORTH vocabulary.  In the normal interpretive mode, only the FORTH vocabulary is searched and you cannot access any of the compiler directives.  Only when the word : is executed, will the COMPILER vocabulary be made available to the compiler, which will then take advantage of the optimizing compiler and compile efficient machine code whenever possible.  At the end of a definition or when an error occurs, the COMPILER vocabulary will be turned off so that you will be protected from the abnormal behavior of these compiler directives.

 

 

: FORTH                      ( -- )

                             Define the FORTH vocabulary.

       1 CONTEXT !           Deposit hash code 1 in the system

                             variable CONTEXT.  This hash code

                             is used to select the FORTH thread

                             for searching and extension. The

                             thread is stored in the cell immediately

                             above the variable CONTEXT.

       ;

 

: COMPILER                   ( -- )

                             Define the COMPILER vocabulary.

       2 CONTEXT !           The hash code of COMPILER vocabulary

                             is 2.  It directs the searching and

                             extension to the COMPILER vocabulary.

                             The head of this vocabulary is stored

                             2 cells above the variable CONTEXT.

       ;

 

 

　
 

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

 

 

　