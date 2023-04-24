 
5.   Serial Disk
       Here we assume that there is only a RS-232 interface to the outside world.  As with any other computer language for serious programming activity, one or more disk drives are necessary to store source code and data.  A serial disk is thus designed to make the maximum utilization of the available serial communication line.  The serial disk requires a host computer at the other end of the RS-232 line to act as the terminal and disk server for this Forth system.  Whenever a disk block is requested, the data will be transferred into the Forth system through the serial link.  When an updated block is flushed back to the disk, the data is also sent through the serial link.
 
 
　
5.1.   Disk Buffer Manager
       Two disk buffers are maintained in the cmFORTH system.  Each buffer is 1024 cells long.  The first buffer starts at memory address 800H and the second buffer is at C00H under the ROM memory.  Two cells in BUFFERS array contain the block numbers associated with the data stored in the two buffers.  The pointer PREV, points to the disk buffer which was referenced most recently.  OLDEST points to the disk buffer least used.
       In this system, two disk buffers are assigned and numbered as 0 or 1.  Two entries in the BUFFERS array are used to store block numbers corresponding to the contents of the two buffers.  Two variables PREV and OLDEST determined which of the two buffers is the most recently accessed.  The manager always looks at the PREV block when a block is requested.  If the block is not in the PREV buffer, it will exchange PREV with OLDEST and look at the PREV block again.  If the requested block is in one of the two buffers, that buffer will certainly become the PREV buffer and no disk access will be necessary.  If the requested block is not in these buffers, the manager will assign the PREV buffer to the new block; and the old data in this buffer which must be the least recently referenced block will be flushed to the disk or discarded.
       This technique is often referred to as the Ping-Pong buffers.  The two buffers are used in the most efficient fashion.
 
 
 
: ADDRESS                    ( n -- a )
                             Given the disk buffer number,
                             return the starting address of
                             that buffer.
       2 +                   Offset of 2048 cells.
       8 TIMES 2*            Multiplied by 1024 to get the
                             buffer address.
       ;
 
: ABSENT                     ( n -- n | a )
                             Search through the disk buffers
                             to see if block n is already in
                             one of the buffers.  If found,
                             return the address of the buffer
                             and skip the next word.  Otherwise,
                             return with n on the stack.
       NB FOR                Scan through the disk buffers.
         DUP                 Copy n, the requested block number.
         I BUFFERS @         Get one block number stored in
                             BUFFERS.
         XOR 2*              Are the 15-bit block numbers the
                             same?
       WHILE NEXT            If not the same, compare the next
                             block number in BUFFERS.
       EXIT THEN             None of the buffers contains the requested
                             block, return as if nothing had happened.
       R> PREV N!            At this point, the request block is
                             found in one of the buffers.
                             Store the buffer number in PREV,
                             and make it the most recently accessed
                             block.
       R>DROP                Discard the return address on
                             top of the return stack, thus
                             exiting the word containing ABSENT.
       SWAP-DROP             Discard the block number.
       ADDRESS               Return with the buffer address.
       ;
 
: UPDATED                    ( -- a n )
                             Exchange PREV and OLDEST buffers
                             and return the address and the
                             block number of the least recently
                             accessed buffer.  If the block
                             is not updated, skip rest of the
                             words following UPDATED.
       OLDEST @              Pointer to the buffer least recently
                             used.
       BEGIN
         1 + NB AND          Map to one of the two buffers
                             allocated in this system.
         DUP                 Save a copy.
         PREV @ XOR          Is it the same as the one stored
                             in PREV?
       UNTIL                 Exit if they are different.
       OLDEST N! PREV N!     Exchange contents of OLDEST and
                             PREV, thus making OLDEST the most
                             recently accessed disk buffer.
       DUP ADDRESS           Find the address of the buffer.
       SWAP BUFFERS          Obtain the right pointer to the
                             BUFFERS array.
       DUP @                 Get the block number stored in
                             BUFFERS.
       8192 ROT !            Store 2000H in this entry of BUFFERS.
       DUP 0< NOT            If the buffers is not updated,
       IF R>DROP DROP THEN   skip rest of the words following
                             UPDATED by thrashing the return
                             address on the top of return stack.
                             It is a very fast and implicit
                             EXIT.
       ;
 
: UPDATE                     ( -- )
                             Set the MSB of the block number
                             in BUFFERS pointed to by PREV.
       PREV @ BUFFERS        Address of the PREV block number.
       0 @+                  Fetch block number while still saving
                             the address of PREV.
       SWAP 32768 OR         Set bit 15.
       SWAP !                Put it back.
       ;
 
: ESTABLISH                  ( n a -- a )
                             Mark the oldest buffer the PREV
                             buffer and identifies it with
                             block n.  Return the buffer address
                             a.
       SWAP                  Get n to the top.
       OLDEST @ PREV N!      Make oldest the newest.
       BUFFERS !             Store block number n into the
                             BUFFERS array.
       ;
 
