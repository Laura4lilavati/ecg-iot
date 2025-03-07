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
#define BUFFER_SIZE 10
#endif

const char* ssid = STASSID;
const char* contrasena = STACONTRASENA;
float ecgBuffer[BUFFER_SIZE];
int indiceBuffer = 0;

Ubidots client(UBIDOTS_TOKEN);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("El mensaje arribo [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void envioBufferAUbidots() {
  if (indiceBuffer == 0) return;
  for (int i = 0; i < indiceBuffer; i++) {
    client.add(VARIABLE_LABEL, ecgBuffer[i]);
  }
  if (client.ubidotsPublish(NOMBRE_DISPOSITIVO)) {
    Serial.println("Datos enviados correctamente:");
    for (int i = 0; i < indiceBuffer; i++) {
      Serial.println(ecgBuffer[i]);
    }
    indiceBuffer = 0;
  } else {
    Serial.println("Error al enviar los datos");
  }
}

void setup() {
  Serial.begin(115200);
  // client.setDebug(true);
  client.wifiConnection(STASSID, STACONTRASENA);
  if (client.getMQTTClient()->setBufferSize(1024)) {
    Serial.println("Tamaño del Buffer cambiado exitosamente.");
} else {
    Serial.println("No se cambio el tamaño del Buffer.");
}
  client.begin(callback);
  pinMode(SENSOR, INPUT);
}

void loop() {
    if(!client.connected()){
    client.reconnect();
    }
  
  client.loop();
  float ecgValue = analogRead(SENSOR);
  ecgBuffer[indiceBuffer++] = ecgValue;
  if (indiceBuffer >= BUFFER_SIZE) {
    envioBufferAUbidots();
  }
  delay(2);
}
