#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Wire.h>
#include <DS3231.h>
#include <EEPROM.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(8,9,10,11,12);


int estado = 0; //0=Normal  |  1=In Menu
int modo =0; //0=Normal  |  1=Grande | 2=Alfa
long tempo_modo_1 = 0;

int pagina = 1; // Pagina do menu de configurações

int luz_lcd = 205;  // Iluminação inicial do LCD
int luz_info=180; //Iluminação mínima do LCD durante infos

//Botões de comando
const int botao_enter = A0;
const int botao_esquerda = A1;
const int botao_direita = A2;

//Buzzer
const int pin_buzzer = 5; 

//Clock
DS3231 Clock;
int segundo,minuto,hora,data,mes,ano,semana; // Valores recolhidos
float temperatura;
String segundos,minutos,horas,d_semana, mes_txt; // Para texto LCD
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;
byte year, month, date, DoW, hour, minute, second;
int set_hora = 0;
int set_minuto =0;
int set_dia =1;
int set_mes =1;
int set_ano =15;
String dia_semana_txt ="SEG";
int set_dia_semana = 1;

//ALARME
int alarme_on = 0; // 0=Desligado / 1=Ligado (normal) / 2=Snooze (normal ligado) / 3=A tocar (alarme simples)  / 4=Snooze a tocar 1/ 5=Snooze a tocar 2 / 6=Snooze a tocar 3  / 10=LIGAR !!! (quando passar 1 minuto ou for desligado (não desativado!!) = reset)
int hora_alarme;
int minuto_alarme;


//Bitmaps
//*******************************************
static const unsigned char PROGMEM main_logo[] =
{
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,
0xff,0xff,0xff,0xff,0x9f,0xff,0xff,0xff,0xff,0xff,0xf0,
0xf0,0xcf,0xff,0x33,0x9f,0xff,0xff,0xff,0xff,0xff,0xf0,
0xe0,0xcf,0xff,0x27,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,
0xc7,0xcf,0xff,0x26,0x1c,0x1c,0x3f,0xff,0xff,0xff,0xf0,
0xcf,0xcf,0xff,0xe,0x1c,0xc,0x1f,0xff,0xff,0xff,0xf0,
0xcf,0xcf,0xff,0x1f,0x9c,0xc9,0x9f,0xff,0xff,0xff,0xf0,
0xcf,0xcf,0x83,0xf,0x9c,0xc9,0xdf,0xff,0xff,0xff,0xf0,
0xcf,0xcf,0xff,0xf,0x9c,0xc9,0xdf,0xff,0xff,0xff,0xf0,
0xc7,0xcf,0xff,0x27,0x9c,0xc9,0x9f,0xf8,0x3f,0xff,0xf0,
0xe0,0xc1,0xff,0x22,0x4,0xcc,0x1f,0xf8,0x3f,0xff,0xf0,
0xf0,0xc1,0xff,0x32,0x4,0xcc,0x3f,0x80,0x1f,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x19,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x0,0x9,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xf8,0x0,0x1,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x2,0x3f,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xf8,0x0,0x0,0x3f,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x0,0x0,0x1f,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0x80,0x0,0x0,0x6f,0xf0,
0xff,0xff,0xff,0xff,0xff,0xf8,0x0,0x0,0x0,0xf,0xf0,
0xff,0xff,0xff,0xff,0xff,0xf0,0x0,0x0,0x0,0x7,0xf0,
0xff,0xff,0xff,0xff,0xff,0xfa,0x20,0x0,0x0,0x1,0xf0,
0xff,0xff,0xff,0xff,0xff,0xfc,0x0,0x20,0x0,0x1,0xf0,
0xff,0xff,0xff,0xff,0xff,0xe0,0x0,0x0,0x0,0x2,0x70,
0xff,0xff,0xff,0xff,0xff,0xf0,0x0,0x0,0x0,0x0,0x70,
0xff,0xff,0xff,0xff,0xff,0xfe,0x0,0x0,0x0,0x0,0x70,
0xff,0xff,0xff,0xff,0xff,0xfc,0x0,0x0,0x0,0x1,0xf0,
0xff,0xff,0xff,0xff,0xff,0xf8,0x0,0xc0,0x0,0x3,0xf0,
0xff,0xff,0xff,0xff,0xff,0xf8,0x11,0x10,0x0,0xf,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x1,0x0,0x7,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xe6,0xe1,0x10,0x3,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x0,0xe4,0xf,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe1,0xf7,0x9f,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf3,0xff,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf3,0xff,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf3,0xff,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf3,0xff,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf3,0xff,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf3,0xff,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xff,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0x0,0x0,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0
  };
  
  
  
  
//CLOCK 32x32
static const unsigned char PROGMEM clock_32[] =
{
0xff,0xf0,0xf,0xff,
0xff,0x80,0x1,0xff,
0xfe,0x0,0x0,0x7f,
0xfc,0x0,0x0,0x3f,
0xf8,0x7,0xe0,0x1f,
0xf0,0x3f,0xfc,0xf,
0xe0,0x7f,0xfe,0x7,
0xc0,0xff,0xff,0x3,
0xc1,0xfc,0x3f,0x83,
0x83,0xfc,0x3f,0xc1,
0x87,0xfc,0x3f,0xe1,
0x87,0xfc,0x3f,0xe1,
0x7,0xfc,0x3f,0xe0,
0xf,0xfc,0x3f,0xf0,
0xf,0xfc,0x3f,0xf0,
0xf,0xfc,0x3f,0xf0,
0xf,0xfc,0x1f,0xf0,
0xf,0xfe,0xf,0xf0,
0xf,0xff,0xf,0xf0,
0x7,0xff,0x9f,0xe0,
0x87,0xff,0xff,0xe1,
0x87,0xff,0xff,0xe1,
0x83,0xff,0xff,0xc1,
0xc1,0xff,0xff,0x83,
0xc0,0xff,0xff,0x3,
0xe0,0x7f,0xfe,0x7,
0xf0,0x3f,0xfc,0xf,
0xf8,0x7,0xe0,0x1f,
0xfc,0x0,0x0,0x3f,
0xfe,0x0,0x0,0x7f,
0xff,0x80,0x1,0xff,
0xff,0xf0,0xf,0xff
  };
  
//ALARME 8x8
static const unsigned char PROGMEM alarme_8[] =
{
0xe7,
0xc3,
0xc3,
0xc3,
0x81,
0x0,
0xff,
0xe7
  };  
  
//TEMPERATURA 16x16
static const unsigned char PROGMEM termo_16[] =
{
0xfe,0x7f,
0xfd,0xbf,
0xfd,0x8f,
0xfd,0xbf,
0xfd,0x3f,
0xfd,0xf,
0xfd,0x3f,
0xfd,0x3f,
0xff,0x3f,
0xfa,0x1f,
0xf4,0xf,
0xf4,0xf,
0xf0,0xf,
0xf0,0xf,
0xf8,0x1f,
0xfe,0x7f
  };    


