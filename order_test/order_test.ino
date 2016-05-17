#define block_number 5
void setup(){
  Serial.begin(9600);
}
void loop() {
  int firstBlock = 1;
  // put your main code here, to run repeatedly:
  //char stream[MAX];
  //byte array[block_number][5] = {{1,0,4,0,0},{2,0,0,0,3},{3,4,2,0,0},{4,0,0,3,1}};
  byte array[block_number][5] = {{1,0,0,0,2},{2,0,1,3,0},{3,2,0,4,0},{5,0,0,0,4},{4,3,5,0,0}};
  byte order[block_number];
  order[0] = firstBlock;
  for(int i=0;i<block_number;i++)//
  {
    for(int j=1;j<5;j++)//
    {
      if(i==0)
      {
        if(array[(order[i]-1)][j]!=0)
        {
          order[i+1]=array[(order[i]-1)][j];
          //Serial.print(order[i+1]);
          //Serial.println("Found");
        }
      }
      else
      {
        if((array[(order[i]-1)][j]!=0)&&(array[(order[i]-1)][j]!=order[i-1]))
        {
          order[i+1]=array[(order[i]-1)][j];
         // Serial.print(order[i+1]);
          //Serial.println("Found");
        }
      }
    }
  }
//  store(array);
//  Serial.println("stored well!");
//  Serial.println(array[0][2]);
//  order[0][0] = 1;
//  for(int i=0;i<(sizeof(array)/5);i++)
//  {
//    if(array[i][3] != 0)
//    {
//      order[i][1] = 1;
//      Serial.println(i);
//    }
//  }
//  Serial.println("below marked!");
//  for(int i=1;i<sizeof(array)/5;i++)
//  {
//    order[i][0] = getter(order[i-1][0], array, order);
//  }
  //test order
  for(int i=0;i<block_number;i++)
  {
    Serial.println(order[i]);
  }
  while(1)
  {}
}