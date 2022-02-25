/*********************************************************
 * Projeto.: Relogio e timer usando o DS1307             *
 *-------------------------------------------------------*
 * SUMARIO DO PROJETO.:                                  *
 * Permite ligar e desligar uma carga AC atraves         *
 * do relogio do DS1307 que mantem as informações        *
 * atraves da bateria e utilizando rele 5V               *
 * ------------------------------------------------------*
 * CLIENTE.:   Prototipo                                 *
 * AUTOR  .:   Renato Correa                             *
 * DATA   .:   20220108                                  *
 * Mod.PIC.:   ARDUINO UNO                               *
 * Arquivo.:   RTC_DS1307_TIMER_V01_ARDUINO_UNO.ino      *
 * Versoes.:                                             *
 * VER  |DESCRICAO                            | DATA     *
 *  1.00|INICIAL                              | 20220115 *
 *                                                       *
 *                                                       *
 * Portas utilizadas (SAIDAS).:                          *
 * Pino 8  = Rele                                        *
 *                                                       *
 * Portas utilizadas (ENTRADAS).:                        *
 * NAO USA                                               *
 *                                                       * 
 * OBS.:                                                 *
 *                                                       *
 *                                                       *
 *                                                       *
 *                                                       *
 *********************************************************/


#include "Wire.h"
#define DS1307_ADDRESS 0x68
#define relay 8

#define Interval 10000
 

byte zero = 0x00;

// Editar as horas liga e desliga abaixo para programar o liga e desliga
int intLiga = 1700;     //Tempo liga o Rele 
int intDesliga = 2101;  //Tempo desliga o rele

int intFim = 0;


void timerAtive(int hora, int minuto)
{
  String strHora = "";
  String strMinuto = "";
  String fimStr = "";
  
  if(minuto < 10)
  {
      strMinuto = String(minuto);
      strMinuto = "0" + strMinuto;
  }
  else
  {
    strMinuto = String(minuto);
  }
  
  if(hora < 10)
  {
      strHora = String(strHora);
      strHora = "0" + strHora;
  }
  else
  {
    strHora = String(hora);
  }

  fimStr = strHora + strMinuto;
  
  //Valor lido do RTC no formato hora + minuto
  Serial.print("String: ");
  Serial.println(fimStr);

  // Convertido para integer
  intFim = fimStr.toInt();

  //Faz a comparacao para verficar se esta no tempo
  // VERFICA SE ATIVA A SAIDA OU NAO
  if((intFim >= intLiga) && (intFim <= intDesliga)) 
  {
    Serial.println("Relay -> ON");
    digitalWrite(relay, LOW);
  }
  else
  {
    Serial.println("Relay -> OFF");
    digitalWrite(relay, HIGH);
  }

  
}



void setup(){
  Wire.begin();
  Serial.begin(9600);
  delay(1000);
  pinMode(relay, OUTPUT);
  //setDateTime();            //Necessário configurar na função "setDateTime()"
  digitalWrite(relay,HIGH);
  Serial.println("** START - by RLC **");
  
  Serial.println("[Parametros]");
  Serial.print("Liga(hhmm): ");
  Serial.println(intLiga);

  Serial.print("Desliga(hhmm): ");
  Serial.println(intDesliga);

  
  Serial.println("** Ini Relay **");
  delay(1000);
  iniRelay();
}

void loop(){
  printDate();
  delay(Interval);
}

void iniRelay()
{
  delay(100);
  int cont;
  for(cont=0;cont < 3;cont++)
  {
    digitalWrite(relay,LOW);
    delay(2000);
    digitalWrite(relay,HIGH);
    delay(2000);
  }
}

void setDateTime(){

  byte segundo =      0;  //0-59
  byte minuto =       33;  //0-59
  byte hora =         11;  //0-23
  byte diasemana =    7;  //1-7
  byte dia =          8;  //1-31
  byte mes =          1; //1-12
  byte ano  =         22; //0-99

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero); 

  Wire.write(decToBcd(segundo));
  Wire.write(decToBcd(minuto));
  Wire.write(decToBcd(hora));
  Wire.write(decToBcd(diasemana));
  Wire.write(decToBcd(dia));
  Wire.write(decToBcd(mes));
  Wire.write(decToBcd(ano));

  Wire.write(zero); 

  Wire.endTransmission();

}

byte decToBcd(byte val){
// Conversão de decimal para binário
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)  {
// Conversão de binário para decimal
  return ( (val/16*10) + (val%16) );
}

void printDate(){

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);

  int segundo = bcdToDec(Wire.read());
  int minuto = bcdToDec(Wire.read());
  int hora = bcdToDec(Wire.read() & 0b111111);    //Formato 24 horas
  int diasemana = bcdToDec(Wire.read());             //0-6 -> Domingo - Sábado
  int dia = bcdToDec(Wire.read());
  int mes = bcdToDec(Wire.read());
  int ano = bcdToDec(Wire.read());
  
  // Verificar se o tempo esta no tempo para ativar/desativar o timer
  timerAtive(hora,minuto);

  //Exibe a data e hora. Ex.:   3/12/13 19:00:00
  Serial.print(dia);
  Serial.print("/");
  Serial.print(mes);
  Serial.print("/");
  Serial.print(ano);
  Serial.print(" ");
  Serial.print(hora);
  Serial.print(":");
  Serial.print(minuto);
  Serial.print(":");
  Serial.println(segundo);

}