//ALARME 32x32
static const unsigned char PROGMEM alarme_32[] =
{
0xff,0xf8,0x1f,0xff,
0xff,0xe0,0x7,0xff,
0xff,0xc0,0x3,0xff,
0xff,0x80,0x1,0xff,
0xff,0x80,0x1,0xff,
0xff,0x0,0x0,0xff,
0xff,0x0,0x0,0xff,
0xff,0x0,0x0,0xff,
0xff,0x0,0x0,0xff,
0xff,0x0,0x0,0xff,
0xff,0x0,0x0,0xff,
0xfe,0x0,0x0,0x7f,
0xfe,0x0,0x0,0x7f,
0xfe,0x0,0x0,0x7f,
0xfc,0x0,0x0,0x3f,
0xfc,0x0,0x0,0x3f,
0xf8,0x0,0x0,0x1f,
0xf0,0x0,0x0,0xf,
0xe0,0x0,0x0,0x7,
0xc0,0x0,0x0,0x3,
0x80,0x0,0x0,0x1,
0x0,0x0,0x0,0x1,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,
0xff,0xf0,0xf,0xff,
0xff,0xf0,0xf,0xff,
0xff,0xf8,0x1f,0xff,
0xff,0xfc,0x3f,0xff
  };  
  
//EJECT 32x32
static const unsigned char PROGMEM eject_32[] =
{
0xff,0xff,0xff,0xff,
0xff,0xfe,0x7f,0xff,
0xff,0xfc,0x3f,0xff,
0xff,0xf8,0x1f,0xff,
0xff,0xf0,0xf,0xff,
0xff,0xf0,0xf,0xff,
0xff,0xe0,0x7,0xff,
0xff,0xc0,0x3,0xff,
0xff,0x80,0x1,0xff,
0xff,0x0,0x0,0xff,
0xff,0x0,0x0,0xff,
0xfe,0x0,0x0,0x7f,
0xfc,0x0,0x0,0x3f,
0xf8,0x0,0x0,0x1f,
0xf0,0x0,0x0,0xf,
0xf0,0x0,0x0,0xf,
0xe0,0x0,0x0,0x7,
0xc0,0x0,0x0,0x3,
0x80,0x0,0x0,0x1,
0x0,0x0,0x0,0x0,
0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0
  };  
  
  //Temperatura 32x32
static const unsigned char PROGMEM termo_32[] =
{
  0xff,0xf0,0xff,0xff,
0xff,0xe0,0x7f,0xff,
0xff,0xc6,0x3f,0xff,
0xff,0xcf,0x30,0x3f,
0xff,0xcf,0x30,0x3f,
0xff,0xcf,0x3f,0xff,
0xff,0xcf,0x3f,0xff,
0xff,0xcf,0x30,0x3f,
0xff,0xcf,0x30,0x3f,
0xff,0xcf,0x3f,0xff,
0xff,0xcf,0x3f,0xff,
0xff,0xc9,0x30,0x3f,
0xff,0xc9,0x30,0x3f,
0xff,0xc9,0x3f,0xff,
0xff,0xc9,0x3f,0xff,
0xff,0xc9,0x38,0x3f,
0xff,0xc9,0x38,0x3f,
0xff,0x89,0x1f,0xff,
0xff,0x9,0xf,0xff,
0xfe,0x30,0xc7,0xff,
0xfe,0x40,0x27,0xff,
0xfc,0x40,0x23,0xff,
0xfc,0x80,0x13,0xff,
0xfc,0x80,0x13,0xff,
0xfc,0x80,0x13,0xff,
0xfc,0x80,0x13,0xff,
0xfc,0x40,0x23,0xff,
0xfe,0x60,0x67,0xff,
0xfe,0x30,0xc7,0xff,
0xff,0xf,0xf,0xff,
0xff,0x80,0x1f,0xff,
0xff,0xf0,0x7f,0xff
  };
  
//Calendario 32x32
static const unsigned char PROGMEM calendar_32[] =
{
0xfe,0x1f,0xf8,0xff,
0xfc,0xf,0xe0,0x3f,
0xfc,0xcf,0xe7,0x3f,
0xfc,0xcf,0xe7,0x3f,
0xc0,0xc0,0x7,0x7,
0x80,0xc0,0x7,0x3,
0x80,0xc0,0x7,0x3,
0x80,0xc0,0x7,0x3,
0x80,0x0,0x0,0x3,
0x80,0x0,0x0,0x3,
0x80,0x0,0x0,0x3,
0x9f,0x7e,0xfd,0xf3,
0x9f,0x7e,0xfd,0xf3,
0x9f,0x7e,0xfd,0xf3,
0x9f,0x7e,0xfd,0xf3,
0x9f,0x7e,0xfd,0xf3,
0x80,0x0,0x0,0x3,
0x9f,0x7e,0xfd,0xf3,
0x9f,0x7e,0xfd,0xf3,
0x9f,0x7e,0xfd,0xf3,
0x9f,0x7e,0xfd,0xf3,
0x9f,0x7e,0xfd,0xf3,
0x9f,0x7e,0xfd,0xf3,
0x80,0x0,0x0,0x3,
0x9f,0x7e,0xfd,0xf3,
0x9f,0x7e,0xfd,0xf3,
0x9f,0x7e,0xfd,0xf3,
0x9f,0x7e,0xfd,0xf3,
0x9f,0x7e,0xfd,0xf3,
0x9f,0x7e,0xfd,0xf3,
0x80,0x0,0x0,0x3,
0xc0,0x0,0x0,0x7
  };  
  
//Lanterna 32x32
static const unsigned char PROGMEM light_32[] =
{
0xff,0x0,0x3,0xff,
0xff,0x0,0x3,0xff,
0xff,0x3f,0xf3,0xff,
0xff,0x3f,0xf3,0xff,
0xff,0x0,0x3,0xff,
0xff,0x0,0x3,0xff,
0xff,0x3f,0xf3,0xff,
0xff,0x3f,0xf3,0xff,
0xff,0x3f,0xf3,0xff,
0xff,0x1f,0xe3,0xff,
0xff,0x80,0x7,0xff,
0xff,0xc0,0xf,0xff,
0xff,0xcf,0xcf,0xff,
0xff,0xcf,0xcf,0xff,
0xff,0xcf,0xcf,0xff,
0xff,0xcf,0xcf,0xff,
0xff,0xcf,0x3,0xff,
0xff,0xcf,0x3,0xff,
0xff,0xcf,0x33,0xff,
0xff,0xcf,0x33,0xff,
0xff,0xcf,0x33,0xff,
0xff,0xcf,0x33,0xff,
0xff,0xcf,0x3,0xff,
0xff,0xcf,0x3,0xff,
0xff,0xcf,0xcf,0xff,
0xff,0xcf,0xcf,0xff,
0xff,0xcf,0xcf,0xff,
0xff,0xcf,0xcf,0xff,
0xff,0xcf,0xcf,0xff,
0xff,0xcf,0xcf,0xff,
0xff,0xc0,0xf,0xff,
0xff,0xe0,0x1f,0xff
  };   

