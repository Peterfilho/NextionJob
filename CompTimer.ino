
#include "Wire.h"                //Biblioteca para I2C
#include "SparkFunMLX90614.h"    //Biblioteca SparkFunMLX90614
#include "Nextion.h"


// ===============================================================================
// --- Declaração de Objetos ---
IRTherm therm;                   //Cria um objeto para o sensor de temperatura infravermelho


// -- Objetos do Nextion --
NexText disp_temp = NexText(0, 9, "t4"); // Temperatura da pagina inicial

NexButton b0 = NexButton(0, 2, "b0");
NexButton b1 = NexButton(0, 5, "b1");
NexButton b2 = NexButton(0, 6, "b2");
NexText t0 = NexText(0, 3, "t0");
NexText t1 = NexText(0, 4, "t1");
NexText t6 = NexText(0, 11, "t6");
NexText t13 = NexText(0, 21, "t13");
NexTimer tm0 = NexTimer(0, 1, "tm0");
NexNumber tmpMax = NexNumber(0, 16, "n0");


char buffer[100] = {0};
char buffer2[100] = {0};
char buffer3[100] = {0};
char buffer4[100] = {0};

uint32_t number_timer = 1;
uint32_t number_enable = 1;
uint32_t number_cycle = 100;      // numero de segundos que (adiciona ou remove) ao clicar no botao (+ ou -) do temporizador
int temperaturaMaxima = 0;        // variavel da temperatura maxima permitida
int temperaturaMinima = 0;        // variavel da temperatura minima permitida
int32_t maxTemperature;           // variavel para conversao da temperatura na tela
/*
int estadoBotao = 0;
int ledPin = 10;
int inPin = 2;
*/
//  DECLARANDO LISTA DE BOTÕES PARA O LISTNER

NexTouch *nex_listen_list[] = 
{
    &b0,
    &b1,
    &b2,
    &t0,
    &t1,
    &tm0,
    NULL
};
/*
 * Button component pop callback function. 
 * In this example,the button can open the timer when it is released.
 */
void b0PopCallback(void *ptr)
{
    if(number_enable == 1)
    {
      digitalWrite(8, HIGH);
        tm0.enable();
        number_enable = 0;
        b0.setText("OFF");
    }
    else if (number_enable ==0)
    {
        tm0.disable();
        number_enable = 1;
        b0.setText("ON");
        digitalWrite(8, LOW);
    }
}

// Botão para aumentar tempo

void b1PopCallback(void *ptr)
{
    tm0.getCycle(&number_cycle);
    number_cycle = number_cycle + 100;
    memset(buffer, 0, sizeof(buffer));
    itoa(number_cycle, buffer, 10);
    
    tm0.setCycle(number_cycle);
    
    t1.setText(buffer);
    number_timer = number_cycle;
}

// Botão para diminuir tempo
void b2PopCallback(void *ptr)
{
    tm0.getCycle(&number_cycle);
    if (number_cycle > 100)
    {
        number_cycle = number_cycle - 100;
    }
    tm0.setCycle(number_cycle);
    memset(buffer, 0, sizeof(buffer));
    itoa(number_cycle, buffer, 10);
    t1.setText(buffer);
    number_timer = number_cycle;
}

//Definindo como o tempo será diminuido 

void tm0TimerCallback(void *ptr)
{
    if (number_timer >= 1){
      number_timer--;
      memset(buffer, 0, sizeof(buffer));
      itoa(number_timer, buffer, 10);
      t0.setText(buffer);
      digitalWrite(8, HIGH);
    }else{
      number_timer = 0;
      tm0.disable();
      number_enable = 1;
      b0.setText("ON");
      digitalWrite(8, LOW);
      t1.setText("0");
    }
}
void setup(void)
{    
    nexInit();                                     //Inicia a biblioteca Nextion
    b0.attachPop(b0PopCallback);                   //Inicia ou Para o Temporizador
    tm0.attachTimer(tm0TimerCallback);             //Diminui em 1 por segundo o temporizador e imprime na tela
    b1.attachPop(b1PopCallback);                   //Aumenta o valor do temporizador máximo
    b2.attachPop(b2PopCallback);                   //Diminui o valor do temporizador máximo

    Serial.begin(9600);                            //Inicializa comunicação serial em 9600 de baud rate
    therm.begin();                                 //Inicializa sensor de temperatura infravermelho
    therm.setUnit(TEMP_C);                         //Sensor infra vermelho dizendo que a temperatura será em graus C
    pinMode(5, OUTPUT);                            //LED TEMPERATURA ACIMA OU ABAIXO DO VALOR CONFIGURADO
    pinMode(6, OUTPUT);                            //LED TEMPERATURA ESTÁVEL
    pinMode(8, OUTPUT);                            //LED TEMPORIZADOR EM ANDAMENTO
/*
    pinMode(ledPin, OUTPUT);                           //LED EMERGENCIA
    pinMode(inPin, INPUT);                             //BOTÃO EMERGENCIA
*/  
    dbSerialPrintln("setup done"); 
}

void loop(void)
{   
/*
  // VERIFICAÇÃO SE O BOTÃO EMERGENCIA ESTA ATIVADO
  
    estadoBotao = digitalRead(inPin);
    if(estadoBotao == HIGH){
      digitalWrite(ledPin, HIGH);
    }else{
      digitalWrite(ledPin, LOW);
    }
*/
  // ADICIONA UM LISTNER PARA VER SE ALGUM BOTÃO DO TEMPORIZADOR É ACIONADO 
    
    nexLoop(nex_listen_list); // Ouvindo ação dos botões para temporizador
  
  //INICIA A LEITURA DO SENSOR INFRA-VERMELHO E FAZ A CONVERSÃO PARA A TELA
     therm.read();

   int TempO = (therm.object());
    
    disp_temp.setText(buffer2);

    tmpMax.getValue(&maxTemperature);
    temperaturaMaxima = maxTemperature;
    temperaturaMinima = temperaturaMaxima - 5;

    memset(buffer3, 0, sizeof(buffer3));
    itoa(temperaturaMaxima, buffer3, 10);
    memset(buffer4, 0, sizeof(buffer4));
    itoa(temperaturaMinima, buffer4, 10);

    t6.setText(buffer3);
    t13.setText(buffer4);

    
    
    //VERIFICAÇÃO SE A TEMPERATURA ESTA ENTRE A MAXIMA E A MINIMA DEFINIDA

    
    if(TempO > temperaturaMaxima || TempO < temperaturaMinima){
      digitalWrite(5, HIGH);
      digitalWrite(6, LOW);
    }else{
      digitalWrite(6, HIGH);   
      digitalWrite(5, LOW);   
    }

    memset(buffer2, 0, sizeof(buffer2));
    itoa(TempO, buffer2, 10);


  //DELAY PARA QUE O TEMPORIZADOR SIGA COM O CONTADOR DE 1 EM 1 SEGUNDO.
    delay(1000);
}

