/* Sensor de umidade + monitor LCD
 *  preparado para oficina Design Rio
 *  Autor: Rainer Grassmann
 * adaptado de Saulo Jacques - testa o % de umidade e acende o LED quando abaixo de 40%
*/

//Carrega a biblioteca LiquidCrystal
#include <LiquidCrystal.h>
 
//Define os pinos que serão utilizados para ligação ao display
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int val_umid = 0; // sensor de umidade
int umidade=0;



    
void setup() {
  Serial.begin(9600);
//Define o número de colunas e linhas do LCD
  lcd.begin(16, 2); 
}

  void led(int state) {
  digitalWrite(7, state);
  }

void loop() {
 //Limpa a tela do LCD
  lcd.clear();  
  //Posiciona o cursor na coluna 3, linha 0;
  lcd.setCursor(3, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("HUA + OLABI");
delay(500);
  lcd.setCursor(1, 1);
  lcd.print("Horta Urb Auto");
  delay(1200);

  
  
  umidade = analogRead(val_umid);    
  umidade = constrain(umidade,0,630); //intervalo dos resultados brutos entre seca e úmido
  umidade = map(umidade,0,630,0,100); //convertendo os resultados em porcentagem de umidade (0 e 100%)

  Serial.println(umidade);

//Limpa a tela do LCD
  lcd.clear();
 lcd.setCursor(0, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("lendo umidade"); 
  delay(300);
  lcd.setCursor(13, 0);
  lcd.print("."); 
  delay(300);
  lcd.setCursor(13, 0);
  lcd.print(".."); 
  delay(300);
  lcd.setCursor(13, 0);
  lcd.print("..."); 
  delay(300);
  //Posiciona o cursor na coluna 6, linha 1;
  lcd.setCursor(6, 1);
  //Envia o texto entre aspas para o LCD
  lcd.print(umidade);
  lcd.setCursor(9, 1);
  lcd.print("%");
  delay(1500);
     
  if(umidade< 65) {
  led(HIGH);
//Limpa a tela do LCD
  lcd.clear();
  //Posiciona o cursor na coluna 3, linha 0;
  lcd.setCursor(2, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("umidade baixa");
  delay(500);
  lcd.setCursor(5, 1);
  lcd.print("REGAR");
  delay(300);
  lcd.setCursor(10, 1);
  lcd.print("!");
  } else   {
  led(LOW);
//Limpa a tela do LCD
  lcd.clear();
  //Posiciona o cursor na coluna 3, linha 0;
  lcd.setCursor(2, 0);
  //Envia o texto entre aspas para o LCD
  lcd.print("umidade boa");
  delay(500);
  lcd.setCursor(1, 1);
  lcd.print("PARAR DE REGAR");
  delay(300);
  lcd.setCursor(15, 1);
  lcd.print("!");
  
  
  }

delay(2600);

    }
