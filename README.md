# IVEE2 - a Powerful Programmable RPN Calculator based on IVEE

Version 1.0, (c) 2026 by emefff based on the code by deetee/zooxo.
This software is covered by the 3-clause BSD license.

## Purpose
IVEE2 is a port of IVEE to nice!nanov2. It includes some changes that make use of the faster MCU, more RAM and flash space. The main driver behind this was the limited space left on the original IVEE on ProMicro. As far as I remember only one single byte was free on the original calculator. The original code of IVEE can be found in

https://github.com/zooxo/iv

Much of it still present in IVEE2, I am publishing this with kind permission of the original developer, whom I hereby want to thank for his great work. I built his calculator many years ago, mostly by hand without any usage of CAD apart from the housing, which turned out to be rather big and clumsy. Being a fan of RPN calculators and especially the HP lineup, it gives me great pleasure to having bit this bullet and come up with this and being able to present a codebase, that has, in my opinion, great possiblities for the future on this chip. 
Nice!nanov2 is probably best known in the DIY keyboard builder community, where it is used for programmable keyboards and the like. It also has BLE capbility and uses very low power, has an onboard LiIon charging controller and USB-C connectivity. Thus, I can even imagine OTA capability for flashing and in operation for this device. Apart from that, I think of it as a worthy replacement for ESP32 in a lot of scenarios, especially for devices on battery power. 
Being an amateur programming and software enthusiast I used Kilo Code with mostly Codex-5.1, MiniMax-M2.1 and Claude Sonnet 4.5 for developing IVEE2. From this rather overwhelming experience I must say, I do not see a bright future for programmers. The evolution of the codebase was rapid, it took me about 2.5 weeks in Dec. 2025 with just 2-3 hours per day (more on weekends though). The debugging features are, from the viewpoint of an amateur C++ programmer, vastly superior than I imagined. On the other hand, being an amateur, I probably do not see where code has turned out laborious and perhaps inefficient. My approach to this was rather practical and user driven. Two of the used coding agents where very analytical in their approach to developing new functions or debugging. In the end, the codebase includes a great amount of diagnostic code, readily available for anyone to dive in. 

## Recommendations

From my experience, I started this project with Arduino, PlatformIO is the much better IDE in conjunction with  nice!nanov2. With Arduino I was not even able to upload anything, some knock-off boards do not even allow uploading .uf2 binaries, although they claim being able. Thus I recommend anyone building his/her own IVEE2 to use:

-) PlatformIO as IDE, my platformio.ini is included.

-) the original Nice!NanoV2. Granted, it is not cheap, but some of the knock-offs clearly do not work. 

-) also solder a reset button to the board. Sometimes it needs two resets before uploading the firmware.

-) a very common LiIon battery like the one used here, which can be found anywhere on this planet. If your battery capacity is even higher, nice!nanov2 allows to solder a resistor to change the charging current to 500mA instead of the standard 100mA 
(fine by me, increases battery life but takes longer to charge)

## Changes in IVEE2

### Overview of changes

-) enhanced floating point precision, doubles are now double precision instead of single.

-) 4 line stack display. I prefer this over the 2 line display of the original.

-) 32kB user memory for more storage in program mode and also longer programs. 

-) Gauss Kronrod integration instead of Simpson rule: Changing to this method allows for solving very difficult integrals
   numerically. The accuracy of this method can be adapted via GK_MAX_DEPTH. Currently it is set for acceptable duration.
   The implemented integration algorithm beats HP42, HP15C and HP41c (with ADVANCED or PPC module integration routines) 
   any time in both speed and accuracy.
   
-) WP34s style solver: the solver of WP34s written in assembler was ported. It is very capable and switches between different
   methods combining bisection, secant, quadratic interpolation, and Ridder's method. The original code can be found here:
   https://sourceforge.net/projects/wp34s/files/ .
   
-) 100 registers in flash ranging from 0-99, storing and recalling via usual commands STO and RCL.

-) 100 registers in RAM ranging from 0-99: storing and recalling via two new commands STR and RCR.

-) 1x1 pixel graphs for more accurate graphs in FPLOT.

-) 5 digit floating point display.

-) wear leveling of flash storage. nice!nanov2 can only delete 4kB pages. In order to be usable for years, a rotating wear-leveling is 
   applied to the whole user storage.
   
