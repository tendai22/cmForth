# CHAPTER III.   THE cmFORTH OPERATING SYSTEM
 
This version of Forth, cmFORTH, was developed by Chuck Moore, the chief architect of the Novix-4000 Forth engine and the inventer of the Forth language.  It is used in Chuck's Gamma Board and in Software Composers' SC1000C single board computer as their operating system.  Chuck Moore and Novix kindly donated this remarkable software package into the public domain to encourage users exploring the phenominal capabilities of the NC4000 chip.

This chapter is intended to be the documentation for cmFORTH.  I will attempt to go through the system in minute detail, in order to help those who are unfamiliar with the Forth language as well as those who are not familiar with Chuck Moore's coding style.  In any case, the source listing itself is the primary documentation and the descriptions in this chapter are at best commentary to the source listing.  The complete source listing of cmFORTH is included in this book as Appendix A.  A program to burn target compiled object code into EPROM's is in Appendix B.  A short program which must be implemented in a host computer to act as a terminal/disk server for the NC4000 computer is in Appendix C.

The original source listing of cmFORTH is your best source of code examples and programming style, when striving to use the NC4000 chip most effectively.  You are encouraged to study this listing very carefully in order to gain maximum benefit from the chip.  The source code is only 30 screens long.  It would not be very difficult to study it and to be thoroughly familiar with it.

There are many important innovations in cmFORTH worthy of your special attention.  First is the optimizing compiler which takes normal Forth source code and compiles very short and efficient NC4000 machine instructions.  Next is the dual threaded vocabulary structure in which all the compiler directives and smart compiling/assembling words are linked together, separate from the regular FORTH vocabulary.  The interpreter searches only the FORTH vocabulary.  The compiler first searches the COMPILER vocabulary and then the FORTH vocabulary.  This searching method eliminates the necessity of immediate words.  Lastly, a very simple linking method is used to compile words into a target dictionary, which can later be isolated from the main dictionary and burned into EPROM.  These innovations in time will make significant impact in the Forth community, leading to better and more efficient Forth systems and programs.

このForthのバージョンであるcmFORTHは、Novix-4000 Forthエンジンのチーフアーキテクトであり、Forth言語の発明者であるChuck Moore氏によって開発されました。 ChuckのGamma BoardやSoftware ComposersのSC1000Cシングルボードコンピュータのオペレーティングシステムとして使用されています。 Chuck MooreとNovixは、NC4000チップの驚異的な能力を探求するユーザーを奨励するために、この驚くべきソフトウェアパッケージをパブリックドメインに寄贈してくれました。

この章は、cmFORTHのドキュメントとなることを意図しています。 Forth言語に不慣れな人や、Chuck Mooreのコーディングスタイルに馴染みのない人の助けになるように、システムの細部まで説明することを試みます。 いずれにせよ、ソースリストそのものが主要な文書であり、本章の記述はせいぜいソースリストの解説に過ぎない。 cmFORTHの完全なソースリストは、本書に付録Aとして収録されています。 ターゲットコンパイルされたオブジェクトコードをEPROMに書き込むプログラムは、付録Bにあります。 NC4000 コンピュータのターミナル/ディスクサーバーとして動作するために、ホストコンピュータに実装されなければならない短いプログラムは、付録Cにあります。

cmFORTHのオリジナルのソースリストは、NC4000チップを最も効果的に使用しようとするとき、コード例とプログラミングスタイルの最良の情報源となります。 このチップから最大限の利益を得るために、このリストを非常に注意深く研究することが推奨されます。 ソースコードの長さはわずか30画面です。 それを勉強し、徹底的に熟知することは、それほど難しいことではないでしょう。

cmFORTHには、特別な注意を払うに値する多くの重要な革新的技術があります。 まず、最適化コンパイラが、通常のForthソースコードから、非常に短く効率的なNC4000マシン命令をコンパイルします。 次に、デュアルスレッド語彙構造で、すべてのコンパイラ指令とスマートコンパイル/アセンブル語は、通常のFORTH語彙とは別に、一緒にリンクされています。 インタプリタは、FORTHの語彙だけを検索します。 コンパイラは、まず COMPILER 語彙を検索し、次に FORTH 語彙を検索します。 この検索方法によって、直前の単語が不要になります。 最後に、非常に簡単なリンク方式で、単語をターゲット辞書にコンパイルし、後にメイン辞書から分離してEPROMに焼くことができる。 これらの技術革新は、Forthコミュニティにおいて大きな影響を与え、より良い、より効率的なForthシステムやプログラムを生み出すことになるでしょう。
.new
 
 
## 1.   The Kernel

