
/* 
 *  Race management made with Arduino
 *    
 *  Sara Aguado Couselo - s.aguado.dev@gmail.com
 */

#include <LiquidCrystal.h>
#include "icons.h"                    // Iconos feitos por min

#define X 0                           // Eje X e Y (para usar como índices  
#define Y 1                           // en los siguientes arrays)

const bool MIN[] = {1,0};             // Posiciones mínima y máxima de la 
const byte MAX[] = {15,1};            // pantalla en cada uno de los ejes
const int button = 7;                 // Pin dixital para o pulsador 

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  pinMode(button, INPUT_PULLUP);      // Pulsador
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
  
  wait_button();                      // Para volver a la pantalla de título
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

    if (i < 8) {           // ...parpadean os iconos...
      print_char(0,  0, 7);
      print_char(1,  0, 7);
      print_char(14, 0, 7);
      print_char(15, 0, 7);
    }
    else {
      print_char(0,  0, 0);
      print_char(1,  0, 0);
      print_char(14, 0, 0);
      print_char(15, 0, 0);
    }
    
    print_char(i,   1, 3);            // ...e movemos o coche.
    print_char(i+1, 1, 4);
    delay(200);
    print_char(i,   1, 0);
    print_char(i+1, 1, 0);
    
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
  print_line(names[i], 7, 15, 1);

  while (true) {                      // Comprobamos a pulsación do botón
    msec = press_button(button);
    if (msec > 300) {                 // mantén pulsado 
      return games[i];
    } else if (msec > 0) {            // pulsa unha vez
      i = (i+1) % NUM_XOGOS;
      t = 0;
      print_line(names[i], 7, 15, 1);
    }

    delay(1);                         
    if (++t == 500) {                 // cada 500ms parpadea
      t = 0;
      print_line("                ", 0, 0, 1);
      delay(150);
      print_line(names[i], 7, 15, 1);
    }
  }
}

/*** MODO CARREIRA ***/
void race_mode() {
  int laps = laps_number();
  summary_screen(laps);
  animated_transition();
  race();
}

/* Selección do número de voltas */
int laps_number() {
  int laps=10, msec, t=0;

  lcd.clear();
  lcd.print(" NUM. DE VOLTAS ");
  print_line(laps, 7, 15, 1);

  while (true) {                      // Comprobamos a pulsación do botón
    msec = press_button(button);      
    if (msec > 300) {                 // mantén pulsado 
      return laps;
    } else if (msec > 0) {            // pulsa unha vez
      laps = (laps % 90) + 10;
      t = 0;
      print_line(laps, 7, 15, 1);
    }
    
    delay(1);
    if (++t == 500) {
      t = 0;
      print_line("                ", 0, 0, 1);
      delay(150);
      print_line(laps, 7, 15, 1);
    }
  }
}

/* Resumo da selección */
void summary_screen(int laps) {
  int t=0;

  lcd.clear();
  lcd.print("Modo Carreira");
  print_char(14, 0, 5);
  print_char(15, 0, 6);
  lcd.setCursor(0, 1);
  lcd.print(laps);
  lcd.print(" voltas");
  
  while (!press_button(button)) {     // Ata que non se pulse o botón...
    delay(1);
    if (++t == 500) {
      t = 0;
      print_char(15, 1, 0);
      delay(150);
      print_char(15, 1, 7);
    }
  }
}

/* Borrando o texto cunha animación */
void animated_transition() {
  for (int i=0; i<20; i++) {
    print_char(i-2, 0, 3);
    print_char(i-1, 0, 4);
    print_char(i,   1, 3);
    print_char(i+1, 1, 4);  
    delay(150);
    print_char(i-2, 0, 0);
    print_char(i-1, 0, 0);    
    print_char(i,   1, 0);
    print_char(i+1, 1, 0);  
  }
}

/* Marcador da carreira */
void race() {
  byte t1[] = {0,0,0}, t2[] = {0,0,0};
  
  lcd.clear();

  print_char(0, 0, 1);
  print_char(1, 0, 5);
  print_char(2, 0, 6);
  print_char(0, 1, 2);
  print_char(1, 1, 5);
  print_char(2, 1, 6);

  
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

/* Escribe frase/icona na liña inferior da pantalla */
void print_line(int message, byte c, byte x, byte y) {
  lcd.setCursor(0,y);
  lcd.print(message);
  print_char(x,y,c);
}
void print_line(String message, byte c, byte x, byte y) {
  print_char(x,y,c);
  lcd.setCursor(0,y);
  lcd.print(message);
}

/* Escribe el carácter en la posición especificada */
void print_char(byte x, byte y, byte c) {
  lcd.setCursor(x,y);
  if (c == 0) lcd.print(" ");
  else        lcd.write(c);
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