-) the new TOP (moveprgtop() in code) command moves a program to top of list in PRG mode: In order to not have to repeatedly UP 
   a program to the top of the list in PRG mode, mainly to FPLOT a function, the PRG chosen directly moves to the top of the list. 
   Sorting is done in RAM, only a single rewrite or user flash is necessary. TOP is on the 9 key (= KEY4 in code) in PRG mode.
   
-) Annunciator (_, f, g) moved to top right corner.

-) Builtin functions use RAM registers instead of flash.

-) CLK function slightly changed, enter hours, minutes, seconds consecutively on stack:
   e.g. 11 ENTER, 22 ENTER, 45 CLK
   starts the clock at 11:22:45. The clock does not use the built-in RTC of the nice!nanov2 but is drift compensated.
   
### More registers

In IVEE2, more registers are usable in flash and in RAM. Both range from 0-99 now. I mainly use RAM regs for intermediate storage of variables in functions, that have to be read and written very quickly. For the flash registers use the usual commands STO and RCL. For the new RAM registers use STR and RCR in a similar fashion. 


### Plotting functions

The new chip has vastly more RAM, so the full screen buffer for SSD1306 is not an issue. 1x1 pixel graphs look a lot finer that the previous 2x2 pixels. A comparison between IVEE and IVEE2 with ln from 0 to 2 is shown in the following image. The new printing algorithm also connects the dots with line elements:

<p align="center">
  <img src="https://github.com/user-attachments/assets/98f39e8e-1504-46ef-aae1-f36a729e2209" style="width:50%; height:auto;" />
</p>

Notice also the size difference, IVEE has been built without CAD (only later the box was modeled 'around' what was already built). IVEE2 was modelled 95% in CAD --> nearly 40% of volume saved.

Just like with IVEE, the function to plot must be placed on top in the program list (use TOP for this in PRG mode). Just enter the limits and plot the function with "0 ENTER 2 FPLOT". 

