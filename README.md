<h1 align="center">ECE3232 Lab #5</h1>

## Overview
This project is a remake of [Bop It](https://en.wikipedia.org/wiki/Bop_It).

## How To Play The Game
1. Wait for an LED to light up.
2. Depending on which LED lit up, you must "bop" (press) the corresponding button, or "twist" the corresponding potentiometer, or "pull" (move) the joystick in any direction.
3. Depending on the difficulty chosen, you must do so within either 5 seconds (easy mode), 3 seconds (normal mode), or 1 second (hard mode). If you fail to "bop", "twist", or "pull" within the time limit, the system will be reset and the difficulty will be changed back to easy mode.
4. You may change the difficulty (easy, normal, or hard) at any time by holding the difficulty button until the three difficulty LEDs flash. Once the three difficulty LEDs flash, you may let go of the difficulty button.

## How Requirements Are Satisfied
1. **At least two digital inputs and two digital outputs.**
    - There are three buttons (which fulfills the requirement of two digital inputs), and nine different LEDs (which fulfills the requirement of two digital outputs) used in this system.
2. **At least one maskable interrupt (traps such as MathError do not count, though you are encouraged to use these for debugging, etc.).**
    - This system enables and uses the Timer1 Interrupt. The custom Timer1 Interrupt function used in this system can be seen [here](https://github.com/edwardchang7/ECE3232Lab5/blob/12cfb4f01e3da6e94781947da125da4091cae131/ECE3232Lab5.X/main.c#L209). This custom Timer1 Interrupt function is then passed onto the Timer1 module using the Timer1 module's [TMR1_SetInterruptHandler() function](https://github.com/edwardchang7/ECE3232Lab5/blob/2f87466221cae17f1e8918ed7596e499af5d830e/ECE3232Lab5.X/main.c#L221) to replace its existing interrupt handler function with the custom Timer1 Interrupt function.
3. **Use of the internal FRC with a divided clock frequency, i.e., a lower clock frequency than was used in Labs 1-3.**
    - The internal FRC clock is set to its default 8 MHz value:
    - ![Image](https://i.imgur.com/Tz2tdlz.png)
    - However, this system uses the Fosc value that is generated from the internal FRC clock. More specifically, the value of Fosc/2 is what the Timer1 module really uses to execute its timer functionalities:
    - ![Image](https://i.imgur.com/PHEMyIo.png)
    - Therefore, the custom Timer1 Interrupt function is executed every one second.
4. **At least one timer, e.g., using Timer 1 or the SCCP/MCCP timer modules.**
    - This system uses one Timer1 module as shown in the picture above.
        - Clock source = Fosc/2 (4 MHz)
        - Prescaler = 1:256
        - Interrupt = enabled
        - Timer Period = 1 second
5. **At least three difficulty levels, e.g., normal, easy, and hard.**
    - This system has the three difficulties (easy, normal, hard) and the difficulty can be changed by pressing a button. As mentioned before, you must "bop", "twist", or "pull" (when an LED lights up) within 5 seconds for easy mode, 3 seconds for normal mode, and 1 second for hard mode.
6. **Assuming your design meets the above requirements, bonus marks will be awarded if your design incorporates an analog input, an analog output, or a serial interface (+10% each).**
    - This system has three analog inputs: two potentiometers and one joystick. Analog-to-Digital Converter (ADC) was used to collect and process the input gained from these analog inputs.

## Contributors
- Edward Chang | [LinkedIn](https://www.linkedin.com/in/edwardwonsukchang/)
- Tully Masterson
