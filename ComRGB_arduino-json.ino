/* Lycée La Fayette 
Pilotage de barre de led RGB 
via le wifi et mqtt
07/12/24
Commande de barre de LED RGB json avec mqtt
topic: -------
{
    "name": "led",
    "status": "on",
    "pin":682,
    "lum":125,
    "colorRED": 0,
    "colorGREEN": 255,
    "colorBLUE": 255
  }
Le champs pin correspond aux led allumées 1010101010 =>682
{
    "name": "led",
    "status": "off"
}

https://www.arduino.cc/reference/en/libraries/arduino_json/
https://github.com/256dpi/arduino-mqtt
https://wiki.seeedstudio.com/Grove-RGB_LED_Stick-10-WS2813_Mini/
*/

#define BROKER_IP    "172.21.28.1"
#define DEV_NAME     "mqttdevice"
#define MQTT_USER    ""
#define MQTT_PW      ""
/*
#define BROKER_IP    "192.168.1.28"
#define DEV_NAME     "mqttdevice"
#define MQTT_USER    "lafayette"
#define MQTT_PW      "lafayette"
*/
#define TOPIC        "carteled/led"
/*
const char ssid[] = "ORBI50";
const char pass[] = "modernwater884";
*/
const char ssid[] = "ciscoTSSN2";
const char pass[] = "btssn@dm1n";

#include <MQTT.h>

#ifdef ARDUINO_SAMD_MKRWIFI1010
#include <WiFiNINA.h>
#elif ARDUINO_SAMD_MKR1000
#include <WiFi101.h>
#elif ESP8266
#include <ESP8266WiFi.h>
#else
#error unknown board
#endif

WiFiClient net;
//Attention par defaut les buffers sont de 128 octets pour les données du topic
//MQTTClient(int readBufSize, int writeBufSize);
MQTTClient client(256,256); 
unsigned long lastMillis = 0;

#include <Arduino_JSON.h>
#include <Adafruit_NeoPixel.h>

// pin où est connecté la chaine de led RGB
#define PIN         1 
String var;
int j=0;

// Nombre de LED
#define NUMPIXELS 10
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 1 // Time (in milliseconds) to pause between pixels

String status;

void connect() {
  Serial.println("wifi ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("La connexion wifi est réalisée");
  Serial.println("Attente de connexion au broker MQTT");
  while (!client.connect(DEV_NAME, MQTT_USER, MQTT_PW)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnecté au serveur MQTT!");
  Serial.println("Souscription au topics");
  client.subscribe(TOPIC); //SUBSCRIBE TO TOPIC hello
}
/*
*
* Reception et traitement de la donnée Json sur le topic
*/
void messageReceived(String &topic, String &payload) {
  int pin;
  byte colorRED,colorGREEN,colorBLUE,lum;
  Serial.println("Reception JSON sur le Topic: " + topic + " \n " + payload);
  JSONVar myObject = JSON.parse(payload);

  //Serial.println(JSON.typeof(myObject));
  Serial.println("Lecture des différents objets du JSON: ");
  if (myObject.hasOwnProperty("name")) {
    Serial.print("Object[\"name\"] = ");      
    Serial.println((const char*) myObject["name"]); //affichage
  }

  if (myObject.hasOwnProperty("status")) {
    Serial.print("myObject[\"status\"] = ");
    status = String((const char*) myObject["status"]);
    Serial.println((const char*) myObject["status"]);
  }

  if (myObject.hasOwnProperty("pin")) {
    Serial.print("myObject[\"pin\"] = ");
    pin =(int) myObject["pin"];
    Serial.println((int) myObject["pin"]);
  }

  if (myObject.hasOwnProperty("colorRED")) {
    Serial.print("myObject[\"colorRED\"] = ");
    colorRED =(byte) myObject["colorRED"];
    Serial.println((byte) myObject["colorRED"]);
  }

  if (myObject.hasOwnProperty("colorGREEN")) {
    Serial.print("myObject[\"colorGREEN\"] = ");
    colorGREEN =(byte) myObject["colorGREEN"];
    Serial.println((byte) myObject["colorGREEN"]);
  }
  if (status == "lum") {
    Serial.print("myObject[\"lum\"] = ");
    lum =(byte) myObject["lum"];
    Serial.println((byte) myObject["lum"]);
  }

  if (myObject.hasOwnProperty("colorBLUE")) {
    Serial.print("myObject[\"colorBLUE\"] = ");
    colorBLUE =(byte) myObject["colorBLUE"];
    Serial.println((byte) myObject["colorBLUE"]);
  }
  
  if (topic == TOPIC) {
    if (status == "on") {
      Serial.println("on");
      Serial.println(pin);
      digitalWrite(LED_BUILTIN, HIGH);
      ledRGB(colorRED,colorGREEN,colorBLUE,lum,pin);
    } 
    if (status == "off") {
      Serial.println("off");
      digitalWrite(LED_BUILTIN, LOW);     
      ledRGBClear();
    }
  }

}

/*
* Pilotage de la barre de led
* arguments:
* byte R
* byte G
* byte B
* int led       1010101010 ->1 led allumée ->0 led éteinte
* retour
*/

void ledRGB(byte R, byte G,byte B,byte L,int led){
  ledRGBClear();
  pixels.setBrightness(25);
  pixels.begin();
  led = led & 0x3ff;

  Serial.print("led: ");
  Serial.println(led,BIN);
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
  
    Serial.print("led numéro: ");
    Serial.print(i);
    Serial.print(" - ");
    Serial.println(led,BIN);
    
    if((led&1)==1){
          Serial.print("Couleur -");
          Serial.print(R,DEC);
          Serial.print("-");
          Serial.print(G,DEC);
          Serial.print("-");
          Serial.println(B,DEC);
          Serial.print("-");
          Serial.println(L,DEC);
          pixels.setPixelColor(i, pixels.Color(R,G,B));
          //pixels.setBrightness(L);
          pixels.show();   // Envoie la valeur sur les led.
                    
        }
      
      else{
        Serial.println(0);
          pixels.setPixelColor(i, pixels.Color(0,0,0));   
          pixels.show();   // Envoie la valeur sur les led.   
      }
      delay(DELAYVAL); // Pause
      led =led>>1;
    } 
}

/*
* Pilotage de la barre de led
* Eteint toutes les led
*/
void ledRGBClear(){
   for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
    pixels.setPixelColor(i, pixels.Color(0,0,0));
    pixels.show();   // Envoie la valeur sur les led.
  }
 
}
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // wait for serial port to connect. Needed for native USB port only
    // attempt to connect to WiFi network:
  }
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println("Connecté au wifi");
  pinMode(LED_BUILTIN, OUTPUT);
  pixels.begin(); // initialise l'objet NeoPixel
  pixels.clear(); // Eteint tous les pixels
  
  // MQTT brokers .
  client.begin(BROKER_IP, 1883, net);
  client.onMessage(messageReceived);
  
  connect();
}
void loop() {
  client.loop();
  if (!client.connected()) {
    connect();
  }

}

