 
## 3.   Terminal Input and Output

The terminal input and output in the RS-232 format is implemented through software via two I/O pins in the X-port, X0 as serial output and X4 as serial input.  With the clock running at 4 MHz, the time interval representing one bit at 9600 baud is about 417 cycles, as specified by the system variable C/B.  The primitive to send an ASCII character to the terminal is EMIT and that to receive a character from terminal is KEY.  Line based I/O words TYPE and EXPECT, and all other terminal I/O words are derived from them.
 
 
 
## 3.1.   Primitive Input and Output Words
```
: EMIT                       ( c -- )
                             Send a character to the terminal
                             via X0.
       30 13 I!              Mask X-port to allow X0 to be
                             output and other bits be input.
       2* 511 XOR            Make room for the start bit and
                                     invert polarity of bits.
       9 FOR                 Send out 8 data bits with one
                             start bit and one stop bit.
         12 I!               Send out one bit.
         2/                  Shift out next bit.
         C/B @ 11 - CYCLES   Wait for one bit period.
       NEXT                  Continue for the entire bit pattern.
       DROP                  Discard the rest of the character.
       ;
 
: RX                         ( -- n )
                             Get one bit from X4 pin.
       12 I@                 Read the X-port.
       16 AND                Save only the X4 pin input.
       ;
 
: KEY                        ( -- c )
                             Read one ASCII character from
                             X4 pin.
       0                     Starting character pattern.
       BEGIN                 Wait for the start bit.
         RX                  Read the input line.
         16 XOR              Exit only when a start bit (low)
       UNTIL                 is detected.
       C/B @                 417 cycles per bit.
       DUP 2/ +              Wait 1.5 bit to the center of
                             the first data bit.
       7 FOR                 Read 8 bits.
         14 - CYCLES         Delay till the center of bit period.
         2/                  Ready the character pattern for
                             the next bit.
         RX                  Read one bit.
         2* 2* 2*            Justify the bit position.
         OR                  Put the bit into the character
                             pattern.
         C/B @               Delay for next bit.
       NEXT                  Repeat until all eight bits are
                             assembled in the character pattern.
       BEGIN RX UNTIL        Now wait until the stop bit is
                             transmitted.
       DROP                  Discard the last C/B cycle number.
       ;
```
 
 
## 3.2.   The Line Input and Output Words

