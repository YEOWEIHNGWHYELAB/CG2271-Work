// Load Wi-Fi library
#include <WiFi.h>

#define RXD2 16
#define TXD2 17
#define OFF_HEX 0x30
#define RED_HEX 0x31
#define GREEN_HEX 0x32
#define BLUE_HEX 0x33

// Replace with your network credentials
const char* ssid = "DESKTOP-RQ7DTNT 4610";
const char* password = "0K91a49-";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String response, ip_address;

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();

// Previous time
unsigned long previousTime = 0;

// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  ip_address = WiFi.localIP().toString();
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /status") >= 0) {
              Serial.println("WiFi Connected: " + ip_address);
            } else if (header.indexOf("GET /forward") >= 0) {
              Serial.println("Red LED on");
              output26State = "on";
              Serial2.write(RED_HEX);
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /left") >= 0) {
              Serial.println("Green LED on");
              Serial2.write(GREEN_HEX);
              output27State = "on";
            } else if (header.indexOf("GET /right") >= 0) {
              Serial.println("Blue LED on");
              Serial2.write(BLUE_HEX);
            } else if (header.indexOf("GET /backward") >= 0) {
              Serial.println("LED off");
              Serial2.write(OFF_HEX);
              digitalWrite(output26, LOW);
            }
            //            // Display the HTML web page
            //            client.println("<!DOCTYPE html><html>");
            //            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            //            client.println("<link rel=\"icon\" href=\"data:,\">");
            //            // CSS to style the on/off buttons
            //            // Feel free to change the background-color and font-size attributes to fit your preferences
            //            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            //            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            //            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            //            client.println(".button2 {background-color: #555555;}</style></head>");
            //
            //            // Web Page Heading
            //            client.println("<body><h1>botNavex Web Server</h1>");
            //
            //            // Display current state, and ON/OFF buttons for GPIO 26
            //            client.println("<p>Red LED " + output26State + "</p>");
            //            // If the output26State is off, it displays the ON button
            //            if (output26State == "off") {
            //              client.println("<p><a href=\"/redon\"><button class=\"button\">ON</button></a></p>");
            //            } else {
            //              client.println("<p><a href=\"/redoff\"><button class=\"button button2\">OFF</button></a></p>");
            //            }
            //
            //            // Display current state, and ON/OFF buttons for GPIO 27
            //            client.println("<p>Green LED " + output27State + "</p>");
            //            // If the output27State is off, it displays the ON button
            //            if (output27State == "off") {
            //              client.println("<p><a href=\"/greenon\"><button class=\"button\">ON</button></a></p>");
            //            } else {
            //              client.println("<p><a href=\"/greenoff\"><button class=\"button button2\">OFF</button></a></p>");
            //            }
            //            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
