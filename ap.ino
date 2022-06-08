// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void accessPointSwitchHandler(){
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      //Also toggle the device's Access Point
      if (buttonState == HIGH) {
        ledState = !ledState;

        //Switch on the device's Access Point if the ledstate is HIGH
        if(ledState == HIGH){
          //Load the Access Point Config File found in the data folder in the sketch folder of this sketch
          if (!loadAccessPointConfig()) {
            Serial.println("Failed to load the AP Config file");
          } else {
            Serial.println("AP Config file loaded SUCCESSFULLY");
          }
        
          //Setup the Access Point
          setUpAccessPoint();
          
        }else{
          //Switch Off the device's Access Point if the ledstate is LOW
          WiFi.softAPdisconnect(true);
          Serial.println("Access Point Switched Off");
        }
      }
    }
  }

  // set the LED:
  digitalWrite(ledPin, ledState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}

void setUpAccessPoint(){
  //Configure device Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ap_ssid, ap_password);
  
  dnsServer.setTTL(300);
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(DNS_PORT, UNIQUE_ID_DNS, apIP);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void updateAccessPointConfig(){
  String _ap_ssid = server.arg("ap_ssid");
  String _ap_password = server.arg("ap_password");
  
  //Save configurations
  StaticJsonDocument<200> doc;
  doc["ap_ssid"] = _ap_ssid;
  doc["ap_password"] = _ap_password;

  File configFile = SPIFFS.open("/apConfig.json", "w");
  if (!configFile) {
    Serial.println("Failed to open the file apConfig.json for writing");
    server.send(200, "text/html", "failed");
  }

  serializeJson(doc, configFile);

  server.send(200, "text/html", "success");
   
  //Resetting the unit's Access Point
  Serial.println("Resetting the unit's AP.");
  loadAccessPointConfig();
  setUpAccessPoint();
}

bool loadAccessPointConfig() {
  File configFile = SPIFFS.open("/apConfig.json", "r");
  if (!configFile) {
    Serial.println("Failed to open the apConfig.json file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("The AP Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse AP config file");
  }

  const char* ssid_ = doc["ap_ssid"];
  const char* password_ = doc["ap_password"];

  ap_ssid = ssid_;
  ap_password = password_;
  
  // Real world application would store these values in some variables for
  // later use.

  Serial.print("Loaded AP SSID: ");
  Serial.println(ap_ssid);
  Serial.print("Loaded AP Password: ");
  Serial.println(ap_password);
  return true;
}

void returnAPConfig(){
  //Encode JSON using all sensor data
  String webPage;
  
  // Allocate JsonBuffer
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<200> jsonDocument;
 
  jsonDocument["ap_ssid"] = ap_ssid;
  jsonDocument["ap_password"] = ap_password;

  // Write JSON document
  serializeJsonPretty(jsonDocument, webPage);

  server.send(200, "text/html", webPage);
}
