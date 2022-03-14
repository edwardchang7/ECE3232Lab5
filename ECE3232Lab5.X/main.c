/**
 * File: main.c
 * Summary: main code for running "Boop It! Reaction Game".
 * 
 * Description: 
 * "Boop It! Reaction Game" main code, game is played by user 
 * reacting to LED lights on the boards. 
 * game consists of 6 game LEDs, 3 difficulty setting LEDs, 2 game buttons, 
 * 1 difficulty setting button,2 potentiometers, 2 game buttons, and 2 joysticks.
 * Game uses interrupts from a 4MHz clock timer to count to desired game 
 * difficulty reaction time setting, being: easy = 5s, normal = 3s, hard = 2s.
 * 
 * Authors: Edward Chang, Tully Masterson
 * Date: March, 19, 2022
 */

#define FCY 4000000UL   // FCY set to 4MHz

#include "mcc_generated_files/system.h"
#include "mcc_generated_files/tmr1.h"
#include "mcc_generated_files/adc1.h"

#include <xc.h>
#include <libpic30.h>

// global variables declarations and initialization
bool isTimerOn = false;
int current_timer = 0;
int current_difficulty = 1; // easy = 1, normal = 2, hard = 3
int difficulty_timer_value = 5; // easy = 5s, normal = 3s, hard = 2s
int score = 0;

int previousLefthandPotentiometerValue;
int previousRighthandPotentiometerValue;
int previousJoystickXAxisValue;
int previousJoystickYAxisValue;


/**
 * convert a measured voltage from a analog input and converts 
 * it to a digital representation
 * 
 * Author: Edward Chang
 * 
 * @param channel - channel to be converted to digital representation
 * @return converted_digital_value - digital representation of analog input
 */
int apply_adc(ADC1_CHANNEL channel) {
    ADC1_Enable();
    ADC1_ChannelSelect(channel);
    ADC1_SoftwareTriggerEnable();
    __delay_ms(1); // TODO: change to a better method of delay
    ADC1_SoftwareTriggerDisable();
    
    while(!ADC1_IsConversionComplete(channel));
    int converted_digital_value = ADC1_ConversionResultGet(channel);
    ADC1_Disable();
    
    return converted_digital_value;
}

/**
 * turns off all the LEDs
 * 
 * Author: Edward Chang
 */
void turnOffAllLEDs() {
    LATCbits.LATC0 = 0; // Turn off Right-hand Button-LED
    LATBbits.LATB7 = 0; // Turn off Left-hand Button-LED
    LATCbits.LATC13 = 0; // Turn off Left-hand Potentiometer-LED
    LATCbits.LATC14 = 0; // Turn off Right-hand Potentiometer-LED
    LATBbits.LATB14 = 0; // Turn off Easy Difficulty LED
    LATBbits.LATB15 = 0; // Normal Difficulty LED
    LATCbits.LATC12 = 0;// Turn off Hard Difficulty LED
    LATDbits.LATD4 = 0; // Turn off Right-hand Joystick-LED
    LATDbits.LATD3 = 0; // Turn off Left-hand Joystick-LED
}

/**
 * flash LEDs in a loop 5 times
 * 
 * Author: Edward Chang
 */
void flashDifficultyLEDs() {
    turnOffAllLEDs();
    __delay_ms(100);

    for(int i = 0; i < 5; i++) {
        LATBbits.LATB14 = 1;
        LATBbits.LATB15 = 0;
        LATCbits.LATC12 = 0;
        __delay_ms(100);
        LATBbits.LATB14 = 0;
        LATBbits.LATB15 = 1;
        LATCbits.LATC12 = 0;
        __delay_ms(100);
        LATBbits.LATB14 = 0;
        LATBbits.LATB15 = 0;
        LATCbits.LATC12 = 1;
        __delay_ms(100);
    }
}

/**
 * Set methods for game difficulty
 * 
 * Author: Edward Chang
 */
void setDifficultyToEasy() {
    LATBbits.LATB14 = 1;
    LATBbits.LATB15 = 0;
    LATCbits.LATC12 = 0;
    
    current_difficulty = 1; // easy = 1, normal = 2, hard = 3
    difficulty_timer_value = 5; // easy = 5s, normal = 3s, hard = 2s
}
void setDifficultyToNormal() {
    LATBbits.LATB14 = 0;
    LATBbits.LATB15 = 1;
    LATCbits.LATC12 = 0;
    
    current_difficulty = 2; // easy = 1, normal = 2, hard = 3
    difficulty_timer_value = 3; // easy = 5s, normal = 3s, hard = 2s
}
void setDifficultyToHard() {
    LATBbits.LATB14 = 0;
    LATBbits.LATB15 = 0;
    LATCbits.LATC12 = 1;
    
    current_difficulty = 3; // easy = 1, normal = 2, hard = 3
    difficulty_timer_value = 2; // easy = 5s, normal = 3s, hard = 2s
}

