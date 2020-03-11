// importa librerias
#include <WiFi.h>
#include <SPI.h>
#include <LoRa.h>
// Librerias para mostrar el tiempo en el servidor NTP
#include <NTPClient.h>
#include <WiFiUdp.h>

// Se reemplaza con los datos de la red

/*const char* ssid     = "cooper 5";
const char* password = "Unr34lT0urn4m3nt19827";*/


const char* ssid     = "SGS8+";
const char* password = "crisant3m0";

/*
// Set your Static IP address
IPAddress local_IP(192, 168, 43, 10);
// Set your Gateway IP address
IPAddress gateway(192, 168, 43, 1);

IPAddress subnet(255, 255, 0, 0);
*/

// Define el cliente NTP para obtener el tiempo
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables para guardar la fecha y el tiempo
String formattedDate;
String dayStamp;
String timeStamp;

// pines que usa el modulo LoRa Heltec
#define SS      18
#define RST     14
#define DI0     26

// Inicia las variables que ayudaran a guardar los datos del lora
String loRaMessage;
String temperatura1,temperatura2,temperatura3;
String suelohum1,suelohum2,suelohum3;
String humedad1,humedad2,humedad3;
String senderID,ID;


int temp1,temp2,temp3,temp1F,temp2F,temp3F;
int hum1,hum2,hum3,hum1F,hum2F,hum3F;
int sue1,sue2,sue3,sue1F,sue2F,sue3F;
int rssi;

// Se establece el servidor web con puerto 80
WiFiServer server(80);

// Variable que guarda la peticion HTTP
String header;

//tiempo actual
unsigned long tiempoActual= millis();

//tiempo previo
unsigned long tiempoPrevio=0;

//
const long tiempoAcaba = 2000;

void setup() { 
  // Se inicia el monitor serial
  Serial.begin(115200);
  
  // Se inicia el LoRa
  //reemplazar en --> LoRa.begin(---E-) la fecuencia que se ocupa en ambos LoRa
  //433E6 para Asia
  //866E6 para Europa
  //915E6 para Norte America
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);

  /*
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  */
  while (!LoRa.begin(915E6)) {
    Serial.println(".");
    delay(500);
  }
  // Establecer la palabra de sincronia (0xF3) debe ser igual a la del emisor
  // la palabra de sincronia (SyncWord) asegura que no se resivan mensajes de otros LoRa
  // tiene que ser un rango entre 0-0xFF
  LoRa.setSyncWord(0xF3);           
  Serial.println("¡LoRa iniciado correctamente!");

  // Se conecta a la red Wi-FI con el SSID y el password establecidos anteriormente
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Imprime la IP local e inicia el servidor Web
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
  /*
  Serial.println(WiFi.gatewayIP().toString());//para saber gateway
  */
  server.begin();

  // Inicializa el cliente NTP para obtener el tiempo
  timeClient.begin();
  // Se pone el tiempo en segundos para que se ajuste con la zona horaria
  // GMT -3 = -10800 (DST)
  // GMT -4 = -14400
  // GMT 0 = 0
  timeClient.setTimeOffset(-10800);
}