The kernel in a Forth system is a collection of low level instructions which drive the computer and are used to construct other high level instructions.  In cmFORTH, the kernel contains primarily simple NC4000 machine instructions.  However, many of the commonly used Forth words do not have corresponding single word NC4000 instructions and they will have to be synthesized from the primitive NC4000 instructions.
 
 
## 1.1.   The primitive Forth Words

Primitive stack operators are the machine instructions of the NC4000 chip.  However, each machine instruction must have two versions, one to be executed by the text interpreter and the other for the compiler.  The executable version must be a colon defintion with names so that they can be found by the text interpreter.  They can be compiled into other colon definitions, but the resulting code will be terribly inefficient because of the overhead in nesting and unnesting.  The other version used by the compiler is smart.  It generates optimized machine code whenever feasible, taking advantage of the unique property of NC4000 in combining many Forth words into a single machine instructions.

The following primitive Forth words are redefined so that they can be executed by the text interpreter.

Forthシステムにおけるカーネルは、コンピュータを駆動し、他の高レベル命令を構築するために使用される低レベル命令のコレクションです。 cmFORTHでは、カーネルは主に単純なNC4000マシン命令を含んでいます。 しかし、よく使われるForthの単語の多くは、対応する一語のNC4000命令を持たないので、原始的なNC4000命令から合成する必要があります。
 
 
## 1.1.   原始的なForthの単語

原始スタック演算子はNC4000チップの機械命令である。 ただし、各機械命令には、テキストインタプリタが実行するバージョンと、コンパイラが実行するバージョンの2つが必要です。 実行可能なバージョンは、テキストインタプリタによって見つけられるように、名前を持つコロン定義でなければなりません。 他のコロン定義にコンパイルすることもできますが、ネストやアンネストのオーバーヘッドが発生するため、出来上がるコードはひどく非効率的なものになります。 コンパイラが使用するもう1つのバージョンはスマートです。 これは、多くのForth語を1つの機械命令にまとめるというNC4000のユニークな特性を利用して、実現可能な限り最適化された機械コードを生成します。

以下の原始的なForth語は、テキストインタプリタによって実行できるように再定義されています。
``` 
: SWAP   SWAP ;
: OVER   OVER ;
: DUP    DUP  ;
: DROP   DROP ;
: XOR    XOR  ;
: AND    AND  ;
: OR     OR   ;
: +      +    ;
: -      -    ;
: 0<     0<   ;
: NEGATE NEGATE ;
: @      @    ;
: !      !    ;
```
Many other commonly used Forth words cannot be reduced to single NC4000 instructions, so they have to be constructed with several NC4000 machine instructions.

その他、よく使われるForthの言葉の多くは、NC4000の単一命令に還元できないため、複数のNC4000マシン命令で構成する必要がある。
``` 
: ROT                        ( n1 n2 n3 -- n2 n3 n1 )
       >R SWAP               Exchange n1 and n2.
       R> SWAP               Exchange n1 and n3.
       ;
 
: 0=                         ( n -- f )
       IF 0 EXIT THEN        Return false if not 0.
                             EXIT is cheaper and faster.
       -1                    Can be obtained from a register.
       ;
 
: NOT                        ( n -- f )
       0=                    Logic NOT, not one's complement.
       ;
 
: <                          ( n1 n2 -- f )
       - 0<
       ;
 
: >                          ( n1 n2 -- f )
       SWAP-                 A NC4000 primitive instruction.
       ;
 
: =                          ( n1 n2 -- f )
       XOR 0=
       ;
 
: U<                         ( u1 u2 -- f )
       - 2/                  Get the carry of subtraction.
       0<                    Return proper flag.
       ;
 
: ?DUP                       ( n -- n n | 0 )
       DUP
       IF DUP EXIT THEN      EXIT is faster.
       ;
 
: WITHIN                     ( n low high -- f )
       OVER - >R             high - low
       -                     n - low
       R> U<                 In range?
       ;
 
: ABS                        ( n -- u )
       DUP
       0<                    Negative?
       IF NEGATE EXIT THEN   Invert negative number.
       ;
 
: MAX                        ( n1 n2 -- n1 | n2 )
       OVER OVER -           n1 - n2
       0<                    Compare.
       IF SWAP-DROP EXIT     n1 < n2, drop n1.
       THEN DROP             Otherwise, drop n2.
       ;
 
: MIN                        ( n1 n2 -- n1 | n2 )
       OVER OVER -           n1 - n2
       0<
       IF DROP EXIT          n1 > n2, drop n2.
       THEN SWAP-DROP        Otherwise, drop n1.
       ;
```
The funny IF-BEGIN... UNTIL-THEN structure in Screen 11, connecting the two definitions MAX and MIN, in effect achieves the above function with a net saving of four instructions.  You can do it because cmFORTH does not have compiler security and protection.  Not recommended for general programming practice.