### Integration algorithm 
Being an RPN calculator enthusiast and aware of their historic significance in science and engineering, I really wanted to include a better algorithm in IVEE2. An algorithm that can even outperform the algorithms present on some of the 'stars' in the RPN sky. There are many examples of functions that cannot be solved adequately with some of the most cherished (and expensive) RPN calculators. To readers of the HP Journal, specifically issue 8/1980, where professor William Kahan presented the capabilties and shortcomings of the integration algorithm developed for HP 34C, which is, to my knowledge, also included in HP 15C, HP41C (Advantage Module) and HP42s, this is not news. Personally, I tested numerous calculators and integration algorithms in user programs of the above mentioned calculators if they are able to solve the following integral numerically, among others. It can be found in HP Journal 8/1980 p. 28 (https://hparchive.com/Journals/HPJ-1980-08.pdf):

$$
\int_{-128}^{128} [u(u^2-47^2)(u^2-88^2)(u^2-117^2)]^2  du = 1.31026895247E28
$$

On first glance, this is solvable analytically, probably by a 16 year old pupil, right? But numerically, this integral is a different story. It is spiky between the roots and reaching large arguments. Any algorithm tackling this must either be able to resolve the spikes with a great number of intervals or be adaptive. To prove this integral is far from trivial numerically, the following image shows some results calculated by some famous RPN calculators:

<p align="center">
  <img src="https://github.com/user-attachments/assets/861c44a0-5399-473c-8537-32584fee2fb1" style="width:50%; height:auto;" />
</p>

HP41c uses the INTEG command from the Advantage module, the others use their onboard integration algorithm. All three calculators fail solving the integral. They are able to solve it in the halved interval from 0 to 128, though. This is also mentioned by Prof. Kahan in above article:

<p align="center">
  <img src="https://github.com/user-attachments/assets/798c3573-2562-4916-b827-b54faa239288" style="width:50%; height:auto;" />
</p>

The only way out with these calculators in solving such integrals is to write your own integration algorithms in user code, die-hard HP41c users might even write MCODE. 
In IVEE2, the above function can make use of RAM register nr. 1 (of course it can be done with the stack too) when entering something like the following. The squares of 47, 88 and 117 are stored in flash registers 97, 98 and 99:

"1, STR, CLR, 1, RCR, 2, POW, 99, RCL, -, 1, RCR, 2, POW, 98, RCL, -, *, 1, RCR, 2, POW, 97, RCL, -, *, 1, RCR, *, 2, POW"

To integrate this function place in on top of the PRG list and enter the limits on the stack: "128 N ENTER ENTER N FINT". 

Let's see how IVEE2 does:
<p align="center">
  <img src="https://github.com/user-attachments/assets/dbb88cfc-3607-41e5-8385-2dc9c3540e19" style="width:50%; height:auto;" />
</p>

Clearly, we find the implemented algorithm can in fact solve this integral. The only downside is the number of digits displayed. Internally, looking at the serial monitor, we will find a vastly better accuracy. On stack level a we find the absolute error of the calculation, stack level b shows the result. 

### Solver/Root finder

The solver implemented is, more or less, an exact port from wp34s (written in Assembler, the source can be found here: https://sourceforge.net/projects/wp34s/files/) to C++. It is a very capable solver, combining and switching between bisection, secant, quadratic interpolation, and Ridder's method. Like with integration (FINT), the function of interesest must be placed on top of the PRG list. To find the root of a function enter a start interval (X and Y) and press FSOLVE. There are measures in place, that start the solver anyway with random values, if only one or no value is on the stack. However, in a function with multiple roots you will likely not find the desired root (or none at all!) if no interval is given. For example, with above function we get:

"40 ENTER 80 FSOLVE" --> "7.17527E-13"

"50 FSOLVE" --> "47"

"90 FSOLVE" --> "88"

"118 FSOLVE" --> "117"


Clearly, the better you already know your function, the more likely you will find all roots. 


## Programming

Programming is basically unchanged from IVEE but with a lot more storage in flash, so I will just give a brief description. If you are familiar with programming the HP41c or HP42s, then you will not have any difficulties. Programs are entered RPN style, like on HP41C, but we can also use the menu like on HP42s (similar but a bit different. We can choose functions from the DICT or USER menu). The main difference to the RPN style on HP41c is that we don't need RTN or VIEW to present a value on the stack, we also don't have the function LBL within the function itself. The name of the function is outside the function/program. Already included complex number capabilites for every function are an improvement over HP41c, but displaying is not as convenient as on HP42s (HP42s: single line complex numbers, but reduced number of digits, IVEE2: two line display of complex numbers). For example, the program "0, SWP, CPX, EXP" returns EXP(i*x). And indeed, "PI F(X)" gives "a -1, i -8.74228E-8", the complex identity 

$$
\exp(i*\pi) = -1
$$

with a bit of a rounding error in the imaginary part, though. 

Now, let us test a version of the TVM formula with monthly payments (we use the notation on the HP12c):

$$
FV(n) = PV * (1+i)^n + PMT * [(1+i)^n − 1)/i]
$$


i...yearly interest rate

n...months

FV...future value

PMT...monthly payment

PV...present value

Entering this on IVEE2 we have different options. Making use of flash and RAM registers we may enter a formula in PRG mode like this:

"0, STR, CLR, 0, RCL, 1, +, 0, RCR, POW, 1, RCL, *, 0, RCL, 1, +, 0, RCR, POW, 1, -, 0, RCL, /, 2, RCL, *, +, 3, RCL, -"

First we store n in RAM reg nr. 0, we CLR the stack, then enter the formula. When we put it on top in PRG mode, we can use FSOLV (fS internally) to solve for n at FV = 0, that is, we want to find the number of months we have to pay back the 100 bucks. For our example we use the follwing numbers in 4 flash registers 0-3:

i = 0.08 ("0.08 ENTER 0 STO")

PV = 1000 ("1000 ENTER 1 STO")

PMT = -100 ("100 N ENTER 2 STO")

FV = 0 ("0 ENTER 3 STO")

Just pressing FSOLV will yield: 20.921237. Comparing with HP12c (i=8, PV=1000, PMT=-100, FV=0) pressing the n-button:

<p align="center">
  <img src="https://github.com/user-attachments/assets/64ddbc61-46fd-4b40-aad4-4a7c078e7a77" style="width:50%; height:auto;" />
</p>

The results match as HP12c rounds up to the next integer. From this funny little example we have learned that an interest rate of 8% is quite high and we have to pay more than double the amount back. Horrible!


## Hardware

Of couse, a one-off model like this calculator is destined for 3D printing. In the shared folder 'hardware' you'll find a FreeCAD file with the latest design. Everything is modeled in CAD. This can be quite deceiving: thinking you have modeled 'everything', you forget about stupid mistakes to be made with stuff you didn't model. This happened here with the wiring. Any sane person would not model this in 3D, the designing of every wire alone would take too long, with limited benefit. But here it would have helped: the distance between MCU board and battery is crucial for the position of the USB-C cutout in the housing! Forgetting about this or not leaving enough room for wires that bend and need space, has left me with a MCU board position, that did not quite fit the modeled USB-cutout. A very basic and stupid mistake. :-(

Overview of the CAD:
<p align="center">
  <img src="https://github.com/user-attachments/assets/bfd7f31b-daef-47b2-bc88-482e52292145" style="width:50%; height:auto;" />
</p>

View of the parts inside:

<p align="center">
  <img src="https://github.com/user-attachments/assets/dc776e3c-7f7a-4126-8034-a8945dfebc21" style="width:50%; height:auto;" />
</p>

The 3D printed parts are all PETG except the display frame which was made in PLA. The parts are:

-) housing

-) display frame