void loop() {
  // Comprueba si hay paquetes de LoRa disponibles
  int packetSize = LoRa.parsePacket();
  if (packetSize) { //si los hay, comienza la funcion getLoraData
      getLoRaData();
      getTimeStamp();
     if(temp1 != 0){
          temp1F=temp1;
          hum1F=hum1;
          sue1F=sue1;
          Serial.println("temperatura sensor 1 finales");
          Serial.println(temp1F);
          Serial.println(hum1F);
          Serial.println(sue1F);
         }
      if(temp2 != 0){
          temp2F=temp2;
          hum2F=hum2;
          sue2F=sue2;
          Serial.println("temperatura sensor 2 finales");
          Serial.println(temp2F);
          Serial.println(hum2F);
          Serial.println(sue2F);
         } 
      if(temp3 != 0){
          temp3F=temp3;
          hum3F=hum3;
          sue3F=sue3;
          Serial.println("temperatura sensor 3 finales");
          Serial.println(temp3F);
          Serial.println(hum3F);
          Serial.println(sue3F);
         }  

             Serial.println("Fecha lectura: "+ dayStamp);
  }
  WiFiClient client = server.available();   // Escucha los clientes

  if (client) {                             // Si se conecta un cliente nuevo,
    Serial.println("Nuevo cliente.");          // imprime un mensaje en el puerto serial
    String currentLine = "";                // crea un String para guardar los datos que vienen desde el cliente
    tiempoActual = millis();
    tiempoPrevio=tiempoActual;
    while (client.connected() && tiempoActual - tiempoPrevio <= tiempoAcaba) {            // mientras el cliente este conectado se queda en el loop
       tiempoActual = millis();
      if (client.available()) {             // si hay algunos bytes que leer desde el cliente,
        char c = client.read();             // lee el byte, y entonces
        Serial.write(c);                    // lo imprime en el monitor serial
        header += c;
        if (c == '\n') {                    // si el byte es un caracter de tipo salto de linea 
          // Si la linea actual esta vacia se obtienen dos saltos de linea de una sola vez
          // ese es el final de la peticion HTTP del cliente, por lo tanto se envia una respuesta
          if (currentLine.length() == 0) {
            // los HTTP headers siempre incian con un codigo de respuesta. ejemplo:(e.g. HTTP/1.1 200 OK)
            // y un content-type para que el cliente sepa que es lo que viene, despues una linea vacia:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Conexión: cerrada");
            client.println();
            
            // Muesta la pagina Web HTML
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS para darle estilo
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}");
            client.println("table { border-collapse: collapse; width:35%; margin-left:auto; margin-right:auto; }");
            client.println("th { padding: 12px; background-color: #3AD78C; color: white; }");
            client.println("tr { border: 1px solid #ddd; padding: 12px; }");
            client.println("tr:hover { background-color: #bcbcbc; }");
            client.println("td { border: none; padding: 12px; }");
            client.println(".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }");
            
            // Heading de la pagina Web
            client.println("</style></head><body><h1>Lecturas de los sensores </h1>");
            client.println("<h3>Fecha: " + dayStamp + "</h3>");
            client.println("<h3>Hora: " + timeStamp + "</h3>");
            client.println("<table><tr><th>Sensor</th><th>Temperatura</th><th>Humedad</th><th>Humedad suelo</th></tr>");
            client.println("<tr><td> (1) </td>");
            client.println("<td><span class=\"sensor\">");
            client.println(temp1F);
            client.println(" *C</span></td><td><span class=\"sensor\">");
            client.println(hum1F);
            client.println("</span></td><td><span class=\"sensor\">"); 
            client.println(sue1F);
            client.println("</span></td></tr>"); 
            client.println("<tr><td> (2) </td>");
            client.println("<td><span class=\"sensor\">");
            client.println(temp2F);
            client.println(" *C</span></td><td><span class=\"sensor\">");
            client.println(hum2F);
            client.println("</span></td><td><span class=\"sensor\">"); 
            client.println(sue2F);
            client.println("</span></td></tr>");  
            client.println("<tr><td> (3) </td>");
            client.println("<td><span class=\"sensor\">");
            client.println(temp3F);
            client.println(" *C</span></td><td><span class=\"sensor\">");
            client.println(hum3F);
            client.println("</span></td><td><span class=\"sensor\">"); 
            client.println(sue3F);
            client.println("</span></td></tr>");          
            client.println("<p>LoRa RSSI: " + String(rssi) + "</p>");
            client.println("</body></html>");
            
            // LA peticion HTTP finaliza con otra linea vacia
            client.println();
            // Rompe el bucle loop
            break;
          } else { // en caso de tener una nueva linea se limpia la actual
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;   
        }
      }
    }
    // se limpia la variable header
    header = "";
    // se cierra la conexion
    client.stop();
    Serial.println("Cliente desconectado.");
    Serial.println("");
  }

}


