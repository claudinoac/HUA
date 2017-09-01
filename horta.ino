// Carrega a biblioteca virtuabotixRTC
#include <virtuabotixRTC.h>

// Determina os pinos ligados ao modulo
// myRTC(clock, data, rst)
virtuabotixRTC myRTC(10, 9, 8);

const int H1 = 17; // hora medida 1
const int M1 = 07; // minuto medida 1

const int H2 = 17; // hora medida 2
const int M2 = 11; // minuto medida 2

const int UMIDADE_MIN = 80;

// para o sensor de umidade
// quantidades usadas como base de calculo para os mililitros de agua por irrigação
const int VOL_MIN = 200;
const int VOL_MAX = 1500;

const int SOIL_PIN = A0;
int umidade ;

// para iluminação
const int LAMPADA = 6;
const int HL = 17; // hora de ligar a lampada
const int ML = 07; // minuto de ligar a lampada
const int HD = 17; // hora de desligar a lampada
const int MD = 07; // minuto de desligar a lampada


// status de irrigação
const int LED_PISCA = 7;
const long intervalo = 200;
long ultimo_pisca;
int led_estado;

// para a valvula solenoide
const int VALV_PIN = 5;

// para o sensor de vazão
byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;
// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;
volatile byte pulseCount;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;

void setup()
{
  Serial.begin(9600);

  // para garantir valvula fechada
  digitalWrite(VALV_PIN, LOW);
  // define como saida
  pinMode(VALV_PIN, OUTPUT);

  digitalWrite(LED_PISCA, LOW);
  pinMode(LED_PISCA, OUTPUT);

  digitalWrite(LAMPADA, LOW);
 // FAZER: decidir se a lampada começa ligada ou desligada de acordo com horario
  pinMode(LAMPADA, OUTPUT);

  // sensor de vazão

  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

  // Informacoes iniciais de data e hora
  // Apos setar as informacoes, comente a linha abaixo
  // (segundos, minutos, hora, dia da semana, dia do mes, mes, ano)
  //myRTC.setDS1302Time(00, 22, 15, 5, 31, 8, 2017); //(tirar barra barra para ajustar horário)
}

void loop()
{
  // Le as informacoes do CI
  myRTC.updateTime();
  imprime_tempo();

  int hora = myRTC.hours;
  int minutos = myRTC.minutes;

  // verifica se chegou a hora de ler a umidade
  if ((hora == H1 && minutos == M1) || (hora == H2 && minutos == M2) ) {
    Serial.println("medindo...");
    // ler umidade
    umidade = analogRead(SOIL_PIN);
    // estabelecendo 0 e 550 como valor máximos captados pelos sensor e transformando em %
    umidade = map(umidade, 0, 550, 0, 100);
    imprime_umidade();

    // verifica se a umidade está abaixo do minimo
    if (umidade < UMIDADE_MIN ) {
      Serial.println("abrindo válvula...");
      digitalWrite(VALV_PIN, HIGH);

      // conta vazão
      zera_volume();
      irriga();

      Serial.println("fechando valvula...");
      digitalWrite(VALV_PIN, LOW);

    } // fim do if da umidade

  } // fim do if do horario de irrigar

 // para ligar a lampada
   if ((hora == HL && minutos == ML) ) {
    // liga lampada
    digitalWrite(LAMPADA,HIGH);
   } 

 // para desligar a lampada
   if ((hora == HD && minutos == MD) ) {
    // desliga lampada
    digitalWrite(LAMPADA,LOW);
   } 

  // para garantir que um minuto passou e não haverá um segundo disparo
  delay( 60000);
} // fim do loop


// funcoes auxiliares
////////////////////////////////////////////////////////

// sensor de vazão
// Interrupt Service Routine
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount = pulseCount + 1;
}

void zera_volume() {
  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;
}

