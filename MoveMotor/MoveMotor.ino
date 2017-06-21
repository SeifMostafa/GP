int stp =8,dir=9;
// analog read angle
void setup()
{               
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);   

  ///temp to read various angles 
  Serial.begin(9600);  
  Serial.write("1");
}

void run(int angle){
  double steps =(double) angle /1.8;
  double a=0;
  while(a<steps){
    a=a+1.8;
    digitalWrite(stp, HIGH);   
    delay(20);               
    digitalWrite(stp, LOW); 
    delay(20);  

  }
}
void changeDirection(){
  digitalWrite(dir, HIGH);
}
void changeDirection2(){
  digitalWrite(dir, LOW);
}
void loop()
{
  if(Serial.available() > 0)
  {
    String str = Serial.readStringUntil('\n');
    DesiredPosition = str.toInt();
    Serial.println(DesiredPosition);
    
    if(DesiredPosition<0){
      changeDirection();
      DesiredPosition=DesiredPosition*-1;
      run(DesiredPosition);
      changeDirection2();
        Serial.write("$");

    }
    else{
      run(DesiredPosition);
        Serial.write("$");
    }
   
  }
}

