
/* 
 *  Race management made with Arduino
 *    
 *  Sara Aguado Couselo - s.aguado.dev@gmail.com
 */

#include <LiquidCrystal.h>
#include "icons.h"                    // Iconos feitos por min

#define X 0                           // Eje X e Y (para usar como índices  
#define Y 1                           // en los siguientes arrays)

#define P1 0
#define P2 1

const bool MIN[] = {1,0};             // Posiciones mínima y máxima de la 
const byte MAX[] = {15,1};            // pantalla en cada uno de los ejes
const byte button = 7;                // Pin dixital para o pulsador 
const byte led[] = {8,9,10};
const byte sensor[] = {A0,A1};

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

unsigned long startTime[2];
unsigned long elapsedTime[2];
unsigned long currentTime[2];
unsigned long minTime[2];
int laps[2] = {0,0};

void setup() {
  pinMode(button, INPUT_PULLUP);      // Pulsador
  pinMode(sensor[P1], INPUT_PULLUP);
  pinMode(sensor[P2], INPUT_PULLUP);
  analogWrite(6,140);                 // Tensión contraste (axusta a luminosidade do lcd)

  lcd.createChar(1, number1);         // iConas
  lcd.createChar(2, number2);
  lcd.createChar(3, sideCarA);
  lcd.createChar(4, sideCarB);
  lcd.createChar(5, frontCarA);
  lcd.createChar(6, frontCarB);
  lcd.createChar(7, rightArrow);
  lcd.begin(16, 2);  
}

void loop() {
  title_screen();                     // Mostramos a pantalla de título ata
                                      // que o usuario pulse o botón.
  switch (select_game()) {            
    case 'R': race_mode(); break;     // Selección do tipo de xogo
    case 'T': time_trial(); break;
    default: print_screen('E'); break;
  }

  reset_game();
}

/* Resetea tódalas variables */
void reset_game() {
  startTime[P1]   = 0;
  startTime[P2]   = 0;
  elapsedTime[P1] = 0;
  elapsedTime[P2] = 0;
  currentTime[P1] = 0;
  currentTime[P2] = 0;
  minTime[P1]     = 0;
  minTime[P2]     = 0;
  laps[P1]        = 0;
  laps[P2]        = 0;
}

/*********************
 * PANTALLAS DO XOGO *
 *********************/

/* Pantalla de título */
void title_screen() {
  byte i = 0;
  
  lcd.clear();
  lcd.print("   SCALEXTRIC   ");
  
  while (!press_button(button)) {     // Ata que non se pulse o botón...

    if (i < 8) {                      // ...parpadean os iconos...
      print_icon(0,  0, 7);
      print_icon(1,  0, 7);
      print_icon(14, 0, 7);
      print_icon(15, 0, 7);
    }
    else {
      print_text(0,  0, " ");
      print_text(1,  0, " ");
      print_text(14, 0, " ");
      print_text(15, 0, " ");
    }
    
    print_icon(i,   1, 3);            // ...e movemos o coche.
    print_icon(i+1, 1, 4);
    delay(200);
    print_text(i,   1, " ");
    print_text(i+1, 1, " ");
    
    i = (i+2) % 16;
  }

  return;
}

/* Pantalla de selección de xogo */
char select_game() {
  const int NUM_XOGOS = 2;
  const char games[] = {'R','T'};
  const String names[] = {"Carreira       ","Contrarreloxo  "};
  int msec, i=0, t=0;
  
  lcd.clear();
  lcd.print("  MODO DE XOGO  ");
  print_text(0, 1, names[i]); 
  print_icon(15, 1, 7);

  while (true) {                      // Comprobamos a pulsación do botón
    msec = press_button(button);
    if (msec > 300) {                 // mantén pulsado 
      return games[i];
    } else if (msec > 0) {            // pulsa unha vez
      i = (i+1) % NUM_XOGOS;
      t = 0;
      print_text(0, 1, names[i]); 
      print_icon(15, 1, 7);
    }

    delay(1);                         
    if (++t == 500) {                 // cada 500ms parpadea
      t = 0;
      print_text(0, 1, "                ");
      delay(150);
      print_text(0, 1, names[i]); 
      print_icon(15, 1, 7);
    }
  }
}

/*** MODO CARREIRA ***/
void race_mode() {
  int laps = laps_number();
  summary_screen(laps);
  animated_transition();
  race(laps);
}

/* Selección do número de voltas */
int laps_number() {
  int msec, t=0, laps=10;
  
  lcd.clear();
  lcd.print(" NUM. DE VOLTAS ");
  print_text(0, 1, laps);
  print_icon(15, 1, 7);

  while (true) {                      // Comprobamos a pulsación do botón
    msec = press_button(button);      
    if (msec > 300) {                 // mantén pulsado 
      return laps;
    } else if (msec > 0) {            // pulsa unha vez
      laps = (laps % 90) + 10;
      t = 0;
      print_text(0, 1, laps);
      print_icon(15, 1, 7);
    }
    
    delay(1);
    if (++t == 500) {                 // cada 500ms parpadea
      t = 0;
      print_text(0, 1, "                ");
      delay(150);
      print_text(0, 1, laps);
      print_icon(15, 1, 7);
    }
  }
}

