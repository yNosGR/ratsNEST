#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D1

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

const char* ssid = "YOURSSID";
const char* password = "THISISYOURPSK";

ESP8266WebServer server(80);
char temperatureString[6];
char temperatureStringF[6];

const int led = 13;

float getTemperature() {
  float temp;
  
  do {
    DS18B20.requestTemperatures(); 
    temp = DS18B20.getTempCByIndex(0);
    delay(100);
  } while (temp == 85.0 || temp == (-127.0));
  
  return temp;
}

void setup(void){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    float temperature = getTemperature();
    // celsius * 9 / 5 + 32 can be simplified down to 1.8 * temp
    float temperatureF = 1.8 * temperature + 32;

    dtostrf(temperature, 2, 2, temperatureString);
    dtostrf(temperatureF, 2, 2, temperatureStringF);
    
    String title = "Temperature";
    String cssClass = "mediumhot";
  
    if (temperature < 0)
      cssClass = "cold";
    else if (temperature > 20)
      cssClass = "hot";      
        
  
    String message = "<!DOCTYPE html><html><head><title>" + title + "</title><meta charset=\"utf-8\" /><meta name=\"viewport\" content=\"width=device-width\" /><link href='https://fonts.googleapis.com/css?family=Advent+Pro' rel=\"stylesheet\" type=\"text/css\"><style>\n";
    message += "html {height: 100%;}";
    message += "div {color: #fff;font-family: 'Advent Pro';font-weight: 400;left: 50%;position: absolute;text-align: center;top: 50%;transform: translateX(-50%) translateY(-50%);}";
    message += "h2 {font-size: 90px;font-weight: 400; margin: 0}";
    message += "body {height: 100%;}";
    message += ".cold {background: linear-gradient(to bottom, #7abcff, #0665e0 );}";
    message += ".mediumhot {background: linear-gradient(to bottom, #81ef85,#057003);}";
    message += ".hot {background: linear-gradient(to bottom, #fcdb88,#d32106);}";
    message += "</style></head><body class=\"" + cssClass + "\"><div><h1>" + title +  "</h1><h2>" + temperatureString + "&nbsp;<small>&deg;C</small><br>" + temperatureStringF + "&nbsp;<small>&deg;F</small></h2></div></body></html>";
    
    server.send(200, "text/html", message);
  });

   server.on("/json", [] () {
    float temperature = getTemperature();
    // celsius * 9 / 5 + 32 can be simplified down to 1.8 * temp
    float temperatureF = 1.8 * temperature + 32;

    dtostrf(temperature, 2, 2, temperatureString);
    dtostrf(temperatureF, 2, 2, temperatureStringF);
    

      String message = "{\n";
     message += " \"tempC\":";
     message += "\"";
     message += temperatureString;
     message += '"';
     message += "\,";
     message += "\n";
     message += " \"tempF\":";
     message += "\"";
     message += temperatureStringF;
     message += "\"";
     message += "\n}";
    //message += "</style></head><body class=\"" + cssClass + "\"><div><h1>" + title +  "</h1><h2>" + temperatureString + "&nbsp;<small>&deg;C</small><br>" + temperatureStringF + "&nbsp;<small>&deg;F</small></h2></div></body></html>";
    
     server.send(200, "application/json", message);
   });

  server.onNotFound( [] () {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i=0; i<server.args(); i++){
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
  });
  
  server.begin();
  
  Serial.println("Temperature web server started!");
}


void loop(void){
  
  server.handleClient();
}