画面11の面白いIF-BEGIN... 画面11のUNTIL-THEN構造で、MAXとMINの2つの定義をつなぐことで、実質的には4命令の正味の節約で上記の機能を実現することができます。 cmFORTHにはコンパイラのセキュリティやプロテクションがないのでできることです。 一般的なプログラミングの実践にはお勧めできません。
``` 
: 2DUP                       ( d -- d d )
       OVER OVER
       ;
 
: 2DROP                      ( d -- )
       DROP DROP
       ;
```

## 1.2.   Memory Accessing Words
``` 
: +!                         ( n a -- )
       0 @+                  Fetch from a, while keeping a
                             on the stack.
       >R                    Save a.
       +                     Add n to contents of a.
       R> !                  Store the sum back into a.
       ;
 
: 2C@+                       ( a -- a+1 l h )
       1 @+                  Fetch a cell and increment a.
       SWAP DUP              Get the contents just fetched.
       127 AND               Isolated the low byte.
       SWAP 6 TIMES 2/       Right justify the high byte.
       ;
 
: 2/MOD                      ( n -- rem quot )
                             Equivalent to   2 /MOD  but faster.
                             Needed to convert byte address
                             to cell address.
       DUP 1 AND             Get the remainder.
       SWAP 2/               Shift left to get quotient.
       ;
 
: C!                         ( b a -- )
                             a is byte address, which has to
                             be converted to cell address.
       2/MOD DUP >R          Save cell address.
       @                     Cell contents.
       SWAP                  Byte offset.
       IF -256 AND           Offset=1.  Mask off lower byte.
       ELSE 255 AND          Offset=0.  Mask off higher byte.
         SWAP                Get the byte b.
         6 TIMES 2*          Shift left by 8 bits.
       THEN
       XOR                   Combine two bytes.
       R> !                  Put the cell back.
       ;
 
: C@                         ( a -- b )
       2/MOD                 Get the cell address.
       @                     Contents of the cell.
       SWAP 1 -              Offset=1 ?
       IF 6 TIMES 2/ THEN    Yes.  Shift right by 8 bits.
       255 AND               Mask off the high byte.
       ;
 
: 2@                         ( a -- d )
       1 @+                  Get the most significant cell.
       @                     Get the least significant cell.
       SWAP                  Put them in correct order.
       ;
 
: 2!                         ( d a -- )
       1 !+                  Store the most significant cell.
       !                     Store the next cell.
       ;
``` 
NC4000 is a 16 bit machine and it can access memory only by 16 bit cells.  Two bytes are packed into one cell, with the first byte in the higher half (MSB) of the cell.  The byte address is twice that of the cell address, with the least significant bit as the byte offset in a cell.  To access one byte in the memory, one has to convert the byte address to a cell address by 2/MOD and use the quotient as an offset to find the requested byte.  It takes lots of extra work to do byte addressing.  Avoid it at all cost.

If you really have to get bytes from the memory, the right word to use is 2C@+ which fetches one cell from the memory and returns both bytes on the stack.  The address is incremented by 1 and preserved as the third element on the stack so you can fetch the next two bytes.  It is faster than C@ and much more powerful.

NC4000は16ビットマシンであり、16ビットセルによってのみメモリにアクセスすることができます。 1セルに2バイトが詰め込まれ、1バイト目はセルの上位半分(MSB)にある。 バイトアドレスはセルアドレスの2倍で、最下位ビットがセル内のバイトオフセットとなる。 メモリ内の1バイトにアクセスするには、バイトアドレスを2/MODでセルアドレスに変換し、その商をオフセットとして使用して要求されたバイトを見つける必要があります。 バイトアドレッシングを行うには、多くの余分な作業が必要です。 何としても避けてください。

