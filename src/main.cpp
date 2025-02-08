#include <Arduino.h>

const int numSamples = 500; // Número de muestras
int ecgValues[numSamples]; // Array para almacenar los valores del ECG

void generateECGSignal();

void setup() {
  Serial.begin(9600); // Inicializa comunicación serial
}

void loop() {
  generateECGSignal(); // Genera señal al estilo ECG
  
  for (int i = 0; i < numSamples; i++) {
    Serial.println(ecgValues[i]); // Imprime cada valor en el monitor serial
    
    delay(5); // Espera antes de imprimir el siguiente valor
  }
  
  delay(1000); // Espera antes de generar otra señal al estilo ECG
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

