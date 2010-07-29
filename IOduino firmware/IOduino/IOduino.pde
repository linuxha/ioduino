/*  IOduino.pde-This IOduino firmware allows easy control of an Arduino's 
                digital/analog input and output from a serial connection
 *  ----------------------------------------------------------------------
 *  Copyright (C) 2010  Harry Eakins
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *  ----------------------------------------------------------------------
 *  Author: Harry Eakins
 *  Email: harry (dot) eakins (at) googlemail (dot) com
 *  Date Modified: 14:35 29/07/2010
 */
 
enum {
  START,
  UNEXPECTED_CHAR,
  REC_P, // Recieved P
  REC_D,
  REC_A,
  PIN_MODE,
  D_WRITE,
  D_READ,
  A_WRITE,
  A_WRITE2,
  A_READ
} 
state;

void setup() {
  state = START;
  Serial.begin(38400);
}

int data1, data2;

void loop() {
  if(Serial.available()) {
    char inChar = Serial.read();
    do{
      switch(state) {
      case START:
      case UNEXPECTED_CHAR:
        if(inChar == 'p') 
          state = REC_P;
        else if (inChar == 'd')
          state = REC_D;
        else if (inChar == 'a')
          state = REC_A;
        else if (inChar == 'v') {
          Serial.print('i');
          state = START;
        }
        else 
          state = START;
        break;
      case REC_P:
        if(inChar == 'm') {
          state = PIN_MODE;
          data1 = 0;
        } 
        else 
          state = UNEXPECTED_CHAR;
        break;
      case REC_D:
        if(inChar == 'w'){
          state = D_WRITE;
          data1 = 0;
        } 
        else if(inChar == 'r')  {
          state = D_READ;
          data1=0;
        }
        else 
          state = UNEXPECTED_CHAR;
        break;
      case REC_A:
        if(inChar == 'w') {
          state = A_WRITE;
          data1 = 0;
        }        
        else if(inChar == 'r') {
          state = A_READ;
          data1 = 0;
        }
        else 
          state = UNEXPECTED_CHAR;
        break;      
      case PIN_MODE:
        if(inChar >= '0' && inChar <= '9')
          data1 = data1*10 + inChar - '0';
        else if (inChar == 'i') {
          if(data1 >= 0 && data1 <= 19) {
            pinMode(data1, INPUT);
            Serial.println('k');
          }
        } 
        else if (inChar == 'o') {
          if(data1 >= 0 && data1 <= 19) {
            pinMode(data1, OUTPUT);
            Serial.println('k');
          }
        }
        else
          state = UNEXPECTED_CHAR;
        break;
      case D_WRITE:
        if(inChar >= '0' && inChar <= '9')
          data1 = data1*10 + inChar - '0';
        else if (inChar == 'h') {
          if(data1 >= 0 && data1 <= 19){
            digitalWrite(data1, HIGH);
            Serial.println('k');
          }
        } 
        else if (inChar == 'l') {
          if(data1 >= 0 && data1 <= 19){
            digitalWrite(data1, LOW);
            Serial.println('k');
          }
        }
        else
          state = UNEXPECTED_CHAR;
        break;
      case D_READ:
        if(inChar >= '0' && inChar <= '9')
          data1 = data1*10 + inChar - '0';
        else if (inChar == '.') {
          if(data1 >= 0 && data1 <= 19) {
            Serial.println(digitalRead(data1));
          }
        }
        else
          state = UNEXPECTED_CHAR;
        break;
      case A_WRITE:
        if(inChar >= '0' && inChar <= '9')
          data1 = data1*10 + inChar - '0';
        else if (inChar == ',') {
          state = A_WRITE2;
          data2 = 0;
        }
        else
          state = UNEXPECTED_CHAR;
        break;
      case A_WRITE2:
        if(inChar >= '0' && inChar <= '9')
          data2 = data2*10 + inChar - '0';
        else if (inChar == '.') {
          if(data1 == 3 || data1 == 5 || data1 == 6 || data1 == 9 || data1 == 10 || data1 == 11) {
            if(data2 >= 0 && data2 <= 255) {
              analogWrite(data1, data2);
              Serial.println('k');
            }
          }
        } 
        else
          state = UNEXPECTED_CHAR;
        break;
      case A_READ:
        if(inChar >= '0' && inChar <= '9')
          data1 = data1*10 + inChar - '0';
        else if (inChar == '.') {
          if(data1 >= 0 && data1 <= 5) {
            Serial.println(analogRead(data1));
          }
        }
        else
          state = UNEXPECTED_CHAR;
        break;
      } 
    }
    while (state == UNEXPECTED_CHAR);
  }
}
























