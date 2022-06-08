//The Vehicle Tracking System's White List, phone numbers that can receive tracking information about the vehicle
//for now the white list can only store 1 number

void updateWtLConfig(){
  String _WtL_phone_number = server.arg("WtL_phone_number");
  
  //Save configurations
  StaticJsonDocument<200> doc;
  doc["WtL_phone_number"] = _WtL_phone_number;

  File configFile = SPIFFS.open("/wtConfig.json", "w");
  if (!configFile) {
    Serial.println("Failed to open the file wtlConfig.json for writing");
    server.send(200, "text/html", "failed");
  }

  serializeJson(doc, configFile);

  server.send(200, "text/html", "success");
   
  //Resetting the unit's Access Point
  Serial.println("Resetting the unit's Whitelist.");
  loadWtLConfig();
}

bool loadWtLConfig() {
  File configFile = SPIFFS.open("/wtConfig.json", "r");
  if (!configFile) {
    Serial.println("Failed to open the WtLConfig.json file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("The Whitelist Config file size is too large");
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
    Serial.println("Failed to parse Whitelist config file");
  }

  const char* WtL_phone_number_ = doc["WtL_phone_number"];

  WtL_phone_number = WtL_phone_number_;
  
  // Real world application would store these values in some variables for
  // later use.

  Serial.print("Loaded Whitelist Phone Number: ");
  Serial.println(WtL_phone_number);
  return true;
}

void returnWtLConfig(){
  //Encode JSON using all sensor data
  String webPage;
  
  // Allocate JsonBuffer
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<200> jsonDocument;
 
  jsonDocument["WtL_phone_number"] = WtL_phone_number;

  // Write JSON document
  serializeJsonPretty(jsonDocument, webPage);

  server.send(200, "text/html", webPage);
}