もし本当にメモリからバイトを取得する必要がある場合、使用する正しい単語は2C@+で、メモリから1つのセルをフェッチし、スタックに両方のバイトを返します。 アドレスは1つインクリメントされ、スタックの3番目の要素として保存されるので、次の2バイトをフェッチすることができます。 C@より速く、より強力です。
``` 
: MOVE                       ( a1 a2 n -- )
                             a1 is the starting address of the
                             source, and a2 is the end address
                             of destination string.  Be careful!
       >R                    Save cell count.
       MD I!                 Save a2 in MD register.
       I TIMES 1 @+          Fetch n cells to the data stack.
       MD I@!                Retrieve a2.
       R>                    Retrieve cell count.
       TIMES 1 !-            Pop cells to destination in
                             reverse order.
       DROP                  Discard last address.
       ;
 
: FILL                       ( a # n -- )
                             Fill a memory range with cell value n.
       SWAP 1 - >R           Push n-1 on return stack as count.
       SWAP                  ( n a -- )
       BEGIN
         OVER SWAP           ( n n a -- )
         1 !+                Non-destructive store with a incremented.
       NEXT
       2DROP                 Clear stack.
       ;
 
: ERASE                      ( a # -- )
                             Zero a range of cells.
       0 FILL
       ;
```
 
## 1.3.   Multiply and Divide

NC4000 does not have single instruction multiply or divide, which requires a lot of gates to implement.  What is provided are multiply steps, divide steps, and square-root steps, which can be used repetitively to achieve the desired results.  Problems in processing the carry bit in the prototype chip cause some restrictions in multiplication.  The software fixes to perform correctly the proper function are not implemented.  You have to work around these bugs.  An example is included in Chapter IV.

NC4000は単命令の乗算、除算を持たないので、実装に多くのゲートを必要とする。 そこで、乗算、除算、平方根の各ステップを用意し、これらを繰り返し使用することで、目的の結果を得ることができる。 試作チップではキャリービットの処理に問題があり、乗算に制約が生じることがある。 本来の機能を正しく実行するためのソフトウェアの修正が実装されていない。 このバグを回避する必要があります。 例題は第IV章に含まれています。
```
OCTAL
 
: U*+                        ( u1 r u2 -- d )
                             Unsigned integers u1 and u2 are
                             multiplied and added to r.  The
                             product is an unsigned double
                             integer on the stack.
                             Warning: u2 must be even!
       MD I!                 Store u2 in MD register.
       16 TIMES *'           Repeat multiply step instruction
                             *' 16 times and the product is
                             left on the stack.
       ;
 
: M/MOD                      ( ud u -- q r )
                             Unsigned double integer ud is divided
                             by unsigned integer u.  Both quotient
                             and remainder are left on the
                             stack.  Note the order of q and
                             r is not standard.
       MD I!                 Store u in MD register.
       D2*                   Left shift d by 1 bit so that
                             it is always even.
       15 TIMES /'           Repeat divide step /' 15 times.
       /''                   Last divide step.
       ;
 
: -M/MOD                     ( d u -- q r )
                             Double integer d is divided by
                             unsigned integer u.
       OVER 0<               Is d negative?
       IF
         DUP >R              Save u.
         +                   Add u to the higher half of d
       THEN                  for floored division.
       M/MOD                 Do the divide now.
       ;
 
: M/                         ( d u -- q )
                             Mixed mode divide.
       -M/MOD DROP           Discard remainder.
       ;
 
: M*+                        ( u1 0 u2 -- d )
                             Unsigned multiply.
       MD I!                 Copy u2 to MD register.
       13 TIMES *'           Repeat multiply step.
       *-                    Last signed multiply step.
       ;
 
: VNEGATE                    ( n1 n2 -- n3 n4 )
                             Negate top two integers on the
                             stack.
       NEGATE                Negate top integer.
       SWAP NEGATE SWAP      Negate next integer.
       ;
 
: M*                         ( n1 n2 -- d )
                             Mixed mode multiplication of two
                             signed integers.
       DUP 0<                Is n2 negative?
       IF VNEGATE THEN       If so, negate both integers.
       0 SWAP                initialize accumulator.
       M*+                   Do the multiplication.
       ;
 
: /MOD                       ( u1 u2 -- r q )
                             Divide unsigned integers and return
                             both remainder and quotient.
       0 SWAP                Insert 0, making dividend a double
                             integer.
       M/MOD                 Do the mixed mode divide.
       SWAP                  Correct the order of results.
       ;
 
: MOD                        ( u1 u2 -- r )
                             Find remainder of unsigned integer
                             division.
       /MOD                  Do the generalized divide.
       DROP                  Discard quotient.
       ;
 
: */MOD                      ( u1 u2 u3 -- r q )
                             Multiply u1 and u2.  Divide the
                             double integer product by u3.
                             Return both remainder and quotient.
       >R                    Save divisor u3.
       0 SWAP U*+            Multiply u1 and u2.
       R> M/MOD              Divide by u3.
       SWAP                  Correct the order of r and q.
       ;
 
: */                         ( n1 n2 u -- r )
                             Ratio of n1*n2/u.
       >R                    Save u.
       M*                    Signed multiply of n1 and n2.
       R> M/                 Divide by u.
       ;
 
: *                          ( n1 n2 -- r )
                             Signed multiply.
       0 SWAP U*+            Signed multiply.
       DROP                  Discard high order cell.
       ;
 
: /                          ( n u -- q )
                             Divide by unsigned integer.
       >R                    Save divisor u.
       DUP 0<                Extend the sign of n.
       R> M/                 Divide.
       ;
.new
``` 
## 2.   System Variables

