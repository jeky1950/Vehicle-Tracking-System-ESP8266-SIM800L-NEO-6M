
 //This  sketch is for a vehicle tracking system that runs 
//on an ESP8266 and sends GPS vehicle position data to a user
//over gsm. The gsm module uses 9600 baud-rate

#include <TinyGPS++.h>
#include <GSMSimSMS.h>
#include <SoftwareSerial.h>
#include <SimpleKalmanFilter.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <FS.h>

/*
 This sample code demonstrates how to use the SimpleKalmanFilter object. 
 Use a potentiometer in Analog input A0 as a source for the reference real value.
 Some random noise will be generated over this value and used as a measured value.
 The estimated value obtained from SimpleKalmanFilter should match the real
 reference value.

 SimpleKalmanFilter(e_mea, e_est, q);
 e_mea: Measurement Uncertainty 
 e_est: Estimation Uncertainty 
 q: Process Noise
 */
 
SimpleKalmanFilter vehicleSpeedFilter(1, 1, 0.01);
SimpleKalmanFilter vehicleAltitudeFilter(1, 1, 0.01);

//Initalize Vehicle GPS parameters
static const int gps_RXPin = D2, gps_TXPin = D3;
static const uint32_t GPSBaud = 9600;

float number_of_satellites, vehicle_gps_lat, vehicle_gps_lon, 
vehicle_altitude, vehicle_speed, vehicle_distance_from_city, 
vehicle_course_string, vehicle_course_degrees;
String vehicle_cardinal_direction;

//Harare
float city_gps_lat = -17.820;
float city_gps_lon = 31.050;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial gps_ss(gps_RXPin, gps_TXPin);

//Initialize the GSM variables
float is_registered, signal_quality;
String operator_name;

String WtL_phone_number;

static const int gsm_RXPin = D5, gsm_TXPin = D6;
static const uint32_t GSMBaud = 9600;

SoftwareSerial gsm_ss(D5, D6); //SIM800L Tx & Rx pins

#define RESET_PIN D1 // gsm module reset pin

static volatile int num = 0;

GSMSimSMS sms(gsm_ss, RESET_PIN); // GSMSimSMS inherit from GSMSim. You can use GSMSim methods with it.

//Initalize the power LED
int powerLED = D7;           // the PWM pin the LED is attached to

//Initialize the Access Point Led and Push button
// constants won't change. They're used here to set pin numbers:
const int buttonPin = D8;    // the number of the pushbutton pin
const int ledPin = D0;      // the number of the LED pin

// Initialize Access Point On Off Push button
int ledState = LOW;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

//AP DNS settings
String UNIQUE_ID_DNS = "settings.local"; //use http://settings.local to access the website on this WiFi network

//Set Wifi credentials
String ap_ssid = "";
String ap_password = "";

//Initialize DNS
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;

ESP8266WebServer server(80);

String getContentType(String filename) {
  if (server.hasArg("download")) {
    return "application/octet-stream";
  } else if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "text/plain";
}

