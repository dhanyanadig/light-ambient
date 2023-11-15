#include <WiFi.h> // Load Wi-Fi library for ESP32

const char* ssid = "OPPO A9 2020"; // Replace with your network credentials
const char* password = "danya123";

WiFiServer server(80); // Set web server port number to 80
String header;
String output5State = "off"; // Auxiliary variables to store the current o/p state
String output4State = "off";
const int output5Pin = 6; // Assign output variables to GPIO pins (change to your pin numbers)
const int output4Pin =  42;

unsigned long currentTime = millis(); // Current time
unsigned long previousTime = 0; // Previous time
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200); // Set baud rate
  pinMode(output5Pin, OUTPUT); // Initialize the output variables as outputs
  pinMode(output4Pin, OUTPUT);
  digitalWrite(output5Pin, LOW); // Set outputs to LOW
  digitalWrite(output4Pin, LOW);

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
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available(); // Listen for incoming clients

  if (client) { // If a new client connects,
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = ""; // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;

    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();

      if (client.available()) { // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c); // print it out the serial monitor
        header += c;

        if (c == '\n') { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Handle LED control
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("LED 5 on");
              output5State = "on";
              digitalWrite(output5Pin, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("LED 5 off");
              output5State = "off";
              digitalWrite(output5Pin, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("LED 4 on");
              output4State = "on";
              digitalWrite(output4Pin, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("LED 4 off");
              output4State = "off";
              digitalWrite(output4Pin, LOW);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server -Dhanya's Home</h1>");
            // Display current state, and ON/OFF buttons for GPIO 5
            client.println("<p>LED 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button
            if (output5State == "off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            // Display current state, and ON/OFF buttons for GPIO 4
            client.println("<p>LED 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button
            if (output4State == "off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();

            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') { // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
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