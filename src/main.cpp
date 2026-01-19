/*

  ____________________

  Welcome to IVEE2 - A Powerful Programable and Expandable RPN Calculator based on IVEE
  Version 1.0, (c) 2026 by emefff
  This software is covered by the 3-clause BSD license.
  See also: https://github.com/emefff/ivee2 
  ____________________


IVEE2 is a port of IVEE to nice!nanoV2. All credit for the orginal design goes to detee, his code for Arduino 
ProMicro is the basis for this calculator. You can find the original code in

https://github.com/zooxo/iv

With his kind permission, I can publish the ported code. Basic functions of the calculator are 
the same, with some enhancements of built-in code and the benefits that come with a more capable chip. 

The main purpose of this port is to have a simple calculator like IVEE and at the same time have a calculator
that still can evolve. This was not possible on ProMicro because there was only a single byte left in flash. 
I strongly encourage everyone to add functions (both in code and FORTH) and to learn about
how a calculator works. Maybe there's also an educational purpose/value in this for EE or SE students.

Kilo Code was used in devloping this calculator, models used were mostly Codex-5.1, MiniMax M2.1 and Claude Sonnet 4.5.
The calculator features a lot of diagnostics over serial and was quite extensively tested. The main development took 
about 2.5 weeks in Dec. 2025 with 2-3 hours per day, more on weekends. The author is not a professional programmer or software
engineer, rather an advanced amateur software and calculator enthusiast. 

Changes and enhancements include:

-) enhanced floating point precision, doubles are now double precision instead of single.
-) 4 line stack display. I prefer this over the 2 line display in the original.
-) 32kB user memory for more storage in program mode and also longer programs. 
-) Gauss Kronrod integration instead of Simpson rule: Changing to this method allows for solving very difficult integrals
   numerically. The accuracy of this method can be adapted via GK_MAX_DEPTH. Currently it is set for acceptable duration.
   The implemented integration algorithm beats HP42, HP15C and HP41c (with ADVANCED or PPC module integration routines) 
   any time in both speed and accuracy.
-) WP34s style solver: the solver of WP34s written in assembler was ported. It is very capable and changes between different
   methods combining bisection, secant, quadratic interpolation, and Ridder's method. The original code can be found here:
   https://sourceforge.net/projects/wp34s/files/ .
-) 100 registers in flash ranging from 0-99, storing and recalling via usual commands STO and RCL.
-) 100 registers in RAM ranging from 0-99: storing and recalling via two new commands STR and RCR.
-) 1x1 pixel graphs for more accurate graphs in FPLOT.
-) 5 digit floating point display.
-) wear leveling of flash store. nice!nanov2 can only delete 4kB pages. In order to be usable for years, a rotating wear-leveling is 
   applied to the whole user storage.
-) the new TOP (moveprgtop() in code) command moves a program to top of list in PRG mode: In order to not have to repeatedly UP 
   a program to the top of the list in PRG mode, mainly to FPLOT a function, the PRG chosen directly moves to the top of the list. 
   Sorting is done in RAM, only a single rewrite or user flash is necessary. TOP is on the 9 key (= KEY4 in code) in PRG mode.
-) Annunciator (_, f, g) moved to top right corner.
-) Builtin functions use RAM registers instead of flash.
-) CLK function slightly changed, enter hours, minutes, seconds consecutively on stack:
   e.g. 11 ENTER, 22 ENTER, 45 CLK
   starts the clock at 11:22:45. The clock does not use the built-in RTC of the nice!nanov2 but is drift compensated

   
This concludes the most important changes exposed to the user.

The following part of this docstring is mostly copied from https://github.com/zooxo/iv and adapted to IVEE2 to explain the basic original
and the new functions:

Ivee2 is operated by 16 keys only it offers a wide range of functions
  and possibilities:
  - 122 intrinsic functions based on FORTH
  - Programming: Handle up to 40 user definable programs with max. 256 steps
                 (Edit, New, Rename, Move, Delete, Send, Load)
  - Dictionary of all commands (sorted), words and programs
  - Fast user definable menu (longpressed F-key)
  - Calculations with complex numbers for nearly every function
  - Calculus (f(x), plot, solve, integrate)
  - Basic math operations in number systems with selectable BASE
  - Business calculator mode (accuracy of 18 digits)
  - Save up to 100 numbers/constants permanently
  - Save up to 100 numbers in RAM
  - Statistics and line best fit (not tested yet)
  - Many unit conversions
  - Clock and Torch function
  - Basic system functions (Battery voltage, Brightness (does not work with my SSD1306, though), Screensaver)


  ____________________

   KEYBOARD, SHORTCUTS
  ____________________

    F(G)[OFF]{MENU}  7(S+)[SCLR]    8(PRG)[BASE]    9(/)[MOD]
    E(SWAP)[ROT]     4(DICT)[USR]   5(FX)[FPLOT]    6(*)[LIT]
    N(OVER)[PICK]    1(RCL)[STO]    2(SOLVE)[FINT]  3(-)[LIT-]
    X(CLR)[TOFF]     0(CPLX)[BATT]  .(R<>P)[CLK]    #(+)[LIT+]

    PRG Shortcuts:                  BASE/HEX Shortcuts:
                    9 TOP            
             5 OUT  6 UP              4 E  5 F
      1 NEW  2 IN   3 DOWN            1 B  2 C    3 D
      0 REN  . DEL  D EDIT            0 A  . A~F

    MENU/DICT/REN Navigation:       SUM/STAT/L.R. Storage Registers:
             Q1  Q2  Q3  Q4           7 SN  8 SX   9 SY
      UP     E   4   5   6                  5 SXX  6 SXY
      DOWN   N   1   2   3
             PREV0   .NEXT


  ____________________

   DISPLAY, INDICATORS
  ____________________

    >        ... Empty stack, ready to calculate
    >        ... Indicates selected program in program selection mode
    <        ... Indicates selected program step in program editing mode
    a~z      ... Stack level
    _        ... Indicates number input
    i, angle ... Imaginary part, imaginary angle
    B        ... BASE mode indicator
    n BASE   ... Numeric base of BASE mode
    !        ... Indicates float of data stack (data loss in lowest stack level)
    ^, v     ... F-key, G-key
                
                
  ____________________

   LIMITS
  ____________________

    26   ... Maximum data stack size (a...z)
    7    ... Maximum number of displayed significant digits of a number
    99   ... Maximum number of decimal exponent digits (1E-99 < X < 1E99)
    12   ... Maximum size of text display (see EMIT, CTX)
    100  ... Maximum amount of (complex) numbers saved permanently (0...99)
    100  ... Maximum amount of (complex) numbers saved in RAM      (0...99)
    3    ... Maximum number of characters for naming an user program
    40   ... Maximum number of user programs
    256  ... Maximum size of an user program (steps)
    ~32000  ... Maximum size (steps) of all user programs
    64   ... Maximum size of address stack ("deep of nesting")
    32   ... Maximal definable command slots of user menu                
                

  ____________________

   BROWSING MENUS
  ____________________

    To navigate through the menu of some functions (MENU, DICT, USR, renamePRG)
    all selectable items are divided into four sections. Every section has its
    own up and down key (section I: E/N, section II: 4/1, section III: 5/2 and
    section IV: 6/3).
    To move to the previous or next entry (regardless which section is selected)
    the keys "0" or "." can be used. To select one of the four presented items
    use the appropriate function key (F/7/8/9) or escape the menu with "X".


  ____________________

   PROGRAMMING
  ____________________

    Ivee2 is able to deal with up to 40 user programs with a total number of
    ~32000 steps/commands. To deal with programs enter the program selector mode
    with PRG.
    While in the program selection mode on the left side of the display the
    available programs are numbered and listed. On the right side of the display
    the number of programs, the memory used and the memory available are shown.
    To navigate between programs use the cursor keys E (up) and N (down).
    Edit a selected program with # or press 1 to enter a new program.
    To delete a program press "." - to rename it use the key 0.
    You can move a program with 6 up or with 3 down. A program can be moved to the 
    top of the list with TOP pressing 9. Note that the top program
    has special calculus features (f(x), plot, solve, integrate).
    To send a program (does not include the program name) via USB to another
    computer press "5". Note that corresponding ascii characters will be sent
    (see chapter DICTIONARY, second column). In a similar way programs can be
    send from another computer to Ivee (press key 2) and will be stored as new
    program.
    With the 3 powerful commands @, ! and EXE (see chapter DICTIONARY) it is
    even possible to manipulate the program memory and execute code.
    A program is structured into the three parts NAME, COMMANDS and EOP-marker.
    Please note that the length of the program name is restricted to 3
    characters and the maximum number of program steps is restricted to 256.
    
    
  ____________________

   BASE, BUSINESS MODE
  ____________________

    To calculate with integer values with other numeric base enter the base and
    select the BASE command (ie 16 BASE to calculate with hexadecimal numbers).
    To return to the scientific mode press BASE again. When changing the mode
    the whole stack will be converted (as far as appropriate) to the new base.
    Numbers will be displayed in groups of four digits in two lines with a
    B-indicator (BASE) on the left side and the base itself on the right side of
    the first line. Pressing "E" enters four zeros.
    Note that only basic operations (/*-+) and some stack operations are
    supported. To enter digits bigger than 9 (ie F) press "." followed by a key
    (ie 5 to enter F).
    The BASE can be stored permanently. However, currently it is set to start in 
    scientific mode, always.
    Entering the BASE mode with 10 activates the business mode to deal with big
    numbers and high accuracy, like trillions of dollars with cent accuracy.
    Numbers are grouped into triples and two digits after the decimal dot
    (cents) are supported.


  ____________________

   CALCULUS
  ____________________

    Ivee2 is able to analyze the first user defined program. Note that you can
    move the desired user program to the first position with the TOP command pressing
    9 directly or using the keys 6 or 3 in program mode (PRG).
    To calculate the value of a function use F(X).
    To find the root of a function enter a start interval (X and Y) and press FSOLVE. 
    There are measures in place, that start the solver anyway with random values, if no
    value is on the stack or only one value. However, in a function with multiple roots
    you will likely not find the desired root (or none at all!) if no interval is given.
    Enter the x-range (from Y to X) before plotting a function (FPLOT) or
    calculating an integral (FINT).


  ____________________

   COMMANDS
  ____________________

  MEMORY
       functions         mem[]       EEPROM
    |<--intrinsic-->|<--builtin-->|<--user-->|
     0               76            121        164
              MAXCMDI^      MAXCMDB^   MAXCMDU^


  OVERVIEW
    Intrinsic
      Forth  DUP DROP NEGATE / * - + MOD DICT SWAP ROT PICK
             < = <> > KEY EMIT BASE BEGIN UNTIL IF ELSE THEN @ !
      System 0~9 . E STO RCL CLR USR NAND INTEGER PI INV SIN EXP LN
             COMPLEX REC<>POL F(X) FPLOT FSOLVE FINTEGRATE
             PRG T+ PSE EXE BREAK CLRTEXT ISREAL
             SUM+ SUM- SUMCLR COMB PERM STAT LR
             BATT LIT LIT+ LIT- CLOCK OFF TOFF STR RCR
    Builtin
      Forth  OVER HEX AND NOT OR ABS
      Other  $ SQRT POWER POWER10 LOG LN! PV ND QE HMS>H H>HMS CLOCK
             COS TAN ASIN ACOS ATAN SINH COSH TANH ASINH ACOSH ATANH
             DEG>RAD RAD>DEG C>F F>C KM>MI MI>KM M>FT FT>M CM>IN IN>CM


  DICTIONARY (in order of "appearance")
    000   0     ... Numbers
    001 ! 1
    002 " 2
    003 # 3
    004 $ 4
    005 % 5
    006 & 6
    007 ' 7
    008 ( 8
    009 ) 9
    010 * .     ... Dot key
    011 + DUP   ... Duplicate, end number input
    012 , DROP  ... Drop X, clear entry
    013 - N     ... NEG, change sign (negate X)
    014 . E     ... Enter 10th exponent (actually Y*10^X)
    015 / F     ... Function key (Shift)
    016 0 CPLX  ... Enter complex number and vice versa (see R<>P)
    017 1 RCL   ... Recall memory (number = X) from permanent memory. Note that
                    the memories 5...9 are used as statistic registers.
    018 2 FSOLV ... Find the root of the first user defined program/function
                    (PRG) using X as start value
    019 3 -     ... Substract (X = Y-X)
    020 4 DICT  ... Catalog all functions (see section BROWSING MENUS)
    021 5 F(X)  ... Calculate first user defined program/function (PRG)
    022 6 *     ... Multiply (X = Y*X)
    023 7 S+    ... Add values of X- and Y-register to sums (see STAT/LR)
    024 8 PRG   ... Enter program mode (see section PROGRAMMING)
    025 9 /     ... Divide (X = Y/X)
    026 : R<>P  ... Toggle rectangular or polar mode (see CPLX)
    027 ; +     ... Addition (X = Y+X)
    028 < CLR   ... Clear stack and alpha register
    029 = OVER  ... Push/copy Y to stack (YX -> YXY)
    030 > SWAP  ... Swap last two stack contents (XY -> XY)
    031 ? G     ... Second function key (Shift-Shift)
    032 @ BATT  ... Show battery voltage (push to stack)
    033 A STO   ... Store Y memory (number = X). Note that the memories 5...9
                    are used as statistic registers (Sxx, Sxy, n, Sx, Sy).
    034 B FINT  ... Integrate the first user defined program (PRG) using
                    the x-range from Y to X
    035 C B-    ... Lower the brightness of the display (5 levels). Note that
                    the display has to be rebooted (needs two seconds).
    036 D USR   ... Set function key of (custom) user MENU
    037 E FPLOT ... Plot the first user defined program (PRG) using the the
                    x-range from Y to X. Escape with key "X".
    038 F LIT   ... Illuminate the whole display
    039 G SCLR  ... Clear all sum register. Note that statistic functions are
                    using the permanent memories 5...9.
    040 H BASE  ... Set number base for integer calculations (i.e. 2, 8, 16).
                    Note that setting the base to 10 enters the business
                    calculator mode (see chapter BASE, BUSINESS MODE).
    041 I MOD   ... Modulo, division remainder (X = Y%X)
    042 J CLOCK ... Simple clock. To set the time use hh.mmss.
    043 K LIT+  ... Rise the brightness of the display (5 levels). Note that
                    the display has to be rebooted (needs two seconds).
    044 L TOFF  ... Set the screen off time (x 10 s) (>=3)
    045 M PICK  ... Copy n-th number of stack to stack
    046 N ROT   ... Rotate 3 numbers of stack (ZYX -> YXZ)
    047 O DARK  ... Enter screen off manually (triple press F)
    048 P <     ... Condition less than (returns true, if Y<X)
    049 Q =     ... Condition equal (returns true, if Y=X)
    050 R <>    ... Condition not equal (returns true, if Y<>X)
    051 S GT    ... Condition greater than (returns true, if Y>X)
    052 T NAND  ... Logical NAND (used for other logical functions)
    053 U T+    ... Adds duration in ms to stack (since last call)
    054 V PSE   ... Pause program execution for X*250 ms and print screen
    055 W INT   ... Calculate integer value of X
    056 X @     ... Fetch the value from address (X) (see also ! and EXE)
    057 Y !     ... Store Y at X-address (see also @ and EXE)
    058 Z BEGIN ... Begin a BEGIN-UNTIL-Loop
    059 [ UNTIL ... Continue executing a BEGIN-UNTIL-Loop until X is true
    060 \ EXE   ... Execute code at X-address (see also @ and !)
    061 ] IF    ... Execute the following code if true
    062 ^ ELSE  ... Execute the following code if IF failed
    063 _ THEN  ... Terminate an IF(-ELSE) condition
    064 ` BREAK ... Pause code execution for input (key "X" resumes)
    065 a KEY   ... Wait for keypress and push the keycode to stack
    066 b CTX   ... Clear text area (written with EMIT)
    067 c EMIT  ... Emit ascii character of X
    068 d RE?   ... Return true, if X is a real value
    069 e nCr   ... Calculate the combination (of yCx)
    070 f nPr   ... Calculate the permutation (of yPx)
    071 g PI    ... Push the value of PI to stack
    072 h INV   ... Push the reciprocal value of X to stack
    073 i SIN   ... Push the sine of X to stack
    074 j EXP   ... Push the exponential value of X to stack
    075 k LN    ... Push the natural logarithm of X to stack
    076 l $     ... Activate the business calculator mode (10 BASE)
    077 m HEX   ... Activate HEX mode (16 BASE)
    078 n AND   ... Logical AND
    079 o NOT   ... Logical NOT
    080 p OR    ... Logical OR
    081 q OVER  ... Push/copy Y to stack (YX -> YXY)
    082 r ABS   ... Push the absolute value of X to stack
    083 s SQRT  ... Push the quare root of X to stack

    084 t COS   ... Push the cosine of X to stack
    085 u TAN   ... Push the tangent of X to stack
    086 v POW   ... Push Y raised to the power of X to stack (X = Y^X)
    087 w 10^   ... Push 10 raised to the power of X to stack (X = 10^X)
    088 x LOG   ... Push the 10 based logarithm of X to stack
    089 y ASIN  ... Push the arcus (inverse) sinus of X to stack
    090 z ACOS  ... Push the arcus (inverse) cosine of X to stack
    091 { ATAN  ... Push the arcus (inverse) tangent of X to stack
    092 | SINH  ... Push the hyperbolic sine of X to stack
    093 } COSH  ... Push the hyperbolic cosine of X to stack
    094 ~ TANH  ... Push the hyperbolic tangent of X to stack
    095 ? ASINH ... Push the area (inverse) hyperbolic sine of X to stack
    096 ? ACOSH ... Push the area (inverse) hyperbolic cosine of X to stack
    097 ? ATANH ... Push the area (inverse) hyperbolic tangent of X to stack
    098 ? LN!   ... Push the natural logarithm of gamma of X to stack
    099 ? >h    ... Convert hh.mmss to h (HMS->H)
    100 ? h>    ... Convert h to hh.mmss (H->HMS)
    101 ? PV    ... Present value of given interest rate and periods
    102 ? ND    ... PDF (X) and CDF (Y) of standard normal distribution
    103 ? QE    ... Quadratic equation xx+Yx+X=0 X|Y=-Y/2+-sqrt(Y*Y/4-X)
    104 ? CLOCK ... Simple clock (set with hh ENTER mm ENTER ss CLK)
    105 ? SCLR  ... Clear statistic registers (permanent memories 5...9)
    106 ? S+    ... Add values of X- and Y-register to sums (see STAT/LR)
    107 ? S-    ... Substract values of X- and Y-register to sums (see STAT/LR)
    108 ? STAT  ... Mean value (X) and standard deviation (Y). Note that the
                    permanent memories 5...9 (see RCL/STO) are used as statistic
                    registers (Sxx, Sxy, n, Sx, Sy).
    109 ? LR    ... Line best fit (y = X * x + Y)
    110 ? %     ... Percent (X/Y*100%)
    111 ? %CHG  ... Percent change (X-Y)/Y*100%
    112 ? FRAC  ... Fractional part
    113 ? ><d   ... Convert degrees to radians (and vice versa)
    114 ? ><C   ... Convert Celsius to Fahrenheit (and vice versa)
    115 ? ><km  ... Convert kilometer to miles (and vice versa)
    116 ? ><m   ... Convert meter to feet (and vice versa)
    117 ? ><cm  ... Convert centimeter to inches (and vice versa)
    118 ? ><kg  ... Convert kilogram to lbs (and vice versa)
    119 ? ><l   ... Convert liter to gallons (and vice versa)
    120 ? STR   ... Store Y memory (number = X) in RAM register.
    121 ? RCR   ... Recall memory (number = X) from RAM register. 


  ____________________

   MEMORY, STACKS
  ____________________

                                 | sizeof(mem)
           _ _ _ _ _ _ _ _ _ _ _ v
    mem[] |_|_|_|_|_ _ _ _ _|_|_|_|
            |<--builtinprograms-->|
            |         ^           |                    | sizeof(mem) +
           0|1        mp          |                    | sou=sizeofusr()
                      v           |_ _ _ _ _ _ _ _ _ _ v _ _ _ _ _ _ _ _
                         EEPROM[] |_|_|_|_ _ _ _ _|_|_|_|_|_|_|_|_|_|_|_|
                                  |<---user-programs--->|<-- @ ! EXE -->|
                                  |^EEUSTART                     EEUEND^|
                                  |<------- sou ------->|               |
                                  |<----------- 32768 bytes ----------->|


           DATASTACK                 ADDRESSSTACK            CONDITIONAL
      (2 x double, int64)                (int)                 POINTER
    |      |      |       |             |     |
    |______|______|_______|             |_____|               <cp (byte)
    |______|______|_______|<dp (byte)   |_____|
    |______|______|_______|             |_____|<ap (int)
    |__re__|__im__|_int64_|             |_____|
             ds[]                         as[]


  ____________________

   CIRCUIT DIAGRAM
  ____________________

  The hardware is quite simple:
    - nice!nanov2 (I recommend to use the original board)
    - OLED display (128x64 pixel) with SSD1306 controller
    - 16 keys (push buttons)
    - LiIon battery
    - On/Off switch. Connected to USB-C and switched ON, the battery is charged directly via the built-in 
      charging regulator on nice!nanov2. Switched OFF: USB-C flashing and serial monitoring
          
        
           ____________________________________    _________________________________________________          
          |  LED-DISPLAY 128x64 SPI SSD1306    |   |   __________  ______________________________  |  
          |____ 16___15___8____7____4___2___1__|   |   |         \                |             |  |
                |    |    |    |    |   |   |      |   |     ON/OFF switch        |            +|  |-
       _________|____|____|____|____|___|___|______|___|__________________        |          ___|__|___   
      |       P100 P024 P017 P020 P022 VCC GND    GND BAT+               |        |         |          |
      |        D6   D5   D2   D3   D4  3.3                               |        |         |  LiIon   |
      |               NICE!NANOV2                                        |        |         |  BL-5C   |
      |                                                                  |        |         |  1020mAh |
      |     P106 P009 P010 P111 P113 P115 P002 P029  P011                |        |         |__________|
      |__GND_D9__D10__D16__D14__D15__D18__D19__D20___D14___GND_______P031|        |         
          |   |    |    |    |    |    |    |    |    |     |          |          | 
          |   |    |    |    |    |    |    |    |    |     |_[R1 10k]_|_[R2 10k]_|
          |   F    |    7----8----9----+    |    |    |     
          |___|    |    |    |    |         |    |    |     voltage divider 1:1 for BAT+ measurement
                   E----4----5----6---------+    |    |   
                   |    |    |    |              |    |   4x4 Keyboard
                   N----1----2----3--------------+    |   (F-key on interruptable
                   |    |    |    |                   |   pin to enable sleep mode.
                   X----0----.----#-------------------+   ALL PINS ON NICE!NANOV2 
                                                          ARE INTERRUPTIBLE)
                  
A LiIon-Battery is recommended with nice!nanoV2 as it doesn't need additional charging board. 
I recommend a BL-5C because they are available everywhere, derived from a Nokia battery that seems to live forever.
The only board necessary in this calculator is nice!nanov2. Thus the design can be quite compact, depending on the display used.
Don't forget to include a reset button during development for flashing.

  ____________________

   ASCII TABLE:
  ____________________

      DEC     |  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
          HEX |  0 1 2 3 4 5 6 7 8 9 a b c d e f
      ------------------------------------------
      032 20  |    ! " # $ % & ' ( ) * + , - . /
      048 30  |  0 1 2 3 4 5 6 7 8 9 : ; < = > ?
      064 40  |  @ A B C D E F G H I J K L M N O
      080 50  |  P Q R S T U V W X Y Z [ \ ] ^ _
      096 60  |  ` a b c d e f g h i j k l m n o
      112 70  |  p q r s t u v w x y z { | } ~



*/

// ***** I N C L U D E S

#include "EEPROM.h" // substitute for <EEPROM.h>, a wrapper for its commands
#include <Arduino.h> // for serial monitor
#include <stdlib.h>

// display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// new solver
#include <cmath>
#include <utility>


#ifndef LOG_SEEKUSR
#define LOG_SEEKUSR 0 // 0 = logging off, 1 = logging on
#endif
#ifndef LOG_SORT
#define LOG_SORT 0
#endif
#ifndef LOG_IDOFUSR
#define LOG_IDOFUSR 0
#endif
#ifndef LOG_RENAME
#define LOG_RENAME 0
#endif
#ifndef LOG_PRG_MOVE
#define LOG_PRG_MOVE 0
#endif
#ifndef LOG_PRG_MEMPTR // Enable logging for prgmem pointerdiagnostics
#define LOG_PRG_MEMPTR 0
#endif
#ifndef LOG_SEEKMEM // Enable logging for seekmem diagnostics
#define LOG_SEEKMEM 0
#endif
#ifndef LOG_RAM_STORCL // Enable logging for RAM STO/RCL diagnostics
#define LOG_RAM_STORCL 0 
#endif
#ifndef LOG_FLASH_STORCL // Enable logging for FLASH STO/RCL diagnostics
#define LOG_FLASH_STORCL 0 
#endif
#ifndef LOG_FX_FI_STACK // Enable logging for fX/fI stack state tracking
#define LOG_FX_FI_STACK 0
#endif
#ifndef LOG_USER_MENU // Enable logging for user menu diagnostics
#define LOG_USER_MENU 0
#endif
#ifndef LOG_OVERFLOW // Enable logging for overflow detection diagnostics
#define LOG_OVERFLOW 0  
#endif
#ifndef LOG_COMPLEX // Enable logging for complex number diagnostics
#define LOG_COMPLEX 0  
#endif
#ifndef LOG_INPUT_PRINTING // Enable logging for printing diagnostics
#define LOG_INPUT_PRINTING 0  
#endif
#ifndef LOG_TIME_CONV // Enable logging for h↔hms troubleshooting
#define LOG_TIME_CONV 0
#endif
#ifndef LOG_CLOCK // Enable logging of clock state
#define LOG_CLOCK 0
#endif
#ifndef LOG_FLASH // Enable logging of flash state
#define LOG_FLASH 0
#endif
#ifndef LOG_SOLVE // Enable logging of solver state
#define LOG_SOLVE 0
#endif
#ifndef LOG_PLOT // Enable logging of plotting pipeline diagnostics
#define LOG_PLOT 0
#endif
#ifndef SHOW_BUILD_SCREEN
#define SHOW_BUILD_SCREEN 1
#endif

// Compiler flag to preserve user functions on startup
// By default, user functions are cleared on startup to prevent gibberish.
// If you have valid user programs and want to keep them, uncomment this:
#define DONT_CLEAR_USER_FUNCTIONS

// Compiler flag for battery power, the waiting for Serial connection is skipped, startup is faster
#define BATTERY_POWER

// ***** F O N T S
#define FONT4 4
//#define FONT5 5
#define FONT6 6

// BIGNUMBERFONT 6x8
#define FONTBIGWIDTH 6
#define FONTOFFSETBIG '-'
const byte fontbig [] PROGMEM = {
  0x00, 0x08, 0x08, 0x08, 0x08, 0x00, // -
  0x60, 0x60, 0x00, 0x00, 0x00, 0x00, // .
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // / space
  0x7f, 0x7f, 0x41, 0x41, 0x7f, 0x7f, // 0
  0x40, 0x42, 0x7f, 0x7f, 0x40, 0x40, // 1
  0x79, 0x79, 0x49, 0x49, 0x4f, 0x4f, // 2
  0x41, 0x49, 0x49, 0x49, 0x7f, 0x7f, // 3
  0x0f, 0x0f, 0x08, 0x08, 0x7f, 0x7f, // 4
  0x4f, 0x4f, 0x49, 0x49, 0x79, 0x79, // 5
  0x7f, 0x7f, 0x49, 0x49, 0x79, 0x78, // 6
  0x03, 0x03, 0x01, 0x01, 0x7f, 0x7f, // 7
  0x7f, 0x7f, 0x49, 0x49, 0x7f, 0x7f, // 8
  0x0f, 0x4f, 0x49, 0x49, 0x7f, 0x7f, // 9
  0x00, 0x00, 0x36, 0x36, 0x00, 0x00, // :
};

// NORMAL FONT 4x8
#define FONTWIDTH 4
#define FONTOFFSET ' '
const byte font4 [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, // space
  0x00, 0x5f, 0x00, 0x00, // !
  0x0b, 0x07, 0x0b, 0x07, // "
  //0x3e, 0x14, 0x14, 0x3e, // #
  0x6d, 0x11, 0x11, 0x6d, // ; # mean value
  0x2e, 0x6a, 0x2b, 0x3a, // $
  0x26, 0x12, 0x48, 0x64, // %
  0x76, 0x49, 0x76, 0x60, // &
  //0x00, 0x0b, 0x07, 0x00, // '
  0x22, 0x72, 0x27, 0x22, // ' leftright
  0x7f, 0x41, 0x41, 0x00, // (
  0x00, 0x41, 0x41, 0x7f, // )
  0x24, 0x18, 0x18, 0x24, // *
  0x08, 0x1c, 0x08, 0x00, // +
  //0x00, 0x58, 0x38, 0x00, // ,
  0x10, 0x7f, 0x01, 0x01, // , squareroot
  0x08, 0x08, 0x08, 0x08, // -
  0x00, 0x60, 0x60, 0x00, // .
  0x20, 0x10, 0x08, 0x04, // /
  0x7f, 0x41, 0x41, 0x7f, // 0
  0x00, 0x02, 0x7f, 0x00, // 1
  0x79, 0x49, 0x49, 0x4f, // 2
  0x41, 0x49, 0x49, 0x7f, // 3
  0x0f, 0x08, 0x08, 0x7f, // 4
  0x4f, 0x49, 0x49, 0x79, // 5
  0x7f, 0x49, 0x49, 0x79, // 6
  0x03, 0x01, 0x01, 0x7f, // 7
  0x7f, 0x49, 0x49, 0x7f, // 8
  0x4f, 0x49, 0x49, 0x7f, // 9
  0x00, 0x36, 0x36, 0x00, // :
  0x00, 0x5b, 0x3b, 0x00, // ;
  0x08, 0x14, 0x22, 0x41, // <
  0x14, 0x14, 0x14, 0x14, // =
  0x41, 0x22, 0x14, 0x08, // >
  0x03, 0x59, 0x09, 0x0f, // ?
  0x3e, 0x41, 0x5d, 0x5e, // @
  0x7f, 0x09, 0x09, 0x7f, // A
  0x7f, 0x49, 0x4f, 0x78, // B
  0x7f, 0x41, 0x41, 0x40, // C
  0x41, 0x7F, 0x41, 0x7F, // D
  0x7F, 0x49, 0x49, 0x41, // E
  0x7F, 0x09, 0x09, 0x01, // F
  0x7f, 0x41, 0x49, 0x79, // G
  0x7F, 0x08, 0x08, 0x7F, // H
  0x41, 0x7f, 0x41, 0x40, // I
  0x60, 0x40, 0x40, 0x7f, // J
  0x7F, 0x08, 0x0f, 0x78, // K
  0x7F, 0x40, 0x40, 0x40, // L
  0x7F, 0x07, 0x07, 0x7F, // M
  0x7F, 0x06, 0x0c, 0x7F, // N
  0x7f, 0x41, 0x41, 0x7f, // O
  0x7F, 0x09, 0x09, 0x0f, // P
  0x7f, 0x41, 0x61, 0x7f, // Q
  0x7F, 0x09, 0x79, 0x4f, // R
  0x4f, 0x49, 0x49, 0x78, // S
  0x01, 0x7f, 0x01, 0x01, // T
  0x7F, 0x40, 0x40, 0x7F, // U
  0x1F, 0x70, 0x70, 0x1F, // V
  0x7F, 0x70, 0x70, 0x7F, // W
  0x77, 0x08, 0x08, 0x77, // X
  0x4f, 0x48, 0x48, 0x7f, // Y
  0x71, 0x49, 0x49, 0x47, // Z
  //0x7f, 0x41, 0x41, 0x00, // [
  //0x7f, 0x47, 0x47, 0x7f, // [ upper
  0x02, 0x7f, 0x7f, 0x02, // [ arrow up
  //0x04, 0x08, 0x10, 0x20, // backslash
  0x2a, 0x1c, 0x1c, 0x2a, // backslash sun
  //0x00, 0x41, 0x41, 0x7f, // ]
  //0x7f, 0x71, 0x71, 0x7f, // ] lower
  0x20, 0x7f, 0x7f, 0x20, // ] arrow down
  0x02, 0x01, 0x01, 0x02, // ^
  0x40, 0x40, 0x40, 0x40, // _
  //0x00, 0x07, 0x0b, 0x00, // `
  0x3c, 0x20, 0x28, 0x20, // ` angle
  0x70, 0x54, 0x54, 0x7c, // a
  0x7F, 0x44, 0x44, 0x7c, // b
  0x7c, 0x44, 0x44, 0x44, // c
  0x7c, 0x44, 0x44, 0x7F, // d
  0x7c, 0x54, 0x54, 0x5c, // e
  0x04, 0x7f, 0x05, 0x01, // f
  0x5c, 0x54, 0x54, 0x7c, // g
  0x7F, 0x04, 0x04, 0x7c, // h
  0x40, 0x44, 0x7d, 0x40, // i
  0x40, 0x40, 0x44, 0x7d, // j
  0x7f, 0x10, 0x1c, 0x70, // k
  0x01, 0x7f, 0x40, 0x40, // l
  0x7C, 0x0c, 0x0c, 0x7c, // m
  0x7C, 0x04, 0x04, 0x7c, // n
  0x7c, 0x44, 0x44, 0x7c, // o
  0x7c, 0x14, 0x14, 0x1c, // p
  0x1c, 0x14, 0x14, 0x7c, // q
  0x7C, 0x04, 0x04, 0x04, // r
  0x5c, 0x54, 0x54, 0x74, // s
  0x04, 0x7F, 0x44, 0x40, // t
  0x7C, 0x40, 0x40, 0x7C, // u
  0x1c, 0x60, 0x60, 0x1c, // v
  0x7C, 0x60, 0x60, 0x7C, // w
  0x6c, 0x10, 0x10, 0x6c, // x
  0x5c, 0x50, 0x50, 0x7c, // y
  0x74, 0x54, 0x54, 0x5c, // z
  0x08, 0x1c, 0x3e, 0x7f, // { left arrow
  0x00, 0x7f, 0x00, 0x00, // |
  0x7f, 0x3e, 0x1c, 0x08, // } right arrow
  //0x04, 0x02, 0x04, 0x02, // ~
  0x77, 0x5d, 0x49, 0x63, // ~ sum
  0x55, 0x2a, 0x55, 0x2a  // del grey rectangle
};

