/*
Problem:
  - A mechanism coin slot can receive and detect inputs of 1, 5, and 12 yuan coins. 
  - The machine can return 1 and 5 yuan coins as change.
  - After a sufficient number of coins have been inserted the payment kiosk rings a bell and dispenses proper change.
  - The LCD backlight should turn off after 30 seconds of inactivity
  * Ignoring the concept of an improperly sized coin as most would fall pass the mechanical sensor, triggering nothing
  * Assume stickers exist within the vending machine that show prices and the correct LetterNumber combo to buy food
    - One should still account for a mis-entry
*/

/*
What's needed?
  - An LCD display to show the requested amount and amount entered
    - Should probably blink between "Requested: x", "Entered: y", "Left to pay: z"
  - A buzzer/ bell that rings when a user has entered the correct amount of money
  - Buttons to enter the amount of change and request change be returned if the user does not have the requested funds
  - A keypad for the user to select which option they want (A1, A2, A3, B1, B2, B3, etc.)
  - A PISO register for the inputs of the buttons and keypad
  - A resistor for the LCD display and four resistors for the four buttons (totaling five resistors)
*/

/*
NOTES:
  - Change is despensed if necessary OR "exact change" is put on the LCD screen indicating the transaction has completed
    - "Don't forget your change!" or...
    - "Exact change entered"...
    - with both being followed by "Have a Great Day!", to show the user the transaction has ended
*/

//Library imports
#include <LiquidCrystal.h>
//Declaring const pins
//PISO (Parallel-In Serial-Out) (to board, buttons, and keypad PISO Input)
const int data = 0; //Data pin (Q7)
const int cp = 1; //clock
const int pl = 2; //latch
/*
  In PISO:
    Buttons
      4: 1 Yuan
      5: 5 Yuan
      6: 12 Yuan
      7: Request Change
*/
//Buzzer
const int buzzer = 6;
//LCD
const int backlight_control = 7;
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
long int last_press = -30000; //Backlight starts as off (-30 secs)

void setup() {
  // put your setup code here, to run once:
  //Setting up the LCD
  //Specifies the width and height of the LCD
  lcd.begin(16, 2);
  lcd.print("  Please enter ");
  lcd.setCursor(3, 1);
  lcd.print("change...");

  //PISO (Input) setup
  pinMode(data, INPUT);
  pinMode(cp, OUTPUT);
  pinMode(pl, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Sample PISO Input
  digitalWrite(pl, LOW);
  delay(10);
  digitalWrite(pl, HIGH);
  //Read main PISO
  byte main_piso = 0x00;
  byte keypad_piso = 0x00;
  //Starting
  Serial.print("START ");
  for(int i= 7; i >= 0; i--) {
    digitalWrite(cp, HIGH);
    delay(10);
    digitalWrite(cp, LOW);
    if(digitalRead(data)) {
      bitSet(main_piso, i);
    }
    Serial.print("\t"); Serial.print(i); Serial.print(": "); Serial.print((main_piso >> i) & 0x01);
  }
  Serial.print("\t STOP\n");
  delay(500);
}

/*
What I learned:
  - How to use PISO
    + Allows for the sampling of eight input pins, reading the results one at a time.
    + When PL is LOW the register is sampling, reading the inputs from the pins and storing them
    + When PL is HIGH the register is shifting. It retains the values read from the inputs and allows them to be read one bit at a time.
      + The bits are shifted through by pulsing CP (with the serial clock) HIGH. Starts backwards (i.e. D7, D6, D5, etc.)
  - How to chain PISO and receive INPUTs from one connected to another by reading the PISO bit repeatedly with a pulse clock pin
    + Using shared board pins for the latch and clock pin between both PISO registers
  - Using buttons with the PISO requires input_pullup resistors 
    + Typically, resistors connected to button input and ground
    + In this case, resistors connected to 5V and button because of the button's connection to the PISO

*/
