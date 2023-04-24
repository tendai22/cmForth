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
 
 
　