-) MCU board adapter (includes a tiny socket for a reset button)

-) keyboard matrix (a thin matrix covering the buttons and leaving a flat surface for a thin printed keyboard overlay)

The layout of the calculator is based around a perforated circuit board with standoffs to the bottom for making the necessary space for wiring, battery and MCU board. The MCU board is mounted onto an adapter which is in turn mounted to the circuit board. Everything is made to last and to be replaceable. The keys are quite generic, as is the SSD1306 display. The whole assembly is held in the housing with 6 magnets glued into the housing. The battery is glued in place, as is the display. Screw heads are covered with the display frame and the keyboard matrix, respectively. The inner workings of my build look like this:

<p align="center">
  <img src="https://github.com/user-attachments/assets/030ba5bf-cb78-4fdb-bcee-62c573c99cb4" style="width:50%; height:auto;" />
</p>


Try to keep it nice and tidy, a long battery cable pays off in the end and during the build. 
The keyboard matrix shown in these pictures is still the old version. 

Recommendations for 3D printing:
If you can, use at least PETG on a TXT sheet (not as brittle as PLA, a PLA housing can break when falling off a table). Print a 5mm brim or mouse ears at the corners of the housing as it is quite large. Also use a brim on the keyboard matrix, the adapter is quite small, maybe a smaller brim will do also. The display frame in PLA does not need a brim, it sticks very well to a PEI sheet. Use structural mode and 3 perimeters for the housing and two perimeters for the keyboard matrix (the see some mechanical loads), maybe even 0.15m layers. The keyboard matrix must stay somewhat flexible for a lower button force. There are even variants of the housing in the FreeCAD file that include a brim in CAD. Feel free to use this or add a brim in your slicer. 

### Additional Parts needed

-) Cables

-) wires

-) 6x standoff nuts for short M3 * 5mm screws

-) 6x 4 * 4 *2mm magnets

-) 6x M3 * 5mm screws

-) 1x perforated PCB, prefer the ones with lugged holes, not the ones with just holes. The lugs make soldering something to the holes easier

-) double-sided tape, also to correct height differences

-) hot glue

-) Kapton tape for isolation where needed

-) 2x resistor for the voltage divider, here 2x 10k (not good regarding voltage resulotion but ok)

-) 1x On/Off switch

-) USB-C cable for flashing and charging the LiIon battery

-) 1x 4 * 4mm reset button, fits the twerp on the adapter for the MU

-) optional phone charger 



# ToDos

-) new keyboard overlay to be designed fitting this keyboard layout. The layout shown in the images is still the old design. I will upload the files when this is completed.

-) anything important that comes up, we'll see.

maybes:

-) perhaps move the annunciator back to the old position for base modes other than scientific.

-) the statistical functions were not tested, because I do not need them. With the increased flash registers there are much more possibilities.

-) perhaps use the onboard RTC for the clock and maybe implement a stopwatch, date and time functions. Maybe similar to TIME module on HP41c. 

Have fun,

emefff@gmx.at