FS* filesystem = &SPIFFS;

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) {
    path += "index.htm";
  }
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (filesystem->exists(pathWithGz) || filesystem->exists(path)) {
    if (filesystem->exists(pathWithGz)) {
      path += ".gz";
    }
    File file = filesystem->open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void setup (){
  // Don't save WiFi configuration in flash - optional
  WiFi.persistent(false);

  //On system start switch off the Access Point
  WiFi.softAPdisconnect(true);
  
  //Initialize the power LED
  pinMode(powerLED, OUTPUT);
  //Switch ON the power LED
  analogWrite(powerLED, 150);

  //Set the baud rate of the serial monitor connection
  Serial.begin(9600);
  Serial.println();
  Serial.println("App Started");

  //Initialize SPIFF
  if (!SPIFFS.begin())
  {
    // Serious problem
    Serial.println("SPIFFS Mount failed");
  } else {

    Serial.println("SPIFFS Mount succesfull");
  }
  
  //Set the baud rate of the GPS and GSM modules
  gps_ss.begin(GPSBaud);
  gsm_ss.begin(GSMBaud);

  while(!gsm_ss) {
    ; // wait for module for connect.
  }

  // Init module...
  sms.init(); // use for init module. Use it if you dont have any valid reason.

  delay(6000);
  
  Serial.print("is Module Registered to Network?... ");
  Serial.println(sms.isRegistered());
  is_registered = sms.isRegistered();
  delay(100);

  Serial.print("Signal Quality... ");
  Serial.println(sms.signalQuality());
  delay(100);

  Serial.print("Operator Name... ");
  Serial.println(sms.operatorNameFromSim());
  operator_name = sms.operatorNameFromSim();
  delay(100);

  Serial.print("Init SMS... ");
  Serial.println(sms.initSMS()); // Its optional but highly recommended. Some function work with this function.
  delay(100);

  Serial.print("List Unread SMS... ");
  Serial.println(sms.list(true)); // Its optional but highly recommended. Some function work with this function.
  delay(100);

  //Load the White List Phone Number Config File
  if (!loadWtLConfig()) {
    Serial.println("Failed to load the Whitelist Config file");
  } else {
    Serial.println("Whitelist Config file loaded SUCCESSFULLY");
  }

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // set initial LED state
  digitalWrite(ledPin, ledState);

  //Handle Server Callbacks
  server.on("/returnData", returnSystemData);
  server.on("/reboot", restartSystem);
  server.on("/updateWtLConfig", updateWtLConfig);
  server.on("/updateAccessPointConfig", updateAccessPointConfig);
  server.on("/returnAPConfig", returnAPConfig);
  server.on("/returnWtLConfig", returnWtLConfig);
  
  //Handle Gzip
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "FileNotFound");
    }
  });

  server.serveStatic("/js", SPIFFS, "/js");
  server.serveStatic("/img", SPIFFS, "/img");
  server.serveStatic("/", SPIFFS, "/index.html");
  server.begin();

  //Wait for the GPS module to be tracking at least 4 satellites
  Serial.println("GPS module searching for Satellites");
  gpsModuleHandler();

  while(number_of_satellites <= 4){
    flashLED(powerLED);
    gpsModuleHandler();
    
    if (gps.satellites.isUpdated()){
      number_of_satellites = gps.satellites.value();
      Serial.println("GPS module is now tracking " + String(number_of_satellites) + " Sats (Minimum: 4)");
    }
    
  }

  Serial.println("The GPS module can now get the vehicle's geolocation");
  Serial.println("Now you can send an sms to this device but only from this number (+"+ WtL_phone_number +") or connect the device's Online Control Panel");
}

void gsmModuleHandler(){
  if(gsm_ss.available()) {
      String buffer = "";
      buffer = gsm_ss.readString();
      num = num + 1;
      Serial.print(num);
      Serial.print(". ");
      //Serial.println(buffer);

      /**/
      // This example for how you catch incoming calls.
      if(buffer.indexOf("+CMTI:") != -1) {
        Serial.print("SMS Index No... ");
        int indexno = sms.indexFromSerial(buffer);
        Serial.println(indexno);

        Serial.print("Who send the message?...");
        Serial.println(sms.getSenderNo(indexno));

        Serial.println("Read the message... ");
        Serial.println(sms.readFromSerial(buffer));
        
        //If the the sender is the same as the white list phone number
        //then send an sms with the tracking information
        if("+" + WtL_phone_number == sms.getSenderNo(indexno)){
          String sender_number = String(sms.getSenderNo(indexno));     
          String message = "Tracking GPS Sats: " + String(number_of_satellites) +"\n" +
          "GPS Latitude: "+ String(gps.location.lat(), 6) +" \n" + 
          "GPS Longitude: "+ String(gps.location.lng(), 6) +" \n" + 
          "Speed: "+ vehicle_speed +"kmph \n" + 
          "Dist frm Hre CBD: "+ String(vehicle_distance_from_city) +"km \n" + 
          "Dir frm Hre CBD: " + String(vehicle_cardinal_direction) + " ("+ String(vehicle_course_degrees) +" Deg) \n" + 
          "Altitude: "+ vehicle_altitude +"m \n\n" + 
          "Go to http://maps.google.com/maps?q="+String(gps.location.lat(), 6)+","+ String(gps.location.lng(), 6);

          //Convert the sender_number and message string variables to char*
          std::vector<char> sender_number_(sender_number.begin(), sender_number.end());
          sender_number_.push_back('\0');
          
          std::vector<char> message_(message.begin(), message.end());
          message_.push_back('\0');
          
          Serial.println("Sending message with tracking parameters");
          Serial.println(sms.send(&*sender_number_.begin(), &*message_.begin()));
        }else{
          Serial.println("No whitelist match");
        }
      } else {
        Serial.println(buffer);
      }
      
  }
}

