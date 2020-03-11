#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <LoRa.h>


const char *ssid = "loro";
const char *password = "pingu4prez";
#define SS      18
#define RST     14
#define DI0     26


WebServer server(80);



void handleSentVar1() {
  Serial.println("handleSentVar function called...");
  if (server.hasArg("sensor1h")) { // Esta es la variable que se envia desde el cliente
    Serial.println("Sensor 1 , datos recibidos...");
                
                int humedad1 = server.arg("sensor1h").toInt();
                int temperatura1 = server.arg("sensor1t").toInt();
                int suelohum1 = server.arg("sensor1s").toInt();
                int ID=1;
                Serial.print("Lectura de temperatura recibida: ");
                Serial.println(temperatura1);
                Serial.print("Lectura de humedad recibida: ");
                Serial.println(humedad1);
                Serial.print("Lectura de suelo humedad recibida: ");
                Serial.println(suelohum1);               
                Serial.println();
                server.send(200, "text/html", "Datos recibidos");
                //------------desde aqui envia al LoRa---------------
                String message1 = String(ID)+"#"+String(temperatura1) + "/" + String(humedad1) + "&" + String(suelohum1);
                delay(1000);
                LoRa.beginPacket();
                LoRa.print(" ");
                LoRa.print(message1);
                LoRa.endPacket();
                  
                delay(2000);
               
 
          }
   }
 
void handleSentVar2() {
  Serial.println("handleSentVar2 function called...");
  if (server.hasArg("sensor2h")) { // Esta es la variable que se envia desde el cliente
    Serial.println("Sensor 2 , datos recibidos");
                
                int humedad2 = server.arg("sensor2h").toInt();
                int temperatura2 = server.arg("sensor2t").toInt();
                int suelohum2 = server.arg("sensor2s").toInt();
                int ID=2;
                Serial.print("Lectura de temperatura recibida: ");
                Serial.println(temperatura2);
                Serial.print("Lectura de humedad recibida: ");
                Serial.println(humedad2);
                Serial.print("Lectura de suelo humedad recibida: ");
                Serial.println(suelohum2);               
                Serial.println();
                server.send(200, "text/html", "Datos recibidos");
                //------------desde aqui envia al LoRa---------------
                String message2 = String(ID)+"#"+String(temperatura2) + "/" + String(humedad2) + "&" + String(suelohum2);
                delay(1000);
                LoRa.beginPacket();
                LoRa.print(" ");
                LoRa.print(message2);
                LoRa.endPacket();  

                delay(2000);
                
          }
   }

void handleSentVar3() {
  Serial.println("handleSentVar3 function called...");
  if (server.hasArg("sensor3h")) { // Esta es la variable que se envia desde el cliente
    Serial.println("Sensor 3 , datos recibidos");
                
                int humedad3 = server.arg("sensor3h").toInt();
                int temperatura3 = server.arg("sensor3t").toInt();
                int suelohum3 = server.arg("sensor3s").toInt();
                int ID=3;
                Serial.print("Lectura de temperatura recibida: ");
                Serial.println(temperatura3);
                Serial.print("Lectura de humedad recibida: ");
                Serial.println(humedad3);
                Serial.print("Lectura de suelo humedad recibida: ");
                Serial.println(suelohum3);               
                Serial.println();
                server.send(200, "text/html", "Datos recibidos");
                //------------desde aqui envia al LoRa---------------
                String message3 = String(ID)+"#"+String(temperatura3) + "/" + String(humedad3) + "&" + String(suelohum3);
                delay(1000);
                LoRa.beginPacket();
                LoRa.print(" ");
                LoRa.print(message3);
                LoRa.endPacket();  

                delay(2000);

               //return temperatura3; 
               }
               
   }  

   // Serial.print("(1)"+temperatura1 +" "+humedad1+" "+suelohum+" "+"(2)"+temperatura2+" "+humedad2+" "+suelohum2+" "+"(3)"+temperatura3+" "+humedad3+" "+suelohum3);

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configurando AP...");

  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Dirección IP del AP: ");
  Serial.println(myIP);
  server.on("/data1/", HTTP_GET, handleSentVar1); // Cuando el servidor recibe una peticion con "/data/" en el string,corre la funcion "handleSentVar" 
  server.on("/data2/", HTTP_GET, handleSentVar2);
  server.on("/data3/", HTTP_GET, handleSentVar3);
  server.begin();
  Serial.println("servidor HTTP iniciado");
  //-----------------------desde aqui empieza configuracion LoRa--------------------
  while (!Serial);
  Serial.println("LoRa Sender");

  //Se ajusta el modulo LoRa
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);
  
  //Se reemplaza el argumento en LoRa.begin(---E-) con la frecuencia deseada
  //433E6 para Asia
  //866E6´para Europe
  //915E6 para Norte America
  if (!LoRa.begin(915E6)) {
    Serial.println("Error al inicializar el modulo LoRa ");
    while (1);
  }
  LoRa.setSyncWord(0xF3);
  Serial.println("¡LoRa incializado correctamente!");

  //int data3 = handleSentVar3();
  //Serial.println("variable devuelta "+data3);
}

void loop() {
  server.handleClient();
}
