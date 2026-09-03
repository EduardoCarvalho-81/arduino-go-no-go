#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int ledVerde = 10;
const int ledVermelho = 9;
const int botao = 2;
const int buzzer = 6;

int vidas = 3;
int rodada = 0;
int acertos = 0;
int erros = 0;

int tempoResposta = 1500;

unsigned long melhorTempo = 99999;
unsigned long somaTempos = 0;

void aguardarInicio() {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("GO / NO-GO");

  lcd.setCursor(0, 1);
  lcd.print("Aperte Botao");

  while (digitalRead(botao) == HIGH) {
    delay(10);
  }

  tone(buzzer, 1500);
  delay(200);
  noTone(buzzer);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Preparar...");

  lcd.setCursor(0, 1);
  lcd.print("3");
  delay(1000);

  lcd.setCursor(0, 1);
  lcd.print("2");
  delay(1000);

  lcd.setCursor(0, 1);
  lcd.print("1");
  delay(1000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("INICIANDO");

  tone(buzzer, 2000);
  delay(500);
  noTone(buzzer);

  delay(1000);
}

void reiniciarJogo() {

  vidas = 3;
  rodada = 0;
  acertos = 0;
  erros = 0;

  tempoResposta = 1500;

  melhorTempo = 99999;
  somaTempos = 0;

  aguardarInicio();
}

void gameOver() {

  for (int i = 0; i < 3; i++) {

    digitalWrite(ledVermelho, HIGH);
    tone(buzzer, 300);

    delay(300);

    digitalWrite(ledVermelho, LOW);
    noTone(buzzer);

    delay(300);
  }

  lcd.clear();

  lcd.setCursor(2, 0);
  lcd.print("GAME OVER");

  lcd.setCursor(0, 1);
  lcd.print("Perdeu 3 vidas");

  delay(4000);

  reiniciarJogo();
}

void atualizarDificuldade() {

  if (rodada >= 2)
    tempoResposta = 1200;

  if (rodada >= 4)
    tempoResposta = 1000;

  if (rodada >= 6)
    tempoResposta = 800;

  if (rodada >= 8)
    tempoResposta = 600;
}

void atualizarLCD() {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("A:");
  lcd.print(acertos);

  lcd.print(" E:");
  lcd.print(erros);

  lcd.setCursor(0, 1);

  lcd.print("N:");

  if (tempoResposta == 1500)
    lcd.print("Facil");
  else if (tempoResposta == 1200)
    lcd.print("Medio");
  else if (tempoResposta == 1000)
    lcd.print("Dificil");
  else if (tempoResposta == 800)
    lcd.print("Expert");
  else
    lcd.print("Ninja");
}

void mostrarTempo(unsigned long tempo) {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Tempo:");

  lcd.setCursor(0, 1);
  lcd.print(tempo);
  lcd.print(" ms");

  delay(1500);
}

void mostrarResultadoFinal(float media) {

  while (true) {

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Acertos:");
    lcd.print(acertos);

    lcd.setCursor(0, 1);
    lcd.print("Erros:");
    lcd.print(erros);

    delay(3000);

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Melhor:");

    lcd.setCursor(0, 1);
    lcd.print(melhorTempo);
    lcd.print("ms");

    delay(3000);

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Media:");

    lcd.setCursor(0, 1);
    lcd.print(media);
    lcd.print("ms");

    delay(3000);

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Nivel:");

    lcd.setCursor(0, 1);

    if (media < 250 && acertos > 0)
      lcd.print("NINJA");
    else if (media < 350 && acertos > 0)
      lcd.print("PRO");
    else if (media < 500 && acertos > 0)
      lcd.print("AVANCADO");
    else if (media < 700 && acertos > 0)
      lcd.print("INTERMED");
    else
      lcd.print("INICIANTE");

    delay(3000);

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Segure Botao");

    lcd.setCursor(0, 1);
    lcd.print("3s Reiniciar");

    unsigned long inicioTela = millis();

    while (millis() - inicioTela < 3000) {

      if (digitalRead(botao) == LOW) {

        unsigned long inicioPress = millis();

        while (digitalRead(botao) == LOW) {

          if (millis() - inicioPress >= 3000) {

            tone(buzzer, 1500);
            delay(300);
            noTone(buzzer);

            reiniciarJogo();
            return;
          }
        }
      }
    }
  }
}

void setup() {

  pinMode(ledVerde, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(botao, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  randomSeed(analogRead(A0));

  aguardarInicio();
}

void loop() {

  atualizarDificuldade();
  atualizarLCD();

  if (vidas <= 0) {
    gameOver();
    return;
  }

  if (rodada >= 10) {

    float media = 0;

    if (acertos > 0)
      media = somaTempos / (float)acertos;

    tone(buzzer, 2000);
    delay(1000);
    noTone(buzzer);

    mostrarResultadoFinal(media);

    return;
  }

  delay(random(2000, 5000));

  bool verde = random(2);

  if (verde) {

    digitalWrite(ledVerde, HIGH);

    unsigned long inicio = millis();
    bool apertou = false;

    while (millis() - inicio < tempoResposta) {

      if (digitalRead(botao) == LOW) {

        unsigned long tempo = millis() - inicio;

        somaTempos += tempo;
        acertos++;

        if (tempo < melhorTempo)
          melhorTempo = tempo;

        mostrarTempo(tempo);

        apertou = true;
        break;
      }
    }

    digitalWrite(ledVerde, LOW);

    if (!apertou) {

      vidas--;
      erros++;

      tone(buzzer, 300);
      delay(500);
      noTone(buzzer);
    }

  } else {

    digitalWrite(ledVermelho, HIGH);

    bool erro = false;
    unsigned long inicio = millis();

    while (millis() - inicio < tempoResposta) {

      if (digitalRead(botao) == LOW) {

        vidas--;
        erros++;

        erro = true;

        tone(buzzer, 300);
        delay(500);
        noTone(buzzer);

        break;
      }
    }

    digitalWrite(ledVermelho, LOW);

    if (!erro)
      acertos++;
  }

  rodada++;

  delay(1000);
}