void irriga() {
  int vol_irriga = map(umidade, 0, UMIDADE_MIN, VOL_MAX, VOL_MIN);

  Serial.print("volume a irrigar: ");
  Serial.println(vol_irriga);

  ultimo_pisca = millis();
  led_estado = HIGH;
  digitalWrite(LED_PISCA, led_estado);

  while (totalMilliLitres < vol_irriga) {
    if ((millis() - oldTime) > 1000)   // Only process counters once per second
    {
      // Disable the interrupt while calculating flow rate and sending the value to
      // the host
      detachInterrupt(sensorInterrupt);

      // Because this loop may not complete in exactly 1 second intervals we calculate
      // the number of milliseconds that have passed since the last execution and use
      // that to scale the output. We also apply the calibrationFactor to scale the output
      // based on the number of pulses per second per units of measure (litres/minute in
      // this case) coming from the sensor.
      flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;

      // Note the time this processing pass was executed. Note that because we've
      // disabled interrupts the millis() function won't actually be incrementing right
      // at this point, but it will still return the value it was set to just before
      // interrupts went away.
      oldTime = millis();

      // Divide the flow rate in litres/minute by 60 to determine how many litres have
      // passed through the sensor in this 1 second interval, then multiply by 1000 to
      // convert to millilitres.
      flowMilliLitres = (flowRate / 60) * 1000;

      // Add the millilitres passed in this second to the cumulative total
      totalMilliLitres += flowMilliLitres;

      imprime_vazao();

      // Reset the pulse counter so we can start incrementing again
      pulseCount = 0;

      // Enable the interrupt again now that we've finished sending output
      attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
    } // fim do if do tempo das medicoes

    // pisca led
    if (millis() - ultimo_pisca > intervalo) {
      ultimo_pisca = millis();
      if (led_estado == HIGH) {
        led_estado = LOW;
        digitalWrite(LED_PISCA, led_estado);
      } // fim do if HIGH
      else {
        led_estado = HIGH;
        digitalWrite(LED_PISCA, led_estado);
      } // fim do else
    } // fim do intervalo do led

  } // fim do while do volume

  // apaga o led ao sair
  led_estado = LOW;
  digitalWrite(LED_PISCA, led_estado);
} // fim do irriga

void imprime_vazao() {
  unsigned int frac;
  // Print the flow rate for this second in litres / minute
  Serial.print(totalMilliLitres);
  Serial.println("mL");
  Serial.print("Flow rate: ");
  Serial.print(int(flowRate));  // Print the integer part of the variable
  Serial.print(".");             // Print the decimal point
  // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
  frac = (flowRate - int(flowRate)) * 10;
  Serial.print(frac, DEC) ;      // Print the fractional part of the variable
  Serial.print("L/min");
  // Print the number of litres flowed in this second
  Serial.print("  Current Liquid Flowing: ");             // Output separator
  Serial.print(flowMilliLitres);
  Serial.print("mL/Sec");

  // Print the cumulative total of litres flowed since starting
  Serial.println("  Output Liquid Quantity: ");             // Output separator
}

void imprime_umidade() {
  Serial.print("Umidade : ");
  Serial.print(umidade);
  Serial.println("%");
}
void imprime_tempo() {
  // Imprime as informacoes no serial monitor
  Serial.print("Data : ");
  Serial.print(myRTC.dayofmonth);
  Serial.print("/");
  Serial.print(myRTC.month);
  Serial.print("/");
  Serial.print(myRTC.year);
  Serial.print("  ");
  Serial.print("Hora : ");
  // Adiciona um 0 caso o valor da hora seja <10
  if (myRTC.hours < 10)
  {
    Serial.print("0");
  }
  Serial.print(myRTC.hours);
  Serial.print(":");
  // Adiciona um 0 caso o valor dos minutos seja <10
  if (myRTC.minutes < 10)
  {
    Serial.print("0");
  }
  Serial.print(myRTC.minutes);
  Serial.print(":");
  // Adiciona um 0 caso o valor dos segundos seja <10
  if (myRTC.seconds < 10)
  {
    Serial.print("0");
  }
  Serial.println(myRTC.seconds);
}