// *****  D I S P L A Y

// DEFINES
#define SCREENBYTES 256 // Number of bytes to address SCREEN (SCREENWIDTH x SCREENHEIGHT)/8 
#define EEBRIGHTNESS   0 // EEPROM address for brightness

#define SCREEN_WIDTH 128 // real screen width
#define SCREEN_HEIGHT 64 // real screen height

#define SCREENWIDTH 64 // Virtual screen width 
#define SCREENHEIGHT 32 // Virtual screen height 
#define MAXLIN 4 // Maximum of lines

#define GRAPH_PIXEL_WIDTH  SCREEN_WIDTH   // Physical OLED width for graph overlay
#define GRAPH_PIXEL_HEIGHT SCREEN_HEIGHT  // Physical OLED height for graph overlay
#define GRAPHBUF_BYTES ((GRAPH_PIXEL_WIDTH * GRAPH_PIXEL_HEIGHT) / 8)

// ==============================
// Display Pin Assignments (Nice!Nano V2)
// ==============================
// Chip Select (CS)
#define PIN_CS   PIN_024   // SSD1306 CS = PIN15
// Data/Command (DC)
#define PIN_DC   PIN_022  // SSD1306 DC = PIN4
// Reset (RST)
#define PIN_RST  PIN_100  // SSD1306 RST = PIN16
// SPI pins (use software SPI)
//#define PIN_MISO  // of course not needed here, a display is only 'one-way'
#define PIN_MOSI  PIN_017 // SSD1306 SPI_DATA = PIN8
#define PIN_SCK   PIN_020 // SSD1306 SPI_CLK = PIN7
#define PIN_SS    PIN_024  // or your CS pin if using software control
#define PIN_VCC_MEASURE  PIN_031 // voltage divider 1:1

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT,
                         PIN_MOSI, PIN_SCK, PIN_DC, PIN_RST, PIN_CS);

// VARIABLES
static byte dbuf[SCREENBYTES]; // Buffer for virtual screen (costs 256 bytes of dynamic memory)
static byte graphbuf[GRAPHBUF_BYTES]; // Full-resolution graph overlay buffer (1,024 bytes)
static byte eachframemillis, thisframestart, lastframedurationms; // Framing times
static boolean justrendered; // True if frame was just rendered
static boolean isscreensave = false; // True if screensaver is active
static long powertimestamp = 0; // Needed for timing of power manangement

static byte brightness;  // declare only 
static boolean graph_dirty = false; // Tracks whether the high-res graph buffer has content

// FORWARD DECLARATIONS
static void delayshort(byte);
static byte expand4bit(byte);
static int userAreaCapacity(void);
static void sort(void);
static bool rewriteUserAreaImage(const uint8_t*, int, const char*, uint8_t* overrideConfig = nullptr);
static double dpoprd(void);


// SUBPROGRAMS

static void screenoff(void) {
  oled.ssd1306_command(SSD1306_DISPLAYOFF);  // 0xAE
  oled.ssd1306_command(0x8D);                // Charge pump
  oled.ssd1306_command(0x10);                // Disable charge pump
  digitalWrite(PIN_RST, LOW);                // Optional: reset low for power save
}

static void drawGraphOverlay(void) {
  if (!graph_dirty) return;
  const byte pages = GRAPH_PIXEL_HEIGHT / 8;
  for (byte page = 0; page < pages; ++page) {
    for (byte x = 0; x < GRAPH_PIXEL_WIDTH; ++x) {
      byte column = graphbuf[x + page * GRAPH_PIXEL_WIDTH];
      if (!column) continue;
      for (byte bit = 0; bit < 8; ++bit) {
        if (column & (1 << bit)) {
          int y = page * 8 + bit;
          oled.drawPixel(x, y, SSD1306_WHITE);
        }
      }
    }
  }
}

static void screenon(void) {
  digitalWrite(PIN_RST, HIGH);               // Reset high
  delayshort(5);
  
  if (!oled.begin(SSD1306_SWITCHCAPVCC)) {   // Re-init display
    // Handle init failure if needed
  }
  
  oled.ssd1306_command(SSD1306_DISPLAYON);   // 0xAF - turn display back on
}

void flush_dbuf_to_oled() {
  oled.clearDisplay();
  for (byte l = 0; l < MAXLIN; l++) {       // 0..3 lines (each line = 8 pixels vertical)
    for (byte k = 0; k < 2; k++) {          // 0..1 nibbles (each nibble = 4 vertical pixels)
      for (byte j = 0; j < SCREENWIDTH; j++) {  // 0..63 columns in virtual space
        byte nibble = (dbuf[j + l * SCREENWIDTH] >> (k * 4)) & 0x0f;
        byte pattern = expand4bit(nibble);  // Convert nibble to 8-pixel vertical pattern

        for (byte bit = 0; bit < 8; bit++) {
          bool on = pattern & (1 << bit);

          // Calculate x & y
          int x = j * 2;
          int y = l * 8 * 2 + k * 8 + bit;

          if (on) {
            oled.drawPixel(x, y, SSD1306_WHITE);
            oled.drawPixel(x + 1, y, SSD1306_WHITE);
          }
        }
      }
    }
  }
  drawGraphOverlay();
  oled.display();
}

const char* skipLeading(const char* s) {
  while (*s && (unsigned char)*s <= ' ') ++s;
  return s;
}

// ***** K E Y B O A R D

// PINS
#define KEYBOARDCOL1 PIN_009 // IV BIG
#define KEYBOARDCOL2 PIN_010
#define KEYBOARDCOL3 PIN_111
#define KEYBOARDCOL4 PIN_113
#define KEYBOARDROW1 PIN_115
#define KEYBOARDROW2 PIN_002
#define KEYBOARDROW3 PIN_029
#define KEYBOARDROW4 PIN_011 // should have been PIN_031, but 031 has ADC

// DEFINES
#define MAGICKEYPIN PIN_106 // Pin of magic key
#define KEY1 '?' // SHIFT    1-?  2-7  3-8  4-9
#define KEY2 '7' //          5->  6-4  7-5  8-6
#define KEY3 '8' //          9-= 10-1 11-2 12-3
#define KEY4 '9' //         13-< 14-0 15-: 16-;
#define KEY5 '>' // EE
#define KEY6 '4'
#define KEY7 '5'
#define KEY8 '6'
#define KEY9 '=' // CHS
#define KEY10 '1'
#define KEY11 '2'
#define KEY12 '3'
#define KEY13 '<' // CLX
#define KEY14 '0'
#define KEY15 ':' // DOT
#define KEY16 ';' // #

#define PRINTNOKEY   254 // Only evaluate keys smaller
#define NOPRINTNOKEY 255 // Evaluate keys smaller

// VARIABLES
static byte key = PRINTNOKEY; // Holds entered key
static byte oldkey = PRINTNOKEY; // Holds oldkey - to prevent keyrepeat

// FORWARD DECLARATIONS
static void _n0(void), _n1(void), _n2(void), _n3(void), _n4(void), _n5(void);
static void _n6(void), _n7(void), _n8(void), _n9(void), _dot(void), _dup(void);
static void _drop(void), _neg(void), _e(void), _keyf(void), _complex(void), _mrcl(void);
static void _fnsolve(void), _dict(void), _fnfx(void), _mul(void), _div(void), _recpol(void);
static void _add(void), _clr(void), _over(void), _keyg(void), _batt(void), _msto(void);
static void _fnintegrate(void), _litdown(void), _fnplot(void), _base(void), _mod(void), _clock(void);
static void _condlt(void), _condeq(void), _condne(void), _condgt(void), _nand(void), _addduration(void);
static void _pause(void), _integer(void), _peek(void), _poke(void), _begin(void), _exe(void);
static void _condif(void), _condelse(void), _condthen(void), _break(void), _getkey(void), _alphaclear(void);
static void _litup(void), _offset(void), _pick(void), _keygoff(void), _alphaput(void), _isreal(void);
static void _comb(void), _perm(void), _pi(void), _inv(void), _exp(void), _ln(void);
static void _business(void), _hex(void), _land(void), _lnot(void), _lor(void), _absolute(void);
static void _cos(void), _log(void), _asin(void), _acos(void), _atan(void), _cosh(void);
static void _atanh(void), _gammaln(void), _hms2h(void), _nd(void), _percent(void), _percentdelta(void);
static void _asinh(void), _acosh(void), _h2hms(void), _frac(void), _cvdeg2rad(void), _cvc2f(void);
static void _cvkm2mi(void), _cvm2ft(void), _cvcm2in(void), _cvkg2lbs(void), _cvl2gal(void), _condseek(void);
static void _numinput(byte);
static bool runImmediateProgram(uint16_t maxSteps = 0);
static bool evaluateFxImmediate(double x, double& fx);
static void _mstorcl_ram(boolean issto); // RAM storage functions
static void _sto_ram(void); // STO-RAM wrapper
static void _rcl_ram(void); // RCL-RAM wrapper
static int apop(void), apush(void), seekusr(byte);

// SUBPROGRAMS
static byte getkey() {
  pinMode(MAGICKEYPIN, INPUT_PULLUP);
  if (!digitalRead(MAGICKEYPIN)) return (KEY1); // F-key pressed
  byte pinscol[4] = {KEYBOARDCOL1, KEYBOARDCOL2, KEYBOARDCOL3, KEYBOARDCOL4}; // Pins
  byte pinsrow[4] = {KEYBOARDROW1, KEYBOARDROW2, KEYBOARDROW3, KEYBOARDROW4};
  char k[4][4] = {
    {'\0', KEY2, KEY3, KEY4}, // was NULL
    {KEY5, KEY6, KEY7, KEY8},
    {KEY9, KEY10, KEY11, KEY12},
    {KEY13, KEY14, KEY15, KEY16},
  };
  byte kee = NOPRINTNOKEY;
  for (byte r = 0; r < 4; r++) pinMode(pinsrow[r], INPUT_PULLUP); // Set rows to Vcc
  for (byte c = 0; c < 4; c++) {
    pinMode(pinscol[c], OUTPUT); // Set cols as OUTPUT and to GND
    digitalWrite(pinscol[c], LOW);
    for (byte r = 0; r < 4; r++) if (!digitalRead(pinsrow[r])) kee = (k[r][c]); //Assign key
    digitalWrite(pinscol[c], HIGH); // Reset cols to Vcc and INPUT
  }
  return (kee);
}

// ***** S Y S T E M

// DEFINES
#define FRAMERATE 5 // was 9  // Maximum number of screen refreshes per second (MUST BE >3) 
#define SIZES 0x01 // Printing size
#define SIZEM 0x02 // Printing size
#define SIZEL 0x04 // Printing size
#define EECONTRAST 0 // EEPROM address to save brightness (1 byte)

// SUBPROGRAMS
static double _abs(double f) { // Absolute value
  return (f < 0.0 ? -f : f);
}

static void dbuffill(byte n) { // Fill display buffer with pattern
  memset(dbuf, n, SCREENBYTES);
}

static inline void clearGraphBuffer(void) {
  memset(graphbuf, 0, GRAPHBUF_BYTES);
  graph_dirty = false;
}

static inline void graphPlotPixel(int x, int y) {
  if (x < 0 || x >= GRAPH_PIXEL_WIDTH || y < 0 || y >= GRAPH_PIXEL_HEIGHT) return;
  uint16_t idx = x + (y / 8) * GRAPH_PIXEL_WIDTH;
  graphbuf[idx] |= 1 << (y % 8);
  graph_dirty = true;
}

static void setframerate(byte rate) { // Calculate frameduration
  eachframemillis = 1000 / rate;
  Serial.printf("[FRAME] rate=%u -> each=%u\n", rate, eachframemillis);
}

static void idle(void) {
    __WFE(); // Wait For Event (ARM intrinsic)
}

static bool nextFrame(void) { // Wait (idle) for next frame
  byte now = (byte) millis(), framedurationms = now - thisframestart;
  if (justrendered) {
    lastframedurationms = framedurationms;
    justrendered = false;
    return false;
  }
  else if (framedurationms < eachframemillis) {
    if (++framedurationms < eachframemillis) idle();
    return false;
  }
  justrendered = true;
  thisframestart = now;
  return true;
}

static byte expand4bit(byte b) { // 0000abcd  Expand 4 bits (lower nibble)
  b = (b | (b << 2)) & 0x33;     // 00ab00cd
  b = (b | (b << 1)) & 0x55;     // 0a0b0c0d
  return (b | (b << 1));         // aabbccdd
}

static void printcat(byte c, byte fnt, boolean bitshift, byte w, byte h, byte x, byte y) {
  // Print char c (one bit shifted down) with width (1, 2, 4), size (1, 2) at position (x, y)
  byte printbitshift = bitshift ? 1 : 0;
  byte fwidth = FONTWIDTH;
  byte foffset = FONTOFFSET;

  if (fnt != FONT4) {
    fwidth = FONTBIGWIDTH;
    foffset = FONTOFFSETBIG;
    if (c == '.') fwidth = 2; // Dot
  }
  for (byte k = 0; k < h; k++) {
    for (int i = 0; i < fwidth; i++) {
      byte tmp;
      //if (fnt == FONT4) tmp = pgm_read_byte(font4 + FONTWIDTH * (c - foffset) + i) << printbitshift;
      if (fnt == FONT4) tmp = font4[FONTWIDTH * (c - foffset) + i] << printbitshift;
      //else tmp = pgm_read_byte(fontbig + FONTBIGWIDTH * (c - foffset) + i) << printbitshift;
      else tmp = fontbig[FONTBIGWIDTH * (c - foffset) + i] << printbitshift;
    
#if LOG_INPUT_PRINTING
      // Debug logging for comma character rendering
      if (c == '.') {
        Serial.print("[PRINT_DEBUG] Rendering comma character (.) at position x=");
        Serial.print(x); Serial.print(", y="); Serial.print(y);
        Serial.print(", font="); Serial.print(fnt);
        Serial.print(", pattern=0x"); Serial.println(tmp, HEX);
      }
#endif
      if (h == 2) tmp = expand4bit(tmp >> (4 * k) & 0x0f);
      for (byte j = 0; j < w; j++) dbuf[x + (w * i + j) + (y + k) * SCREENWIDTH] = tmp;
    }
  }
  flush_dbuf_to_oled();
}

static void printsat(char * s, boolean bitshift, byte w, byte h, byte x, byte y) {
  byte i = 0;
  while (s[i]) {
      printcat(s[i], FONT4, bitshift, w, h, x + i * (FONTWIDTH + 1) * w , y );
      i++;
  }
  flush_dbuf_to_oled();
}

static void printpixel(int x, int y) { // Print graph pixel at x,y
  graphPlotPixel(x, y);
}

// Bresenham's line algorithm for drawing smooth lines between points
static void drawLine(int x0, int y0, int x1, int y1) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;

  while (true) {
    graphPlotPixel(x0, y0);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}
static void printhline(int y) { // Print horizontal line
  for (byte i = 0; i < GRAPH_PIXEL_WIDTH; i++) printpixel(i, y);
}
static void printvline(int x) { // Print horizontal line
  for (byte i = 0; i < GRAPH_PIXEL_HEIGHT; i++) printpixel(x, i);
}

static void delayshort(byte ms) { // Delay (with timer) in ms with 8 bit duration
  long t = millis();
  while ((byte)(millis() - t) < ms) ;
}
static void delaylong(byte nr) { // Delay nr quarters of a second
  for (byte i = 0; i < nr; i++) delayshort(250);
}

static double pow10(int8_t e) { // Calculates 10 raised to the power of e
  double f = 1.0;
  if (e > 0) while (e--) f *= 10.0;
  else while (e++) f /= 10.0;
  return (f);
}

static void wakeupnow() {} // Dummy wakeup code

static void sleepnow(void) {
  screenoff();  // Turn display off (replaces your commented line)
  pinMode(MAGICKEYPIN, INPUT_PULLUP);
  attachInterrupt(MAGICKEYPIN, wakeupnow, CHANGE);
  
  __WFE();  // CPU halts until pin interrupt
  
  detachInterrupt(MAGICKEYPIN);
  screenon();  // Turn display back on (replaces bootscreen())
  powertimestamp = millis();
  delayshort(200);
}

//#define MX0 0
#define MX1 1
#define MX2 2
#define MX3 3
#define MX4 4


// ***** A P P L I C A T I O N

// MACROS
#define _ones(x) ((x)%10)        // Calculates ones unit
#define _tens(x) (((x)/10)%10)   // Calculates tens unit
#define _huns(x) (((x)/100)%10)  // Calculates hundreds unit
#define _tsds(x) (((x)/1000)%10) // Calculates thousands unit

// CONSTANTS
#define DIGITS           10 // Number of digits 
#define INDICATOR        6 // X-offset for numbers (room for indicator)
#define TINYNUMBER   1e-12 // Number for rounding to 0, was 1e-8
#define ALMOSTZERO   1e-37 // Limits to decide if sci or fix
#define OVERFLOW        99 // Max power of 10 before overflow, was 36
#define OVERFLOWEXP    227 // Max power of e before overflow, was 87
#define FIXMIN        1e-3 // Limits for fix display guarantee maximum
#define FIXMAX         1e7 //             number of significant digits
#define FIXSCI           5 // Display 5 decimals in scientific view
#define FIXMANT          3 // FIX in 10th-exponent
#define ISF              1 // F-key demanded
#define ISG              2 // G-key demanded
#define FLONGPRESSTIME 350 // Time in ms when f longpress starts
#define MAXSTRBUF       23 // Maximum length of string buffer sbuf[]
#define ALPHABUFSIZE    13 // maximum size of alpha
#define PRGNAMEMAX       3 // maximum number of characters of program name
#define PRGSIZEMAX     256 // Max size of prg, prgptr, prgbuf, was 128
#define RAD  ((180.0)/(PI)) // 180/PI
#define GK_POINTS       15 // Gauss-Kronrod G7-K15: 15 evaluation points
#define DARKTIMEMIN      6 // Minimal time for auto display off in 10 x s, was 6  
#define BDIGITS         20 // Number of displayed digits in business mode

#define C0 (struct data){'\0', '\0', '\0'} // Complex NULL 

#define MAXCMDI  76 // Number of commands of intrinsic functions
#define MAXCMDB 122 // Include RAM STR/RCR in builtin command space
#define MAXCMDU (MAXCMDB + 40) // Leave room for up to 40 user commands

// EEPROM addresses
#define EEDARKTIME 1 // EEPROM address for display off time
//#define EEBASE     2 // Store base
static const int EE_BASE = 2; // Store base 

#define MEMNR     100 // Maximum number of constant memories in EEPROM (slots) 
#define EEMEM     32 // Starting EE-address for constant reals (uses doubles) – align to 8 bytes
#define EEMEMB    (EEMEM + 2 * MEMNR * sizeof(double)) // Base-mode storage sits after real+imag blocks, 192b with MEMNR=10, 1632b with MEMNR=100
#define MENUITEMS 32 // Number of selectable user menu items
static constexpr uint8_t kLegacyUserAreaMagic = 0xA5;
static constexpr uint8_t kUserAreaMagic = 0xA6;
#define EEMENU    (EEMEMB + sizeof(int64_t)) // User menu stored after base-mode slot, 200b with MEMNR=10, 1640b with MEMNR=100 
#define EECLEANFLAG (EEMENU + MENUITEMS) // EEPROM flag for user functions cleanup (1 byte)
#define CONFIG_REGION_END (WL_BRIGHTNESS_INDEX + 1) // Wear-level data extends through brightness slots+index
#define EEUSTART  CONFIG_REGION_END // User programs start after entire config + constant storage region

static_assert((EEMEM % alignof(double)) == 0, "EEMEM must be double-aligned");
static_assert((EEMEMB % alignof(int64_t)) == 0, "EEMEMB must be 8-byte aligned");
static_assert(EEMENU + MENUITEMS <= WL_BRIGHTNESS_START, "Menu slots overlap wear-level region"); //224b with MEMNR=10, 1672b with MEMNR=100
static_assert(EEUSTART > EEMENU + MENUITEMS, "User area must start after menu/config region");
#define EEUEND   EEPROM.length()
#define EEU (EEUEND-EEUSTART) // Available user memory

#if LOG_FLASH_STORCL
struct ConstantSlotBits {
  ConstantSlotBits() : raw(0) {}
  union {
    double value;
    uint64_t raw;
  };
};

static void formatHex64(uint64_t value, char* out, size_t len) {
  if (!out || len < 17) return;
  for (int i = 15; i >= 0; --i) {
    uint8_t nibble = value & 0x0F;
    out[i] = (nibble < 10) ? static_cast<char>('0' + nibble)
                           : static_cast<char>('A' + nibble - 10);
    value >>= 4;
  }
  out[16] = '\0';
}

static void logConstantSlotSnapshot(byte slot, const char* phase) {
  if (!Serial) return;
  if (slot != 3 && slot != 5) return;
  ConstantSlotBits realBits;
  ConstantSlotBits imagBits;
  EEPROM.get(EEMEM + slot * sizeof(double), realBits);
  EEPROM.get(EEMEM + (slot + MEMNR) * sizeof(double), imagBits);
  char realRaw[17];
  char imagRaw[17];
  formatHex64(realBits.raw, realRaw, sizeof(realRaw));
  formatHex64(imagBits.raw, imagRaw, sizeof(imagRaw));
  Serial.print(F("[FLASH_STO] slot="));
  Serial.print(slot);
  Serial.print(F(" phase="));
  Serial.print(phase ? phase : "n/a");
  Serial.print(F(" real="));
  Serial.print(realBits.value, 12);
  Serial.print(F(" raw=0x"));
  Serial.print(realRaw);
  Serial.print(F(" imag="));
  Serial.print(imagBits.value, 12);
  Serial.print(F(" raw=0x"));
  Serial.print(imagRaw);
  Serial.println();
}
#endif


// GLOBAL VARIABLES
struct data { // Structure for numbers
  double r, i; // Real, imaginary
  int64_t b; // Integer, base
};

static bool snapshotUserArea(uint8_t** buffer, int* length);
static bool commitConstantSlot(byte slot, const struct data& value);
static bool commitBusinessSlot(int64_t businessValue);
static bool commitMenuSlot(byte slot, byte cmdId);
static void warnIfConstantSlotOutOfRange(byte slot, const char* region, uint32_t startAddr);

#define DATASTACKSIZE 26 // DATA STACK
struct data ds[DATASTACKSIZE];
static byte dp = 0;

// RAM Storage - 100 memory slots for fast access in an array using ~2.4kB of RAM
#define RAMMEMNR 100 // Number of RAM memory slots
struct data ram_mem[RAMMEMNR]; // RAM store for fast access numbers
static byte ram_mem_initialized = 0; // Flag to initialize RAM memory to zero

#define ADDRSTACKSIZE 64 // ADDRESS STACK
static int as[ADDRSTACKSIZE];
static byte ap = 0;

byte cl = 0; // CONDITIONAL LEVEL
static int mp; // MEMPOINTER (builtin and user functions)

static char sbuf[MAXSTRBUF]; // Holds string to print
static byte fgm = 0, setfgm = 0; // F-key variables
static byte decimals = 0; // Number of decimals entered (input after decimal dot)
static boolean isdot = false; // True if dot was pressed and decimals will be entered
static boolean isnewnumber = true; // True if stack has to be lifted before entering a new number
static boolean isprintscreen = true; // True, if screen should be printed
static boolean darkscreen = false; // Shows empty dbuf, if true
static boolean ispolar = false; // Shows imaginary number in polar format (else rectangulsr)
static boolean isfloated = false; // True, if data stack was floated
boolean isgetkey = false; // Needed for getkey function
static char alpha[ALPHABUFSIZE] = {'\0'}; // String buffer (alpha) for user text output // was NULL
static boolean isprintalpha = false; // Print alpha if true
static long timestamp; // Timestamp for f longpress
static boolean freleased = false; // Used for releasing longpressed f-key
static byte darktime; // Time of inactivity for dark screen (in 10 x s)
static long durationtimestamp = millis();
static byte pause = 0; // Pause time in n * 250 ms
static byte base = 0; // BASE 0-sci 2-bin 6-oct 10-business 16-hex
static boolean isAF; //Next keypress is hexA ... hexF
static int breakmp; // Holds memory pointer while break
static boolean isprintinput = false; // True, if inputnumber is printed

//static byte select; // Selected operations
static byte sel; // because of "'byte select' redeclared as different kind of entity" error

static boolean isdict = false; // True for dict browsing
static boolean ismenu = false; // True, if menu was demanded
static boolean issetusr = false; // True if dict is used for setting usr menu
static byte setusrselect; // Stores selected cmd value to store
static boolean ismenusetusr; // True for choosing user menu place
static boolean istorch; // Shows full display it true

static byte prgselect = 0; // Number of selected program
static boolean isprgselect = false; // Used for selecting program
static boolean isprgedit = false; // True, if program is editet
static boolean isprgdict = false, isprgmenu = false, isprgnew = false;
static boolean isprgeditfirst = false;
static int sou; // Size of user programs
static byte nou; // Number of user programs
static byte prgbuf[PRGSIZEMAX], oldprgbuflen; // Program buffer for fast editing
static int prgaddr; // EEPROM address of actual program
static byte prgbuflen; // Size of program in program buffer
static byte prgeditstart = 0;
static boolean isprgrename = false; // True if program should be renamed
static byte renamenr = 0; // Stores selected prg value to rename

static byte cycles; // Solver cycles
double x0, x1, w0, w1; // Old and new solve-values, w0 and w1 because conflict with cmath Bessel function
static boolean issolve = false, isint = false; // True if solving or integrating is demanded
static double inta, intb; // Integration bounds (sorted)
// Gauss-Kronrod G7-K15: Exact values from QUADPACK dqk15.f
// XGK = abscissae of the 15-point Kronrod rule
// WGK = weights of the 15-point Kronrod formula
static const double gk_nodes[15] = {
  -0.9914553711208126392068845967e0,
  -0.9491079123427585245261897e0,
  -0.8648644233597690727897128e0,
  -0.7415311855993944398638648e0,
  -0.5860872354676911302941448e0,
  -0.4058451513773971669066064e0,
  -0.2077849550078984676006894e0,
   0.0000000000000000000000000e0,
   0.2077849550078984676006894e0,
   0.4058451513773971669066064e0,
   0.5860872354676911302941448e0,
   0.7415311855993944398638648e0,
   0.8648644233597690727897128e0,
   0.9491079123427585245261897e0,
   0.9914553711208126392068845967e0
};
static const double gk_weights[15] = {
  0.2293532201052922496373200e-1,
  0.6309209262997855329070066e-1,
  0.1047900103222501838398763e0,
  0.1406532597155259187451896e0,
  0.1690047266392679028265834e0,
  0.1903505780647854099132564e0,
  0.2044329400752988906697426e0,
  0.2094821410847278280129992e0,
  0.2044329400752988906697426e0,
  0.1903505780647854099132564e0,
  0.1690047266392679028265834e0,
  0.1406532597155259187451896e0,
  0.1047900103222501838398763e0,
  0.6309209262997855329070066e-1,
  0.2293532201052922496373200e-1
};
static const double gk_gauss_weights[15] = {
  0.0,
  0.1294849661688696932706114e0,
  0.0,
  0.2797053914892766679014678e0,
  0.0,
  0.3818300505051189449503698e0,
  0.0,
  0.4179591836734693877551020e0,
  0.0,
  0.3818300505051189449503698e0,
  0.0,
  0.2797053914892766679014678e0,
  0.0,
  0.1294849661688696932706114e0,
  0.0
};
static constexpr double GK_DEFAULT_ABS_TOL = 1e-7;
static constexpr double GK_DEFAULT_REL_TOL = 1e-7;
static constexpr double GK_MIN_INTERVAL = 1e-12;
static constexpr uint8_t GK_MAX_DEPTH = 12; // was 16, increase if more accuracy needed
static constexpr byte GK_STACK_LIMIT = 48;

struct AdaptiveGKInterval {
  double a;
  double b;
  double tol;
  uint8_t depth;
};

struct AdaptiveGKController {
  bool active;
  double orientation;
  double total;
  double targetAbsTol;
  double targetRelTol;
  double lastError;
  AdaptiveGKInterval current;
  double center;
  double halfwidth;
  double kronrodSum;
  double gaussSum;
  uint8_t nodeIndex;
};

static AdaptiveGKInterval gkIntervalStack[GK_STACK_LIMIT];
static byte gkIntervalStackSize = 0;
static AdaptiveGKController gkController = {};
static uint32_t gkEvalCount = 0;
static uint32_t gkRefinementCount = 0;
static uint16_t gkPeakStack = 0;
static uint8_t gkPeakDepth = 0;
static double gkMinIntervalWidth = 1e9;
static double gkMinTolSeen = 1e9;
static unsigned long gkLastLogMillis = 0;

static void gkResetController() {
  gkController = {};
  gkIntervalStackSize = 0;
  gkEvalCount = 0;
  gkRefinementCount = 0;
  gkPeakStack = 0;
  gkPeakDepth = 0;
  gkMinIntervalWidth = 1e9;
  gkMinTolSeen = 1e9;
  gkLastLogMillis = 0;
}

static inline void gkNoteStackSize() {
  if (gkIntervalStackSize > gkPeakStack) gkPeakStack = gkIntervalStackSize;
}

static void gkLogProgress(const char* phase, double localError, double adaptiveTol, bool accept) {
  if (!Serial) return;
  unsigned long now = millis();
  if (now - gkLastLogMillis < 500) return;
  gkLastLogMillis = now;
  Serial.print("[INT] phase="); Serial.print(phase);
  Serial.print(" evals="); Serial.print(gkEvalCount);
  Serial.print(" refinements="); Serial.print(gkRefinementCount);
  Serial.print(" stack="); Serial.print(gkIntervalStackSize);
  Serial.print(" peakStack="); Serial.print(gkPeakStack);
  Serial.print(" depth="); Serial.print(gkController.current.depth);
  Serial.print(" peakDepth="); Serial.print(gkPeakDepth);
  Serial.print(" minWidth="); Serial.print(gkMinIntervalWidth, 12);
  Serial.print(" minTol="); Serial.print(gkMinTolSeen, 12);
  Serial.print(" err="); Serial.print(localError, 12);
  Serial.print(" tol="); Serial.print(adaptiveTol, 12);
  Serial.print(" accept="); Serial.println(accept ? "y" : "n");
}

static bool gkPushInterval(double a, double b, double tol, uint8_t depth) {
  if (gkIntervalStackSize >= GK_STACK_LIMIT) {
    if (Serial) Serial.println("[INT] Adaptive GK stack overflow");
    return false;
  }
  gkIntervalStack[gkIntervalStackSize++] = {a, b, tol, depth};
  gkNoteStackSize();
  return true;
}

static inline void gkRecordIntervalStats() {
  gkMinIntervalWidth = min(gkMinIntervalWidth, gkController.halfwidth);
  gkMinTolSeen = min(gkMinTolSeen, gkController.current.tol);
  if (gkController.current.depth > gkPeakDepth) gkPeakDepth = gkController.current.depth;
}

static bool gkStartNextInterval() {
  if (!gkIntervalStackSize) return false;
  gkController.current = gkIntervalStack[--gkIntervalStackSize];
  gkController.center = 0.5 * (gkController.current.a + gkController.current.b);
  gkController.halfwidth = 0.5 * (gkController.current.b - gkController.current.a);
  gkController.kronrodSum = 0.0;
  gkController.gaussSum = 0.0;
  gkController.nodeIndex = 0;
  gkRecordIntervalStats();
  return true;
}

static double gkCurrentNodePosition() {
  return gkController.center + gkController.halfwidth * gk_nodes[gkController.nodeIndex];
}

static double plot[GRAPH_PIXEL_WIDTH]; // Y-values of plot graph
static double plota, plotb, plotd, ymax, ymin; // Variables used for plotting
static boolean isplot = false, isplotcalc = false; // True if plotting or plot calculation is demanded
static boolean isplotEvalInFlight = false; // True while a user program evaluation is running