System variables contain vital information needed by the Forth system to function.  Most of them are pointers to various areas in the Forth system, such as the top of the dictionary, the disk buffers, the terminal input buffer, the vocabulary threads, etc.  System variables in this implementation are kept at the bottom of RAM space, starting from location 16.  Thus the first 16 system variables are in the so called local memory, which can be accessed by single cell instructions.  These are the most frequently used system variables.  Less frequently used variables are kept above location 35.

Following is the list of system variables defined in this implementation, their memory locations, their initial values if initialized, and their function.

システム変数には、Forthシステムが機能するために必要な重要な情報が含まれています。 そのほとんどは、辞書の先頭、ディスクバッファ、端末入力バッファ、語彙スレッドなど、Forthシステム内のさまざまな領域へのポインタである。 この実装におけるシステム変数は、RAM空間の最下部に位置する16番から保持されます。 したがって、最初の16個のシステム変数は、いわゆるローカルメモリにあり、シングルセル命令でアクセスすることができる。 これらは、最も頻繁に使用されるシステム変数です。 使用頻度の低い変数は、ロケーション35より上に保持されます。

以下に、この実装で定義されたシステム変数のリストとそのメモリ位置、初期化された場合の初期値、およびその機能を示します。
``` 
PREV                    Memory 16, not initialized.
                             Pointer to the most recently
                             referenced disk buffer.
 
OLDEST              Memory 17, not initialized.
                             Pointer to the oldest loaded
                             disk buffer.
 
BUFFERS             Memory 18 and 19, not initialized.
                             Storing block numbers of blocks
                             in the disk buffer.
 
NB                       A constant of value 1.
                             Number of disk buffers less 1.
 
BASE                   Memory 20, initialized to 10.
                             Number base for numeric I/O
                             conversion.
 
CNT                     Memory 21, not initialized.
                             Count of characters received from
                             the terminal device.
 
>IN                       Memory 22, not initialized.
                             Pointer to the input stream of
                             characters.  Used by WORD to parse
                             strings.
 
BLK                      Memory 23, initialized to 0.
                             Contains the block number under
                             interpretation.
 
?CODE                 Memory 24, initialized to 0.
                             Storing the address of the machine
                             code most recently compiled.
                             Used by the optimizing compiler
                             to construction multi-function
                             instructions.
 
dA                        Memory 25, initialized to 0.
                             Memory address offset to be subtracted
                             from the current address so that the
                             dictionary compiled can be relocated
                             to another part of memory.
 
MSG                    Memory 26, initialized to the
                             end of the system variable area.
                             Pointer to the terminal input
                             buffer.
 
CURSOR              Memory 27, initialized to 0.
                             Pointer to the memory location where
                             the last input character is stored.
 
WIDTH                Memory 28, initialized to 2.
                             Cells in the name field of an
                             entry in the dictionary.
 
OFFSET               Memory 29, initialized to 0.
                             Block number which became the
                             logic 0 block during disk access.
 
H                         Memory 30, initialized to 64 cells
                             after terminal input buffer MSG.
                             Pointer to the top of the current
                             dictionary.
 
C/B                       Memory 31, initialized to 417.
                             Machine cycles equivalent to the
                             width of a bit riding the RS-232
                             terminal port.
 
Interrupt Vector    Memory 32 to 33, not initialized.
                             Machine instructions are stored
                             here to handle interrupt requests.
 
Thread Table        Memory 34 and 35, pointers to the
                             ends of 2 threads in the dictionary.
                             The dictionary and vocabularies
                             are hashed into 2 threads.  The
                             name field addresses at the end
                             of each thread are stored in this
                             table for dictionary searching.
 
CONTEXT            Memory 36, initialized to 1.
                             Hash code of the context
                             vocabulary.
```