//Lâmpada 32x32
static const unsigned char PROGMEM bulb_32[] =
{
0xff,0xf8,0x1f,0xff,
0xff,0xc0,0x3,0xff,
0xff,0x87,0xe1,0xff,
0xff,0x1f,0xf8,0xff,
0xfe,0x7f,0x1c,0x7f,
0xfc,0xff,0x6,0x3f,
0xf8,0xff,0xe3,0x3f,
0xf9,0xff,0xf1,0x9f,
0xf9,0xff,0xf9,0x9f,
0xf3,0xff,0xfc,0xcf,
0xf3,0xff,0xfc,0xcf,
0xf3,0xff,0xfc,0xcf,
0xf3,0xff,0xff,0xcf,
0xf3,0xff,0xff,0xcf,
0xf3,0xff,0xff,0xcf,
0xf9,0xff,0xff,0x9f,
0xf9,0xff,0xff,0x9f,
0xf8,0xff,0xff,0x3f,
0xfc,0xff,0xff,0x3f,
0xfe,0x7f,0xfe,0x7f,
0xff,0x1f,0xf8,0xff,
0xff,0x9f,0xf9,0xff,
0xff,0xc0,0x3,0xff,
0xff,0xc0,0x3,0xff,
0xff,0xcf,0xf3,0xff,
0xff,0xcf,0xf3,0xff,
0xff,0xc0,0x3,0xff,
0xff,0xc0,0x3,0xff,
0xff,0xc7,0xe3,0xff,
0xff,0xe1,0x87,0xff,
0xff,0xf0,0x1f,0xff,
0xff,0xfe,0x7f,0xff
  };   
//*******************************************

void setup()   {
  Serial.begin(9600);//**********************************************************************
  Wire.begin();

hora_alarme = EEPROM.read(0);
minuto_alarme = EEPROM.read(1);
alarme_on= EEPROM.read(3);

  display.begin();
  display.setContrast(55);
  analogWrite(3,luz_lcd);
  display.clearDisplay(); 

  // Mostra logotipo
  display.drawBitmap(0, 0,  main_logo, 84, 48, WHITE, BLACK);
  display.display(); 
  delay(1500);
  display.clearDisplay();  
}

void loop() {
     //Verificar alarme
     if (hora_alarme == hora && minuto_alarme == minuto && alarme_on != 0 && alarme_on != 10){
       //***************************************************************************************************************************************
       toca_alarme();
       }
     //Reset do alarme depois de tocar (o valor fica em 10 depois de 1/2 minutos a tocar sem ser parado)
     if(((minuto - minuto_alarme)>2 || ((minuto - minuto_alarme) < -2)) && alarme_on == 10){
       alarme_on = 1;
       }
     
    //Luz do LCD
    analogWrite(3,luz_lcd);  
    //Verificar modo atual de funcionamento
    if (modo == 0){
      tempo_modo_1 = 0;
      modo_normal();           
    }
    else if (modo == 1){
      modo_grande();
      }
    else if (modo == 2){
      modo_alfa();
      }      

      
  //Verificar comandos (botões)
  int longo = 1;
  if (digitalRead(botao_enter)==HIGH){   
    //DESLIGAR ALARME
    if (alarme_on == 3){
      // a tocar / desligar
      analogWrite(pin_buzzer, 0);
      alarme_on = 10;
      delay(1500);     
      }
      else
      {
       float temp_tempo = millis(); 
        while (digitalRead(botao_enter)==HIGH){
          //Verifica tempo que botão esteve pressionado
          if ((millis()-temp_tempo) > 1000){
            //PRESS AND HOLD (1 segundo)
            //Entrar no estado 'MENU'
            longo = 1;
            estado=1;
            buzz(1000,50);
            menu();
            break;
            }
           else {
             longo = 0;
             }
          }    
         if (longo != 1){
         //Menos de 1 segundo pressionado
         //Luz temporária para relógio
         if (luz_lcd > luz_info){
           analogWrite(3, luz_info);
           delay(5000);               
          }
       }        
        }

    }
  if (digitalRead(botao_direita)==HIGH){    
    //DESLIGAR ALARME
    if (alarme_on == 3){
      // a tocar / desligar
      analogWrite(pin_buzzer, 0);
      alarme_on = 10;
      delay(1500);  
      }
      else{
        //Mudar modo
        mudar_modo();
        delay(300);    
        }
    
    }
    
  if (digitalRead(botao_esquerda)==HIGH){    
    //DESLIGAR ALARME
    if (alarme_on == 3){
      // a tocar / desligar
      analogWrite(pin_buzzer, 0);
      alarme_on = 10;
      delay(1500);            
      }   
      else
      {
    long temp_contagem = millis();
    boolean longo = false;
    while (digitalRead(botao_esquerda)==HIGH){
    if (millis() - temp_contagem > 1000){
      // Longo
      longo = true;
      if (alarme_on == 0){
        alarme_on = 1;
        EEPROM.write(3,1);
        buzz(0,0);
        delay(700);  
        break;
        }
      else {
        alarme_on = 0;
        EEPROM.write(3,0);        
        buzz(0,0);
        delay(700);          
        break;
        }
      longo = true;
      delay(1000);  
      temp_contagem = millis();
      }      
      }
      if (longo == false){
        //Mostrar informações (apresenta temporariamente)
        delay(300);
        temp_info_temperatura();   
        }        
        }
  }
      
  //Leitura do RTC (Data, Hora, etc.)
  ler_rtc(); 

}

