/* Sensor de umidade + monitor LCD
    preparado para oficina de introdução à automação de hortas - PLASTCOLAB
    Autor: Rainer Grassmann
   adaptado de Saulo Jacques - testa o % de umidade e acende o LED quando abaixo de 40%
*/

//Carrega a biblioteca LiquidCrystal
#include <LiquidCrystal.h>

//Define os pinos que serão utilizados para ligação ao display
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int val_umid = 0; // sensor de umidade
int umidade = 0;

void setup() {
  Serial.begin(9600);
  //Define o número de colunas e linhas do LCD
  lcd.begin(20, 4);
}

void led(int state) {
  digitalWrite(7, state);
}

void loop() {
  //Limpa a tela do LCD
  lcd.clear();
  //Posiciona o cursor na coluna 3, linha 0;
  lcd.setCursor(5, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("oficina HUA");
  delay(1200);
  lcd.setCursor(1, 1);
  lcd.print("MEDIDOR de UMIDADE");
  delay(2000);
  lcd.setCursor(5, 3);
  lcd.print("PLASTCOLAB");
  delay(2000);

  umidade = analogRead(val_umid);
  umidade = constrain(umidade, 0, 750); //intervalo dos resultados brutos entre seca e úmido
  umidade = map(umidade, 0, 741, 0, 100); //convertendo os resultados em porcentagem de umidade (0 e 100%)

  Serial.println(umidade);

  //Limpa a tela do LCD
  lcd.clear();
  lcd.setCursor(3, 1);
  //Envia o texto entre aspas para o LCD
  lcd.print("lendo umidade");
  delay(300);
  lcd.setCursor(16, 1);
  lcd.print(".");
  delay(300);
  lcd.setCursor(16, 1);
  lcd.print("..");
  delay(300);
  lcd.setCursor(16, 1);
  lcd.print("...");
  delay(300);
  //Posiciona o cursor na coluna 6, linha 1;
  lcd.setCursor(10, 2);
  //Envia o texto entre aspas para o LCD
  lcd.print(umidade);
  lcd.setCursor(13, 2);
  lcd.print("%");
  delay(1500);

  if (umidade < 65) {
    led(HIGH);
    //Limpa a tela do LCD
    lcd.clear();
    //Posiciona o cursor na coluna 3, linha 0;
    lcd.setCursor(4, 1);
    //Envia o texto entre aspas para o LCD
    lcd.print("umidade baixa");
    delay(500);
    lcd.setCursor(7, 2);
    lcd.print("REGAR");
    delay(300);
    lcd.setCursor(12, 2);
    lcd.print("!");
  } else   {
    led(LOW);
    //Limpa a tela do LCD
    lcd.clear();
    //Posiciona o cursor na coluna 3, linha 0;
    lcd.setCursor(4, 1);
    //Envia o texto entre aspas para o LCD
    lcd.print("umidade boa");
    delay(500);
    lcd.setCursor(3, 2);
    lcd.print("PARAR DE REGAR");
    delay(300);
    lcd.setCursor(17, 2);
    lcd.print("!");
  }
  delay(2600);
}
