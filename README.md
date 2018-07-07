# putter_scoreboard


## Operation
### Definitions
Button A – Top Left Remote Button

Button B – Top Right Remote Button

Button C – Bottom Left Remote Button

Button D – Bottom Right Remote Button

Timer – Top Display (2 Digits)

Score – Bottom Display (2 Digits)



Timer Display starts at 30 –

Score Display starts at 00 –

### Press Button A
* Starts timer countdown from 30 to 00.
* Buzzer Sounds for .5s at initial button press
* Pauses timer while between 30 and 00, B button resets timer if paused
* Starts timer when paused.

### Press Button B
* Reset timer if paused.

### Hold Button A and B together for 3 seconds
* Resets high score to 00

### Press Button D
* Adds to Score

### Hold Button D
* Displays High Score for 2 Seconds
* **(this might work better if it displays the high score while the button is held down and only if the timer isn't running)**

### Press Button C
* Subtracts from Score

### Hold Button C
* **(for how long? not quite sure the purpose of this function)**
* Resets Score

### While Running, No Button Press
* When Timer reaches 00, buzzer sounds.
* 2 seconds of buzzer after 00
* 5 seconds after 00, clock resets to 30
* If Score at 00 is higher than previous high score, save high score
