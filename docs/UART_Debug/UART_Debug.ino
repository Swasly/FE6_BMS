void setup(){
  Serial.begin(115200);
  Serial1.begin(57600);
}

void loop(){
  while(1){
  if (Serial1.available()){
    Serial.print((char)Serial1.read());
  }
  }
}
