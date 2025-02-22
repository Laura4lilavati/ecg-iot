#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <UbidotsESPMQTT.h>

#ifndef STASSID
#define STASSID ""
#define STACONTRASENA ""
#define UBIDOTS_TOKEN ""
#define NOMBRE_DISPOSITIVO "dispositivoecg"
#define VARIABLE_LABEL "senal_ecg"
#define SENSOR A0
#endif

const char* ssid = STASSID;
const char* contrasena = STACONTRASENA;

Ubidots client(UBIDOTS_TOKEN);

const int TAMANO_VENTANA = 10; 
float lecturasEcg[TAMANO_VENTANA];
int indiceLectura = 0;

float obtenerPromedio(float nuevaLectura) {
  float suma = -lecturasEcg[indiceLectura];
  lecturasEcg[indiceLectura] = nuevaLectura;
  suma += nuevaLectura;
  indiceLectura = (indiceLectura + 1) % TAMANO_VENTANA;
  float sumaActual = 0;
  for (int i = 0; i < TAMANO_VENTANA; i++) {
    sumaActual += lecturasEcg[i];
  }
  return sumaActual / TAMANO_VENTANA;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("El mensaje arribo [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {

  Serial.begin(115200);
  // client.setDebug(true); // To activate debug messages
  client.wifiConnection(STASSID, STACONTRASENA);
  client.begin(callback);
  pinMode(SENSOR, INPUT);
}

void loop() {

  if(!client.connected()){
    client.reconnect();
    }

  float ECGsinFiltro = analogRead(SENSOR); 
  float ECGfiltrado = obtenerPromedio(ECGsinFiltro);
  client.add(VARIABLE_LABEL, ECGfiltrado);
  client.ubidotsPublish(NOMBRE_DISPOSITIVO);
  client.loop();
  delay(10);
}
