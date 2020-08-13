/*
  ESP8266-WebSocket_Receiver

  This sketch:
        1. Connects to a WiFi network
        2. Connects to a Websockets server
        3. Sends the websockets server an authentication message
        4. Prints all incoming messages
        5. Recognizes commands and executes them
            - If message received is "{AuthenticationString}_LED0-1" it turn on builtin led. (Replace AuthenticationString with your own)

  Before running replace connection and identification constants with your own.
  
  Hardware:
        For this sketch you only need an ESP8266 board.

  By David Steinmann
  https://github.com/DavidSteinmann/SmartHome

  ***A modification of Esp8266 Websockets Client By Gil Maimon
  ***https://github.com/gilmaimon/ArduinoWebsockets

*/

#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>

// Connection constants
const char* ssid = "....."; //Enter SSID
const char* password = "....."; //Enter Password
const char* websockets_server_host = "....."; //Enter server adress
const uint16_t websockets_server_port = .....; // Enter server port

// Custom login (identification) phrase
// It's the first thing it sends
char AuthenticationString[] = ".....";

using namespace websockets;

WebsocketsClient client;


// Pin declarations
const int Pin_LED = 2; // GPIO 2 = Built in led of Esp8266

// Code variables
const int messageLength = 129;

char* destination;
char* command;
char* value;


void setup() {
  Serial.begin(115200);
  // Connect to wifi
  WiFi.begin(ssid, password);


  // Make sure we're connected to wifi
  while (WiFi.status() != WL_CONNECTED) {
    // Wait some time to connect to wifi
    for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
      Serial.print(".");
      delay(1000);
    }

    // Check if connected to wifi
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("No Wifi!");

      // Wait for some time
      delay(10000);
    }
  }

  Serial.println("Connected to Wifi, Connecting to server.");


  // Try toconnect to Websockets server
  bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
  if (connected) {
    Serial.println("Connected!");
    // Send identification message
    client.send(String(AuthenticationString));
  } else {
    Serial.println("Not Connected!");
  }

  // Rrun callback when messages are received
  client.onMessage([&](WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
    char messageText[messageLength];
    (message.data()).toCharArray(messageText, messageLength);
    checkForValidCommand(messageText);
  });


  // Set up pins
  pinMode(Pin_LED, OUTPUT);
  digitalWrite(Pin_LED, 1);
}

void loop() {
  // Let the websockets client check for incoming messages
  if (client.available()) {
    //Serial.println("polling:");
    client.poll();
  }
  delay(10);
}

// Check if message is a command we understand if yes run it
void checkForValidCommand(char messageText[messageLength]) {
  // Split the message into parts
  parseCommand(messageText);
  
  Serial.println(command);

  // Check if we're the destination of the message
  if (String(destination) != String(AuthenticationString)) {
    Serial.println("command not valid");
    return;
  }
  Serial.println("command valid!");

  int valueInt = (String(value)).toInt();
  Serial.println(valueInt);

  // Check what command it is
  if (String(command) == "LED0") {
    Serial.println("command LED0");
    switch (valueInt) {
      case 0:
        valueInt = 1;
        break;
      case 1:
        valueInt = 0;
        break;
      default:
        valueInt = 1;
        break;
    }
    //Serial.println(valueInt);
    digitalWrite(Pin_LED, valueInt);
  }

}


// Split the message into parts
void parseCommand(char text[messageLength]) {
  destination = strtok(text, "_-");
  Serial.print("destination: ");
  Serial.print(destination);
  command = strtok(NULL, "_-");
  Serial.print("  command: ");
  Serial.print(command);
  value = strtok(NULL, "_-");
  Serial.print("  value :");
  Serial.println(value);
}
