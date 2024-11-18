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
  int row_counter= 0; int col_counter= 0; int found_row= -1;
  for(int i= 0; i <= 3; i++) {
    pinMode(cols[i], OUTPUT);
    digitalWrite(cols[i], LOW);
  }

  while((row_counter <= 3) && (found_row == -1)) {
    digitalWrite(rows[row_counter], HIGH);
    col_counter = 0;
    while((col_counter <= 3) && (found_row == -1)) {
      if(digitalRead(cols[col_counter]) == HIGH) {
        found_row = row_counter;
        Serial.print("HERE: ");
        Serial.print("Row: "); Serial.print(row_counter);
        Serial.print("\tCol: "); Serial.print(col_counter);
        Serial.println();
      }
      col_counter++;
    }
    digitalWrite(rows[row_counter], LOW);
    row_counter++;
    delay(1000);
  }

  for(int i= 0; i <= 3; i++) {
    pinMode(cols[i], INPUT_PULLUP);
    digitalWrite(cols[i], HIGH);
  }

  return found_row;
}


/*
 Watch the lights. It appears the selected col light stays off while the others turn on when checking the rows
So when checking cols, save a int8_t of the cols, if only ONE 1 is in the first four then the row is found*/
