<p align="center">
    ECE3232 Lab 5
</p>

# Overview
This project is a remake of [Bop It](https://en.wikipedia.org/wiki/Bop_It).

# How To Play The Game
1. Wait for an LED to light up.
2. Depending on which LED lit up, you must "bop" (press) the corresponding button, or "twist" the corresponding potentiometer, or "pull" (move) the joystick in any direction.
3. Depending on the difficulty chosen, you must do so within either 5 seconds (easy mode), 3 seconds (normal mode), or 1 second (hard mode). If you fail to "bop", "twist", or "pull" within the time limit, the system will be reset and the difficulty will be changed back to easy mode.
4. You may change the difficulty (easy, normal, or hard) at any time by pressing the difficulty button.

# Requirements
1. At least two digital inputs and two digital outputs.
This system has two buttons (which fulfills the requirement of two digital inputs), and nine different LEDs (which fulfills the requirement of two digital outputs).
2. At least one maskable interrupt (traps such as MathError do not count, though you are encouraged to use these for debugging, etc.).
This system enables and uses the Timer1 Interrupt. The custom Timer1 Interrupt function used in this sytem can be seen [here](https://github.com/edwardchang7/ECE3232Lab5/blob/12cfb4f01e3da6e94781947da125da4091cae131/ECE3232Lab5.X/main.c#L209).
3. Use of the internal FRC with a divided clock frequency, i.e., a lower clock frequency than was used in Labs 1-3
The internal FRC clock is set to its default 8 MHz value. However, this system uses the Fosc value that is generated from the internal FRC clock. More specifically, the value of Fosc/2 is what the Timer1 module uses to drive its timer functionality.
4. At least one timer, e.g., using Timer 1 or the SCCP/MCCP timer modules.
This system uses one Timer1 module.
- Clock source = Fosc/2 (4 MHz)
- Prescaler = 1:256
- Interrupt = enabled
- Timer Period = 1 second
5. At least three difficulty levels, e.g., normal, easy, and hard.
The system's difficulty button can be pressed to change the difficulty of the game.
6. Assuming your design meets the above requirements, bonus marks will be awarded if your design incorporates an analog input, an analog output, or a serial interface (+10% each).
Analog input = Two potentiometers + One joystick (ADC)

# Contributors
- Edward Chang | [LinkedIn](https://www.linkedin.com/in/edwardwonsukchang/)
- Tully Masterson
