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

byte master_address = 127;
byte block_address = 127;
//int connection[4]={0,0,0,0};
int connection_new[4];
int connected_ID[4]={0,0,0,0};//up,left,bottom,right (CCW)
int battery,battery_buf=100;
int slave_i2c_receive[10]={0,0,0,0,0,0,0,0,0,0};//change if max_address change
char request_command;
int current_block_ID;
int start_function=1;

int ID[10][5];
//test sting
char Str1[]="This         ";
char Str2[]="is           ";
char Str3[]="apple        ";
char Str4[]="maybe...     ";

char block_str[5][18];//web에서 입려받은 string을 저장할 곳 
int i=0;

void setup() {
  pinMode(2, INPUT);//con1 up
  pinMode(3, INPUT);//con2 left
  pinMode(4, INPUT);//con3 bottom
  pinMode(5, INPUT);//con4 right
  pinMode(6, OUTPUT);

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
  memset(ID,0,sizeof(ID));//array initialization
  updateLCD();
}

void loop(void) {
  delay(1000);
  // LCD_test();
  // updateLCD();

  // for(int i=10;i<max_address;i++){//초기 연결확인-여러번 확인?
  //     Wire.beginTransmission(i);
  //     Wire.write('T');   // for connection check
  //      slave_i2c_receive[i-10]=Wire.endTransmission(); //new line
  //     //Wire.endTransmission();
  //   }

  while(0){
    connection_update();
    updateLCD();
    delay(1000);
  }

  connection_update();
  check_edison_connect();
  get_question();

  block_ID_update();
  block_set_string();
  block_recieve_ID();

  delay(500);
  battery_check();
  
  
  i++;
  if(i == 4){
    display_ID();
    i = 0;
    delay(5000);
  }
    

  //delay(3000);//for debug
}//loop end



void receiveEvent(int howMany) {
  for(int i=0;i<5;i++){
    ID[current_block_ID - start_address][i]= Wire.read(); 
  }
  //==============shoud add 'I' before sent ID from slave to master
  request_command = 0;
  request_command = Wire.read(); 

  if(request_command == 'I'){//receive ID from slave
    while (Wire.available()) { // loop through all but the last
      for(int i=0;i<5;i++){
        ID[current_block_ID - start_address][i]= Wire.read(); 
      }
    }
  }
  else if(request_command == 'Q'){//receive question from edison
    while (Wire.available()) { // loop through all but the last
      for(int i=0;i<question_block_num;i++){
        for(int j=0;j<18;j++){
          char temp = Wire.read();
          if(temp == '*')
            i++; break;
          block_str[i][j] = temp;
        }
        
        i++;
      }
              
    }
  }
}


// void requestEvent(int howMany) {
//   Wire.write(block_address); 
//   for(int i=0;i<4;i++){
//    Wire.write(connected_ID[i]); 
//   }
// }
void LCD_test(){
  tft.setRotation(3);
  tft.fillScreen(ILI9341_WHITE);
  //text info//
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
  tft.print("MASTER");
  tft.print("    ");
  // tft.print("Address:");
  // tft.print(block_address);
  // tft.print("    ");
  tft.print("battery:");
  tft.print((int)battery);
  tft.println("%");

}
unsigned long updateLCD() {
  tft.setRotation(3);
  tft.fillScreen(ILI9341_WHITE);
  unsigned long start = micros();

 ///text info///
  tft.setCursor(3, 3);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
  tft.print("MASTER");
  tft.print("    ");
  // tft.print("Address:");
  // tft.print(block_address);
  // tft.print("    ");
  tft.print("battery:");
  tft.print((int)battery);
  tft.println("%");

//code for slave
  // tft.setCursor(0, 30);
  // tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  // tft.write(24); tft.print(": ");
  // tft.print(connected_ID[0]); tft.print("  ");
  // tft.write(26); tft.print(": ");
  // tft.println(connected_ID[1]);
  // tft.write(25); tft.print(": ");
  // tft.print(connected_ID[2]); tft.print("  ");
  // tft.write(27); tft.print(": ");
  // tft.println(connected_ID[3]);

//debug


  //print connected direction
  tft.setCursor(3, 30);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(3);
  tft.write(24); tft.print(": ");
  tft.print(connection_new[0]); tft.print("  ");
  tft.write(26); tft.print(": ");
  tft.println(connection_new[3]);
  tft.write(25); tft.print(": ");
  tft.print(connection_new[2]); tft.print("  ");
  tft.write(27); tft.print(": ");
  tft.println(connection_new[1]);

  // tft.print("recieve:");
  // tft.println(received_address);

  //print ID connedtion info 
  //which is for debug 
  tft.setCursor(3, 90);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(3);
  for(int i=0;i<2;i++){
    tft.print(ID[i][0]);
      tft.print(":");
    for(int j=1;j<5;j++){
      tft.print(ID[i][j]);
      tft.print(",");
    }
    tft.print("\n");
  }
  tft.print("\n");

  //print state of slave connection
  tft.println("slave received");
  for(int i=0;i<10;i++){
    tft.print(slave_i2c_receive[i]);
    if(i==4)
      tft.print("\t");
  }

 ///text end///

  return micros() - start;
}
void display_ID(){
  tft.setRotation(3);
  tft.fillScreen(ILI9341_WHITE);
 ///text info///
  tft.setCursor(3, 3);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
  tft.print("MASTER");
  tft.print("    ");
  // tft.print("Address:");
  // tft.print(block_address);
  // tft.print("    ");
  tft.print("battery:");
  tft.print((int)battery);
  tft.println("%");

  tft.setCursor(0, 20);  tft.setTextSize(2);
  tft.print("counter clockwise");
  tft.setCursor(0, 40);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(3);
  for(int i=0;i<7;i++){
    tft.print(ID[i][0]);
      tft.print(":");
    for(int j=1;j<5;j++){
      tft.print(ID[i][j]);
      tft.print(",");
    }
    tft.print("\n");
  }
  tft.print("\n");
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

void print_low_battery() {
  tft.setRotation(1);
  tft.fillScreen(ILI9341_WHITE);
  
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_RED);  tft.setTextSize(5);
  tft.println("Low Battery!");
  tft.println("Turn Off");
  tft.println("& Recharge");
}

