  /*
    Arduino and ADXL345 Accelerometer Tutorial
     by Dejan, https://howtomechatronics.com
*/
// Gnd      -  GND
// 3.3v     -  VCC
// 3.3v     -  CS
// Analog 4 -  SDA
// Analog 5 -  SLC
#include <Wire.h>  // Wire library - used for I2C communication
int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float threshold = 6;//valor maximo para contar un paso
float xval[100] = {0};//vectores para encontrar el promedio inicial y tener referencia de los ejes para el contador de pasos
float yval[100] = {0};
float zval[100] = {0};
float xavg, yavg, zavg;//valores promedio de los ejes
int steps, flag = 0;//cuentas y bandera para que cuente un solo paso a la vez
float xaccl,yaccl,zaccl=0;//valores leidos en cada loop de los 3 ejes
float totvect,totave=0;//total del vector de los 3 ejes y promedio
void setup() {
  Serial.begin(9600); // Initiate serial communication for printing the results on the Serial monitor
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  delay(10);
  calibrate();//calibrar el acelerometro y calcular promedios
}
void loop() {
  // === Read acceleromter data === //
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Leer 6 registros en total, cada eje se almacena en 2 registros
  xaccl = ( Wire.read()| Wire.read() << 8)/256;
  delay(1);
  yaccl = ( Wire.read()| Wire.read() << 8)/256;
  delay(1);
  zaccl = ( Wire.read()| Wire.read() << 8)/256;
  delay(1);
  totvect = sqrt(((xaccl - xavg) * (xaccl - xavg)) + ((yaccl - yavg) * (yaccl - yavg)) + ((zaccl - zavg) * (zaccl - zavg)));//calculo de la magnitud total del vector
  totave = (totvect + totvect) / 2 ;
   if (totave > threshold && flag == 0)//cuenta un paso
    {
      steps = steps + 1;
      flag = 1;
    }
    else if (totave > threshold && flag == 1)//sigue el mismo paso
    {
      // Don't Count
    }
    if (totave < threshold   && flag == 1)//termino el paso baja la bandera
    {
      flag = 0;
    }
    Serial.println(steps);//imprimir la cantidad de pasos
}

void calibrate()
{
  float sum = 0;
  float sum1 = 0;
  float sum2 = 0;
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  for (int i = 0; i < 100; i++) {
    Wire.requestFrom(ADXL345, 6, true);
    xval[i] = ( Wire.read()| Wire.read() << 8)/256;
    sum = xval[i] + sum;
  }
  delay(100);
  xavg = sum / 100.0;
  Serial.println(xavg);
  for (int j = 0; j < 100; j++)
  {
    Wire.requestFrom(ADXL345, 6, true);
    Wire.read();
    Wire.read();
    yval[j] = ( Wire.read()| Wire.read() << 8)/256;
    sum1 = yval[j] + sum1;
  }
  yavg = sum1 / 100.0;
  delay(100);
  for (int q = 0; q < 100; q++)
  {
    Wire.requestFrom(ADXL345, 6, true);
    Wire.read();
    Wire.read();
    Wire.read();
    Wire.read();
    zval[q] = ( Wire.read()| Wire.read() << 8)/256;
    sum2 = zval[q] + sum2;
  }
  zavg = sum2 / 100.0;
  delay(100);
}
