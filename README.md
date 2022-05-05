# Saccadic Fixator
Saccadic Fixator project I made on 2020-2021 for my optometrist friend.

A Saccadic Fixator ,also called the "Circle of Lights", is the standard for testing , evaluating, and developing accurate and rapid eye-hand coordination, spatial integration, and reaction times.
Athletic trainers, developmental vision optometrists, and rehabilitation therapists have acclaimed the Fixator for it's ability to motivate users. 

Dimentions: 40x40 inches

Components used:
1- Arduino Mega 
4 - 7-segment displays
4 - 74HC595 8-bit shift registers
33 - 1 kOhm resistors
33 - 2N2222 transistors
33 - CHILONG brand circular arcade LED push button
1 - 12V power brick (DC otput jack)

Libraries used:
<ShiftOutX.h> and <ShiftPinNo.h> by Juan Hernandez(https://codebender.cc/library/ShiftOutX#ShiftOutX.h) were used to drive the 74HC595 shift registers.

Program flow:
1. User is prompted via the 2 blue buttons lighted up near the 7 segment display to select timer countdown duration, either 30 or 60 seconds.
2. Program and countdown starts when the center yellow button is pressed.
3. The 32 LED buttons will light up one at a time at random, and will only progress to the next once the correct lighted-up LED button is pressed.
4. Correctly pressed buttons will be tallied on the 7-segment display of the second row.
5. Time remaining (assuming the user finsihed before the time limit) is displayed on the 7-segment display on the first row.
6. Mistakes made are also tallied, but must be toggled via the lighted-up blue button at the finish sequence. Mistakes tallied are displayed on the 7-segment display on the first row.
7. Press the yellow center button to exit finish sequence and go back to the start sequence. 