//A continuacion la funcion para ller los paquetes desde el LoRa y obtener las lecturas  
void getLoRaData() {
  Serial.print("paquete de LoRa recibido: ");
  // Leer paquete
  while (LoRa.available()) {
    String LoRaData = LoRa.readString();
    // formato LoRaData: temperatura1/humedad1&suelohum
    // String ejemplo: 1#21/34&45
    Serial.println(LoRaData);
    int pos1 = LoRaData.indexOf('#');
    senderID = LoRaData.substring(1, pos1);//se le pone posicion 1 porque en la posicion 0 hay un espacio en blanco
    Serial.println(senderID);
    if (senderID == "1"){
      Serial.println("SenderID es igual a 1");
              int pos1 = LoRaData.indexOf('#');
              int pos2 = LoRaData.indexOf('/');
              int pos3 = LoRaData.indexOf('&');
          
              ID=LoRaData.substring(0, pos1);
              temperatura1 = LoRaData.substring(pos1 +1, pos2);
              humedad1 = LoRaData.substring(pos2+1, pos3);
              suelohum1 = LoRaData.substring(pos3+1, LoRaData.length());
              temperatura2= "*";
              suelohum2= "*";
              humedad2= "*";
              temperatura3= "*";
              suelohum3= "*";
              humedad3= "*";
              
              //lineas de testeo para el puerto serial
              Serial.println("(IFsensor 1) temperatura1:"+temperatura1+" humedad1:"+humedad1+" suelohum1:"+suelohum1+" ");
              Serial.println("(sensor 2) temperatura2:"+temperatura2+" humedad2:"+humedad2+" suelohum2:"+suelohum2+" ");
              Serial.println("(sensor 3) temperatura3:"+temperatura3+" humedad3:"+humedad3+" suelohum3:"+suelohum3+" ");
    }
    if (senderID == "2"){
      Serial.println("senderID es igual a 2");
              int pos1 = LoRaData.indexOf('#');
              int pos2 = LoRaData.indexOf('/');
              int pos3 = LoRaData.indexOf('&');
              
              ID=LoRaData.substring(0, pos1);
              temperatura2 = LoRaData.substring(pos1 +1, pos2);
              humedad2 = LoRaData.substring(pos2+1, pos3);
              suelohum2 = LoRaData.substring(pos3+1, LoRaData.length());
              temperatura1= "*";
              suelohum1= "*";
              humedad1= "*";
              temperatura3= "*";
              suelohum3= "*";
              humedad3= "*";

              //lineas de testeo para el puerto serial
              Serial.println("(sensor 1) temperatura1:"+temperatura1+" humedad1:"+humedad1+" suelohum1:"+suelohum1+" ");
              Serial.println("(IFsensor 2) temperatura2:"+temperatura2+" humedad2:"+humedad2+" suelohum2:"+suelohum2+" ");
              Serial.println("(sensor 3) temperatura3:"+temperatura3+" humedad3:"+humedad3+" suelohum3:"+suelohum3+" ");
    }
    if (senderID == "3"){
      Serial.println("senderID es igual a 3");
              int pos1 = LoRaData.indexOf('#');
              int pos2 = LoRaData.indexOf('/');
              int pos3 = LoRaData.indexOf('&');
              
              ID=LoRaData.substring(0, pos1);
              temperatura3 = LoRaData.substring(pos1 +1, pos2);
              humedad3 = LoRaData.substring(pos2+1, pos3);
              suelohum3 = LoRaData.substring(pos3+1, LoRaData.length());
              temperatura1= "*";
              suelohum1= "*";
              humedad1= "*";
              temperatura2= "*";
              suelohum2= "*";
              humedad2= "*";

              //lineas de testeo para el puerto serial
              Serial.println("(sensor 1) temperatura1:"+temperatura1+" humedad1:"+humedad1+" suelohum1:"+suelohum1+" ");
              Serial.println("(sensor 2) temperatura2:"+temperatura2+" humedad2:"+humedad2+" suelohum2:"+suelohum2+" ");
              Serial.println("(IFsensor 3) temperatura3:"+temperatura3+" humedad3:"+humedad3+" suelohum3:"+suelohum3+" ");
      
    }
 
     //lineas de testeo para el puerto serial              
    Serial.println("(sensor 1) temperatura1:"+temperatura1+" humedad1:"+humedad1+" suelohum1:"+suelohum1+" ");
    Serial.println("(sensor 2) temperatura2:"+temperatura2+" humedad2:"+humedad2+" suelohum2:"+suelohum2+" ");
    Serial.println("(sensor 3) temperatura3:"+temperatura3+" humedad3:"+humedad3+" suelohum3:"+suelohum3+" ");
     
     temp1 =temperatura1.toInt();
     hum1 =humedad1.toInt();
     sue1 =suelohum1.toInt();
     temp2 =temperatura2.toInt();
     hum2 =humedad2.toInt();
     sue2 =suelohum2.toInt();
     temp3 =temperatura3.toInt();
     hum3 =humedad3.toInt();
     sue3 =suelohum3.toInt();

     //lineas de testeo para el puerto serial
     Serial.println("CONVERSION DE STRING A INT");
     
    //linea siguiente solo para testeo
    Serial.println("---------XXXXXXXXXtemp1XXXXXXXX-----------");
    Serial.println(temp1);
    Serial.println("---------XXXXXXXXXhum1XXXXXXXX-----------");
    Serial.println(hum1);
    Serial.println("---------XXXXXXXXXsue1XXXXXXXX-----------");
    Serial.println(sue1);
    Serial.println("---------XXXXXXXXsensor2 temp2XXXXXXXXX-----------");
    Serial.println(temp2);
    Serial.println("---------XXXXXXXXhum2XXXXXXXXX-----------");
    Serial.println(hum2);
    Serial.println("---------XXXXXXXXsue2XXXXXXXXX-----------");
    Serial.println(sue2);
    Serial.println("---------XXXXXXXXsensor3 temp3XXXXXXXXX-----------");
    Serial.println(temp3);
    Serial.println("---------XXXXXXXXhum3XXXXXXXXX-----------");
    Serial.println(hum3);
    Serial.println("---------XXXXXXXXXsue3XXXXXXXX-----------");
    Serial.println(sue3);
    Serial.println("------------------------");


  
  
  
  // Obtener el RSSI 
  rssi = LoRa.packetRssi();
  Serial.print(" con RSSI ");    
  Serial.println(rssi);

 
 }
}

// Funcion para obtener la fecha y hra del cliente NTP
void getTimeStamp() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // LA variable formattedDate tiene el siguiente formato:
  // 2018-05-28T16:00:13Z
  // Se necesita extraer la fecha y la hora
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);

  // Extraer fecha
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.println("Aqui se viene la fecha");
  Serial.println(dayStamp);
  // Extraer tiempo
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.println("Aqui se viene el tiempo");
  Serial.println(timeStamp);
}  
