#include <WiFi.h>
#include <DHT.h>
#include <DHT_U.h>
#include "DHT.h"

#define DHTTYPE DHT11

const char *ssid = "loro";
const char *password = "pingu4prez";

const int analogInPin = 0;  // Analog input pin al que el potenciometro esta conectado 
int sensorValue = 0;        // Valor que lee desde el potenciometro
int outputValue = 0;
int outputValue2 = 0; // Valor que se envia al servidor
int outputValue3 = 0;
const int DHTPin = 22;

DHT dht(DHTPin, DHTTYPE);



void setup() {
 // Se inicializa el sensor DHT 11
  dht.begin();
  Serial.begin(115200);
  delay(10);

  // Se conecta a la red WIFI
  Serial.println();
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  /* Se setea Explicitamente que esp32 sea un cliente WIFI, de otro modo,intentara por default
     actuar como el cliente y el punto de acceso por lo que podria causar problemas de red
     con cualquier otro dispositivo de wifi que existan en mi red WIFI */
    
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("dirección IP : ");
  Serial.println(WiFi.localIP());
  Serial.begin(115200);

}

void loop() {
   
   //Lee el valor de la temperatura
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();
  float suelohum = 0.95 * suelohum + 0.05 * analogRead(32);
  // Lee el valor analogico en:
  sensorValue = analogRead(A0);


  // Imprime los resultados en el monitor serial:
  Serial.print("sensor = ");
  Serial.print(sensorValue);
  Serial.print("prueba de temperatura =");
  Serial.println(temperatura);
  Serial.print("prueba de humedad =");
  Serial.println(humedad);
  Serial.print("prueba de suelo humedad =");
  Serial.println(suelohum);


  //Usa la clase WIfi para crear conexiones TCP 
  WiFiClient client;
  const char * host = "192.168.4.1";
  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("conexión fallida");
    return;
  }

  // Ahora se crea una URL para la petición
 String sensor3 = "/data3/";
 sensor3 += "?sensor3h=";//humedad
 sensor3 += humedad;
 sensor3 += "&sensor3t=";//temperatura
 sensor3 += temperatura;
 sensor3 += "&sensor3s=";//humedad del suelo
 sensor3 += suelohum;
  
  Serial.print("Pidiendo URL: ");
  Serial.println(sensor3);
  Serial.println();

  
  // Esto mandara una peticion al servidor
  client.print(String("GET ") + sensor3 + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  Serial.println();
  Serial.println("Cerrando conexión");
  Serial.println();
  Serial.println();
  Serial.println();

  delay(2000);
}