```
: TYPE                       ( a1 -- a2 )
                             Output a stored string to the
                             terminal.  The first character
                             in the string must be a count
                             byte.  This is different from
                             the standard TYPE which takes
                             an address and a count as arguments.
                             a1 is the starting cell address
                             and a2 is the address of the cell
                             following the string.
       2*                    Change a1 to a byte address.
       DUP C@ 1 -            Get the count byte.
       FOR                   Scan the string.
         1 +                 Next character address.
         DUP C@              Get the character.
         EMIT                Send it out.
       NEXT
       2/                    Cell address after the string.
       ;
 
: EXPECT                     ( a n -- )
                             Accept n characters and put them
                             in the memory starting at a.
                             Each character is put in a cell
                             with high byte padded with 40H.
       SWAP CURSOR !         Store address a in CURSOR.
       1 - DUP FOR           Repeat for n characters.
         KEY                 Get one character.
         DUP 8 XOR           Is it a backspace?
         IF                  No.  Not backspace.
           DUP D XOR         Is it a carriage return (CR)?
           IF                Not CR.
             DUP 4000 +      Pad it with a @.
             CURSOR @ 1 !+   Store it in the assigned memory.
             CURSOR !        Refresh CURSOR for next character.
             EMIT            Echo the character to terminal.
           ELSE              If it is CR.
             SPACE           Output a space instead.
             DROP            Discard the CR character.
             R>              Get the current index.
             -               Number of character received so
                             far.
             CNT !           Store it in the character count
                             variable CNT.
             EXIT            CR end of line exit.
           THEN
         ELSE                Yes.  It is backspace.
           DROP              Discard the backspace character.
           DUP I XOR         If the backspace is the first
                             character in the input stream,
           [ OVER ] UNTIL    return to the beginning of the
                             FOR-NEXT loop immediately.
                             [ OVER ] UNTIL compiles a
                             conditional branch to the
                             address left on stack by FOR.
           CURSOR @ 1 -      Get the cursor address again,
           CURSOR !          and decrement it.
           R> 2 + >R         Add 2 to loop index to back
                             up the character pointer by 1.
           8 EMIT            Echo the backspace code.
         THEN
       NEXT
       1 + CNT !             Increment character count.
       ;
 
 
 
3.3.   Other Terminal I/O Words
 
: CR                         ( -- )
       13 EMIT               Carriage return.
       10 EMIT               Line feed.
       ;
 
: SPACE                      ( n -- )
       32 EMIT
       ;
 
: SPACES                     ( n -- )
       0 MAX                 Protection against negative
                             count number.
       ?DUP IF               More than one character?
         1 - FOR             Yes.
           SPACE             Send them out.
         NEXT
       THEN
       ;
 
: HERE                       ( -- n )
       H @                   Top of dictionary or the WORD
                             buffer.
       ;
.new
 
 
4.   The Number Conversion Words
       Number is the most important entity used in a computer.  It has to be entered in ASCII digit strings and converted into the binary representation for the computer to operate on.  The result thus generated will have to be converted back to human readable digit strings so that the user can make some sense of it.  Forth has the best user interface as far as numbers are concerned.  Input numbers are converted to binary form and pushed onto the data stack.  Output numbers can be displayed in several formats depending upon the user's need.  The user can select any reasonable number base for the conversion between the ASCII form and the binary form.
 
　
　
 
4.1.   Convert Digits to Binary Number
       Strings of digits or numbers are one of the two basic syntactic elements in the Forth language.  The numbers typed in by the user must be converted to 16 bit binary numbers and pushed onto the data stack.  The conversion process is controlled by the variable BASE which specifies the number base to be used in the conversion process.
 
HEX                          Change to hexadecimal base because
                             we will use ASCII code values.
 
: -DIGIT                     ( c -- n )
                             Convert one ASCII character c
                             to its binary value n.  Abort
                             it the character is not within
                             the range specified by BASE.
       DUP 39 >              Is c greater than 9?
       IF                    Yes.
         DUP 40 >            Is it also greater than @?
         7 AND -             If so, subtract 7 to take care
                             of the gap between 9 and A.
       THEN
       30 -                  Take off the offset to 0.
       DUP BASE @ U<         Is the result less than base?
       IF EXIT THEN          If so, the conversion is successful.
                             Return with the value.
       2DROP DROP            Otherwise, conversion error.
                             Clear the stack.
       ABORT" ?"             Abort with a message ?.
       DROP ; RECOVER        Forcing the compilation of a DROP
                             and ; instruction and eliminate
                             this instruction to save 1 cell
                             of memory.  It has to be done
                             this way because ABORT" ?" compiles
                             a string literal which is inappropriate
                             to hang a return bit.
 
: 10*+                       ( u1 c -- u2 )
                             Convert character c to its value.
                             Multiply it by the base value hidden
                             in MD and accumulate the product
                             into u1.
       -DIGIT                Convert c to its binary value.
       0E TIMES *'           Repeat multiply step 16 times to
                             obtain the product.
       DROP                  Discard the higher half of the
                             double integer product.
       ;
 
: NUMBER                     ( a -- n )
                             Convert a digit string at a to
                             a 16 bit integer.
       BASE @ MD I!          Store base value in MD register.
       2C@+                  Get first two characters of the
                             string.
       OVER 2D = DUP >R      If the first character is a minus
                             sign, save a true flag on return
                             stack.
       IF                    Yes, it is a minus sign.
         SWAP-DROP           Discard the minus character.
         0                   Initialize accumulator for conversion.
       ELSE                  Not a minus sign.
         SWAP -DIGIT         Convert the first character and
                             use it as the accumulator.
       THEN SWAP             ( next-addr accumulator count )
       1 - ?DUP              More than one character in the
                             string?
       IF                    Yes.
         1 - 2/ FOR          Run down the digit string.
           SWAP 2C@+         Get next two characters.
           SWAP >R >R        Get the character out of the way.
           SWAP              Swap the next address with the
                             accumulator.
           R> 10*+           Convert one character and add
                             its value to the accumulator.
           R> 20 XOR         Is the next character a blank?
           IF 10*+           No.  Convert it.
           ELSE DROP THEN    Yes.  Drop the blank.
         NEXT                Continue for entire string.
       THEN
       SWAP-DROP             Discard the address.
       R>                    Retrieve the sign flag.
       IF NEGATE THEN        Negate the number if it has a
                             leading - sign.
       ;
 
 
　
 
4.2.   Convert Binary Number to ASCII String
       This conversion process is different from those we know well in other Forth systems.  The converted digits are piled up on the stack instead of stored in an output buffer.  This method is much more efficient, as it eliminates the need for a buffer with pointer and management overhead.
 
: HOLD                       ( .. # n c -- .. # n )
                             The output string is piled up
                             on the data stack with a count
                             on top.  Above count #, the number
                             to be converted n and the character
                             c to be added to the string.
                             c is tucked beneath # and # is
                             incremented.
       SWAP >R               Save n.
       SWAP                  Tuck c under #.
       1 +                   Increment count #.
       R>                    Retrieve n.
       ;
 
: DIGIT                      ( n -- c )
                             Convert a number n to its equivalent
                             ASCII code.
       DUP 9 >               Is it greater than 9?
       7 AND +               If so, add 7 to jump to A.
       48 +                  Add offset of 0 in the ASCII scale.
       ;
 
: <#                         ( n -- # n )
                             Prepare a number to start the
                             conversion process.
       -1                    Initial value of the string length.
       SWAP                  Tuck the count # under n.
       ;
 
: #                          ( .. # n -- .. #' n' )
                             Convert one digit from n and add
                             the converted digit to the output
                             string.
       BASE @ /MOD           Divide n by the base.
       SWAP DIGIT            Convert the remainder to an ASCII
                             character.
       HOLD                  Add the converted character to
                             the output string.
       ;
 
: #S                         ( .. # n -- .. #' 0 )
                             Convert the number n until it
                             is reduced to 0, or completely
                             converted.
       BEGIN
         #                   Convert one digit.
         DUP 0=              End?
       UNTIL
       ;
 
: #>                         ( .. # n -- )
                             Output the converted string to
                             the terminal.
       DROP                  n is usually 0.  It is not needed
                             in any case.
       FOR EMIT NEXT         Use the character count # to print
                             that many characters to the terminal.
       ;
 
: SIGN                       ( .. # n -- .. #' )
                             If n is negative, append a - sign
                             to the end of the output string.
       0<                    Is n negative?
       IF 45 HOLD THEN       If so, append - sign.
       ;
 
: (.)                        ( n -- .. # )
                             Convert the number n to a ASCII
                             string on stack.
       DUP >R                Save a copy of n for sign.
       ABS                   Take the absolute value of n.
       <# #S                 Convert the absolute value to
                             string.
       R> SIGN               Append the sign of n.
       ;
 
: .                          ( n -- )
                             Free format display of the number
                             on top of the stack.
       (.)                   Convert n to a string.
       #>                    Print the string.
       SPACE                 Append a space to separate consecutive
                             numbers.
       ;
 
: ?                          ( a -- )
                             Display the contents of a memory
                             cell.
       @ .                   Get the number and display it.
       ;
 
: U.R                        ( u n -- )
                             Display an unsigned integer u
                             in a field of n columns, right
                             justified.  Formatted output.
       >R                    Save the column number n.
       <# #S                 Convert u to a string.
       OVER                  Copy character count to top.
       R> SWAP-              Subtract it from the column width.
       1 - SPACES            First pad the left side with enough
                             spaces.
       #>                    Finally print the number string,
                             right justified.
       ;
 
: U.                         ( u -- )
                             Display an unsigned integer in
                             free format.
       0 U.R                 Display the integer using 0 column
                             field specification.  The result
                             is that the string will be display
                             from the current character position.
       SPACE                 Followed by a space.
       ;
 
 
 
4.3.   Memory Dump
       This memory dump word was designed for Chuck Moore's peculiar CRT display, which has a single line display window.
 
: DUMP                       ( a -- a+8 )
                             Display 8 consecutive cells following
                             the cell at a.  a+8 is returned on the
                             stack so another DUMP can be issued.
       CR                    New line.
       DUP 5 U.R SPACE       Display first the address a.
       7 FOR                 Run down 8 cells.
         1 @+                Fetch one cell and increment a.
         SWAP 7 U.R          Display the contents.
       NEXT
       SPACE                 Add one space at the end of line.
       ;
 
 
 
4.4.   Message Output
       In an interactive programming environment, it is important that the system sends timely messages to the terminal to show the user its current status and any error condition.  Messages to be send to the terminal must be compiled into definitions using special string literal words like ." and ABORT" , etc.  These string literal words have unique behavior during compilation and during execution.  Because these words involve compiler functions, words used to define them seem to be out of place as they are defined much later than the text interpreter.  From among these special words, the ones relevent to the construction of message output words are excerpted here.  Some of these words will be explained later in more detail.
 
: COMPILE                    ( -- )
                             Compile the address following
                             this word to the top of the dictionary.
       R>                    Retrieve the address of the next
                             word from the return stack.
       7FFF AND              Mask off the most significant
                             bit, which is the carry bit.
       1 @+                  Fetch next word.
       >R                    Put the address of the second
                             word after COMPILE back on the
                             return stack.
       ,A                    Compile the address to the
                             dictionary.
       ;
 
: abort"                     ( -- n 0 )
                             Run time routine for ABORT".
                             Print the following message and
                             reinitialize the system.
       H @  TYPE SPACE       Display the name of word currently
                             been executed.
       R> 7FFF AND           Address of the compiled text.
       TYPE                  Display the message text.
       2DROP                 Clean the garbage left by TYPE.
       BLK @                 Leave the block number on stack
                             as a debugging aid.
       QUIT                  Return to the text interpreter.
       ;
 
: ABORT"                     ( -- )
                             Abort the word currently been
                             executed and return to the text
                             interpreter after the following
                             message is displayed.
       COMPILE abort"        Compile the runtime routine.
       4022 STRING           Compile the following message
                             up to " as a string literal.
       ;
 
: dot"                       ( -- )
                             Run time routine of ." , which
                             displays the message immediately
                             following until " .
       R> 7FFF AND           Address of the compiled message
                             text.
       TYPE                  Display the message and leave
                             the address after the message.
       >R                    Push that address back on the
                             return stack to continue the execution
                             process.
       ;
 
: ."                         ( -- )
                             Display the following message
                             at run time.
       COMPILE dot"          Compile the address of dot" .
       4022 STRING           Compile the following text up
                             to " as a string literal.
       ;
 
　