void gpsModuleHandler(){
  // Dispatch incoming characters
  while (gps_ss.available() > 0)
    gps.encode(gps_ss.read());

  if (gps.location.isUpdated())
  {
    vehicle_gps_lat = gps.location.lat();
    vehicle_gps_lon = gps.location.lng();
  }

  else if (gps.speed.isUpdated())
  {
    vehicle_speed = gps.speed.kmph();
    //Filter speed
    vehicle_speed = vehicleSpeedFilter.updateEstimate(vehicle_speed);
    //Round Off speed to the nearest kmph
    vehicle_speed = round(vehicle_speed);
  }

  else if (gps.altitude.isUpdated())
  {
    vehicle_altitude = gps.altitude.meters();
    //Filter altitude
    vehicle_altitude = vehicleAltitudeFilter.updateEstimate(vehicle_altitude);
    //Round Off Altitude to the nearest m
    vehicle_altitude = round(vehicle_altitude);
  }

  else if (gps.satellites.isUpdated())
  {
    number_of_satellites = gps.satellites.value();
  }

  else if (gps.location.isValid())
  {
    //Convert meters to kilometers by dividing by 1000
    vehicle_distance_from_city = TinyGPSPlus::distanceBetween(
      city_gps_lat,
      city_gps_lon,
      vehicle_gps_lat, 
      vehicle_gps_lon
    )/1000;
    
    vehicle_course_degrees = TinyGPSPlus::courseTo(
      city_gps_lat,
      city_gps_lon,
      vehicle_gps_lat, 
      vehicle_gps_lon
    );
    
    vehicle_cardinal_direction = TinyGPSPlus::cardinal(vehicle_course_degrees);
  }   
}

void loop(){
  rebootHandler();
  dnsServer.processNextRequest();
  server.handleClient();
  accessPointSwitchHandler();
  gsmModuleHandler();
  gpsModuleHandler();
  fadeLED(powerLED);
}

void returnSystemData(){
  //Encode JSON using all sensor data
  String webPage;
  
  // Allocate JsonBuffer
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<650> jsonDocument;

  jsonDocument["number_of_satellites"] = number_of_satellites;
  jsonDocument["vehicle_gps_lat"] = vehicle_gps_lat;
  jsonDocument["vehicle_gps_lon"] = vehicle_gps_lon;
  jsonDocument["vehicle_altitude"] = vehicle_altitude;
  jsonDocument["vehicle_speed"] = vehicle_speed;
  jsonDocument["vehicle_distance_from_city"] = vehicle_distance_from_city;
  jsonDocument["vehicle_course_string"] = vehicle_course_string;
  jsonDocument["vehicle_course_degrees"] = vehicle_course_degrees;
  jsonDocument["vehicle_cardinal_direction"] = vehicle_cardinal_direction;
  jsonDocument["is_registered"] = is_registered;
  jsonDocument["signal_quality"] = sms.signalQuality();
  jsonDocument["operator_name"] = operator_name;
  
  // Write JSON document
  serializeJsonPretty(jsonDocument, webPage);

  server.send(200, "text/html", webPage);
}
