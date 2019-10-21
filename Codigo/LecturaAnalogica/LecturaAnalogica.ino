/*Programa:
 * Se encarga de realizar la lectura de tension de la bateria cada cierto intervalo de tiempo
 * Limita la carga entre el rango de 12v a 13,5
 */
#include <avr/io.h>
#include <avr/interrupt.h>


//variables de lectura
unsigned int val_crudo;
float res=204.6;
volatile float tension;
bool ADCstatus=1;

void ADCsetup();  //Configuracion del modulo ADC
void lectura();   //Llamado a la lectura del ADC

//variables de control
float tensionMin=1.1;
float tensionMax=1.39;  //Tension maxima+2

void setup() {
  DDRB|=(1<<PB5);
  PORTB&=~(1<<PB5);
  ADCsetup();
  Serial.begin(9600);
}

void loop() {
  if(ADCstatus)lectura();   //Si ADCstatus==1 se realiza una lectura analogica de la bateria
  if(tension<tensionMin){            //Si la tension <3 entonces la bateria esta con poca carga
    Serial.println("Carga");//3=12v
    PORTB|=(1<<PB5);        //Comienza la carga
  }
  if(tension>tensionMax){         //Si la tension esta por sobre 3.38 entonces la bateria se encuentra con una carga optima
    Serial.println("Fin de carga");//3.38=13.5v
    PORTB&=~(1<<PB5);       //Se detiene la carga
  }
  delay(500);
  ADCstatus=1;
}

void ADCsetup(){
  ADMUX=0b01000011;     // Vref= Vcc;  Justificación: Derecha;  Entrada: ADC3

  ADCSRA=0b00000111;    // ADC: Apagado; Inicia Conversión: NO; 
                        // Disparo Automatizado: NO; Interrupción: NO; 
                        // Prescaler: 128 (125Khz); 
  
  ADCSRB=0b00000000;    // Sin disparo automatizado; por esta razón no es necesario ponerlo
  
  DIDR0=0b00001000;     // Desabilito buffer digital interno en ADC3.
//  DIDR2=0b00000000;
}

void lectura(){
  ADCSRA |=((1<<ADEN)|(1<<ADSC));  //Activa ADC; Inicia la conversión AD
  while(ADCSRA &(1<<ADSC));        // Espera que termine una conversión
                                  // cuando termina ADSC pasa a valer "1"
                                  
  ADCSRA &=~(1<<ADEN);         // desactivo el ADC
  
  val_crudo= ADCL;             // Se debe leer ADCL primero
  val_crudo|=(ADCH<<8);        // luego leo ADCH (SINÓ TRAE PROBLEMAS)
  tension=val_crudo/res;
  Serial.println(tension);
  ADCstatus=0;
  delay(500);
}
  