: IDENTIFY                   ( n a -- a )
                             Make block n the PREV block, as the
                             one most recently referenced.
       SWAP                  Get n .
       PREV @ BUFFERS !      Store n into the PREV entry in
                             the BUFFERS array.
       ;
 
 
 
5.2.   Disk Read and Write
       The serial disk is implemented using a very simple protocol.  A disk read/write request is initiated by sending a NUL byte to the host at the other end of the RS-232 line, followed by two more bytes identifying the disk block requested.  High byte of the block number is send first.  If the most significant bit in the high byte is set to 1, it is a disk write command.  1024 bytes will then be transmitted to the host.  Then it will wait for a key from the keyboard to confirm the termination of transmission.  If the MSB in the high byte is reset, it is a read command and the host is expected to send 1024 bytes of the requested block.
 
 
 
: ##                         ( a n -- a a 1023 )
                             Transmit a read disk command to
                             host and prepare to receive 1024
                             bytes.
       0 EMIT                Disk accessing command.
       256 /MOD EMIT EMIT    Transmit the read block command.
       DUP 1023              Parameters needed to receive the
                             requested block.
       ;
 
: buffer                     ( n -- a )
                             Return the buffer address of block
                             n.  If the buffer had been updated,
                             flush its contents to the host.
       UPDATED               If block n is already in one of
                             the disk buffers, return the buffer
                             address, and return to caller
                             immediately without executing
                             the following words.
       ## FOR                Block n is not in the disk buffers.
                             Get the least used buffer and
                             flush its contents to host if
                             the buffer was updated.
         1 @+                Fetch one cell.
         EMIT                Transmit one byte.
       NEXT
       KEY                   Wait for user response as end
                             of transmission.
       2DROP                 Clean up.
       ;
 
: BUFFER                     ( n -- a )
                             Obtain a disk buffer for block
                             n and return the buffer address
                             a.
       buffer                Do all the hard work to obtain
                             a disk buffer, including flushing
                             if necessary.
       ESTABLISH             Mark this disk buffer as the most
                             recently accessed.
       ;
 
: block                      ( n a -- n a )
                             Read 1024 bytes from the host
                             and put them in the buffer starting
                             at a.
       OVER ##               Transmit the read command.
       FOR                   Repeat 1024 times.
         KEY 16384 XOR       Get one byte and stuff high byte
                             with 40H.
         SWAP 1 !+           Store the cell into disk buffer.
       NEXT
       DROP
       ;
 
: BLOCK                      ( n -- a )
                             Read block n from the host if
                             it is not already in the buffer.
                             Return the buffer address.
       ABSENT                If block n is not in one of the
                             buffers, do the following to read
                             it from the host.  Otherwise,
                             return the buffer address and
                             exit here.
       buffer                Make room in the least used buffer,
                             and flush its contents if updated.
       block                 Read from host.
       ESTABLISH             Make the buffer most recently
                             accessed.
       ;
 
: FLUSH                      ( -- )
                             Write all updated buffer back
                             to disk-host.
       NB FOR                Go through all disk buffers.
         8192 BUFFER         Request block 8192, the default
                             empty block code.
         DROP                Discard the buffer address.
       NEXT
       ;
 
: EMPTY-BUFFERS              ( -- )
                             Erase all buffer pointers to
                             make the disk manager think
                             the buffers are empty.
       PREV                  Address of the PREV variable.
       [ NB 3 + ] LITERAL    The array including PREV, OLDEST,
                             and BUFFERS.
       ERASE                 Erase all these pointers to fool
                             the disk manager.
       FLUSH                 Initialize the buffers.
       ;
.new
 
 
6.   The Text Interpreter
       The text interpreter is the operating system  of Forth and it is the user interface which allows a user to operate the computer interactively.  What the text interpreter does is very simple.  It accepts a line of commands from the terminal, parses out words in the command line and executes them in the order given.  It only has to deal with two types of words, Forth commands which had been compiled into the dictionary and numbers as 16 bit integers.  If the interpreter finds a word in the dictionary, it will execute that word.  If the word is not defined in the dictionary, text interpreter will try to convert it into an integer and push the integer on the stack.  If it fails to convert the word into a number, the word is outside of the computer's vocabulary and it will abort the command line.  It will then come back and ask the user to type another command line and the process continues on for ever.
       The major functions performed by the text interpreter are receiving command lines, parsing words, dictionary searches, command execution, and number conversion.  We have already discussed number conversion and user input functions.  Here we shall discuss the rest of the functions and how they are tied together to form a complete operating system.
 
 
　