/* 
 * Generates a random action based on the random number that is generated
 * between 1 and 5 (inclusive)
 * 
 * 1 = Left-hand Button
 * 2 = Left-hand Potentiometer
 * 3 = Right-hand Potentiometer
 * 4 = Right-hand Button
 * 5 = Joystick
 * 
 * Author: Edward Chang
 */
int generateRandomAction() {
    int randomNumber = (rand() % 5) + 1;
    
    previousLefthandPotentiometerValue = apply_adc(left_potent);
    previousRighthandPotentiometerValue = apply_adc(right_potent);
    previousJoystickXAxisValue = apply_adc(joystick_x_axis);
    previousJoystickYAxisValue = apply_adc(joystick_y_axis);

    switch(randomNumber) {
        case 1:
            LATBbits.LATB7 = 1; // Turn on Left-hand Button-LED
            break;
        case 2:
            LATCbits.LATC13 = 1; // Turn on Left-hand Potentiometer-LED
            break;
        case 3:
            LATCbits.LATC14 = 1; // Turn on Right-hand Potentiometer-LED
            break;
        case 4:
            LATCbits.LATC0 = 1; // Turn on Right-hand Button-LED
            break;
        case 5:
            LATDbits.LATD3 = 1; // Turn on Left-hand Joystick-LED
            LATDbits.LATD4 = 1; // Turn on Right-hand Joystick-LED
            break;
    }
    
    TMR1_Start();
    isTimerOn = true;
    
    return randomNumber;
}

/**
 * Stops the game and sets game values to its default states
 * 
 * Author: Edward Chang
 */
void systemReset() {
    flashDifficultyLEDs();
    setDifficultyToEasy();
    
    TMR1_Stop();
    isTimerOn = false;
    current_timer = 0;
    score = 0;
}

/**
 * Set pins to desired format
 * 
 * Author: Edward Chang
 */
void setPins() {
    // AN (Microbus A) - Right-hand Button-LED
    ANSELCbits.ANSELC0 = 0;
    TRISCbits.TRISC0 = 0;
    
    // RST (Microbus A) - Right-hand Button
    ANSELCbits.ANSELC7 = 0;
    TRISCbits.TRISC7 = 1;
    
    // SCK (Microbus A) - Left-hand Button-LED
    ANSELBbits.ANSELB7 = 0;
    TRISBbits.TRISB7 = 0;
    
    // CS (Microbus A) - Left-hand Button
    ANSELBbits.ANSELB2 = 0;
    TRISBbits.TRISB2 = 1;
    
    // PWM (Microbus A) - Left-hand Potentiometer-LED
    TRISCbits.TRISC13 = 0;
    
    // INT (Microbus A) - Right-hand Potentiometer-LED
    TRISCbits.TRISC14 = 0;
    
    // AN (Microbus B) - Joystick X-axis
    ANSELDbits.ANSELD10 = 1;
    TRISDbits.TRISD10 = 1;
    
    // RST (Microbus B) - Joystick Y-axis
    ANSELDbits.ANSELD11 = 1;
    TRISDbits.TRISD11 = 1;
    
    // SDA (Microbus B) - Joystick Button
    TRISCbits.TRISC8 = 1;
    
    // MOSI (Microbus B) - Left-hand Potentiometer
    ANSELCbits.ANSELC1 = 1;
    TRISCbits.TRISC1 = 1;
    
    // MISO (Microbus B) - Right-hand Potentiometer
    ANSELCbits.ANSELC2 = 1;
    TRISCbits.TRISC2 = 1;
    
    // CS (Microbus B) - Difficulty Button
    ANSELCbits.ANSELC3 = 0;
    TRISCbits.TRISC3 = 1;
    
    // RX (Microbus B) - Easy Difficulty LED
    TRISBbits.TRISB14 = 0;
    
    // INT (Microbus B) - Normal Difficulty LED
    TRISBbits.TRISB15 = 0;
    
    // PWM (Microbus B) - Hard Difficulty LED
    TRISCbits.TRISC12 = 0;
    
    // SDA (Microbus A) - Right-hand Joystick-LED
    TRISDbits.TRISD4 = 0;
    
    // SCL (Microbus A) - Left-hand Joystick-LED
    TRISDbits.TRISD3 = 0;
}

