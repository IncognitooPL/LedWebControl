#include <Arduino.h>
#include <SPI.h>
#include <WiFININA.h>
#include <Adafruit_NeoPixel.h>

#include <credentials.h>                // file with saved network login details
char ssid[] = WiFi_SSID;                // your network SSID (name)
char pass[] = WiFi_PASSWD;              // your netword password

String respone, currentLine;            // make a String to hold incoming data from the client
int connection_status = WL_IDLE_STATUS; 

WiFiServer server(80);                  // creating a server on port 80

void serverSetup() {
  // check for WiFi module response 
  if (WiFi.status() == WL_NO_MODULE) {  
    Serial.println("Communication with WiFi module failed!");
    while (true); // don't continue
  }

  // check for available firmware update
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (connection_status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    // Connect to WPA/WPA2 network.
    connection_status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

String checkRequest(){
  if (currentLine.startsWith("GET /cmd?led=1")) {
    digitalWrite(LED_BUILTIN, HIGH);
    return "<script>console.log('DONE')</script>";
  }
  if (currentLine.startsWith("GET /cmd?led=0")) {
    digitalWrite(LED_BUILTIN, LOW);     
    return "<script>console.log('DONE')</script>";         
  }

  return "";
}

void serverListening() {
  WiFiClient client = server.available();         // listen for incoming clients

  if (!client) return;
   
  respone = "";

  while (client.connected()) {                    // loop while the client's connected
    if (!client.available()) break;               // if there are no headers then break

    char c = client.read();                       // read a byte of header

    if(c != '\n' && c != '\r')
      currentLine += c;

    if(c != '\n')
      continue;

    if(currentLine.length() != 0){
      if(respone == "")
        respone = checkRequest();
      currentLine = "";

      continue;
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();                             // Response headers ends with blank line

    client.print("Hi bitch");
    client.print(respone);
    
    client.println();                             // HTTP response ends with another blank line

    break;
  }
  
  client.stop();                                  // Close the connection with client
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setup() {
  // Initialize serial
  Serial.begin(9600);

  // Initialize server
  serverSetup();
  
  // start listening for incoming connections
  server.begin();
  printWifiStatus();
}

void loop() {
  serverListening();
}