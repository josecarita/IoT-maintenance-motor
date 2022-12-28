
#include <Wire.h>              //libreria de comunicacion serial
#include <Adafruit_ADS1015.h>  //libreria de modulo conversor analógico a digital
#include <Adafruit_MLX90614.h> //libreria de sensor de temperatura

Adafruit_MLX90614 mlx = Adafruit_MLX90614(); //se declara funcion como mlx
Adafruit_ADS1115 ads(0x48);                  //se establece la dirección del modulo para la comunicacion

//variables de corriente
const float FACTOR = 50; //30A/1V
const float multiplier = 0.0625F;
float volt_diferencial;
float corriente;
float sum = 0;
int counter = 0;

//variables de temperatura
double temperatura;

//variables de vibracion
int16_t frecuencia = 0;

//variables de ruido
int16_t senalmax = -32767;
int16_t senalmin = 32767;
int16_t picoapico;

void setup()
{
  // Configuracion de timer1 (interrupcion por timer para lectura y envio de datos)
  TCCR1A = 0;                        // El registro de control A queda todo en 0, pines OC1A y OC1B deshabilitados
  TCCR1B = 0;                        //limpia el registrador
  TCCR1B |= (1<<CS10)|(1 << CS12);   // configura prescaler para 1024: CS12 = 1 e CS10 = 1
  TCNT1 = 0xC2F8;                    // inicia timer para desbordamiento 1 segundo
                                     // 65536-(16MHz/1024/1Hz - 1) = 49912 = 0xC2F8
  
  TIMSK1 |= (1 << TOIE1);            // habilita la interrupcion del TIMER1

  Serial.begin(9600);                //inicia comunicacion serial a 9600 baudios
  ads.setGain(GAIN_TWO);             //establece una ganancia de las lecturas de ADC en 2
  ads.begin();                       //inicia el modulo ADC
  mlx.begin();                       //inicia el sensor de temperatura
}
void loop()
{
  
  //corriente
  volt_diferencial = ads.readADC_Differential_0_1() * multiplier; //fórmula para obtener la corriente rms
  corriente = volt_diferencial * FACTOR;
  corriente /= 1000.0;

  sum += sq(corriente);
  counter = counter + 1;

  //temperatura
  temperatura = mlx.readObjectTempC(); //obtiene el valor de la temperatura
  
  //vibracion
  int16_t vibracion = ads.readADC_SingleEnded(2); //formula para obtener la frecuencia de vibracion
  if (vibracion >900)
  {
    frecuencia = frecuencia + 1;
  }
  
  //ruido
  int16_t ruido = ads.readADC_SingleEnded(3); //formula para obtener el ruido

  if (ruido < 32767)
  {
    if (ruido > senalmax)
      {
        senalmax=ruido;
      }
    if (ruido <senalmin)
      {
        senalmin=ruido;
      }
  }
  picoapico = abs(senalmax-senalmin);
  picoapico = picoapico*0.1834 + 45.714;
  //Serial.println(vibracion);
  //Serial.println(ruido);
  
  
}
ISR(TIMER1_OVF_vect)                              //interrupcion del TIMER1 
{
  TCNT1 = 0xC2F7;                                 // Renicia TIMER

  //envío de datos a raspberry y reset de variables a estado inicial
  //corriente
  corriente = sqrt(sum/counter);
  Serial.print(corriente);
  Serial.print(" - ");
  corriente = 0;
  sum = 0;
  counter = 0;

  //temperatura
  Serial.print(temperatura);
  Serial.print(" - ");
  
  //vibracion
  Serial.print(frecuencia);
  Serial.print(" - ");
  frecuencia = 0;
  //ruido
  Serial.println(picoapico);
  senalmax = -32767;
  senalmin = 32767;
  
}
