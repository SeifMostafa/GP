int stp =8,dir=9;
int prev=0;
// analog read angle
int val;
void setup()
{               
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);   

  ///temp to read various angles 
  Serial.begin(9600);  
Serial.write('1');
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

  String str = Serial.readStringUntil('\n');
  val = str.toInt();
  if(val != 0)
  {
    Serial.println(val);
    val = val - prev;
    int temp_prev = prev;
    prev= prev+val;

    if(val<0){
      changeDirection();
      val=val*-1;

      run(val);
      changeDirection2();

    }
    else run(val);

    if(prev>180){
      prev=prev-temp_prev;
    }
  }
}