void ler_rtc(){
  segundo=Clock.getSecond();
  if (segundo <10) {
    segundos = "0" + String(segundo);
  }
  else {
    segundos = String(segundo);
  }
  minuto=Clock.getMinute();
  if (minuto <10) {
    minutos = "0" + String(minuto);
  }
  else {
    minutos = String(minuto);
  }
  hora=Clock.getHour(h12, PM);
  if (hora <10) {
    horas = "0" + String(hora);
  }
  else {
    horas = String(hora);
  }
  data=Clock.getDate();
//  mes=Clock.getMonth(Century);
  ano=Clock.getYear();
  semana = Clock.getDoW();
  //Traduzir valor de dia de semana para texto
  switch(semana){
    case 1: 
      d_semana = "DOMINGO";
      break;
    case 2: 
      d_semana = "SEGUNDA";      
      break;
    case 3: 
      d_semana = "TERCA";
      break;      
    case 4: 
      d_semana = "QUARTA";
      break;      
    case 5: 
      d_semana = "QUINTA";
      break;      
    case 6: 
      d_semana = "SEXTA";
      break;      
    case 7: 
      d_semana = "SABADO";
      break;      
    }
    
    mes=Clock.getMonth(Century);
  //Traduzir valor de dia de semana para texto
  switch(mes){
    case 1: 
      mes_txt = "JAN";
      break;
    case 2: 
      mes_txt = "FEV";      
      break;      
    case 3: 
      mes_txt = "MAR";
      break;      
    case 4: 
      mes_txt = "ABR";
      break;      
    case 5: 
      mes_txt = "MAI";
      break;      
    case 6: 
      mes_txt = "JUN";
      break;      
    case 7: 
      mes_txt = "JUL";
      break;      
    case 8: 
      mes_txt = "AGO";
      break;      
    case 9: 
      mes_txt = "SET";
      break;      
    case 10: 
      mes_txt = "OUT";
      break;      
    case 11: 
      mes_txt = "NOV";
      break;      
    case 12: 
      mes_txt = "DEZ";
      break;      
    }    
    
  temperatura=Clock.getTemperature();
  
  set_hora = hora;
  set_minuto =minuto;
  set_dia =data;
  set_mes =mes;
  set_ano =ano;
  }
  
void modo_normal(){
  // Apresenta horas e data em "MODO NORMAL"
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(3);
  display.setTextColor(BLACK);
  if (hora < 10){
    display.print("0");
    }
  display.print(hora);
  display.setTextSize(1);  
  if(alarme_on > 0){
    display.drawBitmap(44, 4,  alarme_8, 8, 8, WHITE,BLACK);   
    display.setCursor(54,4);
    if (hora_alarme < 10){
      display.print("0");
      }
    display.print(hora_alarme);
    display.print(":");  
    if (minuto_alarme < 10){
      display.print("0");
      }    
    display.print(minuto_alarme);    
    }
    display.drawBitmap(0, 30,  termo_16, 16, 16, WHITE,BLACK);       
    display.setCursor(15,32);
    if (temperatura < 10){
       display.print("0");
      }
    display.print(temperatura,0);
  display.setCursor(45,24); 
  //display.setTextSize(1);
  display.setTextColor(BLACK);      
  display.setTextSize(3);
  if (minuto < 10){
    display.print("0");
    }  
  display.print(minuto);
  display.display();
  delay(50);
  display.clearDisplay();  
  }
  
void modo_alfa(){
  //Traduzir valor da hora para extenso
  String hora_extenso ="";
  switch(hora){
    case 0:
      hora_extenso = "ZERO";
      break;
    case 1:
      hora_extenso = "UMA";
      break;
    case 2:
      hora_extenso = "DUAS";
      break;      
    case 3:
      hora_extenso = "TRES";
      break;    
    case 4:
      hora_extenso = "QUATRO";
      break;          
    case 5:
      hora_extenso = "CINCO";
      break;          
    case 6:
      hora_extenso = "SEIS";
      break;          
    case 7:
      hora_extenso = "SETE";
      break;          
    case 8:
      hora_extenso = "OITO";
      break;          
    case 9:
      hora_extenso = "NOVE";
      break;          
    case 10:
      hora_extenso = "DEZ";
      break;          
    case 11:
      hora_extenso = "ONZE";
      break;          
    case 12:
      hora_extenso = "DOZE";
      break;          
    case 13:
      hora_extenso = "UMA";
      break;
    case 14:
      hora_extenso = "DUAS";
      break;      
    case 15:
      hora_extenso = "TRES";
      break;    
    case 16:
      hora_extenso = "QUATRO";
      break;          
    case 17:
      hora_extenso = "CINCO";
      break;          
    case 18:
      hora_extenso = "SEIS";
      break;          
    case 19:
      hora_extenso = "SETE";
      break;          
    case 20:
      hora_extenso = "OITO";
      break;          
    case 21:
      hora_extenso = "NOVE";
      break;          
    case 22:
      hora_extenso = "DEZ";
      break;          
    case 23:
      hora_extenso = "ONZE";
      break;                
    }
    
  // Apresenta horas e data em "MODO ALFANUMÉRICO (Horas por extenso)"
  //Barra infos
  display.clearDisplay();
  display.setTextSize(1);  
  display.setCursor(0,0);
  //display.setTextColor(WHITE, BLACK);      
  display.print("T:");
  display.print(temperatura,0);
  display.print("C ");
  display.print("   ");
  
  //DATA
  if (data < 10){
    display.print("0");
    }
  display.print(data);
  
  display.print("/");
  
  if (mes < 10){
    display.print("0");
    }
  display.print(mes);  
  
  display.setTextColor(BLACK);  
  display.setCursor(5,10);
  display.setTextSize(2);
  display.print(hora_extenso);
  display.setCursor(20,28);
  display.print("E");  
  display.setTextSize(3);
  display.setCursor(40,26);
  display.setTextColor(BLACK);  
  if (minuto < 10){
    display.print("0");
    }
  display.print(minuto); 
  
  display.display();
  delay(50);
  }  
  
void modo_grande(){
  tempo_modo_1 +=1;
  display.clearDisplay();
  if ((millis() - tempo_modo_1) < 3000){
    //Apresenta as horas
    display.setCursor(8,0);
    display.setTextSize(6);
    display.setTextColor(BLACK);
    display.fillRect(0,44,84,4,BLACK);
    if (hora < 10){
    display.print("0");
    }
    display.println(hora);
    display.display();  
  }
  else if ((millis() - tempo_modo_1) >= 3000 && (millis() - tempo_modo_1 < 6000) ){
    //Apresenta Minutos
    display.clearDisplay();
    display.setCursor(8,0);
    display.setTextSize(6);
    display.setTextColor(BLACK);
    display.fillRect(80,0,4,48,BLACK);    
    if (minuto < 10){
      display.print("0");
      }  
    display.println(minuto);
    display.display();  
    }
  else if ((millis() - tempo_modo_1) >= 6000){
     //RESET do temporizador
     tempo_modo_1 = millis();
  }
  delay(50);
 }
  
  void mudar_modo(){
    if (modo == 0){
      modo=1;
      }
    else if (modo==1) {
      modo = 2;
      }
    else if (modo == 2){
      modo = 0;
      }      
      tempo_modo_1 = millis();
      delay(50);
    }


