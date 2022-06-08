# Vehicle Tracking System (ESP8266 + GPS + GSM)
This is an ESP8266 Based Vehicle Tracking System which sends an SMS with the GPS coordinates of a vehicle when ever someone, on the the device's whitelist sends the SMS message "Hi" to the device. The device is controlled via the use of a Web Dashboard which you can access by clicking a push button connected to pin D8.

This setup assumes that you are using the Arduino IDE Version 1.8.19. 

<h1><strong>Setup Instructions</strong></h1>
To setup the Arduino IDE to be able to support the ESP8266 follow <a href="https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/">this</a> link. To Flash the Web Interface Dashboard to the ESP8266's SPIFF Memory follow <a href="https://randomnerdtutorials.com/install-esp8266-filesystem-uploader-arduino-ide/">this</a> link (The Web Dashboard files can be found in the data/ folder, the files are compressed to save space and to make the server faster).

<h1><strong>Project Structure</strong></h1>
This project makes use of the ESP8266 microcontroller, a GPS Module (NEO-6m), a GSM Module (SIM800L), a push button, a red LED and a blue LED.
