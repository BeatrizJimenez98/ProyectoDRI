/*
Codigo placa 2,  tiene el sensor de humedad conectado al pin GPI036 de la placa ESP32 y el sensor se temperatura conectado mediante I2C, 
la unica funcion de esta placa es el envio de la humedad y temperatura al servidor Thingsboard, como vemos en el codigo nos conectamos a una red Wifi,
que sera la misma que el servidor Thingsboard, nos conectamos con  un token que tiene cada una de las placas.

 En el metodo loop() nos conectamos al servidor y con la variable SensorValue almacenamos el valor del la humedad y 
 temperature para almacenar el valor de la temperatura, a la hora de enviar los datos los convertimos a String para realizar el envio
 en un Json para facilitar el trabajo en thingsboard
 tenemos un contador para enviar muestras cada 40 segundos, enviamos una muestra y ponemos ese contador a 0 para iniciar otra vez.
 Para comprobar el correcto funcionamiento mientras esperamos que se envien los datos, vamos mostrando por el serial los datos.

 Finalmente tenemos el metodo InitiWifi() para conectarnos a la red y el metodo reconnect() para reconectarnos en caso de error.
 
 */
#include <WiFi.h>           
#include <ThingsBoard.h>    
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <PubSubClient.h>



// WiFi 
#define WIFI_AP_NAME        "wifigrupo8"
#define WIFI_PASSWORD       "wifigrupo8"

//  token
#define TOKEN               "d0J11eBkmfubu26nT9IC"
// IP ThingsBoard 
#define THINGSBOARD_SERVER  "192.168.0.101"

// Inicializar cliente ThingsBoard 
WiFiClient espClient;
// Inicializar instancia ThingsBoard 
ThingsBoard tb(espClient);
// status Wifi
int status = WL_IDLE_STATUS;


PubSubClient client(espClient);

Adafruit_BME280 bme;
float temperature = 0;
int contador= 0;
//Pin sensor humedad
const int Pinhumedad = 36;


void setup() {
  Serial.begin(115200);
  status = bme.begin(); 
    if (!status) {
      Serial.println("No se ha encontrado el sensor BME280 comprueba la conexion!");
      while (1);
    } 
    
  pinMode(Pinhumedad, INPUT); 
  status = WL_IDLE_STATUS;
  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  InitWiFi();

}

void loop() {

  
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
   temperature = bme.readTemperature(); 
if (contador>=20){
 
  
    Serial.println("Enviando datos...");


  String temperature2 = String(temperature);
  String SensorValue2 = String(SensorValue);
  
String payload = "{";
  payload += "\"Temperatura2\":"; payload += temperature2; payload += ",";
  payload += "\"Humedad2\":"; payload += SensorValue2;
  payload += "}";

   char attributes[100];
  payload.toCharArray( attributes, 100 );
  tb.sendTelemetryJson(attributes);
  Serial.println( attributes );
  contador=0;
}
  Serial.println(temperature);
  Serial.println(SensorValue);
  Serial.println(contador);
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
