
// ***************************************
//     ESP8266 Switch for Amazon Echo
// ***************************************
// target: ES-12F ESP8266 module, 80MHz CPU clock, 40MHz flash clock, DIO mode
// v.1.0.0
// 2018.01. by Bird

#include <ESP8266WiFi.h>          //standard ESP8266 Wi-Fi library

#include <fauxmoESP.h>            // fauxmoESP library
#include <WeMo.h>

#define DEVICE_NAME     "Magic Bird"    // name of the device identified by Echo
#define ESSID           "AWV2-7F-RIGHT" // Wi-Fi ESSID name
#define WIFI_PASSWORD   "connectme"     // Wi-Fi password
#define SSR             14              // pin to control SSR, active high
#define LED             16              // pin to control on-board LED, active high

//***********************************************************************************************

fauxmoESP fauxmo;                 // fauxmo object, the emulated WeMo switch

//***********************************************************************************************

void wifiInit()                   // Wi-Fi initialization
{
    WiFi.mode(WIFI_STA);
    Serial.printf("[WIFI] Connecting to %s ", ESSID);
    WiFi.begin(ESSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)       // wait while connecting
    {
        Serial.print(".");
        delay(100);
        digitalWrite(LED,~digitalRead(LED));   // blink the LED while connecting
    }
    Serial.println();

    Serial.printf("Connected. IP address: %s\n",  WiFi.localIP().toString().c_str());
    digitalWrite(LED, LOW);
}

void IO_init()   //misc I/O initialization
{
  pinMode(LED, OUTPUT);       
  pinMode(SSR, OUTPUT);   
  digitalWrite(LED, LOW);
  digitalWrite(SSR, LOW);
}

void onSetHandler(unsigned char device_id, const char * device_name, bool state)    // callback function of setting state
{
  Serial.printf("[MAIN] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
  digitalWrite(SSR, state);
  digitalWrite(LED, state);
}

bool onGetHandler(unsigned char device_id, const char * device_name)                // callback function of getting state
{
  return digitalRead(SSR);
}


void fauxmoInit()
{
  fauxmo.enable(true);                // enable fauxmo
  fauxmo.addDevice(DEVICE_NAME);      // add device
  
  fauxmo.onSetState(onSetHandler);    // register callback functions  
  fauxmo.onGetState(onGetHandler);

}
//***********************************************************************************************

void setup() 
{
  Serial.begin(115200);
  IO_init();
  wifiInit();
  fauxmoInit();
}

void loop()
{
  fauxmo.handle();        // fauxmoESP 2.0 does not use asyn. UDP library so it needs polling in loop
}
