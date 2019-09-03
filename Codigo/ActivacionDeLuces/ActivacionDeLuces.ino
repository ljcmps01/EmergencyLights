/*Programa:
 * Se lee el cambio de estado de un pin de interrupcion externa
 * Para determinar si hay suministro electrico
 * El pin se encontrará energizado siempre y cuando haya suministro 
 * electrico
 * Caso contrario el pin se encontrará conectado a referenciado a vcc
 * y se procederá a evaluar si se deben prender las luces o no
 * en base al dia y horario
 * 
 * FALTA AGREGAR:Leer si PD2 es 1 o 0 para prender o apagar las luces
 */

int dias=0; //lunes,martes,miercoles...domingo
            //0,1,2...6
int seg,minutos,hora;

bool Horario=0;
void mostrarHorario();

void setup() {
  Serial.begin(9600);
  DDRB|=(1<<PB6);
  DDRD&=~(1<<PD2);
  cli();
/////TIMER/////
    TCCR1A=0;
    TCCR1B=0;
    OCR1A=15625;                //Ajustado a 1 seg
    TCCR1B=(1<<CS10)|(1<<CS12); //Precaler a 1024
    TCCR1B|=(1<<WGM12);

    TIMSK1|=(1<<OCIE1A);
    
/////INTERRUPCION EXTERNA/////
    EIMSK|=(1<<INT2);   //Habilito INT2 (PD2)
    EICRA|=(1<<ISC21);  //Disparo por cambio de estado
    EICRA&=~(1<<ISC20);
  sei();
}

void loop() {
  if(Horario)
    mostrarHorario();
}

ISR(TIMER1_COMPA_vect){
  if(seg<59){
    seg++;
  }
    else{
      seg=0;
      if(minutos<59){
        minutos++;
        }
        else{
          minutos=0;
          if(hora<23){
            hora++;
          }
          else{
            hora=0;
            if(dias<5)dias++;
            else dias=0;
          }
      }
    }
  Horario=1;
}

ISR(INT0_vect){
  byte aux=PIND&=(1<<2);
  if (aux)PORTB&=~(1<<PB6); //Si esta energizado el pin, se apagan las luces
  else{
    if(dias<5){ //Si es dia de semana
      if((hora>7&&hora<12)||(hora>17&&hora<23)){//Y se encuentra dentro del rango horario de mañana o vespertino
        PORTB|=(1<<PB6);    //Se prenden las luces
      }
    }
  }
}

void mostrarHorario(){
  Serial.println("Hora\tMinutos\tsegundos");
  Serial.print(hora);
  Serial.print("\t");
  Serial.print(minutos);
  Serial.print("\t");
  Serial.print(seg);
  Serial.print("\n");
  Horario=0;
}
