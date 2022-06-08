unsigned long reboot_interval = 86400000; //24 hours
unsigned long reboot_previousMillis = 0; 

void restartSystem(){
  //Restart the ESP8266-12
  //Restarting the SYSTEM
  Serial.println("");
  Serial.println("Restarting the Sytem");
  server.send(200, "text/html", "success");
  delay(2000);
  ESP.restart();
}

void rebootHandler(){
  //Wait for 15 seconds before sending Data to the Remote Server
  unsigned long reboot_currentMillis = millis();
  if (reboot_currentMillis - reboot_previousMillis >= reboot_interval) {
    restartSystem();
  }
}
