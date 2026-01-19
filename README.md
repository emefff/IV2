# IVEE2 - a Powerful Programable RPN Calculator based on IVEE

## Purpose
IVEE2 is a port of IVEE to nice!nanov2. It includes some changes that make use of the faster MCU, more RAM and flash space. The main driver behind this was the limited space left on the original IVEE on ProMicro. As far as I remember only one single byte was free on the original calculator. The original code of IVEE can be found in

https://github.com/zooxo/iv

Much of it still present in IVEE2, I am publishing this with kind permission of the original coder, whom I hereby want to thank for his great work. I built his calculator many years ago, mostly by hand without any usage of CAD apart from the housing, which turned out to be rather big and clumsy. Being a fan of RPN calculators and especially the HP lineup, it gives me great pleasure to having bit this bullet and come up with this and being able to present a codebase, that has, in my opinion, great possiblities for the future on this chip. 
Nice!nanov2 is probably best known in the DIY keyboard builder community, where it is used for programmable keyboards and the like. It also has BLE capbility and uses very low power, has an onboard LiIon charging controller and USB-C connectivity. Thus, I can even imagine OTA capability for flashing and in operation for this device. Apart from that, I think of it as a worthy replacement for ESP32 in a lot of scenarios, especially for devices on battery power. 
Being an amateur programming and software enthusiast I used Kilo Code with mostly Codex-5.1, MiniMax-M2.1 and Claude Sonnet 4.5 for developing IVEE2. From this rather overwhelming experience I must say, I do not see a bright future for programmers. The evolution of the codebase was rapid, it took me about 2.5 weeks in Dec. 2025 with just 2-3 hours per day (more on weekends though). The debugging features are, from the viewpoint of an amateur C++ programmer, vastly superior than I imagined. On the other hand, being an amateur, I probably do not see where code has turned out to be laborious and perhaps inefficient. My approach to this was rather practical and user driven. Two of the used coding agents where very analytical in their approach to developing new functions or debugging. In the end, the codebase includes a great amount of diagnostic code, readily available for anyone to dive in. 

## Recommendations

From my experience, I started this project with Arduino, PlatformIO is the much better IDE in conjunction with  nice!nanov2. With Arduino I was not even able to upload anything, some knock-off boards do not even allow uploading .uf2 binaries, although they claim being able. Thus I recommend anyone building his/her own IVEE2 to use:

-) PlatformIO as IDE, my platformio.ini is included.

-) the original Nice!NanoV2. Granted, it is not cheap, but some of the knock-offs clearly do not work. 

-) also solder a reset button to the board. Sometimes it needs two resets before uploading the firmware.

-) a very common LiIon battery like the one used here, which can be found anywhere on this planet. If your battery capacity is even higher, nice!nanov2 allows to solder a resistor to change the charging current to 500mA instead of the standard 100mA 
(fine by me, increases battery life but takes longer to charge)

## Changes in IVEE2

-) Integration algorithm: Being an RPN calculator enthusiast and being aware of their historic significance in science and engineering, I really wanted to include a better algorithm in IVEE2. An algorithm that can even outperform the algorithms present on some of the 'stars' in the RPN sky. There are many examples of functions that cannot be solved adequately with some of the most cherished (and expensive) RPN calculators. To readers of the HP Journal, specifically issue 8/1980, where professor William Kahan presented the capabilties and shortcomings of the integration algorithm developed for HP 34C, which is, to my knowledge, also included in HP 15C, HP41C (Advantage Module) and HP42s, this is not news. Personally, I tested numerous calculators and integration algorithms in user programs of the above mentioned calculators if they are able to solve the following integral numerically among others. It can be found in HP Journal 8/1980 p. 28:

$$
\int_{-128}^{128} [u(u^2-47^2)(u^2-88^2)(u^2-117^2)]^2 \, du = 1.31026895247E28
$$





