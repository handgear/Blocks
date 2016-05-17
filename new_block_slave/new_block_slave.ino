#include <Wire.h>
#include "SPI.h"
#include "PDQ_GFX.h"
#include "PDQ_ILI9341_config.h"
#include "PDQ_ILI9341.h"
PDQ_ILI9341 tft;
// sclk 13, mosi 11, cs 10, dc 9, rst 8

#define max_address 20 //~20
#define start_address 10
#define battery_pin 0 //A0
#define out_vcc 6 //pin number

byte block_address = 11; //starts from 10
byte master_address = 127;
//int connection[4]={0,0,0,0};
int connection_new[4]={0,0,0,0};
int connected_ID[4]={0,0,0,0};//up,left,bottom,right (CCW)
int battery,battery_buf=100;
char request_order;
int ID_request=0;
int on_block_address;
//int slave_i2c_receive[10]={0,0,0,0,0,0,0,0,0,0};
char str[18]="===defult text===";//for question sentence

void setup() {
  pinMode(2, INPUT);//con1 up
  pinMode(3, INPUT);//con2 left
  pinMode(4, INPUT);//con3 bottom
  pinMode(5, INPUT);//con4 right
  pinMode(out_vcc, OUTPUT);//pin6

  analogReference(INTERNAL);//1.1v to 1023 need to set resister outside
//LCD setup
#if defined(ILI9341_RST_PIN)  // reset like Adafruit does
FastPin<ILI9341_RST_PIN>::setOutput();
FastPin<ILI9341_RST_PIN>::hi();
FastPin<ILI9341_RST_PIN>::lo();
delay(1);
FastPin<ILI9341_RST_PIN>::hi();
#endif
  tft.begin();

  Wire.begin(block_address);
  Wire.onReceive(receiveEvent);//string or ID of each block
  //Wire.onRequest(requestEvent);//edison request ID information
  battery_check();
  updateLCD();

}

void loop(void) {
  
 
  delay(1500);
  updateLCD();

  //test while()
  // while(0){
  //   digitalWrite(6, HIGH);
  // }

  if(ID_request==1){
    Wire.beginTransmission(127);//to main block
    Wire.write(block_address); 
    for(int i=0;i<4;i++){
      Wire.write(connected_ID[i]); 
    }
    Wire.endTransmission();
    ID_request=0;
  }




  battery_check();
  
  //updateLCD();
  //delay(3000);//for debug
}//loop end

void receiveEvent(int howMany) {
  int i=0;
  request_order = 0;
  request_order = Wire.read();    // receive char as an char
  // if(request_order=='T'){
  //   request_order = 0;
  // }
  if(request_order=='I'){//send ID information
    ID_request=1;
  }
  else if(request_order=='O'){//'ON' connect signal
    digitalWrite(6, HIGH);
    str[0]='O';
  }
  else if(request_order=='F'){//'OFF' connect signal
    digitalWrite(6, LOW);
    str[0]='F';
  }
  else if(request_order=='S'){//recieve question string 
    while (Wire.available()) { // loop through all but the last
    str[i] = Wire.read();
    i++;
      if(i>=17){//prevent overflow
        break;
      }
    }
    updateLCD();
  }
  else if(request_order=='C'){//check connection
    on_block_address=Wire.read();
    connection_update();
    ID_update();
    str[0]='C';
    updateLCD();
  }
  else if(request_order=='T'){//'Test' for connection check
    str[0]='T';
  }
  // else
  //   str[0]='E';


}


void requestEvent(int howMany) {
  // Wire.write(block_address); 
  // for(int i=0;i<4;i++){
  //  Wire.write(connected_ID[i]); 
  // }
}

 

unsigned long updateLCD() {
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();

 ///text info///
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
  tft.print("Address:");
  tft.print(block_address);
  tft.print("    ");
  tft.print("battery:");
  tft.print((int)battery);
  tft.println("%");

  tft.setCursor(0, 30);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  tft.write(24); tft.print(": ");
  tft.print(connected_ID[0]); tft.print("  ");
  tft.write(26); tft.print(": ");
  tft.println(connected_ID[3]);
  tft.write(25); tft.print(": ");
  tft.print(connected_ID[2]); tft.print("  ");
  tft.write(27); tft.print(": ");
  tft.println(connected_ID[1]);
//debug
/*
  tft.print("connection:");
  tft.print(connection[0]);
  tft.print(connection[1]);
  tft.print(connection[2]);
  tft.println(connection[3]);
  tft.print("conn_new:");
  tft.print(connection_new[0]);
  tft.print(connection_new[1]);
  tft.print(connection_new[2]);
  tft.println(connection_new[3]);

  tft.print("recieve:");
  tft.println(received_address);
*/
  tft.print("battery:");
  tft.println(analogRead(battery_pin));

  tft.print(str);
  // for(int i=0;i<10;i++){
  //   tft.print(slave_i2c_receive[i]);
  // }
 ///text end///

  return micros() - start;
}

void connection_update(){
  int temp;
  for(int i=2;i<6;i++){
    temp = digitalRead(i);
    if(temp==true){
      connection_new[i-2]=1;
    }
    if(temp==false){
      connection_new[i-2]=0;
    }
  }
}

void ID_update(){
  if(connection_new[0]==1)
    connected_ID[0] = on_block_address;
  if(connection_new[1]==1)
    connected_ID[1] = on_block_address;
  if(connection_new[2]==1)
    connected_ID[2] = on_block_address;
  if(connection_new[3]==1)
    connected_ID[3] = on_block_address;


}

void battery_check(){
  float temp;
  temp= analogRead(battery_pin);
  temp= (temp-296)*(100/59.0);
  if(temp>=100){
    temp=100;
  }
  if(temp<0){//저전압 경고
    print_low_battery();
    while(1);
  }
  battery=temp;
  //배러리가 일정량 소모된 경우 화면에 업데이트한다
  if(battery_buf > battery+4){
    updateLCD();
    battery_buf = battery;
  }
  

}

unsigned long print_low_battery() {
  tft.setRotation(1);
  tft.fillScreen(ILI9341_WHITE);
  unsigned long start = micros();

  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_RED);  tft.setTextSize(5);
  tft.println("Low Battery!");
  tft.println("Turn Off");
  tft.println("& Recharge");

  return micros() - start;
}