void temp_info_temperatura(){
  boolean sair = false;//Variável para sair de temp_info()
  long tempo_info = millis(); // Tempo para mudar para próxima info
  
  while (sair==false){
  //iluminação mínima do LCD
   iluminacao_info();
  //Verificar pressionamento do botão para próxima info
  if (digitalRead(botao_esquerda) == HIGH){
    //Botão pressionado
    //Chamar próxima info
    sair=true;
    delay(300);
    temp_calendario();
    }    
  if (digitalRead(botao_enter) == HIGH){
    //Botão pressionado
    //Chamar próxima info
    sair=true;
    delay(300);
    }    
     //Verificar qual a info a apresentar de acordo com o tempo
     if ((millis() - tempo_info) < 4000){
      //Apresentar temporariamente temperatura)
      display.clearDisplay(); 
      // Mostra temperatura
      display.drawBitmap(0, 12,  termo_32, 32, 32, WHITE, BLACK);
      display.setCursor(0,0);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print("  TEMPERATURA ");
      display.setCursor(35,13);
      display.setTextSize(4);
      display.setTextColor(BLACK);
      display.print(temperatura,0);
      display.display(); 
      delay(50);//***********************************************************
      display.clearDisplay();      
      }
      else if ((millis()-tempo_info) >= 4000){
        //SAIR !!
        sair=true;
        }
  }
}

void temp_calendario(){
  boolean sair = false;//Variável para sair de temp_calendario()
  long tempo_calendario = millis(); // Tempo para mudar para próxima info ou sair
  while (sair==false){
   //iluminação mínima do LCD
   iluminacao_info();
   //Verificar pressionamento do botão para próxima info
   if (digitalRead(botao_esquerda) == HIGH){
   //Botão pressionado
   //Chamar próxima info
   delay(300);
   temp_luz_fundo();
   sair=true;
   break;
   }
  if (digitalRead(botao_enter) == HIGH){
    //Botão pressionado
    //Chamar próxima info
    sair=true;
    delay(300);
    calendario_info(); //*******************************************************
    }       
 
 //Verificar qual a info a apresentar de acordo com o tempo
 if ((millis() - tempo_calendario) < 4000){

  display.clearDisplay(); 
  display.drawBitmap(0, 12,  calendar_32, 32, 32, WHITE, BLACK);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.print("  CALENDARIO  ");
  display.setCursor(35,13);
  display.setTextSize(4);
  display.setTextColor(BLACK);
  if (data < 10){
    display.print("0");
  }
  display.print(data);
  display.display(); 

  }
  else if ((millis()-tempo_calendario) >= 4000){
    //SAIR !!
    sair=true;
    }
  }
 }  
 
 void calendario_info(){
  boolean sair = false;//Variável para sair de calendario_info()
  long tempo_calendario = millis(); // Tempo para mudar para próxima info ou sair
  while (sair==false){
   //iluminação mínima do LCD
   iluminacao_info();
   //Verificar pressionamento do botão para próxima info
   if (digitalRead(botao_esquerda) == HIGH){
   //Botão pressionado
   //Chamar próxima info (Voltar)
   delay(300);
   //temp_calendario();
   sair=true;
   break;
   }
  if (digitalRead(botao_enter) == HIGH){
    //Botão pressionado
    //Voltar para mini calendario   **********************************************************
    sair=true;
    delay(300);
    temp_calendario();
    }       
 
 //Verificar qual a info a apresentar de acordo com o tempo
 if ((millis() - tempo_calendario) < 8000){

  //Apresentar info calendario)
  display.clearDisplay(); 
  display.drawBitmap(0, 12,  calendar_32, 32, 32, WHITE, BLACK);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.print("  CALENDARIO  ");
  display.setCursor(36,13);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.print(d_semana);
  display.setCursor(38,23);
  display.setTextSize(2);
  if (data < 10){
    display.print("0");
    }
  display.print(data);
  display.setCursor(64,24);
  display.setTextSize(1);
  display.print(mes_txt); 
  display.setCursor(38,40);
  display.setTextColor(WHITE, BLACK);
  display.print("  20"); 
  display.print(ano);  
  display.print("  "); 
  display.setTextColor(BLACK);
  display.display(); 

  }
  else if ((millis()-tempo_calendario) >= 8000){
    //SAIR !!  -  Voltar!!
    sair=true;
    }
  }
 }  
 
 
 

void temp_luz_fundo(){
  boolean sair = false;//Variável para sair de set__luz_fundo()
  long tempo_luz_fundo = millis(); // Tempo para mudar para próxima info (ou sair)
  while (sair==false){
   //iluminação mínima do LCD
   iluminacao_info();
   //Verificar pressionamento do botão para próxima info
   if (digitalRead(botao_esquerda) == HIGH){
   //Botão pressionado
   //Chamar próxima info (ou sair)
   delay(300);
   sair=true;
   break;
   }
   
   if (digitalRead(botao_enter) == HIGH){
   //Botão pressionado
   //Chamar próxima info
   delay(300);
   sair=true;
   set_luz_fundo();
   break;
   }
 
 //Verificar qual a info a apresentar de acordo com o tempo ou sair
 if ((millis() - tempo_luz_fundo) < 4000){

  //Apresentar valor da luz lcd
  display.clearDisplay(); 
  display.drawBitmap(0, 12,  bulb_32, 32, 32, WHITE, BLACK);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.print("  LUZ FUNDO   ");
  display.setCursor(35,13);
  display.setTextSize(4);
  display.setTextColor(BLACK);
  int temp_luz_lcd = map(luz_lcd,5,255,10,0);
  if (temp_luz_lcd < 10){
    display.print("0");
    }  
  display.print(temp_luz_lcd);
  display.display(); 

  }
  else if ((millis()-tempo_luz_fundo) >= 4000){
    //SAIR !!
    sair=true;
    }
  }  
  }
  
void set_luz_fundo(){
  boolean sair = false;//Variável para sair de set__luz_fundo()
  long tempo_set_luz_fundo = millis(); // Tempo para mudar para próxima info (ou sair)
  while (sair==false){
    //Luz de fundo do LCD
    analogWrite(3, luz_lcd);
   
   //Verificar pressionamento de botão
   if (digitalRead(botao_enter) == HIGH){
   //Botão pressionado
   //Chamar próxima info
   delay(300);
   sair=true;
   temp_luz_fundo();
   break;
   }
   
   if (digitalRead(botao_esquerda) == HIGH){
   //Botão pressionado
   //Diminuir intensidade da luz de fundo
   delay(300);
   if (luz_lcd <= 230){
        luz_lcd += 25;     
        tempo_set_luz_fundo = millis();
     }
   }
   
   if (digitalRead(botao_direita) == HIGH){
   //Botão pressionado
   //Chamar próxima info ou sair
   delay(300);
  if (luz_lcd >= 30){
   luz_lcd -= 25;    
   tempo_set_luz_fundo = millis();
 }
   }   
 
 //Verificar tempo para saida
 if ((millis() - tempo_set_luz_fundo) < 6000){

  //Apresentar
  display.clearDisplay(); 
  // Mostra temperatura
  display.drawBitmap(0, 12,  bulb_32, 32, 32, WHITE, BLACK);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.print("< LUZ FUNDO > ");
  display.setCursor(35,13);
  display.setTextSize(4);
  display.setTextColor(WHITE,BLACK);
  int temp_luz_lcd = map(luz_lcd,5,255,10,0);
  if (temp_luz_lcd < 10){
    display.print("0");
    }
  display.print(temp_luz_lcd);
  display.display(); 

  }
  else if ((millis()-tempo_set_luz_fundo) >= 6000){
    //SAIR !!
    sair=true;
    }
  }  
}  