void block_ID_update(){//each slaves update ID's from connected blocks
  if(start_function==1){//주소 확인
    for(int i= start_address;i<max_address;i++){//초기 연결확인-여러번 확인?
      Wire.beginTransmission(i);
      Wire.write('T');   //'Test' for connection check
      slave_i2c_receive[i- start_address]=Wire.endTransmission(); //new line
      //Wire.endTransmission();
    }
     updateLCD();

    for(int i= start_address;i<max_address;i++){//확인 시작
      if(slave_i2c_receive[i- start_address]==0){//ACK 받은 블럭만
        Wire.beginTransmission(i);
        Wire.write('O');//'ON' commend-for 'the' block
        Wire.endTransmission();

        for(int j= start_address;j<max_address;j++){//for 'other' blocks
          if((i != j) && slave_i2c_receive[j- start_address]==0){
            Wire.beginTransmission(j);
            Wire.write('C');
            Wire.write(i);//'the' block address
            Wire.endTransmission();
          }
        }
        delay(50);//블럭들이 각자의 상태정보를 업데이트 할때까지 기다림

        Wire.beginTransmission(i);
        Wire.write('F');//off commend-for 'the' block
        Wire.endTransmission();
      }
    }
  }
}

void block_set_string(){
  if(start_function==2){//text 전송용
    for(int i= start_address;i<max_address;i++){//초기 연결확인-여러번 확인?
      Wire.beginTransmission(i);
      // Wire.send('T');   // for connection check
       slave_i2c_receive[i- start_address]=Wire.endTransmission(); //new line
      //Wire.endTransmission();
    }
    updateLCD();
    for(int i= start_address;i<max_address;i++){
      if(slave_i2c_receive[i- start_address]==0){//ACK 받은 블럭만
        Wire.beginTransmission(i);
        Wire.write('S');//string send initialization byte
        Wire.write(Str1);
        Wire.endTransmission();
      }
    }
    delay(50);
  }
}

void block_recieve_ID(){
  delay(200);
  if(start_function==1 || start_function==3){
    for(int i=10;i<max_address;i++){//확인 시작
      if(slave_i2c_receive[i-10]==0){//ACK 받은 블럭만
        Wire.beginTransmission(i);
        Wire.write('I');//ID request
        current_block_ID=i;
        Wire.endTransmission();
      }
      delay(100);//wait for receive
    }


  }
  // if(start_function==1 || start_function==3){
  //   for(int k=10;k<max_address;k++){
  //     if(slave_i2c_receive[k-10]==0){
  //       Wire.requestFrom(k, 5);
  //       int a=0;
  //       while (Wire.available()) { // slave may send less than requested
  //         ID[k][a]; // receive a byte as character
  //         a++;
  //         if(a>5) break;//prevent overflow
  //       }
  //     }
  //   }
  // }
}
void check_edison_connect(){
  if(question_recived == 0){
    if(connection_new[0] || connection_new[1] || 
    connection_new[2] || connection_new[3]){
    Wire.beginTransmission(126);//edison
    Wire.write('E');//'edison' check commend-for edison
    Wire.endTransmission();
    }
  }
}
void get_question(){

}