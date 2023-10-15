int digitalIO[11] = {27,29,31,33,35,37,39,41,43,45,47};
void setup() {
  // put your setup code here, to run once:
  for(int i=0;i<13;i++){pinMode(digitalIO[i],INPUT_PULLUP);} 
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly
  for(int i=0;i<13;i++){
    Serial.print(digitalRead(digitalIO[i]));
    Serial.print("  ");
  } 
  Serial.println();
}
