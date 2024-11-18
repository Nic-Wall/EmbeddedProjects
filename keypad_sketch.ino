/*
Why this exists and why it's as confusing as it is:
Whether due to a lack of understanding on the functionality of the kepad or a bug in Wokwi the digitalRead of columns
set to pinMode(input) and digitalWrite(LOW) will sometimes read HIGH or LOW when read no matter which row is pressed.
Because of this I've fashioned the row finder below to (using bytes to keep it light) that measures the columns default
status (after being set to INPUT and LOW), then compares to the rows (0,1,2,3). This allows for a control byte that will
recognize a mismatch in the default column set and the first row. It's not effecient but it works, is light, and fast.
Now to include a PISO INPUT register and test it again!
*/


const int cols[] = {2,3,4,5};
const byte rows[] = {A0,A1,A2,A3};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  for(int i= 0; i <= 3; i++) {
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], LOW);
    pinMode(cols[i], INPUT_PULLUP);
    digitalWrite(cols[i], HIGH);
  }
}

void loop() {
  int column= -1; int row= -1;
  int counter= 0;
  while((counter <= 3) && (row == -1)) {
    if(digitalRead(cols[counter]) == LOW) {
      column= counter;
      row= locate_row();
    }
    counter++;
  }

  if((column != -1) || (row != -1)) {
    Serial.print("Row: "); Serial.print(row);
    Serial.print("\tCol: "); Serial.print(column);
    Serial.println();
  }

  delay(150);
}

int locate_row() {
  //Declarations
  int row_counter= -1; int col_counter= 0; int found_row= -1;
  int8_t primary_row = 0x0; int8_t previous_row = 0x0;
  //Setup columns to be read
  for(int i= 0; i <= 3; i++) {
    pinMode(cols[i], INPUT);
    digitalWrite(cols[i], LOW);
  }

  while((row_counter <= 3) && (found_row== -1)) { //Itterate through each row
    if(row_counter != -1) {digitalWrite(rows[row_counter], HIGH);} //Set the row to HIGH so the col intersection can be read
    col_counter = 0;  //Reset col_counter for while loop
    previous_row = primary_row; //Set previous_row for future comparison
    primary_row = 0x0;  //Reset primary row
    while(col_counter <= 3) { //Itterate through each column
      if(digitalRead(cols[col_counter]) == HIGH) {  //If the column has a HIGH charge...
        primary_row = primary_row ^ (1 << col_counter); //Set the col_counter (from most significant) to 1
      }
      col_counter++;  //Itterate col_counter row so while loop is eventually broken
    }
    if((primary_row ^ previous_row) && (row_counter != -1))  { //XOR shows a change in the rows
      found_row = row_counter; //The row is found
    }
    if(row_counter != -1) {digitalWrite(rows[row_counter], LOW);} //Set the row back to its default (LOW). Only one row at a time should be HIGH
    //Serial.print(row_counter); Serial.print("(CURR): "); print_byte(primary_row); //Debug message
    //Serial.print(row_counter); Serial.print("(PREV): "); print_byte(previous_row);  //Debug message
    row_counter++;  //Itterate row_counter so while loop is eventually broken
  }
  //Need to check second, third, fourth, first due to the random HIGH/LOW of each column when checking
  
  //Reset columns back to their default positions
  for(int i= 0; i <= 3; i++) {
    pinMode(cols[i], INPUT_PULLUP);
    digitalWrite(cols[i], HIGH);
  }

  return found_row; //Return the found row
}

void print_byte(int8_t by) {  //Debugging print
  for(int bi= 0; bi <= 7; bi++) {
    Serial.print((by >> bi) & 0x01);
  }
  Serial.println();
}

/*
 Watch the lights. It appears the selected col light stays off while the others turn on when checking the rows
So when checking cols, save a int8_t of the cols, if only ONE 1 is in the first four then the row is found

NOW I KNOW: Use current setup, save bits, find which changes from 0 to 1. All the others are set as 1 for some reason
1011
1111
1011
1011
Row: -1	Col: 1
I guess just save each row, then iterate through with XOR until the flipped bit is found. Don't even need first col check
  I don't think (NOTE: YOU DO, to find the row a column must first be triggered)
*/

/*
while((row_counter <= 3) && (found_row == -1)) {
    digitalWrite(rows[row_counter], HIGH);
    col_counter = 0;
    while((col_counter <= 3) && (found_row == -1)) {
      Serial.print(digitalRead(cols[col_counter]));
      if(digitalRead(cols[col_counter]) == HIGH) {
        found_row = row_counter;
        Serial.print("HERE: ");
        Serial.print("Row: "); Serial.print(row_counter);
        Serial.print("\tCol: "); Serial.print(col_counter);
        Serial.println();
      }
      delay(10);
      col_counter++;
    }
    Serial.println();
    digitalWrite(rows[row_counter], LOW);
    row_counter++;
    delay(1000);
  }
*/
