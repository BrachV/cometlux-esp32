#include <AsyncTCP.h>

#include <AsyncEventSource.h>
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <StringArray.h>
#include <WebAuthentication.h>
#include <WebHandlerImpl.h>
#include <WebResponseImpl.h>

#include <AsyncUDP.h>

/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

#include <WiFi.h>
#include <Grove_LED_Bar.h>
#include <math.h>

const char* ssid     = "cometlux";
const char* password = "cocorico";

// REFERENCE PIN : https://cdn.shopify.com/s/files/1/0609/6011/2892/files/doc-esp32-pinout-reference-wroom-devkit.png?width=692

// liste des pins pour le projecteur n°1 et sa led bar
// AVANT : 17, 16, 27
const int LED_1_PIN_CLOCK = 19;
const int LED_1_PIN_DATA = 23;
const int PROJECTEUR_1_PIN = 33;

// liste des pins pour le projecteur n°2 et sa led bar
const int LED_2_PIN_CLOCK = 5;
const int LED_2_PIN_DATA = 18;
const int PROJECTEUR_2_PIN = 4;

// initialisation des variables bar
Grove_LED_Bar bar1(LED_1_PIN_CLOCK, LED_1_PIN_DATA, 0, LED_BAR_10); // LED BAR N°1
Grove_LED_Bar bar2(LED_2_PIN_CLOCK, LED_2_PIN_DATA, 0, LED_BAR_10); // LED BAR N°2
// ....

WiFiServer server(80);

void setup()
{
  
    Serial.begin(115200);

    // ouvre les PIN pour les projecteurs PWM
    pinMode(PROJECTEUR_1_PIN, OUTPUT);
    pinMode(PROJECTEUR_2_PIN, OUTPUT);

    delay(10);
    
    // initialise les variables pour les LED BAR V2
    bar1.begin();
    bar2.begin();

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.print(" with password ");
    Serial.print(password);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

    delay(10000);
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}


void loop(){
    delay(50);  
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character


if (currentLine.indexOf("/P") > 0) {
            Serial.println("--POST");
  
            // exemple : POST /P?lum=255&id=0 HTTP/1.1
            String queryString = currentLine.substring(12,(currentLine.length()-8));
  
            // récupère dans la chaîne de caractères l'index où se trouve le séparateur "&"
            int indexSeparateur = queryString.indexOf("&");
  
            // extrait la valeur de la clé "lum"
            float lum = queryString.substring(0, indexSeparateur).toInt();
            
            // sécurité
            if (lum > 255) lum = 255;
            if (lum < 0) lum = 0;
            
            // extrait la valeur de la clé "id"
            int id = queryString.substring(indexSeparateur+4).toInt();
            Serial.print("id : ");
            Serial.println(id);
  
            // le led bar grove est composé de 10 niveaux, produit en croix sur l'intensité pour obtenir un résultat compris entre 0 et 10.
            float level = (lum/255)*10;
            Serial.print("level : ");
            Serial.println(level);

  
            switch(id)
            {
              // concerne tous les projecteurs 
              case 0:
                // pour le premier projecteur
                bar1.setLevel(level);
                analogWrite(PROJECTEUR_1_PIN, lum);
                
                // pour le second projecteur
                bar2.setLevel(level);
                analogWrite(PROJECTEUR_2_PIN, lum);
                
                // pour le ...
                break;
  
              // concerne le premier projecteur uniquement
              case 1:
                bar1.setLevel(level);
                analogWrite(PROJECTEUR_1_PIN, lum);
                break;
  
              // concerne le second projecteur uniquement
              case 2:
                bar2.setLevel(level);
                analogWrite(PROJECTEUR_2_PIN, lum);
                break;
  
              // /...
  
              // .../
              
              default:
                Serial.print("ID inconnu.");
                break;
            }
          }

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) 
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }


          
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        // if (currentLine.endsWith("GET /L/H")) {
        //    analogWrite(pwm, 250);
        // }
        // if (currentLine.endsWith("GET /L")) {
        //    analogWrite(pwm, 20);              // GET /L turns the LED off
        //    Serial.print("low");
        // }
        
        
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
