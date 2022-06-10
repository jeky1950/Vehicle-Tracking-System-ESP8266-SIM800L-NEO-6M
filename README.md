# Vehicle Tracking System (ESP8266 + GPS + GSM)
<p>This is an ESP8266 Based Vehicle Tracking System which sends an SMS with the GPS coordinates of a vehicle when ever someone, on the the device's whitelist sends the SMS message "Hi" to the device. The device is controlled via the use of a Web Dashboard which you can access by clicking a push button connected to pin D8.</p>

<p>This setup assumes that you are using the Arduino IDE Version 1.8.19.</p>

<h1><strong>Setup Instructions</strong></h1>
To setup the Arduino IDE to be able to support the ESP8266 follow <a href="https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/">this</a> link. To Flash the Web Interface Dashboard to the ESP8266's SPIFF Memory follow <a href="https://randomnerdtutorials.com/install-esp8266-filesystem-uploader-arduino-ide/">this</a> link (The Web Dashboard files can be found in the data/ folder, the files are compressed to save space and to make the server faster).

<h1><strong>Project Structure</strong></h1>
This project makes use of the ESP8266 microcontroller, a GPS Module (NEO-6m), a GSM Module (SIM800L), a push button, a red LED and a blue LED.

<h1>Bill of Materials:</h1>
<h2>Assembly List</h2>
<table>
  <thead>
   <tr>
    <th>Label</th>
    <th>Part Type</th>
    <th>Properties</th>
    </tr>
  </thead>
  <tbody>
  <tr>
    <td>BAT1</td>
    <td>18650 battery holder</td>
    <td class="props">variant 18650; package battery-18650-kit</td>
</tr><tr>
    <td>GSM, SIM, GPSR, SMS, SIM900, SIM1</td>
    <td>SIM800L</td>
    <td class="props">pin spacing 100mil; variant variant 1; pins 7; editable pin labels false; layout Single Row; package THT; chip label ?</td>
</tr><tr>
    <td>LED1</td>
    <td>Red (633nm) LED</td>
    <td class="props">color Red (633nm); package 1206 [SMD]</td>
</tr><tr>
    <td>LED2</td>
    <td>Blue (525nm) LED</td>
    <td class="props">color Blue (525nm); package 0805 [SMD]</td>
</tr><tr>
    <td>M1</td>
    <td>NEO-6M GPS Module</td>
    <td class="props">model NEO-6M; pins 4; type GPS; part # GY-GPS6MV2</td>
</tr><tr>
    <td>M2</td>
    <td>RioRand LM2596</td>
    <td class="props">variant variant 2; chip label lm2596</td>
</tr><tr>
    <td>Mod1</td>
    <td>Amica NodeMCU DevKit 1.0</td>
    <td class="props">model Amica; pins 30; type NodeMCU</td>
</tr><tr>
    <td>R1</td>
    <td>1kΩ Resistor</td>
    <td class="props">resistance 1kΩ; package 2010 [SMD]; tolerance ±5%</td>
</tr><tr>
    <td>R2</td>
    <td>1kΩ Resistor</td>
    <td class="props">resistance 1kΩ; package 0603 [SMD]; tolerance ±5%</td>
</tr><tr>
    <td>R3</td>
    <td>1kΩ Resistor</td>
    <td class="props">resistance 1kΩ; package 0603 [SMD]; tolerance ±5%</td>
</tr><tr>
    <td>S1</td>
    <td>Pushbutton</td>
    <td class="props">package [SMD]</td>
</tr>
  </tbody>
</table>
<h2>Shopping List</h2>
<table>
  <thead>
	<tr>
    <th>Amount</th>
    <th>Part Type</th>
    <th>Properties</th>
    </tr>
  </thead>
  <tbody>
<tr>
    <td>1</td>
    <td>18650 battery holder</td>
    <td class="props">variant 18650; package battery-18650-kit</td>
</tr><tr>
    <td>1</td>
    <td>SIM800L</td>
    <td class="props">pin spacing 100mil; variant variant 1; pins 7; editable pin labels false; layout Single Row; package THT; chip label ?</td>
</tr><tr>
    <td>1</td>
    <td>Red (633nm) LED</td>
    <td class="props">color Red (633nm); package 1206 [SMD]</td>
</tr><tr>
    <td>1</td>
    <td>Blue (525nm) LED</td>
    <td class="props">color Blue (525nm); package 0805 [SMD]</td>
</tr><tr>
    <td>1</td>
    <td>NEO-6M GPS Module</td>
    <td class="props">model NEO-6M; pins 4; type GPS; part # GY-GPS6MV2</td>
</tr><tr>
    <td>1</td>
    <td>RioRand LM2596</td>
    <td class="props">variant variant 2; chip label lm2596</td>
</tr><tr>
    <td>1</td>
    <td>Amica NodeMCU DevKit 1.0</td>
    <td class="props">model Amica; pins 30; type NodeMCU</td>
</tr><tr>
    <td>1</td>
    <td>1kΩ Resistor</td>
    <td class="props">resistance 1kΩ; package 2010 [SMD]; tolerance ±5%</td>
</tr><tr>
    <td>2</td>
    <td>1kΩ Resistor</td>
    <td class="props">resistance 1kΩ; package 0603 [SMD]; tolerance ±5%</td>
</tr><tr>
    <td>1</td>
    <td>Pushbutton</td>
    <td class="props">package [SMD]</td>
</tr>
  </tbody>
</table>
<p class="meta"><br/>Exported with Fritzing 0.9.3- http://fritzing.org</p>

<h1><strong>Schematic Design (<a href="https://fritzing.org/">Frizting</a>)</strong></h1>
<img src="https://user-images.githubusercontent.com/25394902/173062644-b66e9cbc-15ea-4836-9592-4c98d1ded26c.png" />
