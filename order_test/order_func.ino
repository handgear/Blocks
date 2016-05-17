
#define block_num 4
void setup() {

 Serial.begin(9600);
}


int getter(int idminus,byte (*array)[5], byte (*order)[2]);
void store (byte (*array)[5]);
int finder(byte (*array)[5],byte n);

void loop() {
  byte firstBlock = 1;
   //char stream[MAX];
  byte array[block_num][5] = {{1,0,7,0,0},{3,0,0,0,5},{5,7,3,0,0},{7,0,0,5,1}};
  byte order[block_num];
  order[0] = firstBlock;
  for(int i=0;i<block_num;i++)//
  {
    for(int j=1;j<5;j++)//
    {
      if(i==0)
      {
        if(array[0][j]!=0)
        {
          order[1]=array[0][j];
          Serial.print(order[1]);
          Serial.println("Found");
        }
      }
      else
      {
        if((array[finder(array,order[i])][j]!=0)&&(array[finder(array,order[i])][j]!=order[i-1]))
        {
          order[i+1] = array[finder(array,order[i])][j];
          Serial.print(order[i+1]);
          Serial.println("Found");
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
  for(int i=0;i<block_num;i++)
  {
    Serial.println(order[i]);
  }
  while(1)
  {}
}
int finder(byte (*array)[5],byte n)
{
  for(int i=0;i<block_num;i++)
  {
    if(n==array[i][0])
    {
      return i;
    }
  }
}


int getter(int idminus,byte (*array)[5], byte (*order)[2])
{
  if(array[idminus][2] != 0)
  {
    return array[idminus][0];
  }
  else
  {
     for(int i=idminus;;i--)
     {
      if(order[i][1]==1)
      {
        return array[i][0];
      }
     }
  }
}

void store (byte (*array)[5])
{
  int i=0;
  int j=1;//check if there is any more data coming
  while(Serial.available()==0) { // Wait for User to Input Data  
  }   
  while(j)
  {
      
      //keep listening
      
      j = Serial.readBytes(array[i],5);
      if(array[i][0]=='\n')
      {
        array[i][0]=0;
        break;
      }
      for(int k=0;k<5;k++)
      {
        array[i][k] -=48;
      }
      for(int l=0;l<5;l++)
      {
      Serial.print(array[i][l]);//mark raed.
      Serial.print('/');
      }
      i++;
      //Serial.flush();
     // if(temp[i][0]==0)
     // {Serial.flush(); Serial.write("Empty");}
      //memcpy(temp[i][6],zero,sizeof(zero));
    //Serial.write(temp[2]);
  }
}