void iluminacao_info(){
  //Iluminação mínima durante informações
  if (luz_lcd > 180){
    //Iluminação mínima para infos
    analogWrite(3, luz_info);
    }
  }

void menu(){

 //Display inicial  
  display.clearDisplay(); 
  display.drawBitmap(0, 12,  clock_32, 32, 32, WHITE, BLACK);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.print("  CONFIGURAR  ");
  display.setCursor(35,13);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.print("ACERTAR");
  display.setCursor(35,23);
  display.print("RELOGIO");    
  display.display();
  delay(500);    
    
  while (estado == 1){   
      // Iluminação mínima
    if (luz_lcd > luz_info){
      analogWrite(3, luz_info);
      }
      else {
        analogWrite(3, luz_lcd);
        }
    //Apresentar Páginas
    if (pagina == 1){
      //Acertar relógio (MAIN)  
      display.clearDisplay(); 
      display.drawBitmap(0, 12,  clock_32, 32, 32, WHITE, BLACK);
      display.setCursor(0,0);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print("  CONFIGURAR  ");
      display.setCursor(35,13);
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.print("ACERTAR");
      display.setCursor(35,23);
      display.print("RELOGIO");    
      display.display();
      
      //Botões
      if (digitalRead(botao_direita) == HIGH){
        //Próxima página
        pagina = 2;
        delay(300);        
        }
      if (digitalRead(botao_esquerda) == HIGH){
        //Página anterior
        pagina = 4;
        delay(300);        
        }        
      if (digitalRead(botao_enter) == HIGH){
        //Confirmar ação        
        delay(300);   
        pagina = 11; //Acertar Relógio / Acertar Hora     
        }        
      }
    else if (pagina == 2) {
      //Acertar data (MAIN)  
      display.clearDisplay(); 
      display.drawBitmap(0, 12,  calendar_32, 32, 32, WHITE, BLACK);
      display.setCursor(0,0);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print("  CONFIGURAR  ");
      display.setCursor(35,13);
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.print("ACERTAR");
      display.setCursor(35,23);
      display.print("DATA");    
      display.display();
      
      //Botões
      if (digitalRead(botao_direita) == HIGH){
        //Próxima página
        pagina = 3;
        delay(300);        
        }
      if (digitalRead(botao_esquerda) == HIGH){
        //Página anterior
        pagina = 1;
        delay(300);        
        }        
      if (digitalRead(botao_enter) == HIGH){
        //Confirmar ação     
        pagina = 21;   
        delay(300);        
      }        

      }

    else if (pagina == 3){
      //Acertar alarme (MAIN)  
      display.clearDisplay(); 
      display.drawBitmap(0, 12,  alarme_32, 32, 32, WHITE, BLACK);
      display.setCursor(0,0);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print("  CONFIGURAR  ");
      display.setCursor(35,13);
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.print("ACERTAR");
      display.setCursor(35,23);
      display.print("ALARME");    
      display.display();
      
      //Botões
      if (digitalRead(botao_direita) == HIGH){
        //Próxima página
        pagina = 4;
        delay(300);        
        }
      if (digitalRead(botao_esquerda) == HIGH){
        //Página anterior
        pagina = 2;
        delay(300);
        }        
      if (digitalRead(botao_enter) == HIGH){
        //Confirmar ação    
        pagina = 31;    
        delay(300);        
      }       
      }
      
    else if (pagina == 4) {
      //Sair do menu  
      display.clearDisplay(); 
      display.drawBitmap(0, 12,  eject_32, 32, 32, WHITE, BLACK);
      display.setCursor(0,0);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print("  CONFIGURAR  ");
      display.setCursor(35,13);
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.print("SAIR DO");
      display.setCursor(35,23);
      display.print("MENU");    
      display.display();
      
      //Botões
      if (digitalRead(botao_direita) == HIGH){
        //Próxima página
        pagina =1;
        delay(300);
        }
      if (digitalRead(botao_esquerda) == HIGH){
        //Página anterior
        pagina = 3;
        delay(300);        
        }        
      if (digitalRead(botao_enter) == HIGH){
        //Confirmar ação
        //SAIR DO MENU
        delay(300);   
        pagina = 1;     
        estado = 0;
      } 
     }
      
    else if (pagina == 11){
      //Acertar relógio / Hora
      display.clearDisplay(); 
      display.drawBitmap(0, 12,  clock_32, 32, 32, WHITE, BLACK);
      display.setCursor(0,0);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print("  CONFIGURAR  ");
      display.setCursor(35,13);
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.print("HORA");
      display.setTextSize(2);
      display.setCursor(35,23);
      if (set_hora < 10){
        display.print("0");
        }
      display.print(set_hora);    //**********************
      display.display();      
      
      //Botões
      if (digitalRead(botao_direita) == HIGH){
          if (digitalRead(botao_esquerda) == HIGH){
            //CANCELAR
            estado = 0;
            pagina = 1;
            buzz(1500,100);
            delay (1000);
            break;
            }
        //Hora +
        if (set_hora < 23){
          set_hora += 1;
          }
          else if (set_hora == 23){
            set_hora = 0;
            }
        delay(300);
        }
      if (digitalRead(botao_esquerda) == HIGH){
        //Hora -
        if (set_hora > 0){
          set_hora -= 1;
          }
          else if (set_hora == 0){
            set_hora = 23;
            }
        delay(300);        
        }        
      if (digitalRead(botao_enter) == HIGH){
        //Confirmar ação
        //ACERTAR MINUTOS
        delay(300);   
        pagina = 12;     
      } 
      }

    else if (pagina == 12){
      //Acertar relógio / Minuto
      display.clearDisplay(); 
      display.drawBitmap(0, 12,  clock_32, 32, 32, WHITE, BLACK);
      display.setCursor(0,0);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print("  CONFIGURAR  ");
      display.setCursor(35,13);
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.print("MINUTO");
      display.setTextSize(2);
      display.setCursor(35,23);
      if (set_minuto < 10){
        display.print("0");
        }
      display.print(set_minuto);    //**********************
      display.display();      
      
      //Botões
      if (digitalRead(botao_direita) == HIGH){
          if (digitalRead(botao_esquerda) == HIGH){
            //CANCELAR
            estado = 0;
            pagina = 1;
            buzz(1500,100);            
            delay (1000);
            break;
            }
        //Minuto +
        if(set_minuto < 59){
          set_minuto += 1;          
          }
        else if (set_minuto == 59){
          set_minuto = 0;
          }
        delay(150);
        }
      if (digitalRead(botao_esquerda) == HIGH){
        //Minuto -
        if (set_minuto > 0){
          set_minuto -= 1;            
          }
          else if (set_minuto == 0){
            set_minuto = 59;
            }

        delay(150);        
        }        
      if (digitalRead(botao_enter) == HIGH){
        //Confirmar ação
        //GRAVAR
        delay(300);   
        acertar_hora();   //********************************************************************************************************************     
      } 
      }   
       else if (pagina == 21){
      //Acertar calendário / ano
      display.clearDisplay(); 
      display.drawBitmap(0, 12,  calendar_32, 32, 32, WHITE, BLACK);
      display.setCursor(0,0);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print("  CONFIGURAR  ");
      display.setCursor(35,13);
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.print("ANO");
      display.setTextSize(2);
      display.setCursor(35,23);
      display.print("20");
      if (set_ano < 10){
        display.print("0");
        }
      display.print(set_ano);    //**********************
      display.display();      
      
      //Botões
      if (digitalRead(botao_direita) == HIGH){
          if (digitalRead(botao_esquerda) == HIGH){
            //CANCELAR
            estado = 0;
            pagina = 1;
            buzz(1500,100);            
            delay (1000);
            break;
            }
        //Dia +
         if (set_ano < 99){
          set_ano += 1;            
          }
          else if (set_ano == 99){
            set_dia = 1;
            }
        delay(300);
        }
      if (digitalRead(botao_esquerda) == HIGH){
        //Dia -
        if (set_ano > 0){
          set_ano -= 1;            
          }
          else if (set_ano == 0){
            set_dia = 99;
            }

        delay(300);        
        }        
      if (digitalRead(botao_enter) == HIGH){
        //Confirmar ação
        //GRAVAR
        delay(300);   
        pagina = 22;
      } 
      }     
      
     else if (pagina == 22){
      //Acertar calendário / mes
      display.clearDisplay(); 
      display.drawBitmap(0, 12,  calendar_32, 32, 32, WHITE, BLACK);
      display.setCursor(0,0);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print("  CONFIGURAR  ");
      display.setCursor(35,13);
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.print("MES");
      display.setTextSize(2);
      display.setCursor(35,23);
      if (set_mes < 10){
        display.print("0");
        }
      display.print(set_mes);    //**********************
      display.display();      
      
      //Botões
      if (digitalRead(botao_direita) == HIGH){
          if (digitalRead(botao_esquerda) == HIGH){
            //CANCELAR
            estado = 0;
            pagina = 1;
            buzz(1500,100);            
            delay (1000);
            break;
            }
        //Dia +
         if (set_mes < 12){
          set_mes += 1;            
          }
          else if (set_mes == 12){
            set_mes = 1;
            }
        delay(300);
        }
      if (digitalRead(botao_esquerda) == HIGH){
        //Dia -
        if (set_mes > 1){
          set_mes -= 1;            
          }
          else if (set_mes == 1){
            set_mes = 12;
            }

        delay(300);        
        }        
      if (digitalRead(botao_enter) == HIGH){
        //Confirmar ação
        //Calcular dia máximo de acordo com o mês escolhido
        //**************************************************************************************************************************************
        //Próxima página
        delay(300);   
        pagina = 23;
      } 
      }      
      
     else if (pagina == 23){
      //Acertar calendário / dia
      display.clearDisplay(); 
      display.drawBitmap(0, 12,  calendar_32, 32, 32, WHITE, BLACK);
      display.setCursor(0,0);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print("  CONFIGURAR  ");
      display.setCursor(35,13);
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.print("DIA");
      display.setTextSize(2);
      display.setCursor(35,23);
      if (set_dia < 10){
        display.print("0");
        }
      display.print(set_dia);    //**********************
      display.display();      
      
      //Botões
      if (digitalRead(botao_direita) == HIGH){
          if (digitalRead(botao_esquerda) == HIGH){
            //CANCELAR
            estado = 0;
            pagina = 1;
            buzz(1500,100);            
            delay (1000);
            break;
            }
        //Dia +
         if (set_dia < 31){
          set_dia += 1;            
          }
          else if (set_dia == 31){
            set_dia = 1;
            }
        delay(300);
        }
      if (digitalRead(botao_esquerda) == HIGH){
        //Dia -
        if (set_dia > 1){
          set_dia -= 1;            
          }
          else if (set_dia == 1){
            set_dia = 31;
            }

        delay(300);        
        }        
      if (digitalRead(botao_enter) == HIGH){
        //Confirmar ação
        //Próxima página
        delay(300);   
        pagina = 24;
      } 
     }    
    
      else if (pagina == 24){
      //Acertar calendário / dia da semana
      display.clearDisplay(); 
      display.drawBitmap(0, 12,  calendar_32, 32, 32, WHITE, BLACK);
      display.setCursor(0,0);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print("  CONFIGURAR  ");
      display.setCursor(35,13);
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.print("SEMANA");
      display.setTextSize(2);
      display.setCursor(35,23);
      display.print(dia_semana_txt);    //**********************
      display.display();      
      
      //Botões
      if (digitalRead(botao_direita) == HIGH){
          if (digitalRead(botao_esquerda) == HIGH){
            //CANCELAR
            estado = 0;
            pagina = 1;
            buzz(1500,100);            
            delay (1000);
            break;
            }
        //Dia +
         if (dia_semana_txt == "SEG"){
          dia_semana_txt = "TER";            
          set_dia_semana = 3;          
          }
          else if (dia_semana_txt == "TER"){
            dia_semana_txt = "QUA";
            set_dia_semana = 4;              
            }
          else if (dia_semana_txt == "QUA"){
            dia_semana_txt = "QUI";
            set_dia_semana = 5;            
            }            
          else if (dia_semana_txt == "QUI"){
            dia_semana_txt = "SEX";
            set_dia_semana = 6;            
            }
          else if (dia_semana_txt == "SEX"){
            dia_semana_txt = "SAB";
            set_dia_semana = 7;                
            }            
          else if (dia_semana_txt == "SAB"){
            dia_semana_txt = "DOM";
            set_dia_semana = 1;            
            }
          else if (dia_semana_txt == "DOM"){
            dia_semana_txt = "SEG";
            set_dia_semana = 2;            
            }
              
        delay(300);
        }
      if (digitalRead(botao_esquerda) == HIGH){
       //Dia -
       if (dia_semana_txt == "SEG"){
        dia_semana_txt = "DOM";            
        set_dia_semana = 1;
        }
        else if (dia_semana_txt == "TER"){
          dia_semana_txt = "SEG";
          set_dia_semana = 2;          
          }
        else if (dia_semana_txt == "QUA"){
          dia_semana_txt = "TER";
          set_dia_semana = 3;                  
          }            
        else if (dia_semana_txt == "QUI"){
          dia_semana_txt = "QUA";
          set_dia_semana = 4;
          }
        else if (dia_semana_txt == "SEX"){
          dia_semana_txt = "QUI";
          set_dia_semana = 5;          
          }            
        else if (dia_semana_txt == "SAB"){
          dia_semana_txt = "SEX";
          set_dia_semana = 6;
          }
        else if (dia_semana_txt == "DOM"){
          dia_semana_txt = "SAB";
          set_dia_semana = 7;          
          
          }

        delay(300);        
        }        
      if (digitalRead(botao_enter) == HIGH){
        //Confirmar ação
        //GRAVAR
        delay(300);   
        acertar_data();
      } 
     }    
     
     else if (pagina == 31){
      //Acertar despertador / hora
      display.clearDisplay(); 
      display.drawBitmap(0, 12,  alarme_32, 32, 32, WHITE, BLACK);
      display.setCursor(0,0);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print("  CONFIGURAR  ");
      display.setCursor(35,13);
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.print("HORA");
      display.setTextSize(2);
      display.setCursor(35,23);
      if (hora_alarme < 10){
        display.print("0");
        }
      display.print(hora_alarme);    //**********************
      display.setTextSize(1);
      display.setCursor(52,40);
      if (hora < 10){
        display.print("0");
        }
      display.print(hora);
      display.print(":");
      if (minuto < 10){
        display.print("0");
        }      
      display.print(minuto);        
      display.display();      
      
      //Botões
      if (digitalRead(botao_direita) == HIGH){
          if (digitalRead(botao_esquerda) == HIGH){
            //SAIR
            pagina = 1;
            estado = 0;
            break;
            }
        //Minuto +
         if (hora_alarme < 23){
          hora_alarme += 1;            
          }
          else {
            hora_alarme = 0;
            }
        delay(300);
        }
      if (digitalRead(botao_esquerda) == HIGH){
        //Hora -
        if (hora_alarme > 0){
          hora_alarme -= 1;              
          }
        else {
          hora_alarme = 23;
          }
        delay (300);
        }        
      if (digitalRead(botao_enter) == HIGH){
        //Confirmar ação
        //Próxima página
        pagina = 32;
        delay(300);   
      } 
     }  
      
    else if (pagina == 32){
      //Acertar despertador / hora
      display.clearDisplay(); 
      display.drawBitmap(0, 12,  alarme_32, 32, 32, WHITE, BLACK);
      display.setCursor(0,0);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print("  CONFIGURAR  ");
      display.setCursor(35,13);
      display.setTextSize(1);
      display.setTextColor(BLACK);
      display.print("MINUTO");
      display.setTextSize(2);
      display.setCursor(35,23);
      if (minuto_alarme < 10){
        display.print("0");
        }
      display.print(minuto_alarme);    //**********************
      display.setTextSize(1);
      display.setCursor(52,40);
      if (hora < 10){
        display.print("0");
        }
      display.print(hora);
      display.print(":");
      if (minuto < 10){
        display.print("0");
        }      
      display.print(minuto);              
      display.display();      
      
      //Botões
      if (digitalRead(botao_direita) == HIGH){
          if (digitalRead(botao_esquerda) == HIGH){
            //SAIR
            pagina = 1;
            estado = 0;
            break;
            }
        //Minuto +
         if (minuto_alarme < 59){
          minuto_alarme += 1;            
          }
          else {
            minuto_alarme = 0;
            }
        delay(150);
        }
      if (digitalRead(botao_esquerda) == HIGH){
        //Hora -
        if (minuto_alarme > 0){
          minuto_alarme -= 1;              
          }
        else {
          minuto_alarme = 59;
          }
        delay (150);
        }        
      if (digitalRead(botao_enter) == HIGH){
        //Confirmar ação
        //GRAVAR !!
        gravar_alarme();
        delay(300);   
      } 
      }     
      
     
    }
  }
  
  void acertar_hora(){
    //LCD
    display.clearDisplay(); 
    display.drawBitmap(0, 12,  clock_32, 32, 32, WHITE, BLACK);
    display.setCursor(0,0);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.print("  CONFIGURAR  ");
    display.setCursor(35,23);
    display.setTextSize(1);
    display.setTextColor(WHITE,BLACK);
    display.print("GRAVADO!");
    display.display();
    buzz(1500,100);
    delay(3000);
    //Sair
    pagina = 1;     
    estado = 0;
    
    //Gravar valores para relógio e data no rtc
    Clock.setSecond(0);
    Clock.setMinute(set_minuto);
    Clock.setHour(set_hora);
    //Clock.setDoW(1);
    //Clock.setDate(set_dia);
    //Clock.setMonth(set_mes);
    //Clock.setYear(set_ano);
    }
    
  void acertar_data(){
    //LCD
    display.clearDisplay(); 
    display.drawBitmap(0, 12,  clock_32, 32, 32, WHITE, BLACK);
    display.setCursor(0,0);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.print("  CONFIGURAR  ");
    display.setCursor(35,23);
    display.setTextSize(1);
    display.setTextColor(WHITE,BLACK);
    display.print("GRAVADO!");
    display.display();
    buzz(1500,100);
    delay(3000);
    //Sair
    pagina = 1;     
    estado = 0;
    
    //Gravar valores para relógio e data no rtc
    //Clock.setSecond(0);
    //Clock.setMinute(set_minuto);
    //Clock.setHour(set_hora);
    Clock.setDoW(set_dia_semana);
    Clock.setDate(set_dia);
    Clock.setMonth(set_mes);
    Clock.setYear(set_ano);
    }    
    