6.1.   Parsing of Words
: LETTER                     ( a1 a2 n -- a3 a4 )
                             Copy n characters from a2 to a1.
                             Source strings are stored in cells
                             and destination strings are stored
                             in bytes.  Terminate the copying
                             when a delimiter is detected.
                             The delimiter is stored in register
                             SR.
       FOR                   Scan n characters.
         DUP @               Get one character from a2.
         SR I@ XOR           Is the character the same as the
                             one stored in SR, the delimiter?
       WHILE                 Not equal.
         SWAP >R             Save a1.
         1 @+                Fetch character and increment
                             a2.
         SWAP 127 AND        Retain only the lower byte.
         I C!                Store the character in a1.
         R> 1 + SWAP         Increment a1.
       NEXT
       EXIT THEN             Exit if the string is completely
                             copied.
       R>                    A delimiter was encountered.
                             Retrieve the index count.
       NEGATE >IN +!         Move the interpreter pointer >IN
                             back that many characters.
       ;
 
: -LETTER                    ( a1 a2 # -- a3 a4 )
                             Scan characters stored in buffer
                             a2.  Ignore leading delimiters
                             by comparing with SR.  Then move
                             the string into buffer a1.  Again,
                             a1 and a3 are byte addresses and
                             a2 and a4 are cell addresses.
       ?DUP IF               n has to be greater than 0.
         1 - FOR             Repeat n times.
           1 @+              Read one cell.
           SWAP SR I@ XOR    Is the character same as the one
                             in SR?
         0= WHILE NEXT       Yes.  Skip it and continue on.
         EXIT THEN           If the character string is exhausted
                             without finding the character
                             in SR, exit here.
         1 -                 Backup a2 by one cell.
         R>                  Index of the do-loop when branched
                             out at WHILE.
         LETTER              Scan the rest of the string and
                             copy it into a1 buffer.
       THEN
       ;
 
: WORD                       ( n -- a )
                             Parse out the next word from the
                             input buffer, using n as the delimiter.
                             The parsed word is placed in the
                             buffer at address a as a packed,
                             count string.
       >R                    Save n, the delimiter.
       H @  2* 1 +           Byte address of the destination
                             string buffer.  Leave one byte
                             for the length of string.
       DUP                   Need two copies of this byte address.
       >IN @                 Character pointer of the parser.
       BLK @ IF              If BLK is not zero, we are processing
                             text in a disk buffer.
         BLK @ BLOCK         Get the disk block and the buffer
                             address.
         +                   Address of the character cell
                             currently being processed.
         1024                Maximum characters in the disk buffer.
       ELSE                  BLK is 0.  Input is from the terminal
                             input buffer.
         MSG @ +             Address of the character in buffer
                             to be interpreted.
         CNT @               Total number of characters received.
       THEN
       >IN @                 Interpreter pointer.
       OVER >IN !            Save the total character count
                             in >IN.
       -                     Remaining character count between
                             interpreter pointer and end of
                             input buffer.
       R> SR I!              Store the delimiter in SR register.
       -LETTER               Parse out the next word and copy
                             it into the word buffer above
                             HERE.
       DROP                  Discard the input buffer address.
       32 OVER C!            Append a space after the parsed
                             word.
       SWAP-                 Character count of the parsed
                             word.
       H @  2* C!            Store the count at the beginning
                             of the parsed word as a packed
                             count string.
       H @                   Return the address of the word
                             buffer.
       ;
 
 
 
6.2.   Dictionary Search
       With the NC4000 chip, the code field and the parameter fields in a regular Forth system must be merged into a single field, as the inner interpreters NEXT, NEST ( DOCOL ), and UNNEST ( ;S ) are all taken care of by hardware.  A word defined in this system thus consists of three fields:  a link field, a name field, and the code/parameter field.  The bit arrangements in the name field can be shown as follows:
 
       r0sn nnnn tccc cccc             r: remote bit
       0ccc cccc 0ccc cccc             s: smudge bit
               ...                     n: character count
               ...                     t: truncation bit
       0ccc cccc tccc cccc             c: ASCII character
 
The truncation bit in the last cell of the name field indicates to the text interpreter that a long name is truncated and name comparison must stop at that cell.
       The dictionary contains two vocabularies, FORTH and COMPILER.  The link field addresses  of the last words in these vocabularies are stored in an array, immediately prior to the system variable CONTEXT.  Each cell in this array contains a pointer, pointing to the link field of the last word defined in the corresponding vocabulary.  Each link field contains a pointer pointing to the link field of the previously defined word.  The other end of the thread is the first word in the linked chain, whose link field contains a zero as the end-of-link indicator.  To locate a word in a vocabulary, the link field address is first obtained from the BUFFERS array in front of CONTEXT.  Then the linked chain is followed to see if the parsed string can match a name in that vocabulary.
 
 
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
 