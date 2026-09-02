# MonkiiPad20 PCB

This is the circuit board I designed for the MonkiiPad20. It turns the pile of switches, diodes and a controller into one tidy piece instead of a nest of hand wiring. Twenty switch spots, a diode under each one, an RP2040 footprint in the corner and a little header for the OLED, all on a two layer board.

![The finished MonkiiPad20 board on a cutting mat, switch pads labelled SW1 through SW20 with the RP2040 pin header down the right side](../Medias/MonkiiPad20/MonkiiPad20_PCB/MonkiiPad20_PCB.jpg)

## Downloading the files

If you just want to get this board made, **grab `MonkiiPad20_PCB.zip`**. That one zip holds everything a fab house needs, all the Gerber layers plus the two drill files, and most manufacturers want it handed over exactly like that, as a single archive. Drop the zip into their site, pick your options and you are done.

The loose `.gbr` and `.drl` files sitting next to the zip are the same data unpacked. They are there if you want to peek at a layer or load it into a Gerber viewer, but for ordering you only need the zip.

## What the board is

It came out of KiCad as a two layer board, 120 by 86 millimetres, 1.6 mm thick, with green soldermask. Nothing exotic in there, so any of the usual cheap fab houses will run it without complaint.

![The back of the board showing the diode footprints, each marked with a K for the cathode, and the RP2040 pin map in the corner](../Medias/MonkiiPad20/MonkiiPad20_PCB/MonkiiPad20_PCB_2.jpg)

## The switches and diodes

The twenty switch positions are laid out four rows by six columns and numbered SW1 through SW20 on the silkscreen, so you always know which one you are looking at. Every switch gets its own diode, and the footprint sits right beside it with the cathode end marked K. That little mark matters, since diodes only go one way, so line the band on the diode up with the K and you will not have to desolder anything later. The diodes are what give the pad its clean matrix and let you mash several keys at once without ghosting.

![A close angle across the board with the diode outlines and the row and column traces running between the switches](../Medias/MonkiiPad20/MonkiiPad20_PCB/MonkiiPad20_PCB_1.jpg)

## The controller

Up in the corner is the footprint for the Waveshare RP2040-Zero. I printed the pin names right onto the silkscreen, the power pins down one side as 5V, GND and 3V3, and the GPIO numbers running around the rest, so you can seat the controller and check your wiring against the board itself instead of digging out a pinout. There is also a small header broken out for the OLED so the screen has a clean place to land.

![The controller footprint up close with the pin labels reading 5V, GND, 3V3 and the numbered GPIO pins printed on the board](../Medias/MonkiiPad20/MonkiiPad20_PCB/MonkiiPad20_PCB_3.jpg)

## Getting it made

Send the zip to whichever fab you like, the defaults are fine, two layers and 1.6 mm as above. When the boards land, solder the diodes first while the board is still flat and easy to work on, watching that K mark, then the switches, then the controller and the OLED header. After that you are ready to flash the firmware and drop it into the case.

![The board catching the light at an angle with the MonkiiBoard logo printed on the silkscreen](../Medias/MonkiiPad20/MonkiiPad20_PCB/MonkiiPad20_PCB_4.jpg)

## What it looks like built

Here is one I actually populated, every switch socketed and the OLED window sitting flush in the corner.

![The assembled MonkiiPad20 PCB with every switch socket filled and the OLED window mounted at the top right](../Medias/MonkiiPad20/MonkiiPad20_PCB/MonkiiPad20_PCB_assembled.jpg)

A closer look at the diodes once they are actually soldered down instead of just sitting as a footprint. Every one bent flat and facing the same way, matching the K printed beside it.

![Close up on a row of soldered diodes on the back of the assembled board, each one seated flat and facing the same direction](../Medias/MonkiiPad20/MonkiiPad20_PCB/MonkiiPad20_PCB_assembled_diodes.jpg)

The RP2040-Zero seated on its header, its own silkscreened pin labels lining up right against the ones printed on the board underneath.

![The RP2040-Zero controller soldered onto the board, its own pin labels matching the ones printed on the board's silkscreen](../Medias/MonkiiPad20/MonkiiPad20_PCB/MonkiiPad20_PCB_assembled_MCU.jpg)

And the whole back once everything is in, controller and every diode soldered.

![The back of the fully assembled board with the RP2040-Zero and every diode soldered in place](../Medias/MonkiiPad20/MonkiiPad20_PCB/MonkiiPad20_PCB_assembled_bottom_view.jpg)