/**
 * Handles Timer interrupts to check is game time has run out
 * ends game if difficuly_timer_val is passed
 * 
 * Author: Edward Chang
 */
void Timer1InterruptHandler(void);
void Timer1InterruptHandler(void) {
    current_timer++;
    
    if(current_timer >= difficulty_timer_value) {
        systemReset();
    }
}

/**
 * stop TMR1 and increment score 
 * 
 * Author: Edward Chang
 * Edited: Tully Masterson
 */
void success(){
    TMR1_Stop();
    score++;
    current_timer = 0;
    isTimerOn = false;
}

/**
 * change Difficulty setting
 * 
 * Author: Edward Chang
 * Edited: Tully Masterson
 */
void changeDifficulty(){
    TMR1_Stop();
    isTimerOn = false;
    current_timer = 0;
    score = 0;
    flashDifficultyLEDs();

    switch(current_difficulty) {
        case 1: // Easy -> Normal
            setDifficultyToNormal();
            break;
        case 2: // Normal -> Hard
            setDifficultyToHard();
            break;
        case 3: // Hard -> Normal
            setDifficultyToEasy();
            break;
    }
    __delay_ms(1000);
    continue;
}

/**
 * change Difficulty setting
 * 
 * Author: Edward Chang
 * Edited: Tully Masterson
 */
void checkForSuccess(int ranAct,
                    int currentLefthandPotentiometerValue,
                    int currentRighthandPotentiometerValue,
                    int currentJoystickXAxisValue,
                    int currentJoystickYAxisValue){
    switch(ranAct){
        case 1:
            // If the Left-hand Button was "booped" on time...
            if(PORTBbits.RB2 == 0){
                LATBbits.LATB7 = 0; // Turn off Left-hand Button-LED
                success();
            }
            break;
        case 2:
            // if left potentiometer was moved on time
            if(currentLefthandPotentiometerValue >= 
                    previousLefthandPotentiometerValue + 100 ||
                    currentLefthandPotentiometerValue <= 
                    previousLefthandPotentiometerValue - 100){
                LATCbits.LATC13 = 0; // Turn off Left-hand Potentiometer-LED
                success();      
            }
            break;
        case 3:
            // if right potentiometer was moved on time
            if((currentRighthandPotentiometerValue >= 
                    previousRighthandPotentiometerValue + 100 ||
                    currentRighthandPotentiometerValue <= 
                    previousRighthandPotentiometerValue - 100)){
                LATCbits.LATC14 = 0; // Turn off Right-hand Potentiometer-LED
                success();
            }
            break;
        case 4:
            // If the Right-hand Button was "booped" on time...
            if(PORTCbits.RC7 == 0){
                LATCbits.LATC0 = 0; // Turn off Right-hand Button-LED
                success();
            }
            break;
        case 5:
            // If the joystick was moved on time...
            if((currentJoystickXAxisValue >= previousJoystickXAxisValue + 100 ||
                    currentJoystickXAxisValue <= previousJoystickXAxisValue - 100 ||
                    currentJoystickYAxisValue >= previousJoystickYAxisValue + 100 ||
                    currentJoystickYAxisValue <= previousJoystickYAxisValue - 100)){
                LATDbits.LATD3 = 0; // Turn off Left-hand Joystick-LED
                LATDbits.LATD4 = 0; // Turn off Right-hand Joystick-LED
                success();
            }
            break;    
    }  
}

/**
 * main method
 * 
 * Author: Edward Chang
 * Edited: Tully Masterson
 */
int main(void) {
    // Initialize and setup the system
    SYSTEM_Initialize();
    TMR1_SetInterruptHandler(Timer1InterruptHandler);
    setPins();
    turnOffAllLEDs();
    setDifficultyToEasy();
    
    // Initialize local variables
    int randomAction;
 
    // game loop
    while(1) {
        if(PORTCbits.RC3 == 0) { // If the difficulty button was pressed...
            changeDifficulty();
        }
        
        // check for success in game if timer is on 
        // if timer is off generate new randomAction and start timer 
        if(isTimerOn){
            checkForSuccess(randomAction,
                    apply_adc(left_potent), 
                    apply_adc(right_potent), 
                    apply_adc(joystick_x_axis), 
                    apply_adc(joystick_y_axis));
        }
        else{
            __delay_ms(3000);
            randomAction = generateRandomAction();
        }
    }
    return 0; 
}
