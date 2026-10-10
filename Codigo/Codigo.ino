#include <Arduino.h>
#include <cmath>

// Definición de pines
const int DOUT_PIN = 21;
const int SCK_PIN = 22;

// Constantes configurables iniciales (Etapa C)
const float P_atm = 101.325; // Presión atmosférica local en kPa (actualizar con dato del SENAMHI)
const float V_0 = 1.00;      // Volumen inicial de la jeringa en mL
const float V_m = 0.76;      // Volumen muerto inicial provisional en mL (menor entre Va1 y Va2)
const float P_max = 35.0;    // Presión máxima de seguridad en kPa

// Número constante de lecturas a promediar (N >= 20)
const int N_READINGS = 30;

void setup() {
  Serial.begin(115200);
  pinMode(DOUT_PIN, INPUT);
  pinMode(SCK_PIN, OUTPUT);
  digitalWrite(SCK_PIN, LOW);

  delay(1000);
  Serial.println("--- Sistema Iniciado: Laboratorio 2 - TEB II ---");
  
  // Cálculo de la marca de parada V_j,min (Ecuación 2)
  float r = P_max / P_atm;
  float V_j_min = (V_0 - r * V_m) / (1.0 + r);
  
  Serial.print("Volumen muerto provisional (Vm): "); Serial.print(V_m); Serial.println(" mL");
  Serial.print("Presion atmosferica (Patm): "); Serial.print(P_atm); Serial.println(" kPa");
  Serial.print("Marca de parada calculada (V_j,min): "); Serial.print(V_j_min); Serial.println(" mL");
  Serial.println("Formato de entrada por serial: ciclo,sentido,V_j (Ejemplo: 1,C,0.88)");
  Serial.println("Presione ENTER sin texto para medir cero atmosférico.");
}

// Función para leer el HX710B (devuelve entero de 24 bits con signo)
int32_t readHX710B() {
  while (digitalRead(DOUT_PIN) == HIGH); // Espera a que el sensor esté listo

  int32_t count = 0;
  for (int i = 0; i < 24; i++) {
    digitalWrite(SCK_PIN, HIGH);
    delayMicroseconds(1);
    count = (count << 1);
    digitalWrite(SCK_PIN, LOW);
    if (digitalRead(DOUT_PIN)) {
      count++;
    }
    delayMicroseconds(1);
  }

  // Pulso adicional de reloj para cerrar ciclo de conversión
  digitalWrite(SCK_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(SCK_PIN, LOW);
  delayMicroseconds(1);

  // Extensión de signo para 24 bits en complemento a 2
  if (count & 0x800000) {
    count |= 0xFF000000;
  }

  return count;
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() == 0) {
      // Medición de cero atmosférico si solo se presiona Enter
      double sum = 0;
      double sumSq = 0;
      for (int i = 0; i < N_READINGS; i++) {
        int32_t val = readHX710B();
        sum += val;
        sumSq += (double)val * val;
        delay(30);
      }
      double mean = sum / N_READINGS;
      double variance = (sumSq / N_READINGS) - (mean * mean);
      double stdDev = (variance > 0) ? sqrt(variance) : 0;

      Serial.print("CERO ATMOSFERICO -> Media: ");
      Serial.print(mean, 2);
      Serial.print(" | Desviacion (Ruido): ");
      Serial.println(stdDev, 2);
      return;
    }
    
    // Parseo de datos entrantes: ciclo,sentido,V_j
    int firstComma = input.indexOf(',');
    int secondComma = input.indexOf(',', firstComma + 1);

    if (firstComma > 0 && secondComma > firstComma) {
      int ciclo = input.substring(0, firstComma).toInt();
      String sentido = input.substring(firstComma + 1, secondComma);
      float V_j = input.substring(secondComma + 1).toFloat();

      // Validaciones de volumen según la guía
      if (V_j <= 0 || V_j > V_0) {
        Serial.println("ADVERTENCIA: Volumen V_j fuera de rango (0 < V_j <= 1.00 mL).");
        return;
      }

      // Cálculo de la presión de referencia con la Ley de Boyle (Ecuación 1)
      float P_ref = P_atm * (V_0 - V_j) / (V_j + V_m);

      if (P_ref > 40.0) {
        Serial.println("ADVERTENCIA: La presión de referencia supera los 40 kPa del sensor.");
        return;
      }

      // Adquisición de N lecturas consecutivas
      double sum = 0;
      double sumSq = 0;

      for (int i = 0; i < N_READINGS; i++) {
        int32_t val = readHX710B();
        sum += val;
        sumSq += (double)val * val;
        delay(30);
      }

      double mean = sum / N_READINGS;
      double variance = (sumSq / N_READINGS) - (mean * mean);
      double stdDev = (variance > 0) ? sqrt(variance) : 0;

      // Salida en formato CSV: ciclo,sentido,V_j,P_ref,media,desviacion,N
      Serial.print(ciclo); Serial.print(",");
      Serial.print(sentido); Serial.print(",");
      Serial.print(V_j, 2); Serial.print(",");
      Serial.print(P_ref, 3); Serial.print(",");
      Serial.print(mean, 2); Serial.print(",");
      Serial.print(stdDev, 2); Serial.print(",");
      Serial.println(N_READINGS);

    } else {
      Serial.println("ADVERTENCIA: Formato incorrecto. Use: ciclo,sentido,V_j (ej: 1,C,0.88)");
    }
  }
}
