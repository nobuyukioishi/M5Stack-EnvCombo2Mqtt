boolean checkWifiConnection() {
  int count = 0;
  M5.Lcd.print("Waiting for Wi-Fi connection");
  while ( count < 30 ) {
    if (WiFi.status() == WL_CONNECTED) {
      M5.Lcd.println();
      M5.Lcd.println("Connected!");
      return (true);
    }
    delay(500);
    M5.Lcd.print(".");
    count++;
  }
  M5.Lcd.println("Timed out.");
  return false;
}