static byte msgnr = 0; // MESSAGES
#define MSGASK      0
#define MSGDICT     1
#define MSGMENU     2
#define MSGKEY      3
#define MSGRUN      4
#define MSGOVERFLOW 5
#define MSGNEST     6
#define MSGSAVE     7
const char m0[] PROGMEM = "?";
const char m1[] PROGMEM = "DICT";
const char m2[] PROGMEM = "MENU";
const char m3[] PROGMEM = "KEY?";
const char m4[] PROGMEM = "RUN";
const char m5[] PROGMEM = "OVERFLOW";
const char m6[] PROGMEM = "NEST ERROR";
const char m7[] PROGMEM = "SAVE ERROR";
const char* const msg[] PROGMEM = {
  m0, m1, m2, m3, m4, m5, m6, m7
};

// Command code defines
#define _0 0 // Intrinsic commands
#define _1 1
#define _2 2
#define _3 3
#define _4 4
#define _5 5
#define _6 6
#define _7 7
#define _8 8
#define _9 9
#define _DOT 10
#define _DUP 11
#define _DROP 12
#define _NEG 13
#define _E 14
#define _CMPLX 16
#define _RCL 17
#define _SUB 19
#define _MULT 22
#define _DIV 25
#define _ADD 27
#define _SWAP 30
#define _STO 33
#define _STO_RAM 120  // New command for RAM store (uses dispatch array index 120)
#define _RCL_RAM 121  // New command for RAM recall (uses dispatch array index 121)
#define _BASE 40
#define _PICK 45
#define _ROT 46
#define _LT 48
#define _EQ 49
#define _NE 50
#define _GT 51
#define _NAND 52
#define _ADDDUR 53
#define _PAUSE 54
#define _INT 55
#define _BEGIN 58
#define _UNTIL 59
#define _IF 61
#define _ELSE 62
#define _THEN 63
#define _ISREAL 68
#define _PI 71
#define _INV 72
#define _SIN 73
#define _EXP 74
#define _LN 75
#define _BUS MAXCMDI+0 // Builtin commands (mem)
#define _HEX MAXCMDI+1
#define _AND MAXCMDI+2
#define _NOT MAXCMDI+3
#define _OR MAXCMDI+4
#define _OVER MAXCMDI+5
#define _ABS MAXCMDI+6
#define _SQRT MAXCMDI+7
#define _COS MAXCMDI+8
#define _TAN MAXCMDI+9
#define _POW MAXCMDI+10
#define _PWR10 MAXCMDI+11
#define _LOG MAXCMDI+12
#define _ASIN MAXCMDI+13
#define _ACOS MAXCMDI+14
#define _ATAN MAXCMDI+15
#define _SINH MAXCMDI+16
#define _COSH MAXCMDI+17
#define _TANH MAXCMDI+18
#define _ASINH MAXCMDI+19
#define _ACOSH MAXCMDI+20
#define _ATANH MAXCMDI+21
#define _GAMMALN MAXCMDI+22
#define _HMS2H MAXCMDI+23
#define _H2HMS MAXCMDI+24
#define _PV MAXCMDI+25
#define _ND MAXCMDI+26
#define _QE MAXCMDI+27
#define _CLOCK MAXCMDI+28
#define _CLRSUM MAXCMDI+29
#define _SUM MAXCMDI+30
#define _SUMSUB MAXCMDI+31
#define _STAT MAXCMDI+32
#define _LR MAXCMDI+33
#define _PERCENT MAXCMDI+34
#define _PERCENTDELTA MAXCMDI+35
#define _FRAC MAXCMDI+36
#define _DEGRAD MAXCMDI+37
#define _CF MAXCMDI+38
#define _KMMI MAXCMDI+39
#define _MFT MAXCMDI+40
#define _CMIN MAXCMDI+41
#define _KGLBS MAXCMDI+42
#define _LGAL MAXCMDI+43
#define _END 255 // Function delimiter
static constexpr uint8_t kEndMarkerByte = static_cast<uint8_t>(_END);

// Builtin functions (mem)
const byte mem[] PROGMEM = {
  _END, // Necessary to prevent function starting with mp = 0
  _1, _0, _BASE, _END, //0 BUSINESS
  _1, _6, _BASE, _END, //1 HEX
  _NAND, _DUP, _NAND, _END, //2 AND
  _DUP, _NAND, _END, //3 NOT
  _DUP, _NAND, _SWAP, _DUP, _NAND, _NAND, _END, //4 OR
  _SWAP, _DUP, _ROT, _ROT, _END, //5 OVER
  _DUP, _0, _LT, _IF, _NEG, _THEN, _END, //6 ABS
  _DUP, _0, _EQ, _IF, _DROP, _0, _ELSE, _LN, _2, _DIV, _EXP, _THEN, _END, //7 SQRT sqrt(z)=exp(2*ln(z))
  //_ISREAL, _1, _EQ, _IF, _9, _0, _ADD, _ELSE, _PI, _2, _DIV, _ADD, _THEN, _SIN, _END, //8 COS cos(x)=sqrt(1-sin(z)*sin(z)) cos(x)=sin(x+90)
  _ISREAL, _1, _EQ, _IF, _9, _0, _SWAP, _SUB, _ELSE, _PI, _2, _DIV, _ADD, _THEN, _SIN, _END, //8 COS cos(x)=sqrt(1-sin(z)*sin(z)) cos(x)=sin(90-x)
  _DUP, _SIN, _SWAP, _COS, _DIV, _END, //9 TAN tan(z)=sin(z)/cos(z)
  _SWAP, _DUP, _0, _EQ, _IF, _DROP, _DROP, _0, _ELSE, _LN, _MULT, _EXP, _THEN, _END, //10 POW a^b=exp(b*ln(a)) 0^n=0 (fixed stack leak where exponent remained)
  //_1, _SWAP, _E, _END, //11 POW10
  _1, _0, _LN, _MULT, _EXP, _END, //11 POW10
  _LN, _1, _0, _LN, _DIV, _END, //12 LOG log(z)=ln(z)/ln(10)

  _DUP, _DUP, _DUP, _MULT, _NEG, _1, _ADD, _SQRT, //13 ASIN: asin(z) = -i*ln(i*z+sqrt(1-z*z))
  _SWAP, _0, _DUP, _1, _CMPLX, _MULT, _ADD, _LN, _0, _DUP, _1, _CMPLX, _NEG, _MULT,
  _SWAP, _ISREAL, _1, _EQ, _IF, _DROP, _1, _8, _0, _MULT, _PI, _DIV, // *RAD - if input was real
  _ISREAL, _1, _EQ, _IF, _ELSE, _CMPLX, _DROP, _THEN, // if result is imaginary - set im to 0
  _ELSE, _DROP, _THEN, _END,

  _DUP, _ASIN, _NEG, //14 ACOS acos(z)=PI/2-asin(z)
  _SWAP, _ISREAL, _1, _EQ, _IF, _DROP, _9, _0, _ADD, _ELSE, // +90 - if input was real
  _DROP, _PI, _2, _DIV, _ADD, _THEN, _END, // +PI/2 - if input was complex

  _DUP, _DUP, _MULT, _1, _ADD, _SQRT, _INV, _MULT, _ASIN, _END, //15 ATAN: atan(z)=asin(z/(sqrt(1+z*z)))____z^2 overflow if >10^18.....any changes made were too big! Also: deleting _litup and _litdown influenced _ATAN!

  _EXP, _DUP, _INV, _NEG, _ADD, _2, _DIV, _END, //16 SINH sinh(z)=(exp(z)-exp(-z))/2
  _EXP, _DUP, _INV, _ADD, _2, _DIV, _END, //17 COSH cosh(z)=(exp(z)+exp(-z))/2
  _DUP, _SINH, _SWAP, _COSH, _DIV, _END, //18 TANH tanh(z)=sinh(z)/cosh(z)
  _DUP, _DUP, _MULT, _1, _ADD, _SQRT, _ADD, _LN, _END, //19 ASINH asinh(x)=ln(x+sqrt(x*x+1))
  _DUP, _DUP, _MULT, _1, _SUB, _SQRT, _ADD, _LN, _END, //20 ACOSH acosh(z)=ln(z+sqrt(z*z-1))
  _DUP, _1, _ADD, _SWAP, _NEG, _1, _ADD, _DIV, _SQRT, _LN, _END, //21 ATANH atanh(z)=ln(sqrt((1+z)/(1-z)))

  _1, _ADD, _DUP, _DUP, _DUP, _DUP, _1, _2, _MULT, //22 GAMMALN: ln!=(ln(2*PI)-ln(z))/2+z*(ln(z+1/(12*z-1/10/z))-1)
  _SWAP, _1, _0, _MULT, _INV, _SUB, _INV, _ADD, _LN, _1, _SUB, _MULT,
  _SWAP, _LN, _NEG, _2, _PI, _MULT, _LN, _ADD, _2, _DIV, _ADD, _END,

  _DOT, _0, _0, _0, _0, _0, _1, _ADD,//23 HMS2H - round up to prevent leaps
  _DUP, _DUP, _INT, _SWAP, _OVER, _SUB, _1, _0, _0, _MULT, _INT, // hh mm
  _ROT, _3, _PICK, _SUB, _1, _0, _0, _MULT, _OVER, _SUB, _1, _0, _0, _MULT, // ss
  _3, _6, _0, _0, _DIV, _SWAP, _6, _0, _DIV, _ADD, _ADD, _END,// ->s ->h

  _DUP, _3, _6, _0, _0, _MULT, _DUP, _ROT, _INT, //24 H2HMS - h->s
  _SWAP, _OVER, _3, _6, _0, _0, _MULT, _SUB, _6, _0, _DIV, _INT, // hh mm
  _ROT, _OVER, _6, _0, _MULT, _SUB, _3, _PICK, _3, _6, _0, _0, _MULT, _SUB, // ss
  _1, _0, _0, _0, _0, _DIV, _SWAP, _1, _0, _0, _DIV, _ADD, _ADD, _END, // hh.mmss

  _OVER, _1, _ADD, _SWAP, _POW, _DUP, _1, _SUB, _SWAP, _DIV, _SWAP, _DIV, _END, //25 PV PV(i,n)=((1+i)^n-1)/(1+i)^n/i

  _DUP, _DUP, _DUP, _DUP, _MULT, _MULT, _DOT, _0, _7, _MULT, //26 ND
  _SWAP, _1, _DOT, _6, _MULT, _NEG, _ADD, _EXP, _1, _ADD, _INV, _SWAP,
  _DUP, _MULT, _NEG, _2, _DIV, _EXP, _2, _PI, _MULT, _SQRT, _INV, _MULT, _END,

  _OVER, _2, _DIV, _DUP, _MULT, _SWAP, _SUB, _SQRT, _SWAP, _2, _DIV, _NEG, _SWAP, //27 QE x12=-p/2+-sqrt(p*p/4-q)
  _OVER, _OVER, _SUB, _ROT, _ROT, _ADD, _END,

  _DUP, _ADDDUR, _DROP, //28 CLOCK - Reset duration
  _BEGIN, _HMS2H, _3, _6, _0, _0, _0, _0, _0, _MULT, _ADDDUR, // hms to ms and add duration
  _1, _SUB, // Time correction (in ms) ...0
  _3, _6, _0, _0, _0, _0, _0, _DIV, _H2HMS, // Convert ms to hms
  _DUP, _DUP, _DUP, // Copies for display
  _1, _0, _0, _0, _0, _MULT, _INT, // hhmmss
  _SWAP, _1, _0, _0, _MULT, _INT, _1, _0, _0, _MULT, // hhmm00
  _SUB, // Display ss
  _SWAP, _1, _0, _0, _MULT, _INT, // Display hhmm
  _4, _PAUSE, _DROP, _DROP, _0, _UNTIL, _END, // Print and delete display copies

  _0, _DUP, _DUP, _DUP, _DUP, _DUP, //29 CLRSUM
  //_5, _STO, _6, _STO, _7, _STO, _8, _STO, _9, _STO, _END,
  _5, _STO_RAM, _6, _STO_RAM, _7, _STO_RAM, _8, _STO_RAM, _9, _STO_RAM, _END,

  //_7, _RCL, _1, _ADD, _7, _STO, //30 SUM+ - n
  _7, _RCL_RAM, _1, _ADD, _7, _STO_RAM, //30 SUM+ - n
  //_DUP, _8, _RCL, _ADD, _8, _STO, // X
  _DUP, _8, _RCL_RAM, _ADD, _8, _STO_RAM, // X

  //_DUP, _DUP, _MULT, _5, _RCL, _ADD, _5, _STO, // XX
  _DUP, _DUP, _MULT, _5, _RCL_RAM, _ADD, _5, _STO_RAM, // XX
  //_OVER, _MULT, _6, _RCL, _ADD, _6, _STO, // XY
  _OVER, _MULT, _6, _RCL_RAM, _ADD, _6, _STO_RAM, // XY

  //_9, _RCL, _ADD, _9, _STO, _7, _RCL, _END, // Y push(n)
  _9, _RCL_RAM, _ADD, _9, _STO_RAM, _7, _RCL_RAM, _END, // Y push(n)

  //_7, _RCL, _1, _SUB, _7, _STO, //30 SUM- - n
  _7, _RCL_RAM, _1, _SUB, _7, _STO_RAM, //30 SUM- - n
  //_DUP, _8, _RCL, _SWAP, _SUB, _8, _STO, // X
  _DUP, _8, _RCL_RAM, _SWAP, _SUB, _8, _STO_RAM, // X
  //_DUP, _DUP, _MULT, _5, _RCL, _SWAP, _SUB, _5, _STO, // XX
  _DUP, _DUP, _MULT, _5, _RCL_RAM, _SWAP, _SUB, _5, _STO_RAM, // XX
  //_OVER, _MULT, _6, _RCL, _SWAP, _SUB, _6, _STO, // XY
  _OVER, _MULT, _6, _RCL_RAM, _SWAP, _SUB, _6, _STO_RAM, // XY
  //_9, _RCL, _SWAP, _SUB, _9, _STO, _7, _RCL, _END, // Y
  _9, _RCL_RAM, _SWAP, _SUB, _9, _STO_RAM, _7, _RCL_RAM, _END, // Y

  //_8, _RCL, _7, _RCL, _DIV, //32 STAT - mean (X/n)
  _8, _RCL_RAM, _7, _RCL_RAM, _DIV, //32 STAT - mean (X/n)
  //_DUP, _DUP, _MULT, _7, _RCL, _MULT, _NEG, _5, _RCL, _ADD, // stddev (XX-n*m^2)/(n-1)
  _DUP, _DUP, _MULT, _7, _RCL_RAM, _MULT, _NEG, _5, _RCL_RAM, _ADD, // stddev (XX-n*m^2)/(n-1)
  //_7, _RCL, _1, _SUB, _DIV, _SQRT, _SWAP, _END,
  _7, _RCL_RAM, _1, _SUB, _DIV, _SQRT, _SWAP, _END,

  //_6, _RCL, _7, _RCL, _MULT, _8, _RCL, _9, _RCL, _MULT, _SUB, //33 L.R. - a
  _6, _RCL_RAM, _7, _RCL_RAM, _MULT, _8, _RCL_RAM, _9, _RCL_RAM, _MULT, _SUB, //33 L.R. - a
  //_5, _RCL, _7, _RCL, _MULT, _8, _RCL, _DUP, _MULT, _SUB, _DIV,
  _5, _RCL_RAM, _7, _RCL_RAM, _MULT, _8, _RCL_RAM, _DUP, _MULT, _SUB, _DIV,
  //_DUP, _8, _RCL, _MULT, _NEG, _9, _RCL, _ADD, _7, _RCL, _DIV, _SWAP, _END, // b
  _DUP, _8, _RCL_RAM, _MULT, _NEG, _9, _RCL_RAM, _ADD, _7, _RCL_RAM, _DIV, _SWAP, _END, // b

  _OVER, _DIV, _1, _0, _0, _MULT, _END, //34 % x/B*100%
  _OVER, _SUB, _OVER, _DIV, _1, _0, _0, _MULT, _END, //35 DELTA% (x-B)/B*100%
  _DUP, _INT, _SUB, _END, //36 FRAC

  _DUP, _PI, _MULT, _1, _8, _0, _DIV, //37 DEG<>RAD
  _SWAP, _1, _8, _0, _MULT, _PI, _DIV, _END,

  _DUP, _1, _DOT, _8, _MULT, _3, _2, _ADD, //38 C<>F
  _SWAP, _3, _2, _SUB, _1, _DOT, _8, _DIV, _END,

  _DUP, _1, _DOT, _6, _0, _9, _3, _4, _4, _DUP, _DUP, //39 KM<>MI
  _ROT, _SWAP, _DIV, _ROT, _ROT, _MULT, _END,

  _DUP, _3, _DOT, _3, _7, _0, _0, _7, _9, _DUP, _DUP, //40 M<>FT
  _ROT, _MULT, _ROT, _ROT, _DIV, _END,

  _DUP, _2, _DOT, _5, _4, _DUP, _DUP, _ROT, _SWAP, _DIV, _ROT, _ROT, _MULT, _END, //41 CM<>IN

  _DUP, _2, _DOT, _2, _0, _4, _6, _2, _3, _DUP, _DUP, //42 KG<>LBS
  //_ROT, _SWAP, _DIV, _ROT, _ROT, _MULT, _END,
  _ROT, _MULT, _ROT, _ROT, _DIV, _END,

  _DUP, _3, _DOT, _7, _8, _5, _4, _1, _2, _DUP, _DUP, //43 L<>GAL
  _ROT, _SWAP, _DIV, _ROT, _ROT, _MULT, _END,

};

// Command names
const char c0[] PROGMEM = "0"; //      PRIMARY KEYS
const char c1[] PROGMEM = "1";
const char c2[] PROGMEM = "2";
const char c3[] PROGMEM = "3";
const char c4[] PROGMEM = "4";
const char c5[] PROGMEM = "5";
const char c6[] PROGMEM = "6";
const char c7[] PROGMEM = "7";
const char c8[] PROGMEM = "8";
const char c9[] PROGMEM = "9";
const char c10[] PROGMEM = ".";
const char c11[] PROGMEM = "DUP"; // #, Enter
const char c12[] PROGMEM = "DRP"; // X, Clear
const char c13[] PROGMEM = "N"; // NEG
const char c14[] PROGMEM = "E"; // EE
const char c15[] PROGMEM = "F"; // Shift
const char c16[] PROGMEM = "CPX"; //0         F-KEYS
const char c17[] PROGMEM = "RCL"; //1
const char c18[] PROGMEM = "fS"; //2 SOLVE
const char c19[] PROGMEM = "-"; //3
const char c20[] PROGMEM = "DIC"; //4
const char c21[] PROGMEM = "fX"; //5
const char c22[] PROGMEM = "*"; //6
const char c23[] PROGMEM = "~+"; //7 SUM+

const char c24[] PROGMEM = "PRG"; //8
const char c25[] PROGMEM = "/"; //9
const char c26[] PROGMEM = "R-P"; //d
const char c27[] PROGMEM = "+"; //x
const char c28[] PROGMEM = "CLR"; //c
const char c29[] PROGMEM = "OVR"; //s
const char c30[] PROGMEM = "SWP"; //e
const char c31[] PROGMEM = "G"; //f
const char c32[] PROGMEM = "BAT"; //0       G-KEYS
const char c33[] PROGMEM = "STO"; //1
const char c34[] PROGMEM = "fI"; //2
const char c35[] PROGMEM = "\\-"; //3
const char c36[] PROGMEM = "USR"; //4
const char c37[] PROGMEM = "fP"; //5
const char c38[] PROGMEM = "\\"; //6
const char c39[] PROGMEM = "~C"; //7 SUMCLR
const char c40[] PROGMEM = "BAS"; //8
const char c41[] PROGMEM = "MOD"; //9
const char c42[] PROGMEM = "CLK"; //d
const char c43[] PROGMEM = "\\+"; //x
const char c44[] PROGMEM = "To"; //c
const char c45[] PROGMEM = "PIC"; //s
const char c46[] PROGMEM = "ROT"; //e
const char c47[] PROGMEM = "OFF"; //f

const char c48[] PROGMEM = "<"; //     OTHER INTRINSIC FUNCTIONS
const char c49[] PROGMEM = "=";
const char c50[] PROGMEM = "<>";
const char c51[] PROGMEM = ">";
const char c52[] PROGMEM = "NAN";
const char c53[] PROGMEM = "+t";
const char c54[] PROGMEM = "PSE";
const char c55[] PROGMEM = "INT";
const char c56[] PROGMEM = "@"; // PEEK
const char c57[] PROGMEM = "!"; // POKE
const char c58[] PROGMEM = "BEG";
const char c59[] PROGMEM = "UNT";
const char c60[] PROGMEM = "EXE";
const char c61[] PROGMEM = "IF";
const char c62[] PROGMEM = "ELS";
const char c63[] PROGMEM = "THN";
const char c64[] PROGMEM = "BRK";
const char c65[] PROGMEM = "KEY";
const char c66[] PROGMEM = "cTX";
const char c67[] PROGMEM = "EMI";
const char c68[] PROGMEM = "re?";
const char c69[] PROGMEM = "nCr";
const char c70[] PROGMEM = "nPr";
const char c71[] PROGMEM = "PI";
const char c72[] PROGMEM = "INV";
const char c73[] PROGMEM = "SIN";
const char c74[] PROGMEM = "EXP";
const char c75[] PROGMEM = "LN";

const char c76[] PROGMEM = "$"; //     BUILTIN FUNCTIONS
const char c77[] PROGMEM = "HEX";
const char c78[] PROGMEM = "AND";
const char c79[] PROGMEM = "NOT";
const char c80[] PROGMEM = "OR";
const char c81[] PROGMEM = "OVR";
const char c82[] PROGMEM = "ABS";
const char c83[] PROGMEM = ",#"; // SQRT
const char c84[] PROGMEM = "COS";
const char c85[] PROGMEM = "TAN";
const char c86[] PROGMEM = "POW";
const char c87[] PROGMEM = "10^";
const char c88[] PROGMEM = "LOG";
const char c89[] PROGMEM = "aSN";
const char c90[] PROGMEM = "aCS";
const char c91[] PROGMEM = "aTN";
const char c92[] PROGMEM = "SNh";
const char c93[] PROGMEM = "CSh";
const char c94[] PROGMEM = "TNh";
const char c95[] PROGMEM = "aSh";
const char c96[] PROGMEM = "aCh";
const char c97[] PROGMEM = "aTh";
const char c98[] PROGMEM = "LN!";
const char c99[] PROGMEM = "h";
const char c100[] PROGMEM = "hms";
const char c101[] PROGMEM = "PV";
const char c102[] PROGMEM = "ND";
const char c103[] PROGMEM = "QE";
const char c104[] PROGMEM = "CLK";
const char c105[] PROGMEM = "~C"; //clrSUM
const char c106[] PROGMEM = "~+";
const char c107[] PROGMEM = "~-";
const char c108[] PROGMEM = "#"; // STAT
const char c109[] PROGMEM = "LR";
const char c110[] PROGMEM = "%";
const char c111[] PROGMEM = "%C";
const char c112[] PROGMEM = "FRC";
const char c113[] PROGMEM = "'d";
const char c114[] PROGMEM = "'C";
const char c115[] PROGMEM = "'km";
const char c116[] PROGMEM = "'m";
const char c117[] PROGMEM = "'cm";
const char c118[] PROGMEM = "'kg";
const char c119[] PROGMEM = "'l";
const char c120[] PROGMEM = "STR"; // STO-RAM (RAM store)
const char c121[] PROGMEM = "RCR"; // RCL-RAM (RAM recall)

const char* const cmd[] PROGMEM = {
  c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, c17, c18, c19, c20,
  c21, c22, c23, c24, c25, c26, c27, c28, c29, c30, c31, c32, c33, c34, c35, c36, c37, c38, c39, c40,
  c41, c42, c43, c44, c45, c46, c47, c48, c49, c50, c51, c52, c53, c54, c55, c56, c57, c58, c59, c60,
  c61, c62, c63, c64, c65, c66, c67, c68, c69, c70, c71, c72, c73, c74, c75, c76, c77, c78, c79, c80,
  c29, c82, c83, c84, c85, c86, c87, c88, c89, c90, c91, c92, c93, c94, c95, c96, c97, c98, c99, c100,
  c101, c102, c103, c42, c39, c23, c107, c108, c109, c110, c111, c112, c113, c114, c115, c116,
  c117, c118, c119, c120, c121
};

static byte cmdsort[MAXCMDU]; // Alphabetically sorted commands for DICT

#if LOG_USER_MENU
static void logUserMenuCommand(const char* phase, byte cmdId) {
  if (!Serial) return;
  Serial.print("[USR_MENU] ");
  Serial.print(phase ? phase : "n/a");
  Serial.print(" cmdId=");
  Serial.print(cmdId);
  Serial.print(" label=");
  if (cmdId < MAXCMDB) {
    size_t cmdCount = sizeof(cmd) / sizeof(cmd[0]);
    if (cmdId < cmdCount && cmd[cmdId]) Serial.print(cmd[cmdId]);
    else Serial.print("(intrinsic)");
  }
  else if (cmdId < MAXCMDB + nou) {
    char name[PRGNAMEMAX + 1];
    int ptr = seekusr(cmdId);
    for (byte i = 0; i < PRGNAMEMAX; i++) {
      byte ch = 0;
      EEPROM.get(ptr + i, ch);
      name[i] = (ch >= 32 && ch <= 126) ? ch : '.';
    }
    name[PRGNAMEMAX] = '\0';
    Serial.print(name);
  }
  else {
    Serial.print("(empty)");
  }
  Serial.println();
}
#endif

// FORWARD DECLARATIONS
static void _sub(void);   
static void _sumadd(void);   
static void _prgselect(void);   
static void _swap(void);   
static void _usrset(void);   
static void _torch(void);   
static void _sumclear(void);   
static void _rot(void);   
static void _until(void);   
static void _sin(void);   
static void _sqrt(void);   
static void _tan(void);   

static void _pow(void);   
static void _pwr10(void);   
static void _sinh(void);   
static void _tanh(void);   
static void _pv(void);   
static void _qe(void);   
static void _sumsub(void);   
static void _sumstat(void);   
static void _sumlr(void);   
static data dpop(void);   
static void a(void);   
static void b(void);   
static void im(void);   
static void dpush(data);   
static float dpopr(void);   
static double dpushr(double);   
static void B2stack(void);   
static void stack2B(void);   
static void apush(int);   
static int64_t dpopb(void);  
static void dpushb(int64_t);  
static boolean isreal(void);  
static double absolute(double, double);  
static double angle(double, double);  
static double texp(double);  
static void execute(byte);  
static void limitdarktime(void);  
static void eepromMove(int from, int to, int length);  

// FUNCTION POINTER ARRAY
static void (*dispatch[])(void) = { // Function pointer array
  &_n0, &_n1, &_n2, &_n3, &_n4, &_n5, &_n6, &_n7, &_n8, &_n9, // Primary keys
  &_dot, &_dup, &_drop, &_neg, &_e, &_keyf,

  &_complex, &_mrcl, &_fnsolve, &_sub, // 0123  FKEY Offset = 16
  &_dict, &_fnfx, &_mul, &_sumadd, // 4567
  &_prgselect, &_div, &_recpol, &_add, // 89dx
  &_clr, &_over, &_swap, &_keyg, // csef
  &_batt, &_msto, &_fnintegrate, &_litdown, // 0123  GKEY Offset = 32
  &_usrset, &_fnplot, &_torch, &_sumclear, // 4567
  &_base, &_mod, &_clock, &_litup, // 89dx
  &_offset, &_pick, &_rot, &_keygoff, // csef

  &_condlt, &_condeq, &_condne, &_condgt, // 48
  &_nand, &_addduration, &_pause, &_integer,
  &_peek, &_poke, &_begin, &_until,
  &_exe, &_condif, &_condelse, &_condthen,

  &_break, &_getkey, &_alphaclear, &_alphaput, // 64
  &_isreal, &_comb, &_perm, &_pi,
  &_inv, &_sin, &_exp, &_ln,

  &_business, &_hex, &_land, &_lnot, // 76 Builtin
  &_lor, &_over, &_absolute, &_sqrt,
  &_cos, &_tan, &_pow, &_pwr10,
  &_log, &_asin, &_acos, &_atan, //88
  &_sinh, &_cosh, &_tanh, &_asinh,
  &_acosh, &_atanh, &_gammaln, &_hms2h,
  &_h2hms, &_pv, &_nd, &_qe, //100
  &_clock, &_sumclear, &_sumadd, &_sumsub,
  &_sumstat, &_sumlr, &_percent, &_percentdelta, //108
  &_frac, &_cvdeg2rad, &_cvc2f, &_cvkm2mi,
  &_cvm2ft, &_cvcm2in, &_cvkg2lbs, &_cvl2gal, //116
  
  // RAM store functions at the end of dispatch array
  &_sto_ram, &_rcl_ram, // Commands 120-121: STO-RAM and RCL-RAM
};
static void _nop(void) {} // NOP - no operation
static void _n0(void) { // 0
  _numinput(_0);
}
static void _n1(void) { // 1
  _numinput(_1);
}
static void _n2(void) { // 2
  _numinput(_2);
}
static void _n3(void) { // 3
  _numinput(_3);
}
static void _n4(void) { // 4
  _numinput(_4);
}
static void _n5(void) { // 5
  _numinput(_5);
}
static void _n6(void) { // 6
  _numinput(_6);
}
static void _n7(void) { // 7
  _numinput(_7);
}
static void _n8(void) { // 8
  _numinput(_8);
}
static void _n9(void) { // 9
  _numinput(_9);
}

static void seekmem(byte n);   

static void _absolute(void) { // ABS
  seekmem(_ABS);
}
static void _acos(void) { // ACOS
  seekmem(_ACOS);
}
static void _acosh(void) { // ACOSH
  seekmem(_ACOSH);
}
static void _add(void) { // ADD + (a+i*b)(c+i*d)=(a+c)+i*(b+d)
  struct data b = dpop(), a = dpop();
  double re = a.r + b.r, im = a.i + b.i;
#if LOG_OVERFLOW
  double re_log = log10(_abs(re));
  double im_log = log10(_abs(im));
  int16_t re_exp_int16 = (int16_t)re_log;
  int16_t im_exp_int16 = (int16_t)im_log;
  if (Serial && (_abs(re) > 1e50 || _abs(im) > 1e50)) {
    Serial.print("[OVERFLOW_ADD] re="); Serial.print(re, 3);
    Serial.print(" im="); Serial.print(im, 3);
    Serial.print(" | log10(re)="); Serial.print(re_log, 2);
    Serial.print(" as int16="); Serial.print(re_exp_int16);
    Serial.print(" | log10(im)="); Serial.print(im_log, 2);
    Serial.print(" as int16="); Serial.print(im_exp_int16);
    Serial.print(" | OVERFLOW="); Serial.print(OVERFLOW);
    Serial.print(" | DETECTED="); Serial.println((re_exp_int16 > OVERFLOW || im_exp_int16 > OVERFLOW) ? "YES" : "NO");
  }
#endif
  // FIX: Use int16_t instead of int8_t to prevent wraparound for large exponents
  if ((int16_t)log10(_abs(re)) > OVERFLOW || (int16_t)log10(_abs(im)) > OVERFLOW)
    msgnr = MSGOVERFLOW;
  else {
    struct data tmp = {re, im, a.b + b.b};
    dpush(tmp);
  }
}
static void _addduration(void) { // ADD DURATION
  dpushr(dpopr() + (millis() - durationtimestamp));
  durationtimestamp = millis();
}
static void _alphaclear(void) { // ALPHA CLEAR
  alpha[0] = '\0'; 
  isprintalpha = false;
}
static void _alphaput(void) { // ALPHA PUTCHAR
  byte l = strlen(alpha);
  if (l < ALPHABUFSIZE) {
    alpha[l] = (byte)dpopr(); 
    alpha[l + 1] = '\0'; 
  }
  isprintalpha = true;
}
static void _asin(void) { // ASIN
  seekmem(_ASIN);
}
static void _asinh(void) { // ASINH
  seekmem(_ASINH);
}
static void _atan(void) { // ATAN
  seekmem(_ATAN);
}
static void _atanh(void) { // ATANH
  seekmem(_ATANH);
}
static void _base(void) { // BASE MODE
  if (base) { // Return from base mode
    B2stack(); base = 0;
  }
  else { // Switch to base mode
    base = dpopr();
    if (base > 1) stack2B(); else base = 0;
  }
  EEPROM.put(EE_BASE, base); 
}

static void _batt(void) { // BATT

    // Ensure proper pin configuration
    pinMode(PIN_VCC_MEASURE, INPUT);
    
    // Small delay for stabilization
    delayMicroseconds(100);
    
    // Read ADC value
    int adcValue = analogRead(PIN_VCC_MEASURE);
    
    // Calculate battery voltage
    // adcValue (0-1023) represents 0-3.3V
    // Your 1:1 divider means multiply by 2 for full battery voltage
    double adcVoltage = (adcValue / 1023.0) * 3.3;
    double batteryVoltage = adcVoltage * 2.0 * 3.869/3.602; // No idea why values on calc are too low, ominous factor from measurements
    
    dpushr(batteryVoltage);
}


static void _begin(void) { // BEGIN
  apush(mp);
}
static void _break(void) { // BREAK
  breakmp = mp; mp = 0;
  isprintscreen = true;
}
static void _business(void) { // BUSINESS MODE
  seekmem(_BUS);
}
static void _ce(void) { // CE
  if (isdot) {
    if (base) { // Drop if in isdot-mode
      dpopb();
      isdot = false; isnewnumber = true;
    }
    else {
      if (decimals)
        dpushr(((long)(dpopr() * pow10(--decimals)) / pow10(decimals)));
      else isdot = false;
    }
  }
  else {
    if (base) {
      int64_t a = dpopb();
      a = a / 100LL / base * 100LL;
      if (!a) isnewnumber = true;
      else dpushb(a);
    }
    else {
      long a = dpopr() / 10.0;
      if (!a) isnewnumber = true;
      else dpushr(a);
    }
  }
}

