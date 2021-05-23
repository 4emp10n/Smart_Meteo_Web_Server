//Dziuba Olesandr
// The third annex should form external monitoring and
// control the status of light bulbs by creating a server on the WiFi board
// developer NodeMCU V2 ESP8266 (CP2102), using for visualization
// status of indicators and light bulbs, as well as control of HTML system modes
// Web page. You need to create the page yourself. Opportunities to work with
// the page should be provided with the capabilities of HTML, CSS and JavaScript (to ensure interactivity).
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include<Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

DHT dht(D7, DHT11);

float t_prev = dht.readTemperature(), t_new = dht.readTemperature();
float h_prev = dht.readHumidity(), h_new = dht.readHumidity();
float t_temp, h_temp;

// Wi-Fi
const char* ssid = "373";
const char* password = "2899117fifc";

IPAddress ip(192,168,1,35);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

//LEDS
int D0_pin = D1;
int D2_pin = D2;
int D1_pin = D3;
int n_leds = 3;
const int leds[] = {D0_pin, D1_pin, D2_pin};

String webPage();
bool but_state = 0;

void PrintTempAndHum()
{
    Serial.print("t_prev: ");
    Serial.println(t_prev);
    Serial.print("t_new: ");
    Serial.println(t_new);
    Serial.print("h_prev: ");
    Serial.println(h_prev);
    Serial.print("h_new: ");
    Serial.println(h_new);
}

void WorkMode()
{
    t_new = dht.readTemperature();
    h_new = dht.readHumidity();

    if (isnan(t_prev) || isnan(t_new) || isnan(h_prev) || isnan(h_new))
    {
        t_prev = t_new;
        h_prev = h_new;
        Serial.println("Error");
    }
    else
    {
        PrintTempAndHum();

        for (int i = 0; i < n_leds; i++)
        {
            digitalWrite(leds[i], but_state);
        }

        if (abs(t_prev - t_new) >= 1 && abs(h_prev - h_new) >= 1)
        {
            for (int i = 0; i < n_leds; i++)
            {
                digitalWrite(leds[i], !but_state);
            }
            t_temp = t_prev;
            t_prev = t_new;
            h_temp = h_prev;
            h_prev = h_new;
        }
        else if (abs(t_prev - t_new) >= 1)
        {
            digitalWrite(leds[0], !but_state);
            digitalWrite(leds[1], but_state);
            digitalWrite(leds[2], but_state);
            t_temp = t_prev;
            t_prev = t_new;
        }
        else if (abs(h_prev - h_new) >= 1)
        {
            digitalWrite(leds[0], but_state);
            digitalWrite(leds[1], !but_state);
            digitalWrite(leds[2], but_state);
            h_temp = h_prev;
            h_prev = h_new;
        }
        else
        {
            for (int i = 0; i < n_leds; i++)
            {
                digitalWrite(leds[i], but_state);
            }
        }
    }
}


void setup(void){
  // preparing GPIOs
  for (int i = 0; i < n_leds; i++)
  {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
  
  dht.begin();

  delay(100);
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  
 
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
   Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  //+++++++++++++++++++++++ START  STATE = 0 ++++++++++++++++++++
  server.on("/", [](){
    server.send(200, "text/html", webPage());
  });
  server.on("/socket1On", [](){
    but_state = 1;
    server.send(200, "text/html", webPage());
    delay(100);
    
  });
  server.on("/socket1Off", [](){
    but_state = 0;
    server.send(200, "text/html", webPage());
    delay(100);
 });   
   //+++++++++++++++++++++++ END  STATE = 1 ++++++++++++++++++++ 

  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();
} 

String webPage()
{
  WorkMode();
  String web; 
  web += "<head><meta http-equiv=\"refresh\" content=\"8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/> <meta charset=\"utf-8\"><title>ESP 8266</title><style>button{color:red;padding: 10px 27px;}</style></head>";
  web += "<h1 style=\"text-align: center;font-family: Open sans;font-weight: 100;font-size: 20px;\">ESP8266 Web Server</h1><div>";
  //++++++++++ BUTTON  +++++++++++++
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">----LED 1----</p>";
  if (but_state == 1)
  {
    web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #43a209;margin: 0 auto;\">ON</div>";
  }
  else
  {
    web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #ec1212;margin: 0 auto;\">OFF</div>";
  }
  web += "<div style=\"text-align: center;margin: 5px 0px;\"> <a href=\"socket1On\"><button>ON</button></a>&nbsp;<a href=\"socket1Off\"><button>OFF</button></a></div>";
  // ++++++++ BUTTON +++++++++++++
  
  
  //++++++++++ DHT11 TEMP_PREV  +++++++++++++
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">----DHT11 TEMP PREV----</p>";
  web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #5191e4;margin: 0 auto;\">"+ String((float)t_temp)+"</div>";
  // ++++++++ DHT11 TEMP_PREV  +++++++++++++

   //++++++++++ DHT11 TEMP_NEW  +++++++++++++
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">----DHT11 TEMP NEW----</p>";
  web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #5191e4;margin: 0 auto;\">"+ String((float)t_new)+"</div>";
  // ++++++++ DHT11 TEMP_NEW  +++++++++++++
  
  //++++++++++ DHT11 H_PREV  +++++++++++++
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">----DHT11 H PREV----</p>";
  web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #5191e4;margin: 0 auto;\">"+ String((float)h_temp)+"</div>";
  // ++++++++ DHT11 H_PREV  +++++++++++++

  //++++++++++ DHT11 H_NEW  +++++++++++++
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">----DHT11 H NEW----</p>";
  web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #5191e4;margin: 0 auto;\">"+ String((float)h_new)+"</div>";
  // ++++++++ DHT11 H_NEW  +++++++++++++
  
  // ========REFRESH=============
  web += "<div style=\"text-align:center;margin-top: 20px;\"><a href=\"/\"><button style=\"width:158px;\">REFRESH</button></a></div>";
  // ========REFRESH=============
  web += "</div>";

  t_temp = t_prev;
  h_temp = h_prev;
  return(web);
}