void buzz(int frequencia, int tempo){
  //BUZZER
  //tone(pin_buzzer, frequencia, tempo);
  analogWrite(pin_buzzer,150);
  delay(100);
  analogWrite(pin_buzzer,0);
  }
  
void gravar_alarme(){
    //LCD
  display.clearDisplay(); 
  display.drawBitmap(0, 12,  clock_32, 32, 32, WHITE, BLACK);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.print("  CONFIGURAR  ");
  display.setTextSize(1);
  display.setCursor(35,15);
   display.print("ALARME");
  display.setCursor(35,23);
  display.setTextColor(WHITE,BLACK);
  display.print("GRAVADO!");
  display.setTextSize(2);
  //display.setTextColor(BLACK);  
  display.setCursor(22,30);
  if (hora_alarme < 10){
    display.print("0");
    }
  display.print(hora_alarme);
  display.print(":");
  if (minuto_alarme < 10){
    display.print("0");
    }      
  display.print(minuto_alarme);          
  display.display();
  buzz(1500,100);
  delay(3000);
  //Sair
  pagina = 1;     
  estado = 0;  
    //Gravar alarme na eeprom
  EEPROM.write(0,hora_alarme);
  EEPROM.write(1,minuto_alarme);
  EEPROM.write(2,alarme_on);
  alarme_on = 1;
}


void toca_alarme(){
  alarme_on =3;
  analogWrite(pin_buzzer,20);
  delay(500);
  analogWrite(pin_buzzer,0);  
  delay(300);
 }
