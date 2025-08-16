#include <DHT.h>
#include <DHT_U.h>
#include <dht11esp8266.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define DHTPIN      D1
#define DHTTYPE     DHT11     // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);

#define RELAYPIN D2

#define WATER_DELAY 5000
#define OTHER_DELAY 10000

const char* ssid = "UowmOpen";
const char* password = "";
const String serverUrl = "https://zafora.ece.uowm.gr/~ece01724/"; // Replace with your server URL

//Moisture sensor pins
const int analogInPin0 = A0;  // 


int updatecounter = 0; //used to check and update humidity every 4 cycles

int SoilMoistValue = 0;   //the value readed from each moisture sensor  
long int SoilMoistSum = 0;    //the sum of the 30 times sampling
int SoilMoistPerc = 0;   //soil moisture Perc

float AirTempSum = 0;
float AirTempPerc = 0;

int AirMoistSum = 0;
int AirMoistPerc = 0;

int threshold = 10;

void setup() {
  Serial.begin(9600); 

  pinMode(RELAYPIN,OUTPUT);
  digitalWrite(RELAYPIN,HIGH); //turn relay off on boot

  dht.begin(); //init air temp/ air moisture sensor
  
  WiFiClient wificlient;
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");



  //Fetch threshold
  bool thres_fetched = false;
  while (!thres_fetched){
    thres_fetched = gethttp();
  }


}

void loop() {
  // put your main code here, to run repeatedly:

  moistureSampling();
  airsampling();

  Serial.print("Soil moisture: " );                        
  Serial.print(SoilMoistPerc); 
  Serial.println("%");

  Serial.print(F("Air Humidity: "));
  Serial.print(AirMoistPerc);
  Serial.println(F("%"));

  Serial.print(F("Air Temperature: "));
  Serial.print(AirTempPerc);
  Serial.println(F("C"));

  CheckMoisture();

  if (updatecounter >= 5){//updates every minute
    gethttp();
    Posthttp();
    updatecounter = 0;
  }
  updatecounter++;

}

void CheckMoisture(){
  if (SoilMoistPerc < threshold){ //water for 5'', wait another 5''
    digitalWrite(RELAYPIN,LOW);
    delay(WATER_DELAY);
    digitalWrite(RELAYPIN,HIGH);
    delay(OTHER_DELAY - WATER_DELAY);
  }else{
    digitalWrite(RELAYPIN,HIGH); //wait for 10''
    delay(OTHER_DELAY);
  }

}

void moistureSampling()// read the value of the soil moisture
{     
  
  for(int i = 0; i < 10; i++)//samping 10 time within 0.25 second
  {
    SoilMoistSum = SoilMoistSum + analogRead(analogInPin0);  
    delay(50);
  }
  SoilMoistValue = SoilMoistSum / 10;//get the average value
  
  SoilMoistPerc = (SoilMoistValue - 13) / 10.11;

  if (SoilMoistPerc < 0)
    SoilMoistPerc = 0;
  else if (SoilMoistPerc > 100)
    SoilMoistPerc = 100;
  
  
  SoilMoistSum = 0;//reset the variable

}

void airsampling(){
  sensors_event_t event;
  dht.temperature().getEvent(&event); //This one is only used to test connection
  
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature or humidity!"));
  }
  else {

    for(int i = 0; i < 10; i++)//samping 10 time within 0.5 second
    {
      dht.humidity().getEvent(&event);
      AirMoistSum = AirMoistSum + event.relative_humidity;
      dht.temperature().getEvent(&event);
      AirTempSum = AirTempSum + event.temperature;
      delay(50);
    }
    
    AirMoistPerc = AirMoistSum / 10;
    AirTempPerc = AirTempSum / 10;

    AirMoistSum = 0;
    AirTempSum = 0;
  }
}

bool gethttp(){
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      WiFiClientSecure client;

      String address = serverUrl+"/data.txt";

      Serial.print("Sending HTTP GET request to: ");
      Serial.println(serverUrl);

      client.setInsecure();

      http.begin(client,address);
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
          String payload = http.getString();
          Serial.println("HTTP Response code: " + String(httpResponseCode));
          Serial.println("Server response: " + payload);

          //update thresshold
          if (isNumeric(payload)){
            if(payload.toInt() <= 100 && payload.toInt() >= 0){
              threshold = payload.toInt();
              Serial.println("Threshold updated to " + String(threshold));
              http.end();
              return true;
            }
          }

      } else {
          Serial.println("Error on HTTP request");
          http.end();
          return false;
      }      
  }
  return false;
}

void Posthttp() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        WiFiClientSecure client;

        String address = serverUrl + "/update.php?temp=" + String(AirTempPerc,2) + "&hum=" + String(AirMoistPerc) + "&soil=" + String(SoilMoistPerc);

        Serial.print("HTTP string posted");
        Serial.println(address);

        client.setInsecure();

        http.begin(client, address);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            String payload = http.getString();
            Serial.println("HTTP Response code: " + String(httpResponseCode));
            Serial.println("Server response: " + payload);
        } else {
            Serial.println("Error on HTTP request");
        }

        http.end();
    }
}

boolean isNumeric(String str) {
    unsigned int stringLength = str.length();
 
    if (stringLength == 0) {
        return false;
    }
 
    boolean seenDecimal = false;
 
    for(unsigned int i = 0; i < stringLength; ++i) {
        if (isDigit(str.charAt(i))) {
            continue;
        }
 
        if (str.charAt(i) == '.') {
            if (seenDecimal) {
                return false;
            }
            seenDecimal = true;
            continue;
        }
        return false;
    }
    return true;
}