/* Resumo da selección */
void summary_screen(int laps) {
  int t=0;

  lcd.clear();
  lcd.print("Modo Carreira");
  print_icon(14, 0, 5);
  print_icon(15, 0, 6);
  print_text(0, 1, laps);
  lcd.print(" voltas");
  
  while (!press_button(button)) {     // Ata que non se pulse o botón...
    delay(1);
    if (++t == 500) {
      t = 0;
      print_text(15, 1, " ");
      delay(150);
      print_icon(15, 1, 7);
    }
  }
}

/* Borrando o texto cunha animación */
void animated_transition() {
  for (int i=0; i<20; i++) {
    print_icon(i-2, 0, 3);
    print_icon(i-1, 0, 4);
    print_icon(i,   1, 3);
    print_icon(i+1, 1, 4);  
    delay(150);
    print_text(i-2, 0, " ");
    print_text(i-1, 0, " ");    
    print_text(i,   1, " ");
    print_text(i+1, 1, " ");
  }
}

/* Marcador da carreira */
void race(int total_laps) {
  bool finish = false;
  
  lcd.clear();
  print_icon(0, 0, 1);
  print_icon(1, 0, 5);
  print_icon(2, 0, 6);
  lcd.print(" 0:0.0   v.0");
  print_icon(0, 1, 2);
  print_icon(1, 1, 5);
  print_icon(2, 1, 6);
  lcd.print(" 0:0.0   v.0");

  delay(2000);
  digitalWrite(led[0], HIGH); // Semáforo
  for (int i=1; i<3; i++) {
    delay(2000);
    digitalWrite(led[i-1], LOW);
    digitalWrite(led[i], HIGH);
  }

  startTime[P1] = startTime[P2] = millis();
  while (!finish && !press_button(button)) { // Pulsar o botón para saír
    if (check_sensor(P1) | check_sensor(P2)) {
      print_time(P1);
      print_time(P2);

      if (laps[P1] == total_laps || laps[P2] == total_laps)
        finish = true;
    }
  }

  int i = 2;
  while (finish && !press_button(button)) {
    digitalWrite(led[i], HIGH);
    delay(150);
    digitalWrite(led[i], LOW);
    i = (i+1)%3;
  }
}

/* Comproba a activación dun sensor e actúa en consecuencia */
bool check_sensor(byte player) {
  if (digitalRead(sensor[player]) == LOW) {
    currentTime[player] = millis();
    elapsedTime[player] = currentTime[player] - startTime[player];
    startTime[player]   = currentTime[player];
    
    if (elapsedTime[player] < minTime[player] || minTime[player] == 0) {
      minTime[player] = elapsedTime[player];
    }

    ++laps[player];
    wait(sensor[player], HIGH);
    return true;
  }  
  return false;
}

/* Actualiza as voltas e o tempo no display */
void print_time(byte player) {
  String str = String("");
  int minutes, seconds, milliseconds;
  
  seconds = minTime[player]/1000;
  minutes = seconds/60;
  str = str + minutes + ':' + seconds%60 + '.' + minTime[player]%1000;
  print_text(4, player, str);

  str = " ";
  str = str + "v." + laps[player];
  print_text(11, player, str);
}

/*** MODO CONTRARRELOXO ***/
void time_trial() {
  
}


/*********************
 * FUNCIÓNS DE AXUDA *
 *********************/

/* Escribe distintas mensaxes a pantalla completa */
void print_screen(char opt) {
  lcd.clear();
  
  switch (opt) {
    case 'S':
      lcd.print(" # SCALEXTRIC # ");
      lcd.setCursor(0,1);
      lcd.print(" by Sara Aguado ");
      break;
    default:
      lcd.print("   E R R O R    ");
      lcd.setCursor(0,1);
      lcd.print("  Press Reset   ");
      break;
  }
}

/* Escribe o texto na posición especificada */
void print_text(byte x, byte y, String message) {
  lcd.setCursor(x,y);
  lcd.print(message);
}
void print_text(byte x, byte y, int message) {
  lcd.setCursor(x,y);
  lcd.print(message);
}

/* Escribe a icona na posición especificada */
void print_icon(byte x, byte y, byte c) {
  lcd.setCursor(x,y);
  lcd.write(c);
}

/* Espera pola pulsación e depulsación dun botón (con pullup!) */
void wait_button() {
  wait(button, LOW);  // pulsado
  wait(button, HIGH); // depulsado
}

/* Comproba se está pulsado un botón e devolve o tempo que tardou en depulsarse */
int press_button(int button) {
  if (digitalRead(button) == LOW) { // está pulsado?
    return wait(button, HIGH); // espera a que se depulse
  }
  return 0;
}

/* Evita o rebote do sinal ao pulsar/depulsar o botón */
int wait(int button, int value) { 
  int msec = 1;
  while (true) {
    if (digitalRead(button) == value) {
      delay(50);
      if (digitalRead(button) == value) 
        return msec;
    }
    delay(1);
    msec++; // devolve o tempo en milisegundos que tardou en pulsarse/depulsarse o botón
  }
}
