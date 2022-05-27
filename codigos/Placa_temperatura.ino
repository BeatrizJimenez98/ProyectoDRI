/*
Codigo usado en la placa 4 para el manejo de la electrovalvula, con este codigo nos conectamos al servidor ThingsBoard y esperamos que mediante el protocolo RCP nos envie la señal
para encender o apagar la electrovalvula con el metodo electrovalvulaAutomatica, tambien podemos activarla desde el servidor directamente con el metodo electrovalvulaManual.
La electrovalvula esta conectada a un rele KY-0191 que esta conectado a la placa por el puerto GPI04.




*/

#include <WiFi.h>           // WiFi control for ESP32
#include <ThingsBoard.h>    // ThingsBoard SDK
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

// WiFi 
#define WIFI_AP_NAME        "wifigrupo8"
#define WIFI_PASSWORD       "wifigrupo8"
#define TOKEN               "UM2BcqBXxSqpFOx19KM7"

// IP ThingsBoard.
#define THINGSBOARD_SERVER  "192.168.0.101"


// Inicializar cliente ThingsBoard
WiFiClient espClient;
// Inicializar instancia ThingsBoard
ThingsBoard tb(espClient);
// Wifi  status
int status = WL_IDLE_STATUS;

const int Pinelectrovalvula = 4;
boolean electrovalvula=true;
int contadorB=0;
int contador=0;

bool subscribed = false;

RPC_Response electrovalvulaAutomatica(const RPC_Data &data)
{
  Serial.println("Valor electrovalvula automatica");
  comprobarelectrovalvula(data);
  return RPC_Response(NULL, electrovalvula);
  }

RPC_Response electrovalvulaManual(const RPC_Data &data)
{
  if (data==true){
    
  Serial.println("Apagado manual");
    digitalWrite(Pinelectrovalvula, LOW);
  }
   else if(data==false){
     
  Serial.println("Encendido manual");
     digitalWrite(Pinelectrovalvula, HIGH);
    contadorB=0;
   }
  
  return RPC_Response(NULL, electrovalvula);
}



// RPC handlers
RPC_Callback callbacks[] = {
  { "setValue",         electrovalvulaAutomatica },
  { "gete",         electrovalvulaManual }
};


void setup() {
  Serial.begin(115200);
 
  status = WL_IDLE_STATUS;
  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  InitWiFi();

  // Pin 
   pinMode(Pinelectrovalvula, OUTPUT);
   digitalWrite(Pinelectrovalvula, LOW);
    
  

 

}

//Metodo comprobar electrovalvula

void comprobarelectrovalvula(const RPC_Data data){
  pinMode(Pinelectrovalvula, OUTPUT);
   if(data==true){
     
    contadorB++;
  }else if(data==false){
      Serial.println("Electrovalvula automatica Apagada");
    digitalWrite(Pinelectrovalvula, HIGH);
    contadorB=0;
  }
  if(contadorB>4){
    Serial.println("Electrovalvula automatica Encendida");    
    digitalWrite(Pinelectrovalvula, LOW);
  
  }
 
  electrovalvula=data;
  Serial.print("Electrovalvula: ");
  Serial.println(electrovalvula);
  Serial.print("CONTADOR: ");
  Serial.println(contadorB);
}



// Main application loop
void loop() {

  

  // Reconectar a wifi
  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
    return;
  }

  //Reconectar a Thingsboard
  if (!tb.connected()) {
    subscribed = false;

    Serial.print("Conectando a: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" con el  token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Fallo al conectar");
      return;
    }
  }

  // Subscrirse al RPC
  if (!subscribed) {
    Serial.println("Subscriendose al RPC...");


    if (!tb.RPC_Subscribe(callbacks, COUNT_OF(callbacks))) {
      Serial.println("Fallo al subscribirse al RPC");
      return;
    }

    Serial.println("Subscripcion correcta");
    subscribed = true;
  }
if(contador>=4 && contador <=5){
    Serial.println("Envio de datos...");

   
  
   if(electrovalvula==false){
    tb.sendTelemetryString("Bomba","ON");
   }else{
    tb.sendTelemetryString("Bomba","OFF");
   }
    if(contador>=5){contador=0;}
  
}
  Serial.println(contador);
  contador++;
  delay(1500);
  tb.loop();
}

void InitWiFi()
{
  Serial.println("Conectando al punto de acceso...");
  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado");
}

void reconnect() {
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
