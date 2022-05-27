/*
Codigo placa 1, solo tiene el sensor de humedad conectado al pin GPI036 de la placa ESP32, 
la unica funcion de esta placa es el envio de la humedad al servidor Thingsboard, 
como vemos en el codigo nos conectamos a una red Wifi, que sera la misma que el servidor Thingsboard,
nos conectamos con  un token que tiene cada una de las placas.

Una vez que estamos conectados a la red y con el servidor, en el metodo loop() nos conectamos al servidor y 
con la variable SensorValue almacenamos el valor del la humedad, tenemos un contador para enviar muestras cada 40 segundos, 
enviamos una muestra y ponemos ese contador a 0 para iniciar otra vez.
Para comprobar el correcto funcionamiento mientras esperamos que se envien los datos, vamos mostrando por el serial los datos.

Finalmente tenemos el metodo InitiWifi() para conectarnos a la red y el metodo reconnect() para reconectarnos en caso de error.

*/

#include <WiFi.h>          
#include <ThingsBoard.h>   
#include <Wire.h>



// WiFi 
#define WIFI_AP_NAME        "wifigrupo8"
#define WIFI_PASSWORD       "wifigrupo8"
#define TOKEN               "b0YmGkA47PXn0XWp9gej"
#define THINGSBOARD_SERVER  "192.168.0.101"


// Inicializar cliente ThingsBoard 
WiFiClient espClient;
// Inicializar instancia ThingsBoard
ThingsBoard tb(espClient);
// status Wifi
int status = WL_IDLE_STATUS;


const int Pinhumedad = 36;
int contador= 0;

void setup() {
  Serial.begin(115200);
  pinMode(Pinhumedad, INPUT);  
  
  status = WL_IDLE_STATUS;
  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  InitWiFi();
}

void loop(){   

  // Reconectarse a WiFi
  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
    return;
  }

  // Reconectarse a ThingsBoard
  if (!tb.connected()) {

    // Conectarse a ThingsBoard
    Serial.print("Conectando a: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" con el token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Fallo al conectarse");
      return;
    }
  }

  

  int SensorValue = analogRead(Pinhumedad);
  if (contador>=20){	  
	  
		 Serial.println("Enviando datos ...");        
		 // Envio de datos al thingsboard
		 tb.sendTelemetryInt("Humedad1",SensorValue);
		 Serial.println(SensorValue);
		 contador=0;
	}  
  Serial.println(contador);
  Serial.println(SensorValue);
  contador++;
  delay(1500);
  
  tb.loop();  
}

void InitWiFi(){
  status = WL_IDLE_STATUS;
  Serial.println("Conectando a");
  Serial.print(WIFI_AP_NAME);
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado");
}

void reconnect(){
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Conectado");
  }
}