// Clock state variables
static struct {
  bool active;
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  unsigned long previousMillis;
  unsigned long lastKeyCheck;
} clockState = {false, 0, 0, 0, 0, 0};

static void _clock(void) { // CLOCK
  if (dp > 0) { // Start clock if there's something on the stack
#if LOG_CLOCK
    Serial.println("[CLK] Starting clock");
#endif
   // Get initial time from stack
   // expected format: hours, minutes, seconds (top of stack)
    clockState.seconds = dpopr();
    clockState.minutes = dpopr();
    clockState.hours = dpopr();
    if (clockState.hours >= 24 || clockState.minutes >= 60 || clockState.seconds >= 60) {
#if LOG_CLOCK
      Serial.println("[CLK] Invalid time on stack, resetting to 00:00:00");
#endif
      clockState.hours = 0;
      clockState.minutes = 0;
      clockState.seconds = 0;
    }
#if LOG_CLOCK
    Serial.println("[CLK] ####################");
    Serial.print("[CLK] Starting Time: ");
    if (clockState.hours < 10) Serial.print("0");
    Serial.print(clockState.hours);
    Serial.print(":");
    if (clockState.minutes < 10) Serial.print("0");
    Serial.print(clockState.minutes);
    Serial.print(":");
    if (clockState.seconds < 10) Serial.print("0");
    Serial.println(clockState.seconds);
    Serial.println("[CLK] ####################");
#endif

    // Initialize clock state
    clockState.active = true;
    clockState.previousMillis = millis();
    clockState.lastKeyCheck = millis();
    
    // Clear stack and show initial time
    dp = 0;
    dpushr(clockState.hours); // hours
    dpushr(clockState.minutes); // minutes
    dpushr(clockState.seconds); // seconds
    
    isprintscreen = true;
    isnewnumber = true;
#if LOG_CLOCK    
    Serial.println("[CLK] Clock started");
#endif
  }
}

static void clockUpdate() { // Called from main loop when clock is active
  if (!clockState.active) return;
  
  unsigned long currentMillis = millis();
  
  // Update powertimestamp to prevent device from sleeping during clock operation
  powertimestamp = currentMillis;
  
  // Check for key press to exit
  if (currentMillis - clockState.lastKeyCheck >= 100) {
    clockState.lastKeyCheck = currentMillis;
    key = getkey();
    if (key < NOPRINTNOKEY && key != oldkey) {
      oldkey = key;
#if LOG_CLOCK
      Serial.println("[CLK] Key pressed, stopping clock");
#endif
      clockState.active = false;
      powertimestamp = millis(); // Reset power management timer
      isprintscreen = true;
      return;
    }
  }
  
  // Update time every second with drift compensation
  unsigned long elapsed = currentMillis - clockState.previousMillis;
  if (elapsed >= 1000) {
    // Calculate how many seconds have passed (could be >1 if processing was delayed)
    unsigned long secondsToAdd = elapsed / 1000;
    // Update the previous timestamp to maintain sync, compensating for processing time
    clockState.previousMillis += secondsToAdd * 1000;
    
    // Add the seconds (with proper minute/hour rollover)
    clockState.seconds += secondsToAdd;
    while (clockState.seconds >= 60) {
      clockState.seconds -= 60;
      clockState.minutes++;
      if (clockState.minutes >= 60) {
        clockState.minutes = 0;
        clockState.hours = (clockState.hours + 1) % 24;
      }
    }
    
    // Update stack with new time
    dp = 0;
    dpushr(clockState.hours);
    dpushr(clockState.minutes);
    dpushr(clockState.seconds);
    
#if LOG_CLOCK
    Serial.print("[CLK] Time: ");
    if (clockState.hours < 10) Serial.print("0");
    Serial.print(clockState.hours);
    Serial.print(":");
    if (clockState.minutes < 10) Serial.print("0");
    Serial.print(clockState.minutes);
    Serial.print(":");
    if (clockState.seconds < 10) Serial.print("0");
    Serial.println(clockState.seconds);
#endif
    isprintscreen = true;
    isnewnumber = true;
  }
}

static void _clr(void) { // CLR
#if LOG_FX_FI_STACK
  if (Serial && mp) { // Only log during program execution
    Serial.print("[CLR] BEFORE - dp="); Serial.print(dp);
    Serial.print(" ap="); Serial.print(ap);
    Serial.print(" mp="); Serial.println(mp);
  }
#endif
  dp = 0;
  _alphaclear();
#if LOG_FX_FI_STACK
  if (Serial && mp) {
    Serial.print("[CLR] AFTER - dp="); Serial.print(dp);
    Serial.print(" ap="); Serial.println(ap);
  }
#endif
}
static void _comb(void) { // COMB nCr=n!/r!/(n-r)!=nPr/r!
  byte k = dpushr(dpopr());
  _perm();
  for (byte i = 1; i <= k; i++) dpushr(dpopr() / i);
}
static void _complex(void) { // COMPLEX
  if (!base) {
    Serial.print("[CPX] isreal()="); Serial.print(isreal());
    Serial.print(" ispolar="); Serial.println(ispolar);
    if (dp > 0) {
      Serial.print("[CPX] TOS before: r="); Serial.print(ds[dp-1].r, 8);
      Serial.print(" i="); Serial.println(ds[dp-1].i, 8);
    }
    
    if (isreal()) { // Stack -> complex (two real values → one complex)
      // FIX: Always treat input as RECTANGULAR (real + imag), regardless of ispolar flag
      // The ispolar flag is for DISPLAY only, not for input interpretation!
      double b = dpopr(), a = dpopr();
      dpush({a, b, 0LL});
      Serial.print("[CPX] Created complex from stack: r="); Serial.print(a, 6);
      Serial.print(" i="); Serial.print(b, 6);
      Serial.println(" (ispolar ignored for input)");
    }
    else { // Complex -> stack (one complex → two real values)
      struct data a = dpop();
      if (ispolar) { // Output polar: extract magnitude and angle
        double mag = absolute(a.r, a.i);
        double ang = angle(a.r, a.i);
        Serial.print("[CPX] Complex→Polar: r="); Serial.print(a.r, 6);
        Serial.print(" i="); Serial.print(a.i, 6);
        Serial.print(" → mag="); Serial.print(mag, 6);
        Serial.print(" angle="); Serial.println(ang, 6);
        dpushr(mag); dpushr(ang);
      }
      else { // Output rectangular: extract real and imaginary
        Serial.print("[CPX] Complex→Rect: r="); Serial.print(a.r, 6);
        Serial.print(" i="); Serial.println(a.i, 6);
        dpushr(a.r); dpushr(a.i);
      }
    }
    
    if (dp > 0) {
      Serial.print("[CPX] TOS after: r="); Serial.print(ds[dp-1].r, 8);
      if (dp > 0) Serial.print(" i="); Serial.println(ds[dp-1].i, 8);
    }
  }
}
static void _condelse(void) { // CONDITION ELSE
  if (!cl) msgnr = MSGNEST; // ELSE without corresponding IF
  else {
    _condseek(); // Seek next THEN
    cl--;
  }
}
static void _condeq(void) { // CONDITION =
  dpushr(dpopr() == dpopr());
}
static void _condgt(void) { // CONDITION >
  dpushr(dpopr() < dpopr());
}

static void _condif(void) { // CONDITION IF
  cl++; // Increment conditional level
  if (!dpopr()) _condseek(); // FALSE-Clause - seek next ELSE or THEN
}
static void _condlt(void) { // CONDITION <
  _condgt();
  dpushr(!dpopr());
}
static void _condne(void) { // CONDITION <>
  _condeq();
  dpushr(!dpopr());
}
static void _condseek(void) { // CONDITION - seek next ELSE or THEN
  boolean isloop = true;
  byte cltmp = 0; // Local conditional level
  while (isloop) {
    byte c = '\0'; 
    if (mp < sizeof(mem))  c = mem[mp++]; // Builtin 
    else if (mp < sizeof(mem) + sou) EEPROM.get(mp++ -sizeof(mem) + EEUSTART, c); // User

    if (mp >= sizeof(mem) + sou) { // No corresponding ELSE or THEN
      msgnr = MSGNEST;
      isloop = false;
    }
    else if (c == _IF) cltmp++; // Nested IF found
    else if (cltmp && c == _THEN) cltmp--; // Nested IF ended
    else if (!cltmp && (c == _ELSE || c == _THEN)) isloop = false;
  }
}
static void _condthen(void) { // CONDITION THEN
  if (!cl) msgnr = MSGNEST; // THEN without corresponding IF
  else cl--; // Decrement conditional level
}
static void _cvc2f(void) { // CONVERT C>F
  seekmem(_CF);
}
static void _cvcm2in(void) { // CONVERT CM>IN
  seekmem(_CMIN);
}
static void _cvdeg2rad(void) { // CONVERT DEG>RAD
  seekmem(_DEGRAD);
}
static void _cvkg2lbs(void) { // CONVERT KG<>LBS
  seekmem(_KGLBS);
}
static void _cvl2gal(void) { // CONVERT L<>GAL
  seekmem(_LGAL);
}
static void _cvkm2mi(void) { // CONVERT KM>MI
  seekmem(_KMMI);
}
static void _cvm2ft(void) { // CONVERT M>FT
  seekmem(_MFT);
}
static void _cos(void) { // COS
  seekmem(_COS);
}
static void _cosh(void) { // COSH
  seekmem(_COSH);
}
static void _dict(void) { // DICT
  if (!base) {
    sel = 0;
    isdict = true;
  }
}
static void _div(void) { // DIV / a/b=a*inv(b)
  if (base) {
    int64_t divisor = dpopb();
    if (!divisor) {
      msgnr = MSGOVERFLOW;
      return;
    }

    int64_t scale = 1;
    while (divisor % 10LL == 0) {
      divisor /= 10LL;
      scale *= 10LL;
    }

    int64_t dividend = dpopb();
    if (!divisor) {
      msgnr = MSGOVERFLOW;
      return;
    }

    int64_t quotient = dividend / divisor;
    int64_t gcd = 1;
    if (scale >= 100) gcd = 100;
    else if (scale >= 10) gcd = 10;

    int64_t numeratorFactor = 100LL / gcd;
    int64_t denominatorFactor = scale / gcd;

    int64_t scaled = quotient * numeratorFactor;
    if (denominatorFactor > 1) scaled /= denominatorFactor;

    double asReal = scaled / 100.0;
    if (scaled != 0 && log10(_abs(asReal)) > OVERFLOW) {
      msgnr = MSGOVERFLOW;
      return;
    }

    dpushb(scaled);
  }
  else {
    _inv();
    if (dp < 2) return;
    _mul();
  }
}
static void _dot(void) { // DOT .
  bool was_isnewnumber = isnewnumber;
  bool was_isdot = isdot;
  byte was_decimals = decimals;

  if (isnewnumber) {
    dpush(C0); // Start new number with 0
    decimals = 0; isnewnumber = false;
  }
  if (base && base != 10) isAF = true; // Enter A~F
  else isdot = true;

#if LOG_INPUT_PRINTING
  // Debug logging for comma input
  Serial.println("[PRINT_DEBUG] Comma (.) key pressed - dot function executed");
  Serial.print("[PRINT_DEBUG] Before: isnewnumber="); Serial.print(was_isnewnumber);
  Serial.print(", isdot="); Serial.print(was_isdot);
  Serial.print(", decimals="); Serial.print(was_decimals);
  Serial.print(" | After: isnewnumber="); Serial.print(isnewnumber);
  Serial.print(", isdot="); Serial.print(isdot);
  Serial.print(", decimals="); Serial.print(decimals);
  Serial.print(", base="); Serial.println(base);
#endif
}
static void _drop(void) { // DROP, X
  if (isnewnumber && breakmp) { // Return from break and continue program
    mp = breakmp; breakmp = 0;
    delayshort(250); // To prevent interrupting the run with C key
  }
  else if (isnewnumber && dp) dp--; // CLX
  if (!isnewnumber) _ce(); // Clear entry
}
static void _dup(void) { // DUP, #
  if (isnewnumber && dp) dpush(ds[dp - 1]);
}
static void _e(void) { // E
  if (base) dpushb(dpopb() * base * base * base * base); // Expand 4 digits
  else {
    int tmp = dpushr(dpopr());
    if (tmp < -OVERFLOW || tmp > OVERFLOW) msgnr = MSGOVERFLOW;
    else {
      dpushr(pow10(dpopr()));
      _mul();
    }
  }
}
static void _exe(void) { // EXE
  mp = dpopr() + sizeof(mem);
}
static void _exp(void) { // EXP exp(a+jb)=exp(a)*(cos(b)+i*sin(b))
  struct data a = dpop();
  if (a.r > OVERFLOWEXP) msgnr = MSGOVERFLOW;
  else {
    double tmp = texp(a.r);
    double realPart = tmp * cos(a.i);
    double imagPart = tmp * sin(a.i);
    
    // Round tiny imaginary parts to zero (must check AFTER multiplication by exp)
    // Floating-point errors in sin(2π) etc get amplified by large exp values
    if (_abs(imagPart / tmp) < TINYNUMBER) imagPart = 0.0;
    
#if LOG_FX_FI_STACK
    if (Serial && mp) {
      Serial.print("[EXP] input=("); Serial.print(a.r, 6);
      Serial.print(", "); Serial.print(a.i, 6); Serial.print("rad");
      Serial.print(") exp="); Serial.print(tmp, 6);
      Serial.print(" cos="); Serial.print(cos(a.i), 6);
      Serial.print(" sin="); Serial.print(sin(a.i), 6);
      Serial.print(" -> output=("); Serial.print(realPart, 6);
      Serial.print(", "); Serial.print(imagPart, 6); Serial.println(")");
    }
#endif
    dpush({realPart, imagPart, a.b});
  }
}
static void _fnfx(void) { // FN F(X)
  if (!base) {
#if LOG_FX_FI_STACK
    if (Serial) {
      Serial.println("*** FX CALLED - executing first user program ***");
      Serial.print("  MAXCMDB="); Serial.print(MAXCMDB);
      Serial.print(" nou="); Serial.print(nou);
      Serial.print(" sou="); Serial.println(sou);
      Serial.print("  BEFORE FX - dp="); Serial.print(dp);
      Serial.print(" ap="); Serial.print(ap);
      Serial.print(" STACK: ");
      for (byte i = 0; i < dp && i < 8; i++) {
        Serial.print("["); Serial.print(i); Serial.print("]=");
        Serial.print(ds[i].r, 6);
        if (ds[i].i != 0.0) {
          Serial.print("+j"); Serial.print(ds[i].i, 6);
        }
        Serial.print(" ");
      }
      Serial.println();
      Serial.flush(); // Force output before program starts
      delay(50); // Allow serial buffer to clear
    }
#endif
    execute(MAXCMDB);
#if LOG_FX_FI_STACK
    if (Serial) {
      Serial.print("  AFTER FX - dp="); Serial.print(dp);
      Serial.print(" ap="); Serial.print(ap);
      Serial.print(" STACK: ");
      for (byte i = 0; i < dp && i < 8; i++) {
        Serial.print("["); Serial.print(i); Serial.print("]=");
        Serial.print(ds[i].r, 6);
        if (ds[i].i != 0.0) {
          Serial.print("+j"); Serial.print(ds[i].i, 6);
        }
        Serial.print(" ");
      }
      Serial.println();
      Serial.flush();
    }
#endif
  }
}

static constexpr uint16_t FX_IMMEDIATE_STEP_LIMIT = 2048;

static bool runImmediateProgram(uint16_t maxSteps) {
  if (!maxSteps) maxSteps = FX_IMMEDIATE_STEP_LIMIT;
  uint16_t steps = 0;
  while (mp && steps++ < maxSteps) {
    byte cmdByte = _END;
    if (mp < sizeof(mem)) {
      cmdByte = mem[mp++];
    }
    else if (mp < sizeof(mem) + sou) {
      EEPROM.get(mp - sizeof(mem) + EEUSTART, cmdByte);
      mp++;
    }
    else {
      mp = 0;
      return false;
    }

    if (cmdByte >= MAXCMDB && cmdByte != _END) apush(mp);

    if (cmdByte == _END) {
      if (ap) mp = apop();
      else mp = 0;
      continue;
    }

    execute(cmdByte);
  }
  return mp == 0;
}

static bool evaluateFxImmediate(double x, double& fx) {
  if (base || mp) return false;
  int saved_ap = ap;
  dpushr(x);
  execute(MAXCMDB);
  if (!runImmediateProgram()) {
    mp = 0;
    ap = saved_ap;
    return false;
  }
  if (!dp) {
    ap = saved_ap;
    return false;
  }
  fx = dpoprd();
  ap = saved_ap;
  isnewnumber = true;
  return true;
}
static void _fnintegrate(void) { // FN INTEGRATE
  if (!base) {
    _swap();
    double lowerRaw = dpopr();
    double upperRaw = dpopr();

    double orientation = 1.0;
    if (upperRaw < lowerRaw) {
      orientation = -1.0;
      double tmp = lowerRaw;
      lowerRaw = upperRaw;
      upperRaw = tmp;
    }

    inta = lowerRaw;
    intb = upperRaw;

    if (Serial) {
      Serial.print("Starting adaptive Gauss-Kronrod integration on [");
      Serial.print(inta);
      Serial.print(", ");
      Serial.print(intb);
      Serial.println("]");
    }

    double span = intb - inta;
    if (_abs(span) <= TINYNUMBER) {
      dpushr(0.0);
      isnewnumber = true;
      isprintscreen = true;
      return;
    }

    gkResetController();
    gkController.active = true;
    gkController.orientation = orientation;
    gkController.targetAbsTol = GK_DEFAULT_ABS_TOL;
    gkController.targetRelTol = GK_DEFAULT_REL_TOL;
    gkController.lastError = 0.0;
    gkController.total = 0.0;

    sel = cycles = 0;

    if (!gkPushInterval(inta, intb, GK_DEFAULT_ABS_TOL, 0) || !gkStartNextInterval()) {
      gkController.active = false;
      msgnr = MSGOVERFLOW;
      isprintscreen = true;
      return;
    }

    dpushr(gkCurrentNodePosition());
    isint = true;
  }
}
void _fnplot() { // FN PLOT
  if (!base) {
    plotb = dpopr(); plota = dpopr();
    if (GRAPH_PIXEL_WIDTH > 1) {
      plotd = (plotb - plota) / (GRAPH_PIXEL_WIDTH - 1);
    }
    else {
      plotd = 0.0;
    }
    cycles = 0;
    for (byte i = 0; i < GRAPH_PIXEL_WIDTH; ++i) plot[i] = 0.0;
    isplot = isplotcalc = true;
    isplotEvalInFlight = false;
#if LOG_PLOT
    Serial.print("[PLOT] start interval [");
    Serial.print(plota, 10);
    Serial.print(", ");
    Serial.print(plotb, 10);
    Serial.print("] step=");
    Serial.print(plotd, 10);
    Serial.print(" width=");
    Serial.println(GRAPH_PIXEL_WIDTH);
#endif
  }
}
static void _fnsolve(void) { // FN SOLVE
  if (!base) {
#if LOG_SOLVE
    if (Serial) {
      Serial.print("[SOLVE] dp="); Serial.println(dp);
    }
#endif
    x1 = dpopr();
    if (dp >= 1) { // Interval provided with at least two values in stack, one already popped
      x0 = dpopr();
    }
    else if (dp == 0) {
      if (x1 != 0.0) x0 = x1 * 0.1; // Default interval (0.1 x1, x1) if only one value != 0, probably flaky, at least stay in same quadrant.
      else  {
        x1 = (double)rand() / RAND_MAX * 10; // Default interval if only one value = 0, probably flaky, at least stay in same quadrant.
        x0 = x1 * 0.1; // x1 = random between 0 and 10 and x0 = 0.1 * x1
      }
    }

    sel = cycles = 0;
    issolve = true;

    if (Serial) {
      Serial.print("[SOLVE] Starting solver with x0="); Serial.print(x0);
      Serial.print(" x1="); Serial.println(x1);
    }
  }
}
static void _frac(void) { // FRAC
  seekmem(_FRAC);
}
static void _gammaln(void) { // GAMMALN
  seekmem(_GAMMALN);
}
static void _h2hms(void) { // H2HMS
#if LOG_TIME_CONV
  if (Serial) {
    Serial.print("[TIME] H2HMS entry dp="); Serial.print(dp);
    if (dp) {
      Serial.print(" input="); Serial.print(ds[dp - 1].r, 8);
    }
    Serial.print(" isnewnumber="); Serial.println(isnewnumber);
  }
#endif
  //seekmem(_H2HMS);
  //do not use _H2HMS here to avoid stack mess
  double tmp = dpopr();
  double hours = floor(tmp);
  double minutes = floor((tmp - hours) * 60.0);
  double seconds = (int) roundf((tmp - hours - minutes / 60.0) * 3600.0);

  dpushr(hours + minutes / 100.0 + seconds / 10000.0);

#if LOG_TIME_CONV
  if (Serial) {
    Serial.print("[TIME] tmp="); Serial.print(tmp);
    Serial.print("[TIME] minutes="); Serial.print(minutes);
    Serial.print("[TIME] seconds="); Serial.println(seconds);

    Serial.print("[TIME] H2HMS exit dp="); Serial.print(dp);
    if (dp) {
      Serial.print(" output="); Serial.print(ds[dp - 1].r, 8);
    }
    Serial.print(" isnewnumber="); Serial.println(isnewnumber);
  }
#endif
}
static void _hex(void) { // HEX MODE
  seekmem(_HEX);
}
static void _hms2h(void) { // HMS2H
#if LOG_TIME_CONV
  if (Serial) {
    Serial.print("[TIME] HMS2H entry dp="); Serial.print(dp);
    if (dp) {
      Serial.print(" input="); Serial.print(ds[dp - 1].r, 8);
    }
    Serial.print(" isnewnumber="); Serial.println(isnewnumber);
  }
#endif
  //seekmem(_HMS2H);
  //do not use _HMS2H here to avoid stack mess
  double tmp = dpopr();
  double hours = floor(tmp);
  int minutesSeconds = (int)((tmp - hours) * 10000 + 0.5);
  double minutes = floor(minutesSeconds / 100.0);
  double seconds = minutesSeconds % 100;
  if ((minutes > 59) || (hours > 23) || (seconds > 59)) {
    msgnr = MSGOVERFLOW;
    isprintscreen = true;
    return;
  }
  dpushr(hours + minutes / 60.0 + seconds / 3600.0);

#if LOG_TIME_CONV
  if (Serial) {
    Serial.print("[TIME] HMS2H exit dp="); Serial.print(dp);
    if (dp) {
      Serial.print(" output="); Serial.print(ds[dp - 1].r, 8);
    }
    Serial.print(" isnewnumber="); Serial.println(isnewnumber);
  }
#endif
}
static void _integer(void) { // INTEGER
  dpushr((long)dpopr());
}
static void _inv(void) { // INV inv(a+jb)=a/(a*a+b*b)-i*b/(a*a+b*b)
  struct data a = dpop();
  double denom = a.r * a.r + a.i * a.i;
  if (denom == 0.0 || denom != denom) {
    msgnr = MSGOVERFLOW;
    return;
  }
  double realPart = a.r / denom;
  double imagPart = -a.i / denom;

  if (realPart != realPart || imagPart != imagPart) {
    msgnr = MSGOVERFLOW;
    return;
  }

  double realExponent = (realPart == 0.0) ? -1e6 : log10(_abs(realPart));
  double imagExponent = (imagPart == 0.0) ? -1e6 : log10(_abs(imagPart));

  if (realExponent > OVERFLOW || imagExponent > OVERFLOW) {
    msgnr = MSGOVERFLOW;
    return;
  }

  dpush({realPart, imagPart, 0LL});
}
static void _isreal(void) { // ISREAL?
  dpushr(isreal());
}
static void _getkey(void) { // KEY
  isgetkey = true;
}
static void _keyf(void) { // KEY-F
  fgm = ISF;
  setfgm = 0;
}
static void _keyg(void) { // KEY-G
  fgm = ISG;
  setfgm = 0;
}
static void _keygoff(void) { // KEY-G OFF
  darkscreen = true;
}
static void _lit(byte b) { // LIT
  EEPROM.putBrightness(b); // Wear-leveling write
  screenoff(); delaylong(12); 

  // Set the contrast before turning screen back on
  oled.ssd1306_command(0x81); // Set contrast command
  oled.ssd1306_command(b);    // Contrast value (0-255)
  
  screenon(); // Turn screen back on with new contrast
}


static void _litdown(void) { // LIT DOWN
  byte b = EEPROM.getBrightness(); // Wear-leveling read
  _lit(b > 64 ? b - 64 : 0);
}
static void _litup(void) { // LIT UP
  byte b = EEPROM.getBrightness(); // Wear-leveling read
  _lit(b < 191 ? b + 64 : 255);
}


static void _ln(void) { // LN ln(a+i*b)=ln(r*exp(i*f))=ln(r)+i*f r=|a+i*b| f=atan(b/a)
  struct data a = dpop();
  //Serial.print("LN: x="); Serial.print(a.r); Serial.print(" ln(x)="); Serial.println(log(a.r));
  if (absolute(a.r, a.i) == 0.0) msgnr = MSGOVERFLOW;
  else dpush({log(absolute(a.r, a.i)), angle(a.r, a.i) / RAD, 0LL});
}
static void _log(void) { // LOG
  seekmem(_LOG);
}
static void _land(void) { // LOGIC AND
  seekmem(_AND);
}
static void _lnot(void) { // LOGIC NOT
  seekmem(_NOT);
}
static void _lor(void) { // LOGIC OR
  seekmem(_OR);
}
static void _mod(void) { // MOD
  if (!base) {
    long b = dpopr();
    dpushr((long)dpopr() % b);
  }
}
static void _mstorcl(boolean issto) { // MRCL
#if LOG_FLASH_STORCL
  if (Serial) {
    Serial.print("[FLASH_STORCL] ENTRY op=");
    Serial.print(issto ? "STO" : "RCL");
    Serial.print(" base=");
    Serial.print(base);
    Serial.print(" dp=");
    Serial.print(dp);
    Serial.print(" STACK: ");
    for (byte i = 0; i < dp && i < 5; i++) {
      Serial.print("["); Serial.print(i); Serial.print("]=");
      Serial.print(ds[i].r, 6); Serial.print(" ");
    }
    Serial.println();
  }
#endif
  if (base) {
    if (issto) {
      int64_t businessVal = dpopb();
#if LOG_FLASH_STORCL
      if (Serial) {
        Serial.print("[FLASH_STO] business slot write value=");
        Serial.println((double)businessVal);
      }
#endif
      if (!commitBusinessSlot(businessVal)) msgnr = MSGSAVE;
    }
    else {
      int64_t a;
      EEPROM.get(EEMEMB, a);
#if LOG_FLASH_STORCL
      if (Serial) {
        Serial.print("[FLASH_STO] business slot read value=");
        Serial.println((double)a);
      }
#endif
      dpushb(a);
    }
  }
  else {
#if LOG_FLASH_STORCL
    if (Serial) {
      Serial.print("[FLASH_STORCL] About to pop slot number. dp=");
      Serial.print(dp);
      Serial.print(" TOS=");
      if (dp) Serial.println(ds[dp-1].r, 6);
      else Serial.println("EMPTY");
    }
#endif
    byte tmp = dpopr();
#if LOG_FLASH_STORCL
    if (Serial) {
      Serial.print("[FLASH_STORCL] Popped slot=");
      Serial.print(tmp);
      Serial.print(" dp_after_pop=");
      Serial.print(dp);
      Serial.print(" STACK_AFTER_POP: ");
      for (byte i = 0; i < dp && i < 5; i++) {
        Serial.print("["); Serial.print(i); Serial.print("]=");
        Serial.print(ds[i].r, 6); Serial.print(" ");
      }
      Serial.println();
    }
#endif
    if (tmp < MEMNR) {
      uint32_t realAddr = EEMEM + tmp * sizeof(double);
      uint32_t imagAddr = EEMEM + (tmp + MEMNR) * sizeof(double);
      warnIfConstantSlotOutOfRange(tmp, "real", realAddr);
      warnIfConstantSlotOutOfRange(tmp, "imag", imagAddr);
#if LOG_FLASH_STORCL
      if (Serial) {
        Serial.print("[FLASH_STORCL] slot=");
        Serial.print(tmp);
        Serial.print(" realAddr=0x");
        Serial.print(realAddr, HEX);
        Serial.print(" imagAddr=0x");
        Serial.println(imagAddr, HEX);
      }
#endif
      struct data a;
#if LOG_FLASH_STORCL
      logConstantSlotSnapshot(tmp, issto ? "pre-sto" : "pre-rcl");
#endif
      if (issto) {
        a = dpop();
#if LOG_FLASH_STORCL
        if (Serial) {
          Serial.print("[FLASH_STORCL] STO write R=");
          Serial.print(a.r, 12);
          Serial.print(" I=");
          Serial.println(a.i, 12);
        }
#endif
        if (!commitConstantSlot(tmp, a)) msgnr = MSGSAVE;
#if LOG_FLASH_STORCL
        logConstantSlotSnapshot(tmp, "post-sto");
#endif
      }
      else {
        EEPROM.get(realAddr, a.r);
        EEPROM.get(imagAddr, a.i);
        a.b = 0LL; // Initialize business field
#if LOG_FLASH_STORCL
        if (Serial) {
          Serial.print("[FLASH_STORCL] RCL read R=");
          Serial.print(a.r, 12);
          Serial.print(" I=");
          Serial.print(a.i, 12);
          Serial.print(" dp_before_push=");
          Serial.println(dp);
        }
        logConstantSlotSnapshot(tmp, "post-rcl");
#endif
        dpush(a);
#if LOG_FLASH_STORCL
        if (Serial) {
          Serial.print("[FLASH_STORCL] RCL after dpush. dp=");
          Serial.print(dp);
          Serial.print(" STACK_AFTER_PUSH: ");
          for (byte i = 0; i < dp && i < 5; i++) {
            Serial.print("["); Serial.print(i); Serial.print("]=");
            Serial.print(ds[i].r, 6); Serial.print(" ");
          }
          Serial.println();
        }
#endif
      }
    }
#if LOG_FLASH_STORCL
    else {
      if (Serial) {
        Serial.print("[FLASH_STORCL] ERROR: slot ");
        Serial.print(tmp);
        Serial.println(" out of range (>= MEMNR)");
      }
    }
#endif
  }
}
static void _mrcl(void) { // MRCL
  // log RCL - conditional debugging
#if LOG_FLASH_STORCL
    if (Serial) {
      Serial.println(">>> RCL CALLED <<<");
      Serial.print("  mp="); Serial.print(mp);
      Serial.print(" dp="); Serial.print(dp);
      Serial.print(" isnewnumber="); Serial.println(isnewnumber);
      if (dp > 0) {
        Serial.print("  TOS before="); Serial.println(ds[dp-1].r, 6);
      }
      Serial.flush(); // Force output immediately
      delay(10); // Allow serial to transmit
    }
#endif
  
#if LOG_FLASH_STORCL
  if (Serial) {
    Serial.print("[FLASH_RCL] WRAPPER ENTRY isnewnumber="); Serial.print(isnewnumber);
    Serial.print(" dp="); Serial.print(dp);
    Serial.print(" mp="); Serial.print(mp);
    Serial.print(" STACK_BEFORE: ");
    for (byte i = 0; i < dp && i < 5; i++) {
      Serial.print("ds["); Serial.print(i); Serial.print("]=");
      Serial.print(ds[i].r, 6); Serial.print(" ");
    }
    Serial.println();
  }
#endif
  boolean saved_isnewnumber = isnewnumber; // Preserve state like RAM version
  _mstorcl(false);
  isnewnumber = saved_isnewnumber; // Restore state
  
  // log RCL exit - conditional debugging
#if LOG_FLASH_STORCL
  if (Serial) {
    Serial.println("<<< RCL DONE >>>");
    Serial.print("  dp="); Serial.print(dp);
    Serial.print(" isnewnumber="); Serial.println(isnewnumber);
    if (dp > 0) {
      Serial.print("  TOS after="); Serial.println(ds[dp-1].r, 6);
    }
    Serial.flush(); // Force output immediately
    delay(10); // Allow serial to transmit
  }
#endif
  
#if LOG_FLASH_STORCL
  if (Serial) {
    Serial.print("[FLASH_RCL] WRAPPER EXIT isnewnumber="); Serial.print(isnewnumber);
    Serial.print(" (restored) dp="); Serial.print(dp);
    Serial.print(" STACK_AFTER: ");
    for (byte i = 0; i < dp && i < 5; i++) {
      Serial.print("ds["); Serial.print(i); Serial.print("]=");
      Serial.print(ds[i].r, 6); Serial.print(" ");
    }
    Serial.println();
  }
#endif
}
static void _msto(void) { // MSTO
#if LOG_FLASH_STORCL
  if (Serial) {
    Serial.print("[FLASH_STO] WRAPPER ENTRY isnewnumber="); Serial.print(isnewnumber);
    Serial.print(" dp="); Serial.print(dp);
    Serial.print(" mp="); Serial.println(mp);
  }
#endif
  boolean saved_isnewnumber = isnewnumber; // Preserve state like RAM version
  _mstorcl(true);
  isnewnumber = saved_isnewnumber; // Restore state
#if LOG_FLASH_STORCL
  if (Serial) {
    Serial.print("[FLASH_STO] WRAPPER EXIT isnewnumber="); Serial.print(isnewnumber);
    Serial.print(" (restored) dp="); Serial.println(dp);
  }
#endif
}
static void _mstorcl_ram(boolean issto) { // RAM store functions
#if LOG_RAM_STORCL
  if (Serial) {
    Serial.print("[RAM_STORCL] ENTRY op=");
    Serial.print(issto ? "STO" : "RCL");
    Serial.print(" dp="); Serial.print(dp);
    Serial.print(" isnewnumber="); Serial.print(isnewnumber);
    Serial.print(" mp="); Serial.print(mp);
    Serial.println();
  }
#endif

  // Initialize RAM memory if not already done
  if (!ram_mem_initialized) {
    for (byte i = 0; i < RAMMEMNR; i++) {
      ram_mem[i] = C0; // Initialize all slots to zero
    }
    ram_mem_initialized = 1;
  }

  if (!dp) return; // Need at least the slot designator on stack

  // Pop slot designator first (same ordering as STO/RCL)
  struct data slotData = dpop();
#if LOG_RAM_STORCL
  if (Serial) {
    Serial.print("[RAM_STORCL] After slot pop: dp="); Serial.print(dp);
    Serial.print(" slot_r="); Serial.print(slotData.r);
    Serial.print(" slot_b="); Serial.println((double)slotData.b);
  }
#endif
  
  double slotRaw = slotData.r;
  if (base && slotData.b != 0) {
    slotRaw = slotData.b / 100.0; // Convert business-form slot entry
  }
  if (slotRaw < 0.0 || slotRaw >= RAMMEMNR) return;
  byte slot = static_cast<byte>(slotRaw);

  if (issto) {
    if (!dp) return; // No value available to store
    if (base) {
      int64_t value = dpopb();
#if LOG_RAM_STORCL
      if (Serial) {
        Serial.print("[RAM_STORCL] STO value="); Serial.print((double)value);
        Serial.print(" to slot="); Serial.print(slot);
        Serial.print(" dp_after="); Serial.println(dp);
      }
#endif
      ram_mem[slot].r = value / 100.0;
      ram_mem[slot].i = 0.0;
      ram_mem[slot].b = value;
    }
    else {
      struct data value = dpop();
#if LOG_RAM_STORCL
      if (Serial) {
        Serial.print("[RAM_STORCL] STO value_r="); Serial.print(value.r);
        Serial.print(" value_i="); Serial.print(value.i);
        Serial.print(" to slot="); Serial.print(slot);
        Serial.print(" dp_after="); Serial.println(dp);
      }
#endif
      ram_mem[slot] = value;
    }
  }
  else {
    if (base) {
      dpushb(ram_mem[slot].b);
#if LOG_RAM_STORCL
      if (Serial) {
        Serial.print("[RAM_STORCL] RCL slot="); Serial.print(slot);
        Serial.print(" value="); Serial.print((double)ram_mem[slot].b);
        Serial.print(" dp_after="); Serial.println(dp);
      }
#endif
    }
    else {
      dpush(ram_mem[slot]);
#if LOG_RAM_STORCL
      if (Serial) {
        Serial.print("[RAM_STORCL] RCL slot="); Serial.print(slot);
        Serial.print(" value_r="); Serial.print(ram_mem[slot].r);
        Serial.print(" value_i="); Serial.print(ram_mem[slot].i);
        Serial.print(" dp_after="); Serial.println(dp);
      }
#endif
    }
  }
  
#if LOG_RAM_STORCL
  if (Serial) {
    Serial.print("[RAM_STORCL] EXIT dp="); Serial.print(dp);
    Serial.print(" isnewnumber="); Serial.println(isnewnumber);
  }
#endif
}

