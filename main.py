import paho.mqtt.client as mqtt
from smbus2 import SMBus
from mlx90614 import MLX90614
import time
import math
import Adafruit_ADS1x15
from datetime import datetime

# Create an ADS1115 ADC (16-bit) instance.
adc = Adafruit_ADS1x15.ADS1115()
GAIN = 2

bus = SMBus(1)
sensor = MLX90614(bus, address=0x5A)

print("Conectando...")

MiMQTT = mqtt.Client("Raspberry")
MiMQTT.username_pw_set("monitor-motor", "monitor-motor")
MiMQTT.connect("monitor-motor.cloud.shiftr.io", 1883)

print("Conectado")


while True:
    
    factor = 30 #30a/1v
    multiplier = 0.0625;
    prom = 0.0
    contador = 0

    value = adc.read_adc_difference(0, gain=GAIN)
    corriente = value

    inicio_corriente = time.time()
    #print(inicio)
    
    while time.time() - inicio_corriente < 1.00:
        #print(time.time()-inicio)
        vdiff = corriente * multiplier
        current = vdiff * factor
        current /= 1000.00
    
        prom +=  math.pow(current,2)
        contador = contador + 1

    Irms = math.sqrt(prom/contador)
    
    print('Corriente del Motor: ', round(Irms,2), "Arms ",datetime.now())
    envio_corriente = str(round(Irms,2)) + " " + str(datetime.now())
    MiMQTT.publish("Corriente", envio_corriente)
    
    #print (sensor.get_ambient())
    temperatura = sensor.get_object_1()
    print ('Temperatura del Motor: ',round(temperatura,2), "°C ", datetime.now())
    envio_temperatura = str(round(temperatura,2)) + " " + str(datetime.now())
    MiMQTT.publish("Temperatura", envio_temperatura)
    
    inicio_vibracion = time.time()
    
    frecuencia=0
    while time.time() - inicio_vibracion < 1.00:
        
        vibracion = adc.read_adc(2, gain=GAIN)
        if vibracion > 3500.00:
            frecuencia=frecuencia+1
         
    print('Vibración del Motor: ', frecuencia, " Hz ", datetime.now())
    envio_vibracion = str(round(frecuencia,2)) + " " + str(datetime.now())
    MiMQTT.publish("Vibracion", envio_vibracion)
    
    inicio_ruido = time.time()
    señalmax = 0
    señalmin = 32767*GAIN
    
    while time.time() - inicio_ruido < 0.050:
        
        ruido = adc.read_adc(3, gain=GAIN)
        
        if ruido < 32767:
            
            if ruido > señalmax:
                señalmax = ruido
            elif ruido < señalmin:
                señalmin = ruido
        
    picoapico = abs(señalmax - señalmin)
    '''inputmin= 10
    inputmax= 32767
    outputmin= 0
    outputmax= 100'''
        
    #ruidodb = ((outputmax-outputmin)/(inputmax-inputmin))*(picoapico-inputmin)+outputmin
    ruidodb =  (math.log(picoapico))*29.75 - 134.48   
    print('Ruido del Motor: ', round(ruidodb,2), " dB ", datetime.now())
    envio_ruido = str(round(ruidodb,2)) + " " + str(datetime.now())
    MiMQTT.publish("Ruido", envio_ruido)

bus.close()