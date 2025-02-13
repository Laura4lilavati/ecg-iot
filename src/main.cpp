#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <UbidotsESPMQTT.h>

#ifndef STASSID
#define STASSID ""
#define STAPSK ""
#define UBIDOTS_TOKEN ""
#define DEVICE_NAME "dispositivoecg"
#define VARIABLE_LABEL "senal_ecg"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

Ubidots client(UBIDOTS_TOKEN);

const int numSamples = 500;
int ecgValues[numSamples]; // Array para almacenar los valores del ECG

void generateECGSignal();

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {

  Serial.begin(115200);
  // client.setDebug(true); // Pass a true or false bool value to activate debug messages
  client.wifiConnection(STASSID, STAPSK);
  client.begin(callback);
}

void loop() {

if(!client.connected()){
  client.reconnect();
  }

  generateECGSignal();
  for (int i = 0; i < numSamples; i++) {
    Serial.println(ecgValues[i]); 
    client.add(VARIABLE_LABEL, ecgValues[i]);
    delay(5); 
  }
  delay(1000); // Espera antes de generar otra señal al estilo ECG
  client.ubidotsPublish(DEVICE_NAME);
  client.loop();
}

// Función para generar señal al estilo ECG con ruido aleatorio
void generateECGSignal() {
  for (int i = 0; i < numSamples; i++) {
    float t = (float)i / numSamples * PI * 2.5f; // Parámetro temporal
    
    float pWave = sin(t) * exp(-t) + random(-10, 11); 
    float qrsComplex = sin(t + PI/2) * exp(-(t - PI/4)) + random(-20, 21);
    float tWave = sin(t + PI) * exp(-(t - PI)) + random(-15,16);
    
    if (i < numSamples /8) { 
      ecgValues[i] = pWave;
    } else if (i >= numSamples /8 && i <= numSamples*3/8){
      ecgValues[i] = qrsComplex;
    } else if(i >numSamples*3/8 && i<=numSamples*6/8){
      ecgValues[i] = tWave;
      
     }else{
       ecgValues[i]=random(50);
     }
   }
}

