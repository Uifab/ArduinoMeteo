#include <SPI.h>
#include <Wire.h>

//Sensor
#include <DHTesp.h>

//Wifi
#include <ESP8266WiFi.h>

//Http Client
#include <ESP8266HTTPClient.h>

#include "config.h"

//#define __DEBUG__

// Sustituir con datos de vuestra red
const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASSWORD;

String serverName = "http://uifab.ddns.net/lara/api/meteo";

void debug_print(String msg ){
	#ifdef __DEBUG__
		Serial.println(msg);
	#endif
}

//Saca los datos y los envia al servior
void sendDataToServer(){

	DHTesp dht;
	WiFiClient client;
	HTTPClient http;

	float Temperature;
	float Humidity;


	//Iniciamos el sensor
	dht.setup(13, DHTesp::DHT11); // Connect DHT sensor to GPIO 16 es el D0 //15 D8

	debug_print("Conectando a:\t");

	// Esperar a que nos conectemos
	while (WiFi.status() != WL_CONNECTED) 
	{
		delay(400);
		Serial.print('.');
	}
	
	debug_print("");
	debug_print("Conectado");

  
	//Get Data
	Temperature = dht.getTemperature();
	//Humidity    = dht.getHumidity(); // De momento no se usa

  
	//Break
	if ( WiFi.status() != WL_CONNECTED ){

		debug_print("Error: Wifi not Connected");
		return;
	}
   
  
	//Convert to String
	String Temp = String(Temperature);
	String Hum  = String(Humidity);

	if( http.begin(client, "http://uifab.ddns.net/lara/api/meteo?t=" + Temp + "&h=" + Hum + "&zone=2" ) ) {  

		int httpCode = http.GET();

		if( httpCode > 0 ) {
	  
			// HTTP header has been send and Server response header has been handled
			debug_print("HTTP GET");
  
			// file found at server
			String payload = http.getString();
			debug_print(String(httpCode));
			debug_print(payload);
	  
		} else {
			debug_print("[HTTP] GET... failed, error:");
			debug_print( http.errorToString(httpCode).c_str() );
		}

		http.end();
   
  	}else{
		debug_print("Error: Could not connect");
  	}
}

void setup() {

	String test1 = String();

	//Serial
	#ifdef __DEBUG__
		Serial.begin(9600);
		delay(10);
	#endif

	debug_print("Start");
  
	//Wifi
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	//Send data
	sendDataToServer();

	WiFi.mode(WIFI_OFF);

	debug_print("Sleep");

	ESP.deepSleep(50*60e6);
}


void loop() {
}