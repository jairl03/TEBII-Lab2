# TEB2-Lab2-Caracterización de un sensor de presión manométrica con la Ley de Boyle

Repositorio del Laboratorio 2 del curso de Taller de Equipos Biomédicos II en el cual se caracterizó el sensor piezoresistivo de presión manométrica MPS20N0040D-D (0 a 40 kPa), leído por el conversor de 24 bits HX710B y un ESP32, usando como referencia las presiones generadas al comprimir aire con una jeringa de 1 mL según la Ley de Boyle.

## Objetivo

- Leer con el ESP32 la salida del HX710B y verificar su lectura a presión atmosférica.
- Medir con agua el volumen interno de la llave de tres vías y de la manguera (volumen muerto).
- Armar el circuito neumático cerrado y comprobar que no tiene fugas.
- Registrar la respuesta del sensor frente a presiones de referencia calculadas con la Ley de Boyle, en compresión y descompresión, con tres ciclos.
- Obtener la ecuación de calibración (cuentas = S·Pref + C0) y evaluar la linealidad, la histéresis, la resolución y la repetibilidad del sensor.

## Cómo reproducir el experimento

1. Cargar el archivo de código .ino en el ESP32 usando el IDE de Arduino (Recuerda colocarlo con board: "ESP32 Dev Module"). El programa lee el HX710B, promedia N ≥ 20 lecturas (media y desviación estándar), recibe por el Monitor Serial el ciclo, el sentido y el volumen Vj (mL), y calcula la presión de referencia Pref = Patm · (V0 − Vj) / (Vj + Vm). Cada punto se entrega en una línea separada por comas: ciclo, sentido, Vj, Pref, media, desviación estándar y N.
2. Armar el circuito siguiendo el esquemático:
   - Etapa A: módulo HX710B con VCC al pin de 3,3 V del ESP32, GND a tierra, y DOUT y SCK a dos pines digitales (GPIO 21 y GPIO 22). Observar SCK y DOUT en el osciloscopio.
   - Etapa B: jeringa A (solo agua) – llave de tres vías – manguera, con el sensor desconectado, para medir el volumen interno (Va1 y Va2). Luego secar la llave y la manguera.
   - Etapa C: jeringa B (seca) – llave – manguera – sensor. Cargar en el programa V0 = 1,00 mL, Patm y Vm (el menor entre Va1 y Va2), y leer la marca de parada Vj,min.
   - Importante: el agua no debe llegar nunca al sensor.
3. Abrir el Monitor Serial (115200 baudios).
4. Con el puerto abierto al aire, promediar al menos 20 lecturas para obtener el cero (media y desviación estándar) y comprobar la hermeticidad del circuito.
5. Aplicar la secuencia de venteo, comprimir en pasos de 0,06 mL desde 1,00 mL hasta la marca de parada, esperando a que la lectura se estabilice en cada paso, e ingresar Vj por el Monitor Serial. Descomprimir en los mismos pasos y repetir el ciclo tres veces.
6. Guardar el registro del Monitor Serial sin editar y registrar los datos de la sesión (Va1, Va2, presión atmosférica, altitud, marca de parada y ceros inicial y final).
7. Procesar los datos: calcular Vmed, la presión atmosférica del laboratorio y Pref con Vm = Vmed; estimar Vs; ajustar cuentas = S·Pref + C0 con compresión y descompresión; y determinar linealidad, error máximo, histéresis y resolución (σ0 / S).

## Autores
- Tito Fernandez, Dante Adrian
- Lázaro Canales, Jair Renato
- Arroyo Ramos, Rosbeth Nayelhy