static void _sto_ram(void) { // STO-RAM wrapper function
#if LOG_RAM_STORCL
  if (Serial) {
    Serial.print("[STO_RAM] WRAPPER ENTRY isnewnumber="); Serial.print(isnewnumber);
    Serial.print(" dp="); Serial.println(dp);
  }
#endif
  boolean saved_isnewnumber = isnewnumber; // Preserve state to prevent stack duplication bug
  _mstorcl_ram(true);
  isnewnumber = saved_isnewnumber; // Restore state
#if LOG_RAM_STORCL
  if (Serial) {
    Serial.print("[STO_RAM] WRAPPER EXIT isnewnumber="); Serial.print(isnewnumber);
    Serial.print(" (restored) dp="); Serial.println(dp);
  }
#endif
}

static void _rcl_ram(void) { // RCL-RAM wrapper function
#if LOG_RAM_STORCL
  if (Serial) {
    Serial.print("[RCL_RAM] WRAPPER ENTRY isnewnumber="); Serial.print(isnewnumber);
    Serial.print(" dp="); Serial.println(dp);
  }
#endif
  boolean saved_isnewnumber = isnewnumber; // Preserve state to prevent stack duplication bug
  _mstorcl_ram(false);
  isnewnumber = saved_isnewnumber; // Restore state
#if LOG_RAM_STORCL
  if (Serial) {
    Serial.print("[RCL_RAM] WRAPPER EXIT isnewnumber="); Serial.print(isnewnumber);
    Serial.print(" (restored) dp="); Serial.println(dp);
  }
#endif
}
static void _mul(void) { // MULT * (a+i*b)*(c+i*d)=(a*c-b*d)+i*(b*c+a*d)
  struct data b = dpop(), a = dpop();
  double realPart = a.r * b.r - a.i * b.i;
  double imagPart = a.r * b.i + a.i * b.r;

  if (realPart != realPart || imagPart != imagPart) { // NaN check
    msgnr = MSGOVERFLOW;
    return;
  }

  double realExponent = (realPart == 0.0) ? -1e6 : log10(_abs(realPart));
  double imagExponent = (imagPart == 0.0) ? -1e6 : log10(_abs(imagPart));

  if (realExponent > OVERFLOW || imagExponent > OVERFLOW) {
    msgnr = MSGOVERFLOW;
    return;
  }

  dpush({realPart, imagPart, (a.b * b.b) / 100LL});
}
static void _nand(void) { // NAND
  long b = dpopr();
  dpushr(~((long)dpopr() & b));
}
static void _nd(void) { // ND
  seekmem(_ND);
}
static void _neg(void) { // NEGATE
  struct data a = dpop();
  dpush({ -a.r, -a.i, -a.b});
}
static void _numinput(byte k) { // NUM Numeric input (0...9)
  if (isAF) k += 10;
  if (isdot) { // Append decimal
    if (base == 10) {
      int64_t a = dpopb();
      dpushb(a / 100LL * 100LL + a % 10LL * 10LL + k);
    }
    else dpushr(dpopr() + k / pow10(++decimals));
  }
  else if (isnewnumber) dpush({static_cast<double>(k),0.0,static_cast<int64_t>(static_cast<double>(k) * 100)}); // Push new numeral 
  //// cast double → int64_t
  else if (base) dpushb(((dpopb() / 100) * base + k) * 100); // Append numeral
  else dpushr(dpopr() * 10 + k); // Append numeral
  isnewnumber = isAF = false;
}
static void _offset(void) { // OFF Time Setting
  darktime = dpopr();
  limitdarktime();
  EEPROM.putDarktime(darktime); // Wear-leveling write
}
static void _over(void) { // OVER
  seekmem(_OVER);
}
static void _pause(void) { // PAUSE
  isprintscreen = true;
  pause = dpopr();
}
static void _peek(void) { // @ PEEK (address peek)
  int addr = dpopr();
  //if (addr < EEUEND - EEUSTART) dpushr(EEPROM[EEUSTART + addr]);
  if (addr < EEUEND - EEUSTART) { 
    byte value; // temporary variable to read EEPROM
    EEPROM.get(EEUSTART + addr, value);
    dpushr(value);
  }
}
static void _percent(void) { // PERCENT
  seekmem(_PERCENT);
}
static void _percentdelta(void) { // PERCENTCHANGE
  seekmem(_PERCENTDELTA);
}
static void _perm(void) { // PERM nPr=n!/(n-r)!
  byte a = dpopr(), b = dpopr();
  a = b - a;
  dpushr(1.0);
  for (byte i = a; i < b; i++) dpushr(dpopr() * (i + 1));
}
static void _pi(void) { // PI
  dpushr(PI);
}
static void _pick(void) { // PICK
  byte n = dpopr();
  if (n >= 1 && n <= dp) dpush(ds[dp - n]);
}
void _recpol() { // Toggle RECtangular <-> POLar view of imaginary numbers
  if (!base) {
    Serial.print("[R->P] BEFORE toggle: ispolar="); Serial.print(ispolar);
    if (dp > 0) {
      Serial.print(" TOS: r="); Serial.print(ds[dp-1].r, 8);
      Serial.print(" i="); Serial.println(ds[dp-1].i, 8);
    }
    
    ispolar = ispolar ? false : true;
    
    Serial.print("[R->P] AFTER toggle: ispolar="); Serial.print(ispolar);
    if (dp > 0) {
      Serial.print(" TOS: r="); Serial.print(ds[dp-1].r, 8);
      Serial.print(" i="); Serial.println(ds[dp-1].i, 8);
    }
  }
}
static void _poke(void) { // ! POKE (value address poke)
  int addr = dpopr();
  //if (addr < EEUEND - EEUSTART) EEPROM[EEUSTART + addr] = (byte)dpopr();
  if (addr < EEUEND - EEUSTART) {
    byte value; // temporary variable to read EEPROM
    EEPROM.get(EEUSTART + addr, value);
    (byte)dpopr();
  }
}

void _pow(void) { // POWER
  seekmem(_POW);
}
static void _prgselect(void) { // PRG SELECT
  if (!base) {
    prgselect = 0;
    isprgselect = true;
  }
}
static void _pv(void) { // PV
  seekmem(_PV);
}
void _pwr10(void) { // POWER10
  seekmem(_PWR10);
}
static void _qe(void) { // QE
  seekmem(_QE);
}
static void _rot(void) { // ROT
  if (Serial) {
    Serial.print("[ROT] invoked with dp=");
    Serial.println(dp);
  }

  if (dp < 2) {
    if (Serial) Serial.println("[ROT] skipped (stack has fewer than 2 levels)");
    return;
  }

  if (Serial) {
    Serial.print("[ROT] Before (bottom->top): ");
    for (byte i = 0; i < dp; ++i) {
      Serial.print(ds[i].r);
      if (i < dp - 1) Serial.print(", ");
    }
    Serial.println();
  }

  struct data bottom = ds[0];
  memmove(&ds[0], &ds[1], (dp - 1) * sizeof(struct data));
  ds[dp - 1] = bottom;

  if (Serial) {
    Serial.print("[ROT] After  (bottom->top): ");
    for (byte i = 0; i < dp; ++i) {
      Serial.print(ds[i].r);
      if (i < dp - 1) Serial.print(", ");
    }
    Serial.println();
  }
}
static void _sin(void) { // SIN sin(a+i*b)=sin(a)*cosh(b)+i*cos(a)*sinh(b)
  if (isreal()) dpushr(sin(dpopr() / RAD));
  else {
    struct data a = dpop();
    double e = texp(a.i);
    dpush({sin(a.r) * (e + 1.0 / e) / 2.0, sin(PI / 2 - a.r) * (e - 1.0 / e) / 2.0, 0LL});
  }
}
static void _sinh(void) { // SINH
  seekmem(_SINH);
}
static void _sqrt(void) { // SQRT
  seekmem(_SQRT);
}
static void _sub(void) { // SUB - a-b=a+(-b)
  if (base) {
    int64_t b = dpopb();
    dpushb(dpopb() - b);
  }
  else {
    _neg(); _add();
  }
}
static void _sumadd(void) { // SUM+
  if (!base) seekmem(_SUM);
}
static void _sumclear(void) { // SUM CLEAR
  if (!base) seekmem(_CLRSUM);
}
static void _sumstat(void) { // SUM STAT
  seekmem(_STAT);
}
static void _sumsub(void) { // SUM-
  seekmem(_SUMSUB);
}
static void _sumlr(void) { // SUM L.R.
  seekmem(_LR);
}
static void _swap(void) { // SWAP
  if (dp > 1) {
    struct data a = dpop(), b = dpop();
    dpush(a); dpush(b);
  }
}
static void _tan(void) { // TAN
  seekmem(_TAN);
}
static void _tanh(void) { // TANH
  seekmem(_TANH);
}
static void _torch(void) { // TORCH
  istorch = true;
}
static void _until(void) { // UNTIL
  if (!ap) msgnr = MSGNEST; // No BEGIN for this UNTIL
  else if (dpopr()) apop(); // Go on (delete return address)
  else apush(mp = apop()); // Go back to BEGIN
}
static void _usrset(void) { // USR
  if (!base) {
    sel = 0; isdict = issetusr = true;
  }
}

// SUBROUTINES
static void limitdarktime(void) { // Limit darktime to DARKTIMEMIN
  darktime = max(darktime, DARKTIMEMIN);
}

static void eepromMove(int from, int to, int length) { // Move EEPROM bytes handling overlap 
  if (length <= 0 || from == to) return;
  if (to > from) {
    for (int offset = length - 1; offset >= 0; --offset) {
      byte value;
      EEPROM.get(from + offset, value);
      EEPROM.put(to + offset, value);
    }
  }
  else {
    for (int offset = 0; offset < length; ++offset) {
      byte value;
      EEPROM.get(from + offset, value);
      EEPROM.put(to + offset, value);
    }
  }
}

static double texp(double f) { // Calculate exp 
  return exp(f); // Use standard library exp() instead
}


static double _pow10(int8_t e) { // Calculates 10 raised to the power of e
  double f = 1.0;
  if (e > 0) while (e--) f *= 10.0;
  else while (e++) f /= 10.0;
  return (f);
}

static boolean bothzero(double a, double(b)) { // True, if both arguments = 0.0
  return (a == 0.0 && b == 0.0);
}

static double absolute(double a, double b) { // Returns the absolute value of a complex number
  a = _abs(a); b = _abs(b);
  if (a == 0.0) return (b);
  else if (b == 0.0) return (a);
  return (texp(0.5 * (log(a) + log(a + b / a * b)))); // Prevent big arguments
}

static double angle(double a, double b) { // Returns the angle of a complex number
  double tmp = atan(b / a) * RAD;
  if (a == 0.0) return ((b < 0.0) ? -90.0 : 90.0); // Special case imaginary
  else if (a < 0.0) {
    return ((b < 0.0) ? (tmp - 180.0) : (tmp + 180.0)); // Quadrant 2 or quadrant 3
  }
  else return (tmp); // Quadrant 1 or 4
}

static boolean isreal(void) { // True if TOS is real
  return (dp ? ds[dp - 1].i == 0.0 : true);
}

static void prgstepins(byte c) { // Insert step (character c in prgbuf at prgeditstart)
  Serial.print("prgstepins: c="); Serial.print(c);
  Serial.print(" prgeditstart="); Serial.print(prgeditstart);
  Serial.print(" prgbuflen="); Serial.print(prgbuflen);
  Serial.print(" isprgeditfirst="); Serial.println(isprgeditfirst);
  
  if (prgbuflen < PRGSIZEMAX) {
    // Safety check: ensure prgeditstart is within valid bounds
    if (prgeditstart > prgbuflen) {
      Serial.print("prgstepins ERROR: prgeditstart out of bounds, resetting to 0");
      prgeditstart = 0;
      isprgeditfirst = true;
    }
    
    for (byte i = prgbuflen; i > prgeditstart; i--) prgbuf[i] = prgbuf[i - 1];
    if (isprgeditfirst) prgbuf[prgeditstart] = c; // Don't increment prgeditstart on first insert - stay at position 0 to show the inserted command
    else {
      prgbuf[prgeditstart + 1] = c;
      prgeditstart++;
    }
    prgbuflen++;
    isprgeditfirst = false; // After first insert, subsequent inserts go after current position
    fgm = 0;
    
    Serial.print("prgstepins AFTER: prgbuflen="); Serial.print(prgbuflen);
    Serial.print(" prgeditstart="); Serial.print(prgeditstart);
    Serial.print(" prgbuf[0]="); Serial.println(prgbuf[0]);
  } else {
    Serial.println("prgstepins ERROR: Program buffer overflow prevented");
  }
}

static void execute(byte command) { // Execute command
#if LOG_RAM_STORCL
  if (Serial && (command == _STO_RAM || command == _RCL_RAM)) {
    Serial.print("[EXECUTE] PRE-DISPATCH cmd=");
    Serial.print(command == _STO_RAM ? "STO_RAM" : "RCL_RAM");
    Serial.print(" dp="); Serial.print(dp);
    Serial.print(" isnewnumber="); Serial.print(isnewnumber);
    Serial.print(" mp="); Serial.println(mp);
  }
#endif
#if LOG_FLASH_STORCL
  if (Serial && (command == _RCL || command == _STO)) {
    Serial.print("[EXECUTE] PRE-DISPATCH cmd=");
    Serial.print(command == _STO ? "STO(FLASH)" : "RCL(FLASH)");
    Serial.print(" dp="); Serial.print(dp);
    Serial.print(" isnewnumber="); Serial.print(isnewnumber);
    Serial.print(" mp="); Serial.println(mp);
  }
#endif

  if (command < MAXCMDB) (*dispatch[command])(); // Dispatch intrinsic/builtin command
  else if (command < MAXCMDU)
    mp = (seekusr(command) + PRGNAMEMAX) - EEUSTART + sizeof(mem); // Execute user command
  
  if (!isAF && command > 10 && command != 12) { // New number - except: 0-9.A-F CE
    bool was_isdot = isdot;
    byte was_decimals = decimals;
    decimals = 0; isdot = false;
#if LOG_INPUT_PRINTING
    Serial.print("[PRINT_DEBUG] execute: Resetting comma state for command ");
    Serial.print(command);
    Serial.print(" (was isdot="); Serial.print(was_isdot);
    Serial.print(", decimals="); Serial.print(was_decimals);
    Serial.println(")");
#endif
#if LOG_RAM_STORCL
    if (Serial && (command == _STO_RAM || command == _RCL_RAM)) {
      Serial.print("[EXECUTE] Setting isnewnumber=true (was ");
      Serial.print(isnewnumber);
      Serial.println(")");
    }
#endif
#if LOG_FLASH_STORCL
    if (Serial && (command == _RCL || command == _STO)) {
      Serial.print("[EXECUTE] Setting isnewnumber=true for FLASH cmd (was ");
      Serial.print(isnewnumber);
      Serial.println(")");
    }
#endif
    isnewnumber = true;
  }
  
#if LOG_RAM_STORCL
  if (Serial && (command == _STO_RAM || command == _RCL_RAM)) {
    Serial.print("[EXECUTE] POST-DISPATCH isnewnumber="); Serial.print(isnewnumber);
    Serial.print(" dp="); Serial.println(dp);
  }
#endif
#if LOG_FLASH_STORCL
  if (Serial && (command == _RCL || command == _STO)) {
    Serial.print("[EXECUTE] POST-DISPATCH FLASH isnewnumber="); Serial.print(isnewnumber);
    Serial.print(" dp="); Serial.println(dp);
  }
#endif
  
  if (fgm && setfgm) fgm = setfgm = 0; // Hold demanded f-key status one cycle
  setfgm = 1;
}

static void floatstack() {
  memcpy(ds, &ds[1], (DATASTACKSIZE - 1) * sizeof(struct data));
  dp--;
  isfloated = true;
}

static void dpush(struct data c) { // Push complex number to data-stack
  if (dp >= DATASTACKSIZE) floatstack(); // Float stack
  ds[dp++] = c;
#if LOG_RAM_STORCL
  if (Serial && mp) { // Only log during program execution
    Serial.print("[DPUSH] dp="); Serial.print(dp);
    Serial.print(" r="); Serial.print(c.r);
    Serial.print(" i="); Serial.print(c.i);
    Serial.print(" b="); Serial.print((double)c.b);
    Serial.print(" mp="); Serial.println(mp);
  }
#endif
}
static double dpushr(double f) { // Push real number to data-stack
  dpush({f, '\0', '\0'});
  return (f);
}
static void dpushb(int64_t n) { // Push business number to data-stack
  dpush({'\0', '\0', n});
}

static struct data dpop(void) { // Pop value from data-stack
  struct data result = (dp ? ds[--dp] : C0);
#if LOG_RAM_STORCL
  if (Serial && mp) { // Only log during program execution
    Serial.print("[DPOP] dp="); Serial.print(dp);
    Serial.print(" r="); Serial.print(result.r);
    Serial.print(" i="); Serial.print(result.i);
    Serial.print(" b="); Serial.print((double)result.b);
    Serial.print(" mp="); Serial.println(mp);
  }
#endif
  return result;
}
static float dpopr(void) { // Pop real number from data-stack
  return (dp ? ds[--dp].r : '\0');
}
static double dpoprd(void) { // Pop real number from data-stack (double precision for integration)
  return (dp ? ds[--dp].r : 0.0);
}
static int64_t dpopb(void) { // Pop base number from data-stack
  return (dp ? ds[--dp].b : '\0');
}

static void apush(int addr) { // Push address (int) to address-stack
  if (ap >= ADDRSTACKSIZE) msgnr = MSGNEST; else as[ap++] = addr;
}
static int apop(void) { // Pop address (int) from address-stack
  return (ap ? as[--ap] : '\0'); 
}

static void B2stack (void) { // Copy business stack to stack
  for (byte i = 0; i < dp; i++)  ds[i].r = ds[i].b / 100.0;
}
static void stack2B (void) { // Copy stack to business stack
  for (byte i = 0; i < dp; i++)
    ds[i].b = (ds[i].r * 1000LL + 5LL) / 10LL; // Includes rounding
}

static int seekusr(byte n) { // Find run-address (ptr) of n-th function 
    byte original = n;
    int ptr = EEUSTART;
    int bytesScanned = 0;
    while (n >= MAXCMDB) {
        byte value;
        EEPROM.get(ptr++, value);
        bytesScanned++;
        if (value == _END) {
            n--;
        }
    }
#if LOG_SEEKUSR
    if (Serial) {
        byte sentinel = 0;
        if (ptr > EEUSTART) EEPROM.get(ptr - 1, sentinel);
        char name[PRGNAMEMAX + 1];
        for (byte i = 0; i < PRGNAMEMAX; i++) {
            byte ch;
            EEPROM.get(ptr + i, ch);
            name[i] = (ch >= 32 && ch <= 126) ? ch : '.';
        }
        name[PRGNAMEMAX] = '\0';
        Serial.print("[seekusr] cmd="); Serial.print(original);
        Serial.print(" scans="); Serial.print(bytesScanned);
        Serial.print(" ptr=0x"); Serial.print(ptr, HEX);
        Serial.print(" prev=0x"); Serial.print(sentinel, HEX);
        Serial.print(" name="); Serial.println(name);
    }
#endif
    return ptr;
}

static int seekusrp(int8_t n) { // Seekusr for prgselect+MAXCMDB + n
  return (seekusr(prgselect + MAXCMDB + n));
}

static void seekmem(byte n) { // Find run-address (mp) of n-th builtin function
    bool hadActiveProgram = (mp != 0);
    bool wasBuiltinFrame = hadActiveProgram && (mp < sizeof(mem));
    bool wasUserFrame = hadActiveProgram && !wasBuiltinFrame;

    if (hadActiveProgram) {
        apush(mp); // Preserve current execution point (user or builtin)
#if LOG_SEEKMEM
        if (Serial) {
            Serial.print("[SEEKMEM] Saved return mp="); Serial.print(mp);
            Serial.print(" (" ); Serial.print(wasBuiltinFrame ? "builtin" : "user");
            Serial.print(") ap="); Serial.println(ap);
            Serial.flush();
        }
#endif
    }

#if LOG_SEEKMEM
    if (Serial) {
        Serial.print("[SEEKMEM] entry n="); Serial.print(n);
        Serial.print(" active="); Serial.print(hadActiveProgram);
        Serial.print(" builtinFrame="); Serial.print(wasBuiltinFrame);
        Serial.print(" userFrame="); Serial.print(wasUserFrame);
        Serial.print(" ap="); Serial.println(ap);
    }
#endif

    mp = 0;
    while (n + 1 - MAXCMDI) {
        if (mem[mp++] == _END)
            n--;
    }
#if LOG_SEEKMEM
    if (Serial) {
        Serial.print("[SEEKMEM] Set mp to builtin="); Serial.println(mp);
        Serial.flush();
    }
#endif
}

#if LOG_PRG_MOVE
static void logProgramNameAt(const char* label, int nameAddr) {
  if (!Serial) return;
  char nameBuf[PRGNAMEMAX + 1];
  for (byte i = 0; i < PRGNAMEMAX; ++i) {
    byte c = 0;
    if (nameAddr + i >= EEUSTART && nameAddr + i < EEUEND) EEPROM.get(nameAddr + i, c);
    nameBuf[i] = (c >= 32 && c <= 126) ? c : '.';
  }
  nameBuf[PRGNAMEMAX] = '\0';
  byte sentinel = 0;
  if (nameAddr - 1 >= EEUSTART) EEPROM.get(nameAddr - 1, sentinel);
  Serial.print(F("[PRGMOVE] "));
  Serial.print(label);
  Serial.print(F(" start=0x"));
  Serial.print(nameAddr, HEX);
  Serial.print(F(" sentinel=0x"));
  Serial.print(sentinel, HEX);
  Serial.print(F(" name='"));
  Serial.print(nameBuf);
  Serial.println('\'');
}
#endif

static byte movebuf[2 * (PRGNAMEMAX + PRGSIZEMAX + 2)];

