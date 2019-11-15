void i2cTest() {
  M5.Lcd.fillScreen( BLACK );
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("M5StackFire I2C Tester");

  int address;
  int error;
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("scanning Address [HEX]");
  
  for(address = 1; address < 127; address++ ) 
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if(error==0)
    {
      M5.Lcd.print(address,HEX);M5.Lcd.print(" ");
    }
    else M5.Lcd.print(".");

    delay(10);
  }
}
