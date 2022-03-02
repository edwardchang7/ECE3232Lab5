#define FCY 4000000UL

#include "mcc_generated_files/system.h"
#include "mcc_generated_files/tmr1.h"
#include "mcc_generated_files/adc1.h"

#include <xc.h>
#include <libpic30.h>

bool isTimerOn = false;
int current_timer = 0;
int current_difficulty = 1; // easy = 1, normal = 2, hard = 3
int difficulty_timer_value = 5; // easy = 5s, normal = 3s, hard = 2s
int score = 0;

int previousLefthandPotentiometerValue;
int previousRighthandPotentiometerValue;
int previousJoystickXAxisValue;
int previousJoystickYAxisValue;

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

void systemReset() {
    flashDifficultyLEDs();
    setDifficultyToEasy();
    
    TMR1_Stop();
    isTimerOn = false;
    current_timer = 0;
    score = 0;
}

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

void Timer1InterruptHandler(void);
void Timer1InterruptHandler(void) {
    current_timer++;
    
    if(current_timer >= difficulty_timer_value) {
        systemReset();
    }
}

int main(void) {
    // Initialize and setup the system
    SYSTEM_Initialize();
    TMR1_SetInterruptHandler(Timer1InterruptHandler);
    setPins();
    turnOffAllLEDs();
    setDifficultyToEasy();
    
    // Initialize local variables
    int randomAction;
    
    int currentLefthandPotentiometerValue;
    int currentRighthandPotentiometerValue;
    int currentJoystickXAxisValue;
    int currentJoystickYAxisValue;
    
    while(1) {
        if(PORTCbits.RC3 == 0) { // If the difficulty button was pressed...
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
        
        if(!isTimerOn) { // If Timer1 isn't active or on...
            __delay_ms(3000);
            randomAction = generateRandomAction();
        }
        
        currentLefthandPotentiometerValue = apply_adc(left_potent);
        currentRighthandPotentiometerValue = apply_adc(right_potent);
        currentJoystickXAxisValue = apply_adc(joystick_x_axis);
        currentJoystickYAxisValue = apply_adc(joystick_y_axis);
        
        if(randomAction == 1 && PORTBbits.RB2 == 0 && isTimerOn) { // If the Left-hand Button was "bopped" on time...
            TMR1_Stop();
            LATBbits.LATB7 = 0; // Turn off Left-hand Button-LED
            score++;
            current_timer = 0;
            isTimerOn = false;
        } else if(randomAction == 2 &&
                (currentLefthandPotentiometerValue >= previousLefthandPotentiometerValue + 100 ||
                currentLefthandPotentiometerValue <= previousLefthandPotentiometerValue - 100) &&
                isTimerOn) { // If the Left-hand Potentiometer was "twisted" on time...
            TMR1_Stop();
            LATCbits.LATC13 = 0; // Turn off Left-hand Potentiometer-LED
            score++;
            current_timer = 0;
            isTimerOn = false;
        } else if(randomAction == 3 &&
                (currentRighthandPotentiometerValue >= previousRighthandPotentiometerValue + 100 ||
                currentRighthandPotentiometerValue <= previousRighthandPotentiometerValue - 100) &&
                isTimerOn) { // If the Right-hand Potentiometer was "twisted" on time...
            TMR1_Stop();
            LATCbits.LATC14 = 0; // Turn off Right-hand Potentiometer-LED
            score++;
            current_timer = 0;
            isTimerOn = false;
        } else if(randomAction == 4 && PORTCbits.RC7 == 0 && isTimerOn) { // If the Right-hand Button was "bopped" on time...
            TMR1_Stop();
            LATCbits.LATC0 = 0; // Turn off Right-hand Button-LED
            score++;
            current_timer = 0;
            isTimerOn = false;
        } else if(randomAction == 5 &&
                (currentJoystickXAxisValue >= previousJoystickXAxisValue + 100 ||
                currentJoystickXAxisValue <= previousJoystickXAxisValue - 100 ||
                currentJoystickYAxisValue >= previousJoystickYAxisValue + 100 ||
                currentJoystickYAxisValue <= previousJoystickYAxisValue - 100) &&
                isTimerOn) { // If the joystick was moved on time...
            TMR1_Stop();
            LATDbits.LATD3 = 0; // Turn off Left-hand Joystick-LED
            LATDbits.LATD4 = 0; // Turn off Right-hand Joystick-LED
            score++;
            current_timer = 0;
            isTimerOn = false;
        }
    }
    
    return 0; 
}