static void moveprgup(void) { // Move program in EEPROM up
  int prevStart = seekusrp(-1); // Name start of previous program
  int currStart = seekusrp(0);  // Name start of current program
  int nextStart = seekusrp(1);  // Name start of next program (after current)
  int prevLen = currStart - prevStart; // Length of previous program (name + data + _END)
  int currLen = nextStart - currStart; // Length of current program (name + data + _END)
  int totalLen = prevLen + currLen; // Total length of both programs
#if LOG_PRG_MOVE
  if (Serial) {
    Serial.print(F("[PRGMOVE] move up: prevLen=")); Serial.print(prevLen);
    Serial.print(F(" currLen=")); Serial.print(currLen);
    Serial.print(F(" totalLen=")); Serial.print(totalLen);
    Serial.print(F(" prevStart=0x")); Serial.print(prevStart, HEX);
    Serial.print(F(" currStart=0x")); Serial.println(currStart, HEX);
    logProgramNameAt("prev-before", prevStart);
    logProgramNameAt("curr-before", currStart);
  }
#endif
  if (currLen <= 0 || prevLen <= 0 || totalLen > (int)sizeof(movebuf)) return;

  // Take a snapshot of the entire affected user area
  const int capacity = userAreaCapacity();
  int usedLen = sou;
  if (usedLen <= 0 || usedLen > capacity) usedLen = capacity;
  if (usedLen < 2) usedLen = 2;

  uint8_t* snapshot = (uint8_t*)malloc(usedLen);
  if (!snapshot) {
    Serial.println("[PRGMOVE] ERROR: insufficient RAM for snapshot.");
    return;
  }
  for (int i = 0; i < usedLen; ++i) {
    EEPROM.get(EEUSTART + i, snapshot[i]);
  }
  
#if LOG_PRG_MOVE
  if (Serial) {
    Serial.println(F("[PRGMOVE] SNAPSHOT CAPTURED:"));
    Serial.print(F("  prevProg raw bytes: "));
    for (int i = 0; i < prevLen && i < 32; ++i) {
      Serial.print(snapshot[prevStart - EEUSTART + i], HEX);
      Serial.print(' ');
    }
    Serial.println();
    Serial.print(F("  currProg raw bytes: "));
    for (int i = 0; i < currLen && i < 32; ++i) {
      Serial.print(snapshot[currStart - EEUSTART + i], HEX);
      Serial.print(' ');
    }
    Serial.println();
  }
#endif

  // Create a rebuilt version with the programs swapped
  uint8_t* rebuilt = (uint8_t*)malloc(capacity);
  if (!rebuilt) {
    Serial.println("[PRGMOVE] ERROR: insufficient RAM for rebuild buffer.");
    free(snapshot);
    return;
  }

  // Copy everything before the two programs
  int writeIdx = 0;
  for (int i = 0; i < prevStart - EEUSTART; ++i) {
    rebuilt[writeIdx++] = snapshot[i];
  }

  // Copy current program (name + data + _END) to previous position
  for (int i = 0; i < currLen; ++i) {
    rebuilt[writeIdx++] = snapshot[currStart - EEUSTART + i];
  }

  // Copy previous program (name + data + _END) to current position
  for (int i = 0; i < prevLen; ++i) {
    rebuilt[writeIdx++] = snapshot[prevStart - EEUSTART + i];
  }

  // Copy everything after the two programs
  int suffixStart = (currStart + currLen) - EEUSTART;
  for (int i = suffixStart; i < usedLen; ++i) {
    if (writeIdx < capacity) {
      rebuilt[writeIdx++] = snapshot[i];
    } else {
      break;
    }
  }

#if LOG_PRG_MOVE
  if (Serial) {
    Serial.print(F("[PRGMOVE] Copied suffix from offset ")); Serial.print(suffixStart);
    Serial.print(F(", writeIdx now=")); Serial.println(writeIdx);
    Serial.print(F("  First 16 bytes of rebuilt image: "));
    for (int i = 0; i < 16 && i < writeIdx; ++i) {
      Serial.print(rebuilt[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
  }
#endif

  // Ensure we have proper terminators
  if (writeIdx < capacity) {
    rebuilt[writeIdx++] = kEndMarkerByte;
    rebuilt[writeIdx++] = kEndMarkerByte;
  }

#if LOG_PRG_MOVE
  if (Serial) {
    Serial.print(F("[PRGMOVE] Final writeIdx=")); Serial.print(writeIdx);
    Serial.print(F(" capacity=")); Serial.println(capacity);
    Serial.println(F("[PRGMOVE] About to call rewriteUserAreaImage"));
  }
#endif

  // Fix command references in rebuilt buffer BEFORE writing
  byte cmd1 = MAXCMDB + prgselect - 1, cmd2 = MAXCMDB + prgselect;
#if LOG_PRG_MOVE
  if (Serial) {
    Serial.print(F("[PRGMOVE] Fixing command refs: cmd1=0x")); Serial.print(cmd1, HEX);
    Serial.print(F(" cmd2=0x")); Serial.println(cmd2, HEX);
  }
#endif
  
  // Fix references within user programs in rebuilt buffer
  for (int i = 0; i < writeIdx; ++i) {
    if (rebuilt[i] == cmd1) {
      rebuilt[i] = cmd2;
#if LOG_PRG_MOVE
      Serial.print(F("  Fixed cmd1→cmd2 at rebuilt[")); Serial.print(i); Serial.println("]");
#endif
    }
    else if (rebuilt[i] == cmd2) {
      rebuilt[i] = cmd1;
#if LOG_PRG_MOVE
      Serial.print(F("  Fixed cmd2→cmd1 at rebuilt[")); Serial.print(i); Serial.println("]");
#endif
    }
  }
  
  // Rewrite the user area with the swapped programs
  if (rewriteUserAreaImage(rebuilt, writeIdx, "move-up", nullptr)) {
#if LOG_PRG_MOVE
    if (Serial) {
      Serial.println(F("[PRGMOVE] Successfully swapped programs via rewrite"));
      Serial.println(F("[PRGMOVE] Reading back from EEPROM to verify:"));
      Serial.print(F("  slot0 (prev pos): "));
      for (int i = 0; i < min(currLen, 32); ++i) {
        byte b;
        EEPROM.get(prevStart + i, b);
        Serial.print(b, HEX);
        Serial.print(' ');
      }
      Serial.println();
      Serial.print(F("  slot1 (curr pos): "));
      for (int i = 0; i < min(prevLen, 32); ++i) {
        byte b;
        EEPROM.get(prevStart + currLen + i, b);
        Serial.print(b, HEX);
        Serial.print(' ');
      }
      Serial.println();
      logProgramNameAt("slot0-after", prevStart);
      logProgramNameAt("slot1-after", prevStart + currLen);
    }
#endif
    
    // Fix menu references (menu is in config area, preserved separately)
    uint8_t menuBuf[MENUITEMS];
    for (int i = 0; i < MENUITEMS; ++i) {
      EEPROM.get(EEMENU + i, menuBuf[i]);
      if (menuBuf[i] == cmd1) menuBuf[i] = cmd2;
      else if (menuBuf[i] == cmd2) menuBuf[i] = cmd1;
    }
    for (int i = 0; i < MENUITEMS; ++i) {
      EEPROM.put(EEMENU + i, menuBuf[i]);
    }
    
    sort(); // Refresh program list
  } else {
    Serial.println("[PRGMOVE] ERROR: Failed to rewrite user area");
  }

  free(snapshot);
  free(rebuilt);
}

static void moveprgtop(void) { // Move selected program directly to the top in PRG mode
  if (!nou || !prgselect) return;

#if LOG_PRG_MOVE
  if (Serial) {
    Serial.print(F("[PRGMOVE] move to top requested for index="));
    Serial.println(prgselect);
  }
#endif

  const int capacity = userAreaCapacity();
  int usedLen = sou;
  if (usedLen <= 0 || usedLen > capacity) usedLen = capacity;
  if (usedLen < 2) usedLen = 2;

  uint8_t* snapshot = (uint8_t*)malloc(usedLen);
  if (!snapshot) {
#if LOG_PRG_MOVE
    if (Serial) Serial.println(F("[PRGMOVE] ERROR: insufficient RAM for snapshot."));
#endif
    return;
  }
  for (int i = 0; i < usedLen; ++i) {
    EEPROM.get(EEUSTART + i, snapshot[i]);
  }

  constexpr byte kMaxUserPrograms = MAXCMDU - MAXCMDB;
  struct ProgramRange {
    int offset;
    int length;
  };
  ProgramRange ranges[kMaxUserPrograms];
#if LOG_PRG_MOVE
  struct NameSpan {
    int start;
    int end;
  };
  NameSpan rebuiltNameSpans[kMaxUserPrograms];
  byte rebuiltNameSpanCount = 0;
#endif
  bool rangeError = false;
  for (byte idx = 0; idx < nou; ++idx) {
    int startAddr = seekusr(MAXCMDB + idx);
    int nextAddr = (idx + 1 < nou) ? seekusr(MAXCMDB + idx + 1) : (EEUSTART + usedLen);
    int offset = startAddr - EEUSTART;
    if (offset < 0 || offset >= usedLen) {
      rangeError = true;
      break;
    }

    int searchLimit = nextAddr - EEUSTART;
    if (searchLimit > usedLen) searchLimit = usedLen;
    if (searchLimit <= offset) {
      rangeError = true;
      break;
    }

    int length = 0;
    bool foundTerminator = false;
    for (int pos = offset; pos < searchLimit; ++pos) {
      ++length;
      if (snapshot[pos] == kEndMarkerByte) {
        foundTerminator = true;
        break;
      }
    }

    if (!foundTerminator) {
#if LOG_PRG_MOVE
      Serial.print(F("[PRGMOVE] ERROR: missing _END terminator while scanning program #"));
      Serial.print(idx);
      Serial.print(F(" offset=0x"));
      Serial.print(startAddr, HEX);
      Serial.print(F(" searchLimit=0x"));
      Serial.println((EEUSTART + searchLimit), HEX);
#endif
      rangeError = true;
      break;
    }

    if (offset + length > usedLen) {
      rangeError = true;
      break;
    }

    ranges[idx].offset = offset;
    ranges[idx].length = length;

#if LOG_PRG_MOVE
    Serial.print(F("[PRGMOVE] range#"));
    Serial.print(idx);
    Serial.print(F(" offset=0x"));
    Serial.print(startAddr, HEX);
    Serial.print(F(" length="));
    Serial.println(length);
#endif
  }
  if (rangeError) {
#if LOG_PRG_MOVE
    if (Serial) Serial.println(F("[PRGMOVE] ERROR: invalid program boundaries detected."));
#endif
    free(snapshot);
    return;
  }

  uint8_t* rebuilt = (uint8_t*)malloc(capacity);
  if (!rebuilt) {
#if LOG_PRG_MOVE
    if (Serial) Serial.println(F("[PRGMOVE] ERROR: insufficient RAM for rebuild buffer."));
#endif
    free(snapshot);
    return;
  }

  byte order[kMaxUserPrograms];
  byte newIndex[kMaxUserPrograms];
  for (byte i = 0; i < nou; ++i) {
    order[i] = i;
    newIndex[i] = i;
  }

  for (byte i = prgselect; i > 0; --i) order[i] = order[i - 1];
  order[0] = prgselect;
  for (byte pos = 0; pos < nou; ++pos) newIndex[order[pos]] = pos;

  auto remapByte = [&](byte value) -> byte {
    if (value >= MAXCMDB && value < MAXCMDB + nou) {
      byte oldIdx = value - MAXCMDB;
      value = MAXCMDB + newIndex[oldIdx];
    }
    return value;
  };

  int writeIdx = 0;
  int prefixLen = ranges[0].offset;
  if (prefixLen > usedLen) prefixLen = usedLen;
  for (int i = 0; i < prefixLen && writeIdx < capacity; ++i) {
    rebuilt[writeIdx++] = remapByte(snapshot[i]);
  }
  for (byte pos = 0; pos < nou && writeIdx < capacity; ++pos) {
    const ProgramRange& range = ranges[order[pos]];
    int programWriteStart = writeIdx;
    for (int i = 0; i < range.length && writeIdx < capacity; ++i) {
      rebuilt[writeIdx++] = remapByte(snapshot[range.offset + i]);
    }
#if LOG_PRG_MOVE
    if (rebuiltNameSpanCount < kMaxUserPrograms) {
      int nameEnd = programWriteStart + PRGNAMEMAX;
      if (nameEnd > writeIdx) nameEnd = writeIdx;
      rebuiltNameSpans[rebuiltNameSpanCount++] = {programWriteStart, nameEnd};
    }
#endif
  }
  int tailOffset = ranges[nou - 1].offset + ranges[nou - 1].length;
  for (int i = tailOffset; i < usedLen && writeIdx < capacity; ++i) {
    rebuilt[writeIdx++] = remapByte(snapshot[i]);
  }

  bool hasDoubleEnd = (writeIdx >= 2 &&
                       rebuilt[writeIdx - 1] == kEndMarkerByte &&
                       rebuilt[writeIdx - 2] == kEndMarkerByte);
  if (!hasDoubleEnd) {
    if (writeIdx < capacity) rebuilt[writeIdx++] = kEndMarkerByte;
    if (writeIdx < capacity) rebuilt[writeIdx++] = kEndMarkerByte;
  }

#if LOG_PRG_MOVE
  if (writeIdx != usedLen) {
    Serial.print(F("[PRGMOVE] rebuilt length mismatch: writeIdx="));
    Serial.print(writeIdx);
    Serial.print(F(" usedLen="));
    Serial.println(usedLen);
  } else {
    Serial.print(F("[PRGMOVE] rebuilt length matches usedLen="));
    Serial.println(usedLen);
  }
  Serial.println(F("[PRGMOVE] Rebuilt program order (names at new offsets):"));
  for (byte i = 0; i < rebuiltNameSpanCount; ++i) {
    Serial.print(F("  prog#"));
    Serial.print(i);
    Serial.print(F(" offset=0x"));
    Serial.print(rebuiltNameSpans[i].start + EEUSTART, HEX);
    Serial.print(F(" name='"));
    for (int pos = rebuiltNameSpans[i].start; pos < rebuiltNameSpans[i].end; ++pos) {
      char ch = (pos < writeIdx) ? rebuilt[pos] : '?';
      if (ch < 32 || ch > 126) ch = '.';
      Serial.print(ch);
    }
    Serial.println("'");
  }
#endif

  bool rewriteOk = rewriteUserAreaImage(rebuilt, writeIdx, "move-top", nullptr);
  if (rewriteOk) {
    for (int i = 0; i < MENUITEMS; ++i) {
      byte entry;
      EEPROM.get(EEMENU + i, entry);
      if (entry >= MAXCMDB && entry < MAXCMDB + nou) {
        byte idx = entry - MAXCMDB;
        EEPROM.put(EEMENU + i, MAXCMDB + newIndex[idx]);
      }
    }
    sort();
    prgselect = 0;
    isprintscreen = true;
  } else {
    msgnr = MSGSAVE;
  }

  free(snapshot);
  free(rebuilt);
}

static void upn(byte n, byte l) { // Selection up l lines
  if (sel > n * l && sel <= (n + 1) * l - 1) sel--; else sel = (n + 1) * l - 1;
}
static void downn(byte n, byte l) { // Selection down l lines
  if (sel >= n * l && sel < (n + 1) * l - 1) sel++; else sel = n * l;
}
static byte menuselect(byte lines) { // Selection (1 line = 16 items)
  memset(dbuf, 0, sizeof(dbuf)); 
  char k = key;
  if (k == KEY1 || k >= KEY2 && k <= KEY4) { // Execute
    if (k == KEY1) {
      k = KEY8; // Cast f-key to key 6
      //delayshort(200); // To prevent early activation of the f-key
      key = oldkey;
    }
    return (sel * 4 + k - '6');
  }
  else if (k == KEY5) upn(0, lines); // # Up0
  else if (k == KEY9) downn(0, lines); // # Down0
  else if (k == KEY6) upn(1, lines); // # Up1
  else if (k == KEY10) downn(1, lines); // # Down1
  else if (k == KEY7) upn(2, lines); // # Up2
  else if (k == KEY11) downn(2, lines); // # Down2
  else if (k == KEY8) upn(3, lines); // # Up3
  else if (k == KEY12) downn(3, lines); // # Down3
    else if (k == KEY14) if (sel > 0) sel--; else sel = 4 * lines - 1; // # Left 
    else if (k == KEY15) if (sel < 4 * lines - 1) sel++; else sel = 0; // # Right 
  else if (k == KEY13) return (NOPRINTNOKEY);
  return (PRINTNOKEY);
}

static void idofusr(void) {
    /*Always stop immediately when double _END is found, even if next bytes are printable.
    Avoid counting any function that appears after _END _END.
    The continue vs break matters — continue could allow reading garbage, break stops the loop 
    immediately.*/
#if LOG_IDOFUSR
    Serial.println("idofusr: Starting program count");
#endif
    int ptr = EEUSTART;
    boolean loop = true;
    byte n = 0;
    byte tmp1 = EEPROM[ptr];
#if LOG_IDOFUSR
    Serial.print("idofusr: Initial ptr="); Serial.print(ptr); Serial.print(" tmp1="); Serial.println(tmp1);
#endif

//while (loop && n < 40 && ptr + 3 < EEUSTART + 8192) {  // do not read past allocated 8KB
while (loop && n < 40 && ptr + 3 < EEUSTART + 32768) {  // do not read past allocated 32KB
    byte tmp2 = EEPROM[++ptr];                         // needs changing when FLASH_PAGE_SIZE changes
#if LOG_IDOFUSR
    Serial.print("idofusr: ptr="); Serial.print(ptr); Serial.print(" tmp1="); Serial.print(tmp1); Serial.print(" tmp2="); Serial.println(tmp2);
#endif
    if (tmp1 == _END) {
        char testName[3];
        testName[0] = EEPROM[ptr + 1];
        testName[1] = EEPROM[ptr + 2];
        testName[2] = EEPROM[ptr + 3];
#if LOG_IDOFUSR
        Serial.print("idofusr: testName="); Serial.print(testName[0]); Serial.print(testName[1]); Serial.println(testName[2]);
#endif

        // check if any of the 3 bytes are printable
        boolean hasValidChar = false;
        for (byte j = 0; j < 3; j++) {
            byte c = (byte)testName[j];
            if (c >= 32 && c <= 126) {
                hasValidChar = true;
                break;
            }
        }
#if LOG_IDOFUSR
        Serial.print("idofusr: hasValidChar="); Serial.println(hasValidChar);
#endif

        if (tmp1 == _END && tmp2 == _END) {  // double END
#if LOG_IDOFUSR
            Serial.println("idofusr: Double END found, stopping");
#endif
            loop = false;
            continue;
        } else if (hasValidChar) {
            n++;
#if LOG_IDOFUSR
            Serial.print("idofusr: Valid program found, n="); Serial.println(n);
#endif
        } else {
#if LOG_IDOFUSR
            Serial.println("idofusr: No valid chars, stopping");
#endif
            loop = false;
        }
    }
    tmp1 = tmp2;
}

    nou = n;
    sou = ptr + 1 - EEUSTART;
#if LOG_IDOFUSR
    Serial.print("idofusr: Final nou="); Serial.print(nou); Serial.print(" sou="); Serial.println(sou);
#endif
}

static boolean isValidPrgName(char *name) {
  // Check if a program name is valid (not gibberish)
  // Valid names should NOT have control characters or non-ASCII

  bool hasMeaningfulChar = false; // At least one useful printable character

  for (byte i = 0; i < PRGNAMEMAX; i++) {
    byte c = (byte)name[i];
    // Check for obviously invalid characters
    if (c == 0) continue;  // NULL is OK as terminator
    if (c < 32 || c > 126) {  // Outside printable ASCII range = garbage
      Serial.print("isValidPrgName: non-printable byte 0x");
      Serial.print(c, HEX);
      Serial.print(" in name '");
      Serial.write(name, PRGNAMEMAX);
      Serial.println("'");
      return false;
    }
    if (isalpha(c) || isdigit(c) || c == '-' || c == '_') {
      hasMeaningfulChar = true; // Accept alnum plus placeholder punctuation
    }
  }

  if (!hasMeaningfulChar) {
    Serial.print("isValidPrgName rejected placeholder name '");
    Serial.write(name, PRGNAMEMAX);
    Serial.println("'");
  }
  return hasMeaningfulChar;
}


static void sort(void) { // Sort cmd[] alphabetically to cmdsort[]
  idofusr(); // Calculate nou first
  char s[2][4]; // Strings to compare
  for (byte i = 0; i < MAXCMDU; i++) cmdsort[i] = i; // Initialize
  for (byte k = MAXCMDB + nou - 1; k > 0; k--) { // Outer loop
    for (byte i = 0; i < k; i++) { // Inner loop
      for (byte m = 0; m < 2; m++) { // Catch 2 strings
        if (cmdsort[i + m] < MAXCMDB) // Intrinsic or builtin command
          //strcpy_P(s[m], (char*)pgm_read_word(&(cmd[cmdsort[i + m]])));
          strcpy(s[m], cmd[cmdsort[i + m]]); 
        /*else { // User command
          int ptr = seekusr(cmdsort[i + m]);
          for (byte j = 0; j < 3; j++) s[m][j] = EEPROM[ptr++];
        }*/
        else if (cmdsort[i + m] >= MAXCMDB && cmdsort[i + m] < MAXCMDB + nou) { 
          int ptr = seekusr(cmdsort[i + m]);
          for (byte j = 0; j < 3 && (ptr + j) < EEUSTART + sou; j++) 
            s[m][j] = EEPROM[ptr + j];
        } else {
          // invalid entry: fill with blanks
          s[m][0] = s[m][1] = s[m][2] = ' ';
        }
      }
      if (strcmp(s[0], s[1]) > 0) { // Compare strings and swap commands
#if LOG_SORT
        if (Serial) {
          Serial.print("[sort] swap i="); Serial.print(i);
          Serial.print(" A="); Serial.print(s[0]);
          Serial.print(" B="); Serial.print(s[1]);
          Serial.print(" ids="); Serial.print(cmdsort[i]); Serial.print('/'); Serial.print(cmdsort[i + 1]);
          if (cmdsort[i] >= MAXCMDB) {
            int addrA = seekusr(cmdsort[i]);
            Serial.print(" @A=0x"); Serial.print(addrA, HEX);
          } else {
            Serial.print(" @A=builtin");
          }
          if (cmdsort[i + 1] >= MAXCMDB) {
            int addrB = seekusr(cmdsort[i + 1]);
            Serial.print(" @B=0x"); Serial.print(addrB, HEX);
          } else {
            Serial.print(" @B=builtin");
          }
          Serial.println();
        }
#endif
        byte tmp = cmdsort[i]; cmdsort[i] = cmdsort[i + 1]; cmdsort[i + 1] = tmp;
      }
    }
  }
}

static void printbuf(boolean shift, byte mh, byte y) { // Print sbuf[]
  byte dotx = 0;
#if LOG_INPUT_PRINTING
  Serial.print("[PRINT_DEBUG] printbuf: sbuf content = '");
  for (byte i = 0; i < DIGITS + 1; i++) {
    Serial.print(sbuf[i]);
  }
  Serial.print("', dotx starts at "); Serial.println(dotx);
#endif
  for (byte i = 0; i < DIGITS + 1; i++) {
#if LOG_INPUT_PRINTING
    Serial.print("[PRINT_DEBUG] printbuf: i="); Serial.print(i);
    Serial.print(", char='"); Serial.print(sbuf[i]);
    Serial.print("', x_pos="); Serial.print(INDICATOR + (FONTWIDTH + 1) * i - dotx);
    Serial.print(", dotx="); Serial.println(dotx);
#endif
    printcat(sbuf[i], FONT4, shift, SIZES, mh, INDICATOR + (FONTWIDTH + 1) * i - dotx, y);
    if (sbuf[i] == '.') {
      dotx = 0;
#if LOG_INPUT_PRINTING
      Serial.println("[PRINT_DEBUG] Found comma, setting dotx=4");
#endif
    }
  }
}

static void printint(int n, boolean bitshift, byte x, byte y) { // Print integer number
  byte e = 1;
  int k = n; while (k /= 10) e++; // Number of digits
  sbuf[e] = '\0'; 
  do sbuf[--e] = _ones(n) + '0'; while (n /= 10); // Fill string
  printsat(sbuf, bitshift, SIZES, SIZES, x, y);
}

static double printsign(double f) { // Common code of printfix and printsci
  memset(sbuf, ' ', sizeof(sbuf)); // Clear string buf (space = blank)
  if (f < 0.0) {
    f = - f;
    sbuf[0] = '-';
  }
  return (f);
}

static void printfix(double f, boolean bitshift, byte mh, byte y) { // Print float with fixed decimals
  long m; // Mantissa
  int8_t e = 1; // Exponent
  byte fix = 6;
  f = printsign(f);
  if (f >= 1.0) e = log10(f) + 1; // Calculate new exponent if (f !< 1)
  double d = (f * pow10(fix - e + 1) + 0.5) / pow10(fix - e + 1); // Rounding
  m = d;
  for (byte i = e; i > 0; i--) {
    sbuf[i] = _ones(m) + '0';
    m /= 10;
  }
  sbuf[e + 1] = '.';
  if ((long)f >= (long)d) d = f; // * Calculate after dot (and suppress trailing zeros)
  m = (d - (long)d) * pow10(fix - e + 1) + 0.5;
  boolean istrail = true;
  byte maxFractionColumns = (DIGITS > (e + 1)) ? (DIGITS - (e + 1)) : 0;
  int8_t availablePrecision = fix - e + 1;
  if (availablePrecision < 0) availablePrecision = 0;
  byte effectiveFractionColumns = min(maxFractionColumns, (byte)availablePrecision);
  byte loopStart = (byte)(e + 1 + effectiveFractionColumns);
  if (loopStart > DIGITS) loopStart = DIGITS;
  for (byte i = loopStart; i > e + 1; i--) {
    byte one = _ones(m);
    if (isprintinput) { // Assign/suppress input line
      if (!istrail || (isnewnumber || i - e - 1 <= decimals) && (!isnewnumber || one != 0)) {
        sbuf[i] = one + '0'; // Assign digit
        istrail = false; // End of trailing zeros
      }
    }
    else if (!istrail || one != 0) { // Suppress digit in non input line
      sbuf[i] = one + '0'; // Assign digit
      istrail = false; // End of trailing zeros
    }
    m /= 10L;
  }
  printbuf(bitshift, mh, y);
}

static void printsci(double f, boolean bitshift, byte mh, byte y) { // Print float with mantissa height (mh) at line y
  long m; // Mantissa
  int8_t e; // Exponent
  f = printsign(f);
  double logf = log10(f);
  e = (int8_t)floor(logf); // Exponent normalized so mantissa starts at 1.x
  m = (f / pow10(e - FIXSCI)) + 0.5; // Mantissa
  if (m > 0 && m < pow10(FIXMANT)) m = (f / pow10(--e - FIXSCI)) + 0.5; // "New" mantissa
  for (byte i = (FIXSCI + 2); i > 0; i--) { // Print mantissa
    if (i == 2) sbuf[i] = '.';
    else {
      sbuf[i] = _ones(m) + '0';
      m /= 10;
    }
  }

  const byte expSignIdx = FIXSCI + 3;
  const byte expTenIdx = expSignIdx + 1;
  const byte expOneIdx = expSignIdx + 2;

  if (e < 0) { // Negative exponent
    if (expSignIdx < sizeof(sbuf)) sbuf[expSignIdx] = '-';
    e = -e;
  }
  else if (expSignIdx < sizeof(sbuf)) {
    sbuf[expSignIdx] = ' ';
  }

  if (expTenIdx < sizeof(sbuf)) sbuf[expTenIdx] = (e >= 10) ? _tens(e) + '0' : '0';
  if (expOneIdx < sizeof(sbuf)) sbuf[expOneIdx] = _ones(e) + '0';

  printbuf(bitshift, mh, y);
}

static void printbase() { // Print TOS for base (10 or other)
  printcat(dp ? dp - 1 + 'a' : 'B', FONT4, false, SIZES, SIZES, 0, 0); // Base indicator
  printint(base, false, (base < 10) ? 60 : 55, 0); // Print base
  int64_t n = 0; 
  if (dp) n = ds[dp - 1].b;
  if (base != 10) n /= 100;
  byte space = 0;
  boolean sign = false;
  if (n < 0) { // Handle sign 
    n = -n;
    sign = true;
  }
  for (byte i = BDIGITS; i > 0; i--) { // Number to string
    byte c = n % base + '0';
    if (c > '9') c += 'A' - '9' - 1;
    sbuf[i - 1] = c;
    n /= base;
  }
  byte trnc = 0, left = (base == 10) ? 3 : 1;
  while (sbuf[trnc] == '0' && trnc < BDIGITS - left) sbuf[trnc++] = ' '; // Truncate zeros
  if (sign) sbuf[trnc - 1] = '-'; // Sign
  if (dp) { // Do not print empty stack
    byte start = 0;
    if (base != 10) start = 4;
    byte siz, y, pos, x;
    for (byte i = start; i < BDIGITS; i++) {
      if (base == 10) { // Printing parameters for base=10
        siz = i > BDIGITS - 3 ? SIZES : SIZEM;
        y = (i < 9) ? 0 : (siz == SIZEM) ? 2 : 3;
        pos = y ? (i - 9) * 5 - 3 : i * 5;
        if (i % 3 == 0) space += 2;
        x = 5 + pos + space - (y ? 5 : 2) - (siz == SIZES ? 1 : 0);
      }
      else { // Printing parameters for base!=10
        siz = SIZEM;
        y = (i < 12) ? 0 : 2;
        pos = y ? (i - 12) * 5 : (i - 3) * 5 + 1;
        if (i % 4 == 0) space += 3;
        x = pos + space;
      }
      printcat(sbuf[i], FONT4, true, SIZES, siz, x, y); // Print
    }
  }
}

static void printnum(double f, boolean bitshift, byte mh, byte y, byte ind) { // Print number
  double a = _abs(f);
  if (dp)
    if (a < ALMOSTZERO || a >= FIXMIN && a < FIXMAX) printfix(f, bitshift, mh, y); // Fixed format
    else printsci(f, bitshift, mh, y); // Sci format
  printcat(ind, FONT4, false, SIZES, SIZES, 0, y); // Indicator
}

static void printxy() { // Print X, Y (2 complex numbers max, each uses 2 lines: real + imaginary)
  byte siz = SIZES; // Use small size for normal display

  if (isdict || ismenu || isprgrename) { // Small, lines 0 and 1
    siz = SIZES;
  }

  // For 4-line complex display: Show max 2 complex numbers or 4 real numbers
  // Each complex number uses 2 lines (real + imaginary)
  
  bool tos_complex = (dp > 0) && (ds[dp - 1].i != 0.0);
  bool nos_complex = (dp > 1) && (ds[dp - 2].i != 0.0);
  bool third_complex = (dp > 2) && (ds[dp - 3].i != 0.0);

#if LOG_COMPLEX
  Serial.print("[COMPLEX_DEBUG] dp="); Serial.print(dp);
  Serial.print(" tos_complex="); Serial.print(tos_complex);
  Serial.print(" nos_complex="); Serial.print(nos_complex);
  Serial.print(" third_complex="); Serial.println(third_complex);
#endif
  if (tos_complex) {
    // TOS is complex: show real (line 3) and imaginary (line 2)
    double realPart = ds[dp - 1].r;
    double imagPart = ds[dp - 1].i;
    byte indReal = dp ? 'a' + dp - 1 : '}';
    byte indImag = ispolar ? '`' : 'i';
    
    if (ispolar) {
      realPart = absolute(ds[dp - 1].r, ds[dp - 1].i);
      imagPart = angle(ds[dp - 1].r, ds[dp - 1].i);
    }
    
    isprintinput = true; printnum(realPart, false, siz, 3, indReal); isprintinput = false;
    printnum(imagPart, false, siz, 2, indImag);
#if LOG_COMPLEX
    Serial.print("[COMPLEX_DEBUG] TOS complex displayed: real="); Serial.print(realPart);
    Serial.print(" imag="); Serial.print(imagPart);
    Serial.print(" indicators: "); Serial.print((char)indReal);
    Serial.print(" "); Serial.println((char)indImag);
#endif
    if (nos_complex && !isprintalpha) {
      // NOS is also complex: show real (line 1) and imaginary (line 0)
      double nos_real = ds[dp - 2].r;
      double nos_imag = ds[dp - 2].i;
      byte nos_indReal = 'a' + dp - 2;
      byte nos_indImag = ispolar ? '`' : 'i';
      
      if (ispolar) {
        nos_real = absolute(ds[dp - 2].r, ds[dp - 2].i);
        nos_imag = angle(ds[dp - 2].r, ds[dp - 2].i);
      }
      
      printnum(nos_real, false, siz, 1, nos_indReal);
      printnum(nos_imag, false, siz, 0, nos_indImag);
#if LOG_COMPLEX
      Serial.print("[COMPLEX_DEBUG] NOS complex displayed: real="); Serial.print(nos_real);
      Serial.print(" imag="); Serial.println(nos_imag);
#endif
    }
    else if (dp > 1 && !isprintalpha) {
      // NOS is real: show on line 1 only
      printnum(ds[dp - 2].r, false, siz, 1, 'a' + dp - 2);
#if LOG_COMPLEX
      Serial.print("[COMPLEX_DEBUG] NOS real displayed: "); Serial.println(ds[dp - 2].r);
#endif
    }
  }
  else if (nos_complex) {
    // TOS is real, but NOS is complex: show TOS on line 3, then NOS complex on lines 1+0
    double tos_real = ds[dp - 1].r;
    byte tos_ind = 'a' + dp - 1;
    
    double nos_real = ds[dp - 2].r;
    double nos_imag = ds[dp - 2].i;
    byte nos_indReal = 'a' + dp - 2;
    byte nos_indImag = ispolar ? '`' : 'i';
    
    if (ispolar) {
      nos_real = absolute(ds[dp - 2].r, ds[dp - 2].i);
      nos_imag = angle(ds[dp - 2].r, ds[dp - 2].i);
    }
    
    isprintinput = true; printnum(tos_real, false, siz, 3, tos_ind); isprintinput = false;
    if (!isprintalpha) {
      printnum(nos_real, false, siz, 1, nos_indReal);
      printnum(nos_imag, false, siz, 0, nos_indImag);
    }
#if LOG_COMPLEX
    Serial.print("[COMPLEX_DEBUG] TOS real ("); Serial.print(tos_real);
    Serial.print(") + NOS complex ("); Serial.print(nos_real);
    Serial.print("+i"); Serial.print(nos_imag); Serial.println(")");
#endif
  }
  else if (third_complex) {
    // TOS and NOS are real, but 3rd is complex: show TOS on line 3, NOS on line 2, then 3rd complex on lines 1+0
    double tos_real = ds[dp - 1].r;
    double nos_real = ds[dp - 2].r;
    double third_real = ds[dp - 3].r;
    double third_imag = ds[dp - 3].i;
    
    byte tos_ind = 'a' + dp - 1;
    byte nos_ind = 'a' + dp - 2;
    byte third_indReal = 'a' + dp - 3;
    byte third_indImag = ispolar ? '`' : 'i';
    
    if (ispolar) {
      third_real = absolute(ds[dp - 3].r, ds[dp - 3].i);
      third_imag = angle(ds[dp - 3].r, ds[dp - 3].i);
    }
    
    isprintinput = true; printnum(tos_real, false, siz, 3, tos_ind); isprintinput = false;
    if (!isprintalpha) {
      printnum(nos_real, false, siz, 2, nos_ind);
      printnum(third_real, false, siz, 1, third_indReal);
      printnum(third_imag, false, siz, 0, third_indImag);
    }
#if LOG_COMPLEX
    Serial.print("[COMPLEX_DEBUG] TOS "); Serial.print(tos_real);
    Serial.print(" + NOS "); Serial.print(nos_real);
    Serial.print(" + 3rd complex ("); Serial.print(third_real);
    Serial.print("+i"); Serial.print(third_imag); Serial.println(")");
#endif
  }
  else {
    // All real - use original 4-line stack display
    double a = dp ? ds[dp - 1].r : 0.0;
    double b = (dp > 1) ? ds[dp - 2].r : 0.0;
    double c = (dp > 2) ? ds[dp - 3].r : 0.0;
    double d = (dp > 3) ? ds[dp - 4].r : 0.0;
    
    // Use time indicators (h,m,s) when clock is active, otherwise use stack level indicators (a,b,c)
    byte ca, cb, cc, cd;
    if (clockState.active && dp >= 3) {
        // Clock mode: show h, m, s indicators
        ca = 's';  // seconds why reverse order? I don't get it. 
        cb = 'm';  // minutes
        cc = 'h';  // hours 
        cd = (dp > 3) ? 'a' + dp - 4 : '-'; // 4th stack level still uses letter indicator
    } else {
        // Normal mode: show stack level indicators
        ca = dp ? 'a' + dp - 1 : '}';
        cb = (dp > 1) ? 'a' + dp - 2 : '-';
        cc = (dp > 2) ? 'a' + dp - 3 : '-';
        cd = (dp > 3) ? 'a' + dp - 4 : '-';
    }
    
    // Check if 4th position is complex
    if (dp > 3 && ds[dp - 4].i != 0.0) { cd = 'i'; d = ds[dp - 4].i; }
    
    isprintinput = true; printnum(a, false, siz, 3, ca); isprintinput = false;
    if (dp > 1 && !isprintalpha) printnum(b, false, siz, 2, cb);
    if (dp > 2 && !isprintalpha) printnum(c, false, siz, 1, cc);
    if (dp > 3 && !isprintalpha) printnum(d, false, siz, 0, cd);
#if LOG_COMPLEX
    Serial.println("[COMPLEX_DEBUG] Real stack mode");
#endif
  }
#if LOG_COMPLEX
  Serial.println("[COMPLEX_DEBUG] printxy() EXIT");
#endif
}

static void makesbuf(int8_t tmp) { // Common code to prepare sbuf 
  for (byte i = 0; i < PRGNAMEMAX; i++) {
      sbuf[i] = EEPROM[prgaddr + i];
  }
  sbuf[PRGNAMEMAX] = '\0'; 
}

static void printfkeys(byte i, byte y) { // Common code to print sbuf at arbitrary line
  byte xpos = i * 16 + i / 2;
  printsat(sbuf, true, SIZES, SIZES, xpos, y);
}

static void printmsg(byte nr) { // Print message
  memset(dbuf, 0, sizeof(dbuf)); // Clear entire screen buffer
  strcpy(sbuf, msg[nr]); 
  printsat(sbuf, true, SIZES, SIZES, 0, 2);
  msgnr = 0;
}

static void printindicator(byte sign) {
  printcat(sign, FONT4, false, SIZES, SIZES, 60, 0); // f/g at top right
}

static boolean printseledit(int addr) { // Similar code of printing prg select and edit 
    for (int i = 0; i < PRGNAMEMAX; i++) {
        sbuf[i] = EEPROM[addr + i];
    }
    sbuf[PRGNAMEMAX] = 0;             // terminate buffer
    return true; // if you need a boolean return
}

static void fillsbuf(int addr) { // Fill sbuf with cmd
  Serial.print("fillsbuf: addr="); Serial.print(addr);
  Serial.print(" cmd[addr]="); Serial.println(cmd[addr]);
  strcat(sbuf, cmd[addr]); 
  Serial.print("fillsbuf: sbuf="); Serial.println(sbuf);
}

static boolean printscreen(void) { // Print screen
    dbuffill(0); 
    clearGraphBuffer();

    if (fgm) printindicator((fgm == ISF) ? '[' : ']'); // f/g
    if (!isnewnumber && !darkscreen) printindicator('_'); // Indicate number input
    if (isfloated) { // Indicate stack flow (data loss)
        printindicator('!');
        isfloated = false;
    }
    if (istorch) dbuffill(0xff);
    else if (darkscreen) {}
    else if (isgetkey) printmsg(MSGASK);
    else if (msgnr) printmsg(msgnr);
    else if (base) printbase(); // Print base mode
    else if (issolve || isint || isplotcalc) printmsg(MSGRUN); // # Print "run" message
    else if (isdict || ismenu) { // # Dict and Menu
        printmsg(isdict ? MSGDICT : ismenusetusr ? MSGKEY : MSGMENU);
        
        // Print cursor on line 1
        printcat('}', FONT4, false, SIZES, SIZES, 0, 1);
        printint(sel + 1, false, (sel + 1 <= 9) ? 60 : 55, 1); // Menu page number on line 1

        for (byte i = 0; i < 4; i++) {
            byte tmp = sel * 4 + i;
           
            if (ismenu) EEPROM.get(EEMENU + tmp, tmp);
            else tmp = cmdsort[tmp];

            memset(sbuf, 0, sizeof(sbuf));

            if (tmp < MAXCMDB) {
                strncpy(sbuf, cmd[tmp], sizeof(sbuf)-1);
                sbuf[sizeof(sbuf)-1] = '\0';
            }
            else if (tmp < MAXCMDB + nou) { // User command
                int ptr = seekusr(tmp);
                memset(sbuf, 0, sizeof(sbuf)); // Clear sbuf before reading
                for (byte j = 0; j < 3; j++) sbuf[j] = EEPROM[ptr++];
                sbuf[3] = '\0';
            }
            else {
                sbuf[0] = ' '; sbuf[1] = '_'; sbuf[2] = '\0'; // correct: null terminator for a char array
            }
            printfkeys(i, 3);
          }

        // Don't call printxy() in menu/dict - it would overwrite menu items
    }
    else if (isprgselect) { // # Select program
        byte nr = prgselect;
        for (byte i = 0; i < MAXLIN; i++) {
            if (nr + i < nou) {
              printint(nr + i + 1, false, 8, i); // Print index
              sbuf[0] = '\0';
    
              // Only read if this program exists
              if ((MAXCMDB + nr + i) < MAXCMDB + nou) {
                int addr = seekusr(MAXCMDB + nr + i);
                memset(sbuf, 0, sizeof(sbuf)); // ensure no leftover data 
                printseledit(addr);
        
                // Validate the program name
                if (!isValidPrgName(sbuf)) {
                  sbuf[0] = ' '; sbuf[1] = '_';
                  sbuf[2] = '\0';
                }
              } else {
                // Empty slot placeholder
                sbuf[0] = ' ';
                sbuf[1] = '_';
                sbuf[2] = '\0';
              }
    
              printsat(sbuf, false, SIZES, SIZES, 22, i);
            }
        }
        printcat('}', FONT4, false, SIZES, SIZES, 0, 0); // Cursor
        printint(nou, false, 50, 0); // Print nou, 999 fits line
        if (sou < 1000) printint(sou, false, 50, 2); // Print sou
        if ((sou >= 1000) && (sou < 10000)) printint(sou, false, 45, 2); // Print sou
        if (sou >= 10000) printint(sou, false, 40, 2); // Print sou

        if (EEU - sou < 1000) printint(EEU - sou, false, 50, 3); //  Print free space for user progs, control via FLASH_PAGE_SIZE in EEPROM.h, was 50,3
        if ((EEU - sou >= 1000) && (EEU - sou < 10000)) printint(EEU - sou, false, 45, 3); // Print sou
        if (EEU - sou >= 10000) printint(EEU - sou, false, 40, 3); // Print sou
       
    }
    else if (isprgrename) { // # Rename program
#if LOG_RENAME
        if (Serial) {
            Serial.print("[RENAME_UI] sel="); Serial.print(sel);
            Serial.print(" renamenr="); Serial.print(renamenr);
            Serial.print(" prgaddr=0x"); Serial.print(prgaddr, HEX);
            Serial.print(" firstChar='");
            Serial.print((char)(sel * 4 + FONTOFFSET));
            Serial.print("' (0x"); Serial.print(sel * 4 + FONTOFFSET, HEX);
            Serial.println(")");
        }
#endif
        printcat('}', FONT4, false, SIZES, SIZES, 0, 1);
        printint(sel + 1, false, (sel + 1 <= 9) ? 60 : 55, 1);

        makesbuf(prgselect);
        printsat(sbuf, true, SIZES, SIZES, 0, 2);

        for (byte i = 0; i < 4; i++) {
            sbuf[0] = sel * 4 + FONTOFFSET + i; sbuf[1] = '\0'; 
            printfkeys(i, 3);
        }
    }
    else if (isprgedit) { // # Edit program
        Serial.print("=== PRGEDIT DISPLAY === prgbuflen="); Serial.print(prgbuflen);
        Serial.print(" prgeditstart="); Serial.println(prgeditstart);
        
        byte stepsX = 55; // Default position keeps indicator visible for 1-digit programs
        const byte digitAdvance = FONTWIDTH + 1;
        if (prgbuflen > 9 && stepsX >= digitAdvance) stepsX -= digitAdvance;   // Shift left for two digits
        if (prgbuflen >= 99 && stepsX >= digitAdvance) stepsX -= digitAdvance; // Shift further for three digits
        printint(prgbuflen, false, stepsX, 0); // Print size of prg without hiding f/g arrow
        for (byte i = 0; i < MAXLIN; i++) {
            byte pi = prgeditstart + i;
            byte cmdpi = prgbuf[pi];
            
            Serial.print("  i="); Serial.print(i);
            Serial.print(" pi="); Serial.print(pi);
            Serial.print(" cmdpi="); Serial.print(cmdpi);
            Serial.print(" prgbuflen="); Serial.println(prgbuflen);

            if (pi < prgbuflen) {
                printint(pi + 1, false, 8, i); // Print size of prg
                sbuf[0] = '\0'; 

                if (cmdpi < MAXCMDB) {
                    Serial.print("    Displaying builtin cmd: "); Serial.print(cmdpi); Serial.print(" ");
                    memset(sbuf, 0, sizeof(sbuf));
                    fillsbuf(cmdpi);
                    Serial.print(" -> sbuf='"); Serial.print(sbuf); Serial.println("'");
                }
                else {
                    int addr = seekusr(cmdpi);
                    printseledit(addr);
                }

                printsat(sbuf, false, SIZES, SIZES, 22, i);
            }

            printcat('{', FONT4, false, SIZES, SIZES, 0, 0);
        }
    }
    else if (isplot) { // Plot
        double ymax = plot[0], ymin = plot[0];
        for (byte i = 0; i < GRAPH_PIXEL_WIDTH; i++) { // Find ymax and ymin
            ymax = max(ymax, plot[i]); ymin = min(ymin, plot[i]);
        }

        double yspan = ymax - ymin;
        bool isFlatLine = fabs(yspan) < 1e-12;
        double yscaleForLog = isFlatLine ? 0.0 : yspan / (GRAPH_PIXEL_HEIGHT - 1);
        auto mapSampleToPixel = [&](double sample) -> int {
            if (isFlatLine) return GRAPH_PIXEL_HEIGHT / 2;
            double relative = (sample - ymin) / (yspan == 0.0 ? 1.0 : yspan);
            if (relative < 0.0) relative = 0.0;
            else if (relative > 1.0) relative = 1.0;
            return (int)lround((GRAPH_PIXEL_HEIGHT - 1) * (1.0 - relative));
        };
#if LOG_PLOT
        Serial.print("[PLOT] render ymin="); Serial.print(ymin, 10);
        Serial.print(" ymax="); Serial.print(ymax, 10);
        Serial.print(" yscale="); Serial.print(yscaleForLog, 10);
        Serial.print(" firstSample="); Serial.print(plot[0], 10);
        Serial.print(" lastSample="); Serial.println(plot[GRAPH_PIXEL_WIDTH - 1], 10);
#endif
        if (plota * plotb <= 0) printvline(-plota * (GRAPH_PIXEL_WIDTH / (plotb - plota))); // Y-axis
        if (ymin * ymax <= 0) printhline(mapSampleToPixel(0.0)); // X-axis

        // Draw smooth lines between consecutive points instead of just plotting pixels
        int prevX = -1, prevY = -1;
        for (byte i = 0; i < GRAPH_PIXEL_WIDTH; i++) {
            int currX = i;
            double relative = isFlatLine ? 0.5 : (plot[i] - ymin) / (yspan == 0.0 ? 1.0 : yspan);
            double unclampedRelative = relative;
            if (relative < 0.0) relative = 0.0;
            else if (relative > 1.0) relative = 1.0;
            int currY = (int)lround((GRAPH_PIXEL_HEIGHT - 1) * (1.0 - relative));
            
#if LOG_PLOT
            if (!isFlatLine && (unclampedRelative < 0.0 || unclampedRelative > 1.0)) {
                Serial.print("[PLOT] clipped sample idx="); Serial.print(i);
                Serial.print(" x="); Serial.print(currX);
                Serial.print(" unclampedNorm="); Serial.print(unclampedRelative, 10);
                Serial.print(" ymin="); Serial.print(ymin, 10);
                Serial.print(" ymax="); Serial.print(ymax, 10);
                Serial.print(" sample="); Serial.println(plot[i], 10);
            }
#endif

            if (prevX == -1) {
                // First point - just plot it
                printpixel(currX, currY);
            } else {
                // Draw line from previous point to current point
                drawLine(prevX, prevY, currX, currY);
            }
            
            prevX = currX;
            prevY = currY;
        }
    }
    else printxy();

    if (isprintalpha) printsat(alpha, false, SIZES, SIZES, 0, 0); // Print alpha anyway

    flush_dbuf_to_oled(); 
    return (NULL); // to determine isprintscreen
}

/*void eraseEEPROM() {
    uint32_t flashBase = eepromFlashBase(); // Gets EEPROM start address
    Serial.print("[EEPROM] erase begin base=0x"); Serial.println(flashBase, HEX);
    if (!kEnableEepromWrites) {
        Serial.print("[EEPROM] writes disabled. Base=0x");
        Serial.println(flashBase, HEX);
        return;
    }
    // 1. Fully erase flash page using NVMC ERASEPAGE
    Serial.println("[EEPROM] page erase start");
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een;
    while (!NRF_NVMC->READY) ;
    NRF_NVMC->ERASEPAGE = flashBase; // Erases the page containing this address
    while (!NRF_NVMC->READY) ;
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
    while (!NRF_NVMC->READY) ;
    Serial.println("[EEPROM] page erase done");
    Serial.println("EEPROM flash page erased (all 0xFF).");
}*/

bool eraseEEPROM() {
    bool ok = EEPROM.beginPageRewrite();
    if (!ok && Serial) {
        Serial.println("[EEPROM] Page rotation skipped (writes disabled)");
    }
    return ok;
}

static int userAreaCapacity() {
    return EEPROM.length() - EEUSTART;
}

static void snapshotConfig(uint8_t* buffer) {
    for (int i = 0; i < EEUSTART; ++i) {
        EEPROM.get(i, buffer[i]);
    }
}

static void restoreConfig(const uint8_t* buffer) {
    // Only write bytes that differ from 0xFF (erased state)
    // This dramatically reduces unnecessary writes to config area
    for (int i = 0; i < EEUSTART; ++i) {
        uint8_t existing;
        EEPROM.get(i, existing);
        
        // Only write if value differs (wear-leveling optimization)
        if (existing != buffer[i]) {
            EEPROM.put(i, buffer[i]);
        }
    }
}

static bool rewriteUserAreaImage(const uint8_t* userData,
                                   int length,
                                   const char* reason,
                                   uint8_t* overrideConfig /*= nullptr*/) {
    if (!userData) return false;
    const int capacity = userAreaCapacity();
    if (length < 0) length = 0;
    if (length > capacity) length = capacity;

    uint8_t defaultConfig[EEUSTART];
    uint8_t* configBuf = overrideConfig ? overrideConfig : defaultConfig;
    if (!overrideConfig) snapshotConfig(configBuf);
    configBuf[EECLEANFLAG] = kUserAreaMagic;

    if (Serial) {
        Serial.print("[EEPROM] rewriteUserAreaImage ");
        Serial.print(reason ? reason : "(unknown)");
        Serial.print(" usedBytes=");
        Serial.print(length);
        Serial.print(" capacity=");
        Serial.println(capacity);
        Serial.print(F("[EEPROM] First 32 bytes of userData buffer: "));
        for (int i = 0; i < 32 && i < length; ++i) {
          Serial.print(userData[i], HEX);
          Serial.print(' ');
        }
        Serial.println();
    }

    if (!kEnableEepromWrites) {
        Serial.println("[EEPROM] rewrite skipped (writes disabled)");
        return false;
    }

    if (!eraseEEPROM()) {
        Serial.println(F("[EEPROM] Rewrite aborted: unable to rotate page"));
        return false;
    }
    restoreConfig(configBuf);
    for (int i = 0; i < capacity; ++i) {
        uint8_t value = (i < length) ? userData[i] : 0xFF;
        EEPROM.put(EEUSTART + i, value);
    }
    
    if (Serial) {
        Serial.println(F("[EEPROM] Write complete, reading back first 32 bytes:"));
        for (int i = 0; i < 32 && i < length; ++i) {
          byte check;
          EEPROM.get(EEUSTART + i, check);
          Serial.print(check, HEX);
          Serial.print(' ');
        }
        Serial.println();
    }
    
    return true;
}

static bool snapshotUserArea(uint8_t** buffer, int* length) {
    if (!buffer || !length) return false;
    const int capacity = userAreaCapacity();
    uint8_t* snapshot = (uint8_t*)malloc(capacity);
    if (!snapshot) return false;
    for (int i = 0; i < capacity; ++i) {
        EEPROM.get(EEUSTART + i, snapshot[i]);
    }
    *buffer = snapshot;
    *length = capacity;
    return true;
}

static bool flashLayoutWarningIssued = false;
static void warnIfConstantSlotOutOfRange(byte slot, const char* region, uint32_t startAddr) {
    if (startAddr + sizeof(double) <= EEUSTART) {
        return;
    }
    if (flashLayoutWarningIssued || !Serial) {
        return;
    }
    flashLayoutWarningIssued = true;
    Serial.print("[FLASH_LAYOUT] slot=");
    Serial.print(slot);
    Serial.print(" region=");
    Serial.print(region ? region : "n/a");
    Serial.print(" range=[0x");
    Serial.print(startAddr, HEX);
    Serial.print(", 0x");
    Serial.print(startAddr + sizeof(double), HEX);
    Serial.print(") exceeds config limit EEUSTART=0x");
    Serial.print(EEUSTART, HEX);
    Serial.println(". Constant memory now overlaps user area.");
    Serial.println("[FLASH_LAYOUT] Increase EEUSTART or move constant slots before rewriting user data.");
}

static bool commitConstantSlot(byte slot, const struct data& value) {
    if (slot >= MEMNR) return false;

    uint8_t configBuf[EEUSTART];
    snapshotConfig(configBuf);
    const uint32_t realAddr = EEMEM + slot * sizeof(double);
    const uint32_t imagAddr = EEMEM + (slot + MEMNR) * sizeof(double);
    warnIfConstantSlotOutOfRange(slot, "real", realAddr);
    warnIfConstantSlotOutOfRange(slot, "imag", imagAddr);
    memcpy(configBuf + realAddr, &value.r, sizeof(double));
    memcpy(configBuf + imagAddr, &value.i, sizeof(double));

    uint8_t* userSnapshot = nullptr;
    int userLength = 0;
    if (!snapshotUserArea(&userSnapshot, &userLength)) return false;

    bool ok = rewriteUserAreaImage(userSnapshot, userLength, "commit-const", configBuf);
    free(userSnapshot);
    return ok;
}

static bool commitBusinessSlot(int64_t businessValue) {
    uint8_t configBuf[EEUSTART];
    snapshotConfig(configBuf);
    memcpy(configBuf + EEMEMB, &businessValue, sizeof(businessValue));

    uint8_t* userSnapshot = nullptr;
    int userLength = 0;
    if (!snapshotUserArea(&userSnapshot, &userLength)) return false;

    bool ok = rewriteUserAreaImage(userSnapshot, userLength, "commit-business", configBuf);
    free(userSnapshot);
    return ok;
}

static bool commitMenuSlot(byte slot, byte cmdId) {
    if (slot >= MENUITEMS) return false;
    uint8_t configBuf[EEUSTART];
    snapshotConfig(configBuf);
    configBuf[EEMENU + slot] = cmdId;

    uint8_t* userSnapshot = nullptr;
    int userLength = 0;
    if (!snapshotUserArea(&userSnapshot, &userLength)) return false;

    bool ok = rewriteUserAreaImage(userSnapshot, userLength, "commit-menu", configBuf);
    free(userSnapshot);
    return ok;
}

static bool ensureBlankUserAreaForNewProgram(const char* reason) {
    if (nou) return false; // only repair when we have no user programs to preserve
    uint8_t header[PRGNAMEMAX + 2];
    for (byte i = 0; i < sizeof(header); ++i) {
        EEPROM.get(EEUSTART + i, header[i]);
    }
    bool needsRepair = (header[0] != kEndMarkerByte) || (header[1] != kEndMarkerByte);
    if (!needsRepair) {
        for (byte i = 2; i < sizeof(header); ++i) {
            if (header[i] != 0xFF) {
                needsRepair = true;
                break;
            }
        }
    }
    if (!needsRepair) return false;
    uint8_t blankUserArea[2] = {kEndMarkerByte, kEndMarkerByte};
    bool rewritten = rewriteUserAreaImage(blankUserArea, sizeof(blankUserArea), reason, nullptr);
    if (rewritten) {
        sort();
    }
    return rewritten;
}

static bool patchUserAreaByte(int eeAddr, uint8_t value, const char* reason) {
    const int capacity = userAreaCapacity();
    int usedLen = sou;
    if (usedLen <= 0 || usedLen > capacity) usedLen = capacity;
    if (usedLen < 2) usedLen = 2;

    uint8_t* snapshot = (uint8_t*)malloc(usedLen);
    if (!snapshot) {
        Serial.println("[EEPROM] patchUserAreaByte: insufficient RAM");
        return false;
    }
    for (int i = 0; i < usedLen; ++i) {
        EEPROM.get(EEUSTART + i, snapshot[i]);
    }

    int offset = eeAddr - EEUSTART;
    if (offset < 0 || offset >= usedLen) {
        free(snapshot);
        Serial.println("[EEPROM] patchUserAreaByte: offset out of range");
        return false;
    }
    snapshot[offset] = value;

    bool ok = rewriteUserAreaImage(snapshot, usedLen, reason, nullptr);
    free(snapshot);
    if (ok) sort();
    return ok;
}

static bool rebuildUserAreaSkippingRange(int removeStart, int removeEnd) {
    if (removeEnd <= removeStart) return false;
    const int capacity = userAreaCapacity();
    int usedLen = sou;
    if (usedLen <= 0 || usedLen > capacity) usedLen = capacity;
    if (usedLen < 2) usedLen = 2;

    uint8_t* snapshot = (uint8_t*)malloc(usedLen);
    if (!snapshot) {
        Serial.println("[PRG_DEL] ERROR: insufficient RAM for snapshot.");
        return false;
    }
    for (int i = 0; i < usedLen; ++i) {
        EEPROM.get(EEUSTART + i, snapshot[i]);
    }

    uint8_t* rebuilt = (uint8_t*)malloc(capacity);
    if (!rebuilt) {
        Serial.println("[PRG_DEL] ERROR: insufficient RAM for compaction buffer.");
        free(snapshot);
        return false;
    }

    int writeIdx = 0;
    for (int i = 0; i < usedLen; ++i) {
        int addr = EEUSTART + i;
        if (addr >= removeStart && addr < removeEnd) continue;
        if (writeIdx < capacity) rebuilt[writeIdx++] = snapshot[i];
        else break;
    }
    free(snapshot);

    if (writeIdx < 2) {
        rebuilt[writeIdx++] = kEndMarkerByte;
        rebuilt[writeIdx++] = kEndMarkerByte;
    }

    while (writeIdx < capacity &&
           (writeIdx < 2 || rebuilt[writeIdx - 1] != kEndMarkerByte ||
            rebuilt[writeIdx - 2] != kEndMarkerByte)) {
        rebuilt[writeIdx++] = kEndMarkerByte;
    }

    bool hasTerminator = (writeIdx >= 2 &&
                          rebuilt[writeIdx - 1] == kEndMarkerByte &&
                          rebuilt[writeIdx - 2] == kEndMarkerByte);
    if (!hasTerminator) {
        Serial.println("[PRG_DEL] ERROR: unable to append double END terminator.");
        free(rebuilt);
        return false;
    }

    bool ok = rewriteUserAreaImage(rebuilt, writeIdx, "delete", nullptr);
    free(rebuilt);
    return ok;
}

void dumpUserHeader() {
  //Serial.println("###Dumping first 16b of user area...");
  Serial.print("EEUSTART header: ");
  for (int i = 0; i < 16; ++i) {
    uint8_t b;
    EEPROM.get(EEUSTART + i, b);
    Serial.printf("0x%02X ", b);
  }
  Serial.println();
}

// Helper function declarations for new wp34s style solver
double secantStep(double a, double b, double fa, double fb);
double riddersStep(double a, double b, double fa, double fb);
std::pair<double, double> adjustConstantCase(double a, double b);

// Global error state management for solver functions (replaces exception handling)
enum class SolveStatus {
    Success,
    EqualFunctionValues,
    InvalidSquareRoot,
    DomainError
};

static SolveStatus solverErrorState = SolveStatus::Success;

void setSolverError(SolveStatus status) {
    solverErrorState = status;
}

SolveStatus getSolverError() {
    return solverErrorState;
}

void clearSolverError() {
    solverErrorState = SolveStatus::Success;
}

/**
 * Hybrid root-finder combining bisection, secant, quadratic interpolation, and Ridder's method
 * 
 * @param a, b Initial guesses (ideally bracketing a root)
 * @param tol Desired absolute tolerance on the root
 * @param maxIter Maximum number of iterations before error
 * @return estimated root
 */
double hybridSolve(double a, double b, double fa, double fb, double tol = 1e-12, int maxIter = 250) {
    clearSolverError();

    if (fa == 0.0) {
        return a;
    }
    if (fb == 0.0) {
        return b;
    }
    
    // Check initial bracketing
    bool bracketed = (fa * fb < 0.0);
    double prevC = 0.0;
    bool hasPrevC = false;

    
    if (Serial) {
      Serial.println("[SOLVE] running...");
    }

    for (int iteration = 1; iteration <= maxIter; ++iteration) {
    if (Serial) {
      Serial.print("..");
      }

        // Choose the next estimate
        double c;
        if (bracketed) {
            // Ridder's method if possible, else fallback to bisection
            c = riddersStep(a, b, fa, fb);
            
            // Check if Ridder's method failed due to mathematical issues
            SolveStatus error = getSolverError();
            if (error != SolveStatus::Success) {
                // Fallback to bisection for mathematical issues
                c = 0.5 * (a + b);
                clearSolverError();
            }
        } else {
            // Use secant method when not bracketed
            c = secantStep(a, b, fa, fb);
            
            // Check if secant method failed
            SolveStatus error = getSolverError();
            if (error != SolveStatus::Success) {
                // Fallback to bisection midpoint
                c = a + (b - a) / 2.0;
                clearSolverError();
            }
        }

        // Evaluate function at new point
        double fc;
        if (!evaluateFxImmediate(c, fc)) {
            setSolverError(SolveStatus::DomainError);
            return c;
        }
                
        // Check convergence
        if (_abs(fc) < tol || _abs(b - a) < tol) {
            return c;
        }
        
        // Update interval based on sign changes
        if (fa * fc < 0.0) {
            b = c;
            fb = fc;
            bracketed = true;
        } else if (fb * fc < 0.0) {
            a = c;
            fa = fc;
            bracketed = true;
        } else {
            // function constant or failed to bracket
            if (_abs(fa - fb) < 1e-14) {
                // slight perturbation to avoid flat line
                auto adjusted = adjustConstantCase(a, b);
                a = adjusted.first;
                b = adjusted.second;
            } else {
                bracketed = false;
                a = b;
                b = c;
                fa = fb;
                fb = fc;
            }
        }
        
        // Limit jump size if unstable
        if (hasPrevC && _abs(c - prevC) > 100.0 * _abs(b - a)) {
            c = max(min(c, b + 100.0 * (b - a)), a - 100.0 * (b - a));
        }
        prevC = c;
        hasPrevC = true;
    }
    
    // Maximum iterations reached - return best estimate
    if (Serial) {
    Serial.println("[SOLVE] Maximum iterations reached, returning best estimate...");
    }
    return 0.5 * (a + b);
}

/**
 * Simple secant method step - global error state version
 */
double secantStep(double a, double b, double fa, double fb) {
    if (fb == fa) {
        setSolverError(SolveStatus::DomainError);
        return b; // Return some valid value, caller should check error state
    }
    clearSolverError();
    return b - fb * (b - a) / (fb - fa);
}

/**
 * Ridder's method step - global error state version. See https://en.wikipedia.org/wiki/Ridders%27_method.
 */
double riddersStep(double a, double b, double fa, double fb) {
    double c = 0.5 * (a + b);
    
    if (fa == fb) {
        setSolverError(SolveStatus::EqualFunctionValues);
        return c; // Return midpoint as fallback
    }

    // Evaluate function at new point
    double fc;
    if (!evaluateFxImmediate(c, fc)) {
        setSolverError(SolveStatus::DomainError);
        return c;
    }

    double s_sq = fc * fc - fa * fb;
    
    if (s_sq <= 0.0) {
        setSolverError(SolveStatus::InvalidSquareRoot);
        return c; // Return midpoint as fallback
    }
    
    clearSolverError();
    double s = std::sqrt(s_sq);
    double sign = (fa > fb) ? 1.0 : -1.0;
    double d = (c - a) * (sign * fc / s);
    double x_new = c + d;
    
    return x_new;
}

/**
 * Shift the interval slightly when f(a) ≈ f(b)
 */
std::pair<double, double> adjustConstantCase(double a, double b) {
    double shift = 1e-3 * (b - a);
    return std::make_pair(a - shift, b + shift);
}


// SETUP, LOOP
void setup() {
  // Initialize Serial first
  Serial.begin(115200);

  // Set system clock to 64MHz explicitly
  NRF_CLOCK->TASKS_HFCLKSTART = 1;
  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);


  #ifndef BATTERY_POWER
  const uint32_t serialAttachTimeoutMs = 10000;
  uint32_t serialWaitStart = millis();
  //wait until Serial is initialized from host or timeout
  while (!Serial && (millis() - serialWaitStart) < serialAttachTimeoutMs) {
    delay(10);
  }
  delay(1000); // Allow host to finish opening the port
  #endif

  // Verify the clock frequency
  SystemCoreClockUpdate();
  Serial.print("\nSystemCoreClock: ");
  Serial.print(SystemCoreClock/1E6);
  Serial.println(" MHz");

  if (!oled.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("SSD1306 init failed");
    while (true) { }  // hang if display not found
  }

  //--------------------------------------------------------------------
  //Show a build screen and some features
  #if SHOW_BUILD_SCREEN
    oled.clearDisplay();
    oled.setTextSize(1);      // Normal 1:1 pixel scale
    oled.setTextColor(SSD1306_WHITE); // Draw white text
    oled.setCursor(0, 0);     // Start at top-left corner
    oled.cp437(true);         // Use full 256 char 'Code Page 437' font
    // 21 letters per line!
    oled.write("IVEE2 on nice!nanov2"  "\n");
    //oled.write("Build "__DATE__" "__TIME__"\n");
    oled.write("Build " __DATE__" \n");
    oled.write("      " __TIME__" \n");
    oled.write("with: wear-levlg,GK-" "\n");
    oled.write("integ,wp34s solv,RAM" "\n");
    oled.write("regs,STR<->RCR,4line" "\n");
    oled.write("32KB_flash,100regs  " "\n");
    oled.write("1x1 pix gfx,5 digits" "\n");
    oled.display();
    delay(7000);
  #endif
  //-------------------------------------------------------------------

  if (Serial) {
    Serial.print("CODEPAGESIZE="); Serial.print(NRF_FICR->CODEPAGESIZE);
    Serial.print(" CODESIZE="); Serial.print(NRF_FICR->CODESIZE);
    uint32_t totalFlash = NRF_FICR->CODEPAGESIZE * NRF_FICR->CODESIZE;
    Serial.print(" totalBytes=0x"); Serial.println(totalFlash, HEX);
  Serial.print("BOOTADDR=0x"); Serial.println(bootloaderStartAddr(), HEX);
  Serial.print("EEPROM pool base=0x"); Serial.println(eepromFlashBase(), HEX);
  Serial.print("EEPROM active page=0x"); Serial.println(EEPROM.activePageBase(), HEX);
  Serial.print("Writes enabled="); Serial.println(kEnableEepromWrites ? "yes" : "no");
    dumpUserHeader();
  }
  
#ifndef DONT_CLEAR_USER_FUNCTIONS
  uint8_t firstUserByte;
  uint8_t secondUserByte;
  uint8_t cleanFlag;
  EEPROM.get(EEUSTART, firstUserByte);
  EEPROM.get(EEUSTART + 1, secondUserByte);
  EEPROM.get(EECLEANFLAG, cleanFlag);

  if (Serial) {
    Serial.print("[EEPROM] startup snapshot: first=0x"); Serial.print(firstUserByte, HEX);
    Serial.print(" second=0x"); Serial.print(secondUserByte, HEX);
    Serial.print(" cleanFlag=0x"); Serial.println(cleanFlag, HEX);
    Serial.println("[EEPROM] Forcing user-area erase because DONT_CLEAR_USER_FUNCTIONS is undefined.");
  }

  uint8_t blankUserArea[2] = {kEndMarkerByte, kEndMarkerByte};
  if (!rewriteUserAreaImage(blankUserArea, sizeof(blankUserArea), "startup", nullptr)) {
    Serial.println("[EEPROM] WARNING: forced erase request failed.");
  }
#else
  Serial.println("DONT_CLEAR_USER_FUNCTIONS defined. Skipping EEPROM erase.");
#endif

  // Do some stuff while warming up the display
  sort(); // Count sou and nou - Sort commands alphabetically
  
  brightness = EEPROM.getBrightness(); // Wear-leveling read
  darktime = EEPROM.getDarktime();     // Wear-leveling read
  limitdarktime(); // Ensure darktime respects DARKTIMEMIN minimum
  EEPROM.get(EE_BASE, base);           // Load business mode flag
  
  // DIAGNOSTIC: Log the actual darktime value read from EEPROM
  Serial.print("[SETUP] Darktime from EEPROM: ");
  Serial.print(darktime);
  Serial.print(" (expected: ");
  Serial.print(DARKTIMEMIN);
  Serial.print(" = ");
  Serial.print(darktime * 10);
  Serial.println(" seconds)");
  
  base = 0; // ALWAYS START IN 10 BASE FOR NOW

  delaylong(4);
  setframerate(FRAMERATE);

  oled.clearDisplay();
  oled.display();  // ensure it starts blank

  // if you have brightness in EEPROMEmu, you can later call display.ssd1306_command(SSD1306_SETCONTRAST) etc.

#if ENABLE_WRITE_COUNTERS
  Serial.println(F("\n[WEAR-LEVELING] Diagnostics enabled. Send 'S' for stats, 'R' to reset."));
#endif
  Serial.println("Setup complete.");
}

void loop() {
  //Serial.print("MAGIC: "); Serial.println(digitalRead(MAGICKEYPIN));

#if ENABLE_WRITE_COUNTERS
  // Handle diagnostic commands via Serial
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    if (cmd == 'S' || cmd == 's') {
      EEPROM.printWriteStats();
    } else if (cmd == 'R' || cmd == 'r') {
      EEPROM.resetWriteStats();
    } else if (cmd == 'B' || cmd == 'b') {
      // Brightness wear-leveling test
      Serial.println(F("\n[TEST] Running brightness wear-leveling test..."));
      Serial.println(F("[TEST] Writing 20 brightness values"));
      for (int i = 0; i < 20; i++) {
        EEPROM.putBrightness(100 + i);
        Serial.print('.');
        delay(10);
      }
      Serial.println(F("\n[TEST] Complete. Send 'S' to see distribution."));
    } else if (cmd == 'D' || cmd == 'd') {
      // Darktime wear-leveling test
      Serial.println(F("\n[TEST] Running darktime wear-leveling test..."));
      Serial.println(F("[TEST] Writing 12 darktime values"));
      for (int i = 0; i < 12; i++) {
        EEPROM.putDarktime(6 + i);
        Serial.print('.');
        delay(10);
      }
      Serial.println(F("\n[TEST] Complete. Send 'S' to see distribution."));
    }
  }
#endif

  if (isprintscreen) isprintscreen = printscreen(); // Print screen
  if (pause) { // Pause
    delaylong(pause);
    pause = 0;
  }

  if (mp) { // *** Execute/run code
    if (mp < sizeof(mem)) key = mem[mp++]; // Builtin
    else if (mp < sizeof(mem) + sou){
      byte tmp;
      EEPROM.get(mp - sizeof(mem) + EEUSTART, tmp); // read from EEPROM
      key = tmp;                                    // assign to key
      mp++;                                         // post-increment
    }
    else mp = 0; // cmd > MAXCMDU
    
    // log program execution steps
    #if LOG_PRG_MEMPTR
    if (Serial && key != 255) {
      Serial.print("[PRG] mp="); Serial.print(mp);
      Serial.print(" key="); Serial.print(key);
      Serial.print(" dp="); Serial.println(dp);
      Serial.flush(); // Force output
    }
    #endif
    
#if LOG_FX_FI_STACK
    if (Serial && key != _END && key < NOPRINTNOKEY) {
      Serial.print("[EXEC_STEP] cmd="); Serial.print(key);
      if (key < MAXCMDB && key < sizeof(cmd)/sizeof(cmd[0])) {
        Serial.print(" name="); Serial.print(cmd[key]);
      }
      Serial.print(" BEFORE - dp="); Serial.print(dp);
      Serial.print(" ap="); Serial.print(ap);
      Serial.print(" STACK: ");
      for (byte i = 0; i < dp && i < 6; i++) {
        Serial.print(ds[i].r, 3); Serial.print(" ");
      }
      Serial.println();
      Serial.flush();
    }
#endif
    
    if (key >= MAXCMDB && key != _END) apush(mp); // Subroutine detected - branch (only user programs, not builtin RAM commands)
    if (key == _END) { // _END reached
#if LOG_FX_FI_STACK
      if (Serial) {
        Serial.print("[EXEC_END] ap="); Serial.print(ap);
        Serial.print(" dp="); Serial.print(dp);
        Serial.print(" STACK: ");
        for (byte i = 0; i < dp && i < 6; i++) {
          Serial.print(ds[i].r, 3); Serial.print(" ");
        }
        Serial.println();
      }
#endif
      if (ap) mp = apop(); // End of subroutine - return
      else { // End of run
        mp = 0;
        isprintscreen = true; // Finally print screen
        #if LOG_PRG_MEMPTR
          if (Serial) Serial.println("[PRG] Program END - execution complete");
        #endif
      }
    }
    else {
#if LOG_RAM_STORCL
      if (Serial && (key == _STO_RAM || key == _RCL_RAM)) {
        Serial.print("[LOOP] About to execute cmd=");
        Serial.print(key == _STO_RAM ? "STO_RAM" : "RCL_RAM");
        Serial.print(" dp="); Serial.print(dp);
        Serial.print(" isnewnumber="); Serial.println(isnewnumber);
      }
#endif
#if LOG_PRG_MEMPTR
      if (Serial && key == _RCL) {
        Serial.println("[PRG] *** ABOUT TO EXECUTE RCL ***");
      }
#endif
      execute(key);
#if LOG_FX_FI_STACK
      if (Serial && key < NOPRINTNOKEY) {
        Serial.print("[EXEC_STEP] AFTER - dp="); Serial.print(dp);
        Serial.print(" ap="); Serial.print(ap);
        Serial.print(" STACK: ");
        for (byte i = 0; i < dp && i < 6; i++) {
          Serial.print(ds[i].r, 3); Serial.print(" ");
        }
        Serial.println();
        Serial.flush();
      }
#endif
#if LOG_PRG_MEMPTR
      if (Serial && key == _RCL) {
        Serial.println("[PRG] *** RCL EXECUTE DONE ***");
      }
#endif
#if LOG_RAM_STORCL
      if (Serial && (key == _STO_RAM || key == _RCL_RAM)) {
        Serial.print("[LOOP] After execute dp="); Serial.print(dp);
        Serial.print(" isnewnumber="); Serial.println(isnewnumber);
      }
#endif
    }
    digitalWrite(KEYBOARDCOL1, LOW); // Stop by pressing C
    if (!digitalRead(KEYBOARDROW4)) mp = ap = 0;
    digitalWrite(KEYBOARDCOL1, HIGH);
  }

  else { // *** Evaluate valid new key
    if (!(nextFrame())) return; // Idle for next frame
    key = getkey(); // READ KEY
    if (key < NOPRINTNOKEY) { // only if valid key otherwise flooding with NOPRINTNOKEY
      Serial.print("Key read: ");
      Serial.println(key);
      // Debug matrix pins
      Serial.print("COL1: "); Serial.print(digitalRead(KEYBOARDCOL1));
      Serial.print(" ROW4: "); Serial.println(digitalRead(KEYBOARDROW4));
    }

    if (key == KEY13) { // Stop execution
      issolve = isint =  isplot = istorch = false;
      gkResetController();
      clockState.active = false; // Stop clock
      powertimestamp = millis(); // Reset power management timer
    }
    if (key == KEY1 && !base) { // Check MENU (longpressed f)
      if (millis() - timestamp > FLONGPRESSTIME) {
        if (isprgedit) { // Menu from prgedit demanded
          isprgmenu = true; isprgedit = false;
        }
        else { // Menu demanded
          fgm = 0;
          freleased = false;
        }
        ismenu = isprintscreen = true;
        sel = 0; // Comment out, if menu should't start at 0
      }
    }
    else timestamp = millis();

    if (millis() - powertimestamp > darktime * 10000L) { // Dark if no activity
      // Don't sleep if clock is active - keep device running to maintain clock accuracy
      if (!clockState.active) {
        Serial.print("[POWER] Screen timeout after ");
        Serial.print((millis() - powertimestamp) / 1000);
        Serial.print("s (darktime=");
        Serial.print(darktime);
        Serial.print(" = ");
        Serial.print(darktime * 10);
        Serial.println("s)");
        _keygoff();
        isprintscreen = true;
      }
      // Update powertimestamp to prevent repeated checking when clock is active
      powertimestamp = millis();
    }

    if (issolve) { // # SOLVE
      cycles++;

      dp = 0; // Clear stack for new solve

#if LOG_SOLVE
      Serial.print("[SOLVE] before _fnfx mp=");
      Serial.println(mp);
#endif
      bool endpointsReady = evaluateFxImmediate(x0, w0);
      if (endpointsReady) endpointsReady = evaluateFxImmediate(x1, w1);

      if (!endpointsReady) {
        issolve = false;
        msgnr = MSGRUN;
        isprintscreen = true;
      }
      else {
#if LOG_SOLVE
        if (Serial) {
          Serial.print("[SOLVE] y0="); Serial.print(w0);
          Serial.print(" y1="); Serial.println(w1);
        }
#endif

        double root = hybridSolve(x0, x1, w0, w1, 1e-12, 250); //wp34s style solver
        dpushr(root); // Push root found

        if (Serial) { // always serial.print the root.
          Serial.print("\n[SOLVE] Root="); Serial.println(root);
        }
        isnewnumber = true; issolve = false;
        isprintscreen = true;
      }
    }
    else if (isint) { // # INTEGRATE - Adaptive Gauss-Kronrod
      if (!gkController.active) {
        isint = false;
      }
      else {
        cycles++;
#if LOG_FX_FI_STACK
        if (Serial) {
          Serial.print("[FI] BEFORE execute - eval#"); Serial.print(gkEvalCount + 1);
          Serial.print(" node="); Serial.print((int)gkController.nodeIndex);
          Serial.print(" x="); Serial.print(gkCurrentNodePosition(), 8);
          Serial.print(" dp="); Serial.print(dp);
          Serial.print(" ap="); Serial.print(ap);
          Serial.print(" STACK: ");
          for (byte i = 0; i < dp && i < 8; i++) {
            Serial.print("["); Serial.print(i); Serial.print("]=");
            Serial.print(ds[i].r, 6);
            if (ds[i].i != 0.0) {
              Serial.print("+j"); Serial.print(ds[i].i, 6);
            }
            Serial.print(" ");
          }
          Serial.println();
        }
#endif
        execute(MAXCMDB); // Evaluate user function at current x value
#if LOG_FX_FI_STACK
        if (Serial) {
          Serial.print("[FI] AFTER execute - dp="); Serial.print(dp);
          Serial.print(" ap="); Serial.print(ap);
          Serial.print(" STACK: ");
          for (byte i = 0; i < dp && i < 8; i++) {
            Serial.print("["); Serial.print(i); Serial.print("]=");
            Serial.print(ds[i].r, 6);
            if (ds[i].i != 0.0) {
              Serial.print("+j"); Serial.print(ds[i].i, 6);
            }
            Serial.print(" ");
          }
          Serial.println();
        }
#endif
        double fx = dpoprd();
#if LOG_FX_FI_STACK
        if (Serial) {
          Serial.print("[FI] AFTER dpoprd - fx="); Serial.print(fx, 8);
          Serial.print(" dp="); Serial.print(dp);
          Serial.print(" STACK: ");
          for (byte i = 0; i < dp && i < 8; i++) {
            Serial.print("["); Serial.print(i); Serial.print("]=");
            Serial.print(ds[i].r, 6); Serial.print(" ");
          }
          Serial.println();
        }
#endif
        gkEvalCount++;
        uint8_t node = gkController.nodeIndex;
        gkController.kronrodSum += gk_weights[node] * fx;
        if (gk_gauss_weights[node] != 0.0) gkController.gaussSum += gk_gauss_weights[node] * fx;
        gkController.nodeIndex++;

        if (gkController.nodeIndex < GK_POINTS) {
          dpushr(gkCurrentNodePosition());
        }
        else {
          double kronrodEstimate = gkController.kronrodSum * gkController.halfwidth;
          double gaussEstimate = gkController.gaussSum * gkController.halfwidth;
          double localError = _abs(kronrodEstimate - gaussEstimate);
          gkController.lastError = max(gkController.lastError, localError);
          gkRecordIntervalStats();
          double adaptiveTol = max(gkController.current.tol, gkController.targetRelTol * _abs(kronrodEstimate));
          bool accept = (localError <= adaptiveTol) ||
                        (gkController.current.depth >= GK_MAX_DEPTH) ||
                        (gkController.halfwidth <= GK_MIN_INTERVAL);

          bool aborted = false;
          gkLogProgress("estimate", localError, adaptiveTol, accept);
          if (accept) {
            gkController.total += kronrodEstimate;
          }
          else {
            double mid = 0.5 * (gkController.current.a + gkController.current.b);
            double childTol = gkController.current.tol * 0.5;
            gkRefinementCount++;
            bool pushOk = gkPushInterval(mid, gkController.current.b, childTol, gkController.current.depth + 1) &&
                          gkPushInterval(gkController.current.a, mid, childTol, gkController.current.depth + 1);
            if (!pushOk) {
              gkLogProgress("overflow", localError, adaptiveTol, false);
              gkResetController();
              isint = false;
              msgnr = MSGOVERFLOW;
              isprintscreen = true;
              aborted = true;
            }
          }

          if (aborted) {
            // Integration aborted due to stack overflow
          }
          else if (gkStartNextInterval()) {
            dpushr(gkCurrentNodePosition());
          }
          else {
            double finalResult = gkController.orientation * gkController.total;
            if (Serial) {
              Serial.print("Adaptive GK complete. result="); Serial.print(finalResult);
              Serial.print(" maxErr="); Serial.print(gkController.lastError);
              Serial.print(" evals="); Serial.print(gkEvalCount);
              Serial.print(" refinements="); Serial.println(gkRefinementCount);
            }
            dpushr(localError); // Push estimated error
            dpushr(finalResult); // Push final result
#if LOG_FX_FI_STACK
            if (Serial) {
              Serial.print("[FI] INTEGRATION COMPLETE - finalResult="); Serial.print(finalResult, 8);
              Serial.print(" dp="); Serial.print(dp);
              Serial.print(" FINAL STACK: ");
              for (byte i = 0; i < dp && i < 8; i++) {
                Serial.print("["); Serial.print(i); Serial.print("]=");
                Serial.print(ds[i].r, 6); Serial.print(" ");
              }
              Serial.println();
            }
#endif
            mp = 0;
            isnewnumber = true;
            isint = false;
            gkResetController();
            isprintscreen = true;
          }
        }
      }
    }
    if (isplotcalc) { // # PLOT
#if LOG_PLOT
      Serial.print("[PLOT] state cycle="); Serial.print(cycles);
      Serial.print(" inflight="); Serial.print(isplotEvalInFlight ? "y" : "n");
      Serial.print(" dp="); Serial.print(dp);
      if (dp) {
        Serial.print(" tos="); Serial.print(ds[dp - 1].r, 10);
      }
      Serial.println();
#endif

      // If a previous evaluation finished, capture its result first
      if (isplotEvalInFlight) {
        if (cycles < GRAPH_PIXEL_WIDTH) {
          double y = dpopr();
          plot[cycles] = y;
#if LOG_PLOT
          Serial.print("[PLOT] captured y["); Serial.print(cycles);
          Serial.print("]="); Serial.println(y, 10);
#endif
          cycles++;
        }
        isplotEvalInFlight = false;
      }

      if (cycles >= GRAPH_PIXEL_WIDTH) {
        isplotcalc = false;
        isplot = true; // Ready to render
        isprintscreen = true; // Force display refresh immediately
#if LOG_PLOT
        Serial.println("[PLOT] sampling complete, ready to render");
#endif
      }
      else if (!isplotEvalInFlight) {
        double nextX = plota + cycles * plotd;
        dpushr(nextX);
        execute(MAXCMDB);
        isplotEvalInFlight = true;
#if LOG_PLOT
        Serial.print("[PLOT] launched eval for x="); Serial.println(nextX, 10);
#endif
      }
    }
    
    if (clockState.active) { // # CLOCK
      clockUpdate();
    }

    if (key != oldkey) {
      oldkey = key; // New valid key is old key
      freleased = true;
      if (key < NOPRINTNOKEY) {
        powertimestamp = millis(); // Keep awake when active
        darkscreen = false; // Reactivate screen in case of dark
        
        // DIAGNOSTIC: Log power management reset
        Serial.print("[POWER] Activity detected, resetting timer (darktime=");
        Serial.print(darktime);
        Serial.print(" = ");
        Serial.print(darktime * 10);
        Serial.println("s)");

        if (isgetkey) { // ### Get keypress
          byte k;
          while ((k = getkey()) >= NOPRINTNOKEY) ; // Wait for keypress
          dpushr(k - '0');
          isgetkey = false;
        }

        else if (isdict) { // ### Dictionary
          memset(dbuf, 0, sizeof(dbuf));  // ← CLEAR BUFFER FIRST
          byte tmp = menuselect(MAXCMDU / 16);
          isprintscreen = true;  // ← ADD THIS - triggers redraw
          flush_dbuf_to_oled(); 
          if (tmp < PRINTNOKEY) { // Item selected
          if (issetusr) { // Go back to select menu slot
#if LOG_USER_MENU
              logUserMenuCommand("dict-selection", cmdsort[tmp]);
#endif
              setusrselect = cmdsort[tmp];
              sel = 0;
              issetusr = false; ismenu = ismenusetusr = true;
            }
            else if (isprgdict) { // Go back to prgedit
              prgstepins(cmdsort[tmp]);// was prgstepins(tmp);   caused wrong entries in program 100% of the time!!!!! 21.08.
              isprgdict = false; isprgedit = true;
            }
            else { // Execute command directly
              execute(cmdsort[tmp]); // Execute dict choice
              isnewnumber = true;
            }
            isdict = false;
          }
          else if (tmp == NOPRINTNOKEY) { // Escape
            key = NOPRINTNOKEY;
            isdict = false;
          }
        }

        else if (ismenu) { // ### Menu
          memset(dbuf, 0, sizeof(dbuf));  // ← CLEAR BUFFER FIRST
          if (freleased) { // Execute only if f-key was released
            byte tmp = menuselect(2);
            isprintscreen = true;  // ← ADD THIS - triggers redraw
            flush_dbuf_to_oled();         // ← FLUSH AFTER
            if (tmp < PRINTNOKEY) { // Item selected
              if (ismenusetusr) { // Save user menu entry
#if LOG_USER_MENU
                Serial.print("[USR_MENU] assigning slot ");
                Serial.print(tmp);
                Serial.print(" <- cmdId ");
                Serial.println(setusrselect);
                logUserMenuCommand("assign", setusrselect);
#endif
                byte currentValue = 0xFF;
                EEPROM.get(EEMENU + tmp, currentValue);
                bool writeOk = true;
                if (currentValue != setusrselect)
                  writeOk = commitMenuSlot(tmp, setusrselect);
                if (!writeOk) {
                  msgnr = MSGSAVE;
                }
#if LOG_USER_MENU
                byte verify = 0;
                EEPROM.get(EEMENU + tmp, verify);
                Serial.print("[USR_MENU] slot ");
                Serial.print(tmp);
                Serial.print(" verified cmdId ");
                Serial.println(verify);
                logUserMenuCommand("assign-verify", verify);
#endif
                ismenusetusr = false;
              }
              else if (isprgmenu) { // Go back to prgedit
                byte value; 
                EEPROM.get(EEMENU + tmp, value);  // read byte from EEPROM 
                prgstepins(value);                // pass to function 
                isprgmenu = false; isprgedit = true;
              }
              else { // Execute selected command
                byte value; 
                EEPROM.get(EEMENU + tmp, value); 
#if LOG_USER_MENU
                Serial.print("[USR_MENU] execute slot ");
                Serial.print(tmp);
                Serial.print(" cmdId ");
                Serial.println(value);
                logUserMenuCommand("execute", value);
#endif
                execute(value); 
                isnewnumber = true; 
              }
              ismenu = false;
            }
            else if (tmp == NOPRINTNOKEY) ismenu = ismenusetusr = false; // Escape
          }
        }

        else if (isprgselect) { // ### Select program
          if (key == KEY5) // # 6 - move up
            if (prgselect > 0) prgselect--;
            else prgselect = nou - 1;
          else if (key == KEY9) // # 3 - move down
            if (prgselect < nou - 1) prgselect++;
            else prgselect = 0;
          else if (key == KEY4) { // # 9 - send program straight to top
            moveprgtop();
          }
          else if (key == KEY14) { // # 0 - rename program
            prgaddr = seekusrp(0);
            //select = renamenr = 0;
            sel = renamenr = 0;
            isprgrename = true; isprgselect = false;
          }
          else if (key == KEY10) { // # 1 - new program
            bool repaired = ensureBlankUserAreaForNewProgram("repair-new");
            if (repaired && Serial) {
              Serial.println("[PRG_NEW] user area blanked (repair-new)");
            }
            Serial.print("[PRG_NEW] KEY10 pressed. writesEnabled=");
            Serial.print(kEnableEepromWrites ? "yes" : "no");
            Serial.print(" nou="); Serial.print(nou);
            Serial.print(" sou="); Serial.print(sou);
            Serial.print(" EEU="); Serial.print(EEU);
            Serial.print(" prgbuflen(before)="); Serial.println(prgbuflen);
            Serial.println("=== NEW PROGRAM INIT ===");
            prgeditstart = prgbuflen = 0;
            isprgnew = isprgeditfirst = isprgedit = true; isprgselect = false;
            sort();
          }
          else if (key == KEY15) { // # Dot - delete program
            if (!nou) {
              Serial.println("[PRG_DEL] No programs available to delete.");
            }
            else {
              int victimStart = seekusrp(0);
              int victimEnd = seekusrp(1);
              Serial.print("[PRG_DEL] request index="); Serial.print(prgselect);
              Serial.print(" nou="); Serial.print(nou);
              Serial.print(" sou="); Serial.print(sou);
              Serial.print(" range=[0x"); Serial.print(victimStart, HEX);
              Serial.print(",0x"); Serial.print(victimEnd, HEX);
              Serial.print(") bytes="); Serial.println(victimEnd - victimStart);

              if (victimEnd <= victimStart) {
                Serial.println("[PRG_DEL] ERROR: invalid program range; aborting delete.");
                msgnr = MSGSAVE;
              }
              else if (rebuildUserAreaSkippingRange(victimStart, victimEnd)) {
                Serial.println("[PRG_DEL] Compaction rewrite complete.");
                if (prgselect >= nou - 1 && prgselect) prgselect--;
              }
              else {
                Serial.println("[PRG_DEL] ERROR: compaction failed.");
                msgnr = MSGSAVE;
              }
              sort();
            }
          }
          else if (key == KEY8) { // # 6 - move prg up
            if (prgselect) {
              moveprgup(); prgselect--;
            }
          }
          else if (key == KEY12) { // # 3 - move prg down (= move up with prgselect++)
            if (prgselect < nou - 1) {
              prgselect++;
              moveprgup();
            }
          }
          else if (key == KEY11) { // # 2 - load program (in)
            byte in, nr = 0;
            long tim = millis();
            prgbuf[0] = '\0'; 
            while (Serial.available() > 0) byte tmp = Serial.read(); // Flush serial buffer
            while (Serial.available() <= 0 && millis() - tim < 3000) ; // Wait for serial input
            while (Serial.available() > 0 && (in = Serial.read()) >= ' ' && nr < PRGSIZEMAX) {
              prgbuf[nr++] = in - ' ';
              prgbuf[nr] = _END;
              sort();
            }
            prgbuflen = nr;
            isprgnew = isprgedit = true; isprgselect = false;
          }
          else if (key == KEY7) { // # 5 - save program (out)
            for (int i = seekusrp(0) + 3; i < seekusrp(1) - 1; i++) {
              byte value; 
              EEPROM.get(i, value);           // read EEPROM byte 
              Serial.write(value + ' ');}      // send over Serial 
            delayshort(200);
          }
          else if (key == KEY16 && nou) { // # Enter - edit program
            prgaddr = seekusrp(0);
            
            // Find the actual end of the current program by looking for its _END marker
            int programEnd = prgaddr + PRGNAMEMAX; // Start after the program name
            while (programEnd < EEUSTART + sou) {
              byte value;
              EEPROM.get(programEnd, value);
              if (value == _END) {
                programEnd++; // Include the _END marker
                break;
              }
              programEnd++;
            }
            
            prgbuflen = programEnd - prgaddr - PRGNAMEMAX - 1; // Subtract name and _END marker
            if (prgbuflen < 0) prgbuflen = 0; // Safety check
            oldprgbuflen = prgbuflen; // Save length for later

            Serial.print("[PRG_EDIT] select="); Serial.print(prgselect);
            Serial.print(" prgaddr=0x"); Serial.print(prgaddr, HEX);
            Serial.print(" programEnd=0x"); Serial.print(programEnd, HEX);
            Serial.print(" prgbuflen="); Serial.print(prgbuflen);
            Serial.print(" nou="); Serial.print(nou);
            Serial.print(" sou="); Serial.print(sou);
            Serial.print(" EEUSTART+ sou="); Serial.print(EEUSTART + sou);
            Serial.print(" calculated end valid="); Serial.print(programEnd > prgaddr && programEnd <= EEUSTART + sou ? "YES" : "NO");
            Serial.println();
            Serial.print("[PRG_EDIT] raw bytes (sentinel+name+payload up to 16B): ");

            for (int i = -1; i < PRGNAMEMAX + prgbuflen + 2 && i < 15; ++i) {
              int addr = prgaddr + i;
              if (addr < EEUSTART - 1 || addr >= EEUEND) break;
              byte raw;
              EEPROM.get(addr, raw);
              Serial.print(raw, HEX);
              Serial.print(' ');
            }
            Serial.println();

            for (byte i = 0 ; i < prgbuflen; i++){ 
              byte tmp; 
              EEPROM.get(prgaddr + PRGNAMEMAX + i, tmp); // read EEPROM byte 
              prgbuf[i] = tmp;                            // store in buffer 
            }
            prgbuf[prgbuflen] = '\0'; 
            isprgselect = false; isprgedit = true;
          }
          else if (key == KEY13) isprgselect = false; // # Escape
        }

        else if (isprgrename) { // ### Program rename
          byte tmp = menuselect(6);
          bool advanced = false;
#if LOG_RENAME
          if (Serial) {
            Serial.print("[RENAME_INPUT] key="); Serial.print(key);
            Serial.print(" tmp="); Serial.print(tmp);
            Serial.print(" sel="); Serial.print(sel);
            Serial.print(" renamenr="); Serial.print(renamenr);
            Serial.print(" prgaddr=0x"); Serial.println(prgaddr, HEX);
          }
#endif
          if (tmp < PRINTNOKEY) { // Item selected - replace letter of program name
            uint8_t ascii = static_cast<uint8_t>(tmp + ' ');
            int targetAddr = prgaddr + renamenr;
#if LOG_RENAME
            if (Serial) {
              Serial.print("[RENAME] idx="); Serial.print(renamenr);
              Serial.print(" tmp="); Serial.print(tmp);
              Serial.print(" ascii="); Serial.print(ascii);
              Serial.print(" target=0x"); Serial.println(targetAddr, HEX);
            }
#endif
            if (patchUserAreaByte(targetAddr, ascii, "rename")) {
              advanced = true;
#if LOG_RENAME
              if (Serial) {
                Serial.print("[RENAME_WRITE] stored='"); Serial.print((char)ascii);
                Serial.print("' at renamenr="); Serial.println(renamenr);
              }
#endif
            }
            else {
              msgnr = MSGSAVE;
            }
          }
          if (advanced) renamenr++; // move to next character only on successful write

          if (tmp == NOPRINTNOKEY || renamenr >= PRGNAMEMAX) { // Escape
#if LOG_RENAME
            if (Serial) {
              Serial.print("[RENAME_EXIT] reason=");
              Serial.print(tmp == NOPRINTNOKEY ? "escape" : "name-complete");
              Serial.print(" renamenr="); Serial.println(renamenr);
            }
#endif
            isprgrename = false; isprgselect = true;
          }
          sort();
        }

        else if (isprgedit) { // ### Program edit
          if (key == KEY1) { // # F-key
            if (fgm == ISG) fgm = 0; else if (fgm == ISF) fgm = ISG; else fgm = ISF;
          }
          else if (key == KEY6 && fgm == ISF) { // # F-4 ... select via dict
            sel = 0;
            fgm = 0; isprgdict = isdict = true; isprgedit = false;
          }
          else if (fgm == ISF || fgm == ISG) // # Insert F- or G-shifted key
            prgstepins(key - '0' + fgm * 16);
          else if (key == KEY5) // # Up
          {
            if (prgeditstart > 0) prgeditstart--; else prgeditstart = prgbuflen - 1;
            isprgeditfirst = false;
          }
          else if (key == KEY9) // # Down
          {
            if (prgeditstart < prgbuflen - 1) prgeditstart++; else prgeditstart = 0;
            isprgeditfirst = false;
          }
          else if (key == KEY15) { // # Dot - Delete step
            if (prgbuflen) {
              for (byte i = prgeditstart; i < prgbuflen; i++) prgbuf[i] = prgbuf[i + 1];
              prgbuflen--;
              if (prgeditstart == prgbuflen && prgeditstart) prgeditstart--; // Deleted last step
              if (isprgeditfirst && prgeditstart) prgeditstart--;
              if (!prgeditstart) isprgeditfirst = true;
            }
          }
          else if (key <= KEY4 || key == KEY16) prgstepins(key - '0'); // # Insert direct key
          else if (key == KEY13) { // # Escape and save program to EEPROM
            if (isprgnew) { // Save new program
              Serial.println("=== NEW PROGRAM SAVE START ===");
              Serial.print("sou="); Serial.print(sou);
              Serial.print(" prgbuflen="); Serial.print(prgbuflen);
              Serial.print(" EEU="); Serial.print(EEU);
              Serial.print(" EEUSTART="); Serial.println(EEUSTART);
              
              const size_t usedBytes = (sou == 0) ? 0 : static_cast<size_t>(sou - 1);
              const size_t bytesNeeded = PRGNAMEMAX + prgbuflen + 2; // name + payload + double _END

              dumpUserHeader();

              if (usedBytes + bytesNeeded > EEU) {
                Serial.println("ERROR: Program too big!");
                msgnr = MSGSAVE; // New program is too big
              }
              else { 
                // Fill name area with '-'
                const int newPrgAddr = EEUSTART + usedBytes;
                Serial.print("Writing program name '---' at 0x");
                Serial.println(newPrgAddr, HEX);
                Serial.print("[PRG_NEW] Surrounding bytes before write: ");
                for (int offset = -2; offset <= 6; ++offset) {
                  int inspectAddr = newPrgAddr + offset;
                  if (inspectAddr < EEUSTART) continue;
                  byte snapshot;
                  EEPROM.get(inspectAddr, snapshot);
                  Serial.print("["); Serial.print(offset); Serial.print("]=0x");
                  Serial.print(snapshot, HEX); Serial.print(' ');
                }
                Serial.println();
                Serial.print("[PRG_NEW] Pre-bytes @newPrgAddr: ");
                for (int i = 0; i < PRGNAMEMAX + 6; ++i) {
                  byte snapshot;
                  EEPROM.get(newPrgAddr + i, snapshot);
                  Serial.print(snapshot, HEX);
                  Serial.print(' ');
                }
                Serial.println();
                for (int i = 0; i < PRGNAMEMAX; i++) {
                  int addr = newPrgAddr + i;
                  Serial.print("  Name char "); Serial.print(i);
                  Serial.print(" '-' (0x"); Serial.print('-', HEX);
                  Serial.print(") at addr "); Serial.print(addr);
                  byte before = EEPROM[addr];
                  Serial.print(" before=0x"); Serial.print(before, HEX);
                  uint8_t dash = '-';
                  EEPROM.put(addr, dash);
                  byte after = EEPROM[addr];
                  Serial.print(" after=0x"); Serial.println(after, HEX);
                }

                Serial.print("[PRG_NEW] Persisting prgbuflen="); Serial.println(prgbuflen);
                if (prgbuflen == 0) {
                  Serial.println("[PRG_NEW] prgbuf empty; expect freshly cleared command bytes");
                }

                // Save program buffer to EEPROM
                for (int i = 0; i < prgbuflen; i++) {
                  uint8_t stepByte = prgbuf[i];
                  EEPROM.put(newPrgAddr + PRGNAMEMAX + i, stepByte);
                }

                // Add 2 x _END markers (program terminator + list terminator)
                const int firstEndAddr = newPrgAddr + PRGNAMEMAX + prgbuflen;
                const int secondEndAddr = firstEndAddr + 1;
                byte beforeFirstEnd, beforeSecondEnd;
                EEPROM.get(firstEndAddr, beforeFirstEnd);
                EEPROM.get(secondEndAddr, beforeSecondEnd);
                Serial.print("[PRG_NEW] End bytes before write: first=0x");
                Serial.print(beforeFirstEnd, HEX);
                Serial.print(" second=0x");
                Serial.println(beforeSecondEnd, HEX);
                EEPROM.put(firstEndAddr, kEndMarkerByte);
                EEPROM.put(secondEndAddr, kEndMarkerByte);
                byte afterFirstEnd, afterSecondEnd;
                EEPROM.get(firstEndAddr, afterFirstEnd);
                EEPROM.get(secondEndAddr, afterSecondEnd);
                Serial.print("[PRG_NEW] End bytes after write: first=0x");
                Serial.print(afterFirstEnd, HEX);
                Serial.print(" second=0x");
                Serial.println(afterSecondEnd, HEX);

                Serial.print("[PRG_NEW] Post-bytes @newPrgAddr: ");
                for (int i = 0; i < PRGNAMEMAX + 6; ++i) {
                  byte snapshot;
                  EEPROM.get(newPrgAddr + i, snapshot);
                  Serial.print(snapshot, HEX);
                  Serial.print(' ');
                }
                Serial.println();

                // Sort programs (unchanged)
                Serial.println("Calling sort()...");
                sort();
                Serial.print("After sort: nou="); Serial.print(nou);
                Serial.print(" sou="); Serial.println(sou);
                Serial.println("User area bytes 0-15 after save:");
                for (int dbg = 0; dbg < 16; ++dbg) {
                  byte val;
                  EEPROM.get(EEUSTART + dbg, val);
                  Serial.print("  ["); Serial.print(dbg); Serial.print("]=0x");
                  Serial.println(val, HEX);
                }
              }
              isprgnew = false;
              Serial.println("=== NEW PROGRAM SAVE END ===");
            }
            else { // Save edited program via rewriteUserAreaImage
              Serial.println("[PRG_EDIT_SAVE] Rebuilding user area with edited program");
              Serial.print("  oldprgbuflen="); Serial.print(oldprgbuflen);
              Serial.print(" prgbuflen="); Serial.println(prgbuflen);
              
              const int capacity = userAreaCapacity();
              int usedLen = sou;
              if (usedLen <= 0 || usedLen > capacity) usedLen = capacity;
              if (usedLen < 2) usedLen = 2;
              
              uint8_t* snapshot = (uint8_t*)malloc(usedLen);
              if (!snapshot) {
                Serial.println("[PRG_EDIT_SAVE] ERROR: insufficient RAM for snapshot");
                msgnr = MSGSAVE;
              }
              else {
                // Snapshot current user area
                for (int i = 0; i < usedLen; ++i) {
                  EEPROM.get(EEUSTART + i, snapshot[i]);
                }
                
                // Build new user area with edited program
                uint8_t* rebuilt = (uint8_t*)malloc(capacity);
                if (!rebuilt) {
                  Serial.println("[PRG_EDIT_SAVE] ERROR: insufficient RAM for rebuild");
                  free(snapshot);
                  msgnr = MSGSAVE;
                }
                else {
                  int writeIdx = 0;
                  int programDataStart = prgaddr + PRGNAMEMAX;
                  int oldProgramEnd = programDataStart + oldprgbuflen + 1; // Include _END
                  
                  // Copy everything before edited program's data
                  for (int i = 0; i < programDataStart - EEUSTART; ++i) {
                    rebuilt[writeIdx++] = snapshot[i];
                  }
                  
                  // Write new program data
                  for (int i = 0; i < prgbuflen; ++i) {
                    rebuilt[writeIdx++] = prgbuf[i];
                  }
                  
                  // Add program terminator
                  rebuilt[writeIdx++] = kEndMarkerByte;
                  
                  // Copy everything after old program
                  for (int i = oldProgramEnd - EEUSTART; i < usedLen; ++i) {
                    if (writeIdx < capacity) {
                      rebuilt[writeIdx++] = snapshot[i];
                    }
                  }
                  
                  // Ensure proper list terminator
                  if (writeIdx >= 2 && rebuilt[writeIdx - 1] != kEndMarkerByte) {
                    if (writeIdx < capacity) rebuilt[writeIdx++] = kEndMarkerByte;
                  }
                  
                  Serial.print("[PRG_EDIT_SAVE] Rebuilt writeIdx="); Serial.println(writeIdx);
                  
                  if (rewriteUserAreaImage(rebuilt, writeIdx, "edit-save", nullptr)) {
                    Serial.println("[PRG_EDIT_SAVE] Successfully saved edited program");
                  }
                  else {
                    Serial.println("[PRG_EDIT_SAVE] ERROR: rewrite failed");
                    msgnr = MSGSAVE;
                  }
                  
                  free(rebuilt);
                }
                free(snapshot);
              }
              sort();
            }

            prgbuflen = prgeditstart = 0; isprgselect = true; isprgedit = false;
          }
        }

        else execute(key - '0' + fgm * 16); // ### Execute key
        isprintscreen = true;
      }
    }
  } // End of evaluating key
}
