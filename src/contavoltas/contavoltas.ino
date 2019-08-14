
/* 
 *  Race management made with Arduino
 *    
 *  Sara Aguado Couselo - s.aguado.dev@gmail.com
 */

#include <LiquidCrystal.h>
#include "icons.h"

#define X 0                           // Eje X e Y (para usar como índices)
#define Y 1 

const bool MIN[] = {1,0};             // Posiciones mínima y máxima del mono 
const byte MAX[] = {15,1};            // en cada uno de los ejes

const int button = 7;                 // Pin digital para el pulsador del Joystick
const int X_pin  = 0;                 // Pin analógico para el eje X del Joystick
const int Y_pin  = 1;                 // Pin analógico para el eje Y del Joystick

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

byte t[] = {0,0};                     // Tiempo restante
byte monkey_pos[] = {1,1};            // Posición del mono
byte traps_pos[15];                   // Posición de las trampas
bool bananas_pos;                     // Posición de los plátanos y el tigre

void setup() {
  analogWrite(6,140);                 // Genera tensión contraste en pin 6
  lcd.createChar(1, number1);
  lcd.createChar(2, number2);
  lcd.createChar(3, sideCarA);
  lcd.createChar(4, sideCarB);
  lcd.createChar(5, frontCarA);
  lcd.createChar(6, frontCarB);
  lcd.createChar(7, trafficLightRed);
  lcd.createChar(8, trafficLightAmber);
  lcd.createChar(9, trafficLightGreen);
  lcd.begin(16, 2);
  
  pinMode(button, INPUT_PULLUP);      // Pulsador del Joystick
}

void title_screen() {
  bool pos = 0;
  byte i = 0;
  
  lcd.clear();
  lcd.print("   SCALEXTRIC   ");
  
  while (!press_button(button)) {

    if (i == 0 || i == 8) {
        pos = !pos;
        print_char(pos,     0, 9);
        print_char(!pos,    0, 8);
        print_char(14+pos,  0, 9);
        print_char(14+!pos, 0, 8);
    }
    
    print_char(i,   1, 3);
    print_char(i+1, 1, 4);
    delay(1500/8);
    print_char(i,   1, 0);
    print_char(i+1, 1, 0);
    
    i = (i+2) % 16;
  }
}

void loop() {
  title_screen();
  press_start();                      // "Press Joystick"
  
  start_game();
  
  while (decrement()) {               // Mientras se pueda decrementar el contador...
    move_traps();                     // cambia de sitio las trampas
    for (int i=0; i<8; i++) {         // y cada 1/8 de segundo refresca la pantalla.
      if (game_over()) {              // Si se acaba la partida...
        return press_start();         // esperamos a que pulse y volvemos a empezar
      }
      delay(1000/8);
    }
  }
  
  print_message('T');                 // Se acabó el tiempo!
  press_start();                      // Para volver a la pantalla de título
}

/* Prepara una nueva partida */
void start_game() {
  lcd.clear();                        // Limpiamos la pantalla
  
  t[0]=3; t[1]=1;                     // Reseteamos el tiempo
  
  monkey_pos[X] = 1;                  // Pintamos el mono en su sitio
  monkey_pos[Y] = 1;
  print_monkey();
  
  print_bananas();                    // Y los plátanos/tigre
}

/* Decrementa el reloj y devuelve 'false' cuando se acaba el tiempo */
bool decrement() {  
  if (--t[1] == 255) {
    t[1] = 9;
    t[0]--;
  }

  lcd.setCursor(0,0);                 // Mostramos el tiempo por pantalla
  lcd.print(t[0]);
  lcd.setCursor(0,1);
  lcd.print(t[1]);
  
  if (t[0] + t[1] == 0) return false;
  else                  return true;
}

/* Cambia la posición de las trampas de forma aleatoria */
void move_traps() {
  for (int i=0; i<15; i++) {
    if (i != 0 && i != 1 && i != 3 && i != 6 && i != 10) {      

      print_char(i, traps_pos[i], 0);     // Borramos la trampa
      traps_pos[i] = random(2);           // Cambio aleatorio
      print_char(i, traps_pos[i], 2);     // La dibujamos en la nueva posición   
      print_monkey();                     // Repintamos el mono
        
    }
    else traps_pos[i] = -1;
  }
}

/*  Mueve al mono en función de la entrada del Joystick Analógico. 
 *  Devuelve 'true' cuando el mono llega a la última columna (se acaba la partida).
 */
bool game_over() {
  int val;
  
  val = analogRead(X_pin);
  if (val < 412) return move_monkey(X, -1);
  if (val > 612) return move_monkey(X,  1);
 
  val = analogRead(Y_pin);
  if (val < 412) return move_monkey(Y, -1);
  if (val > 612) return move_monkey(Y,  1);

  return false; // El mono no se ha movido (no se ha acabado el juego)
}

/* Mueve el mono en la direccción especificada */
bool move_monkey(bool axis, int dir) {
  if (trapped()) return false;                            // Si está atrapado no se puede mover
  print_char(monkey_pos[X], monkey_pos[Y], 0);            // Si no lo está, lo borramos
  
  monkey_pos[axis] += dir;
  monkey_pos[axis] = monkey_pos[axis] < MIN[axis] || monkey_pos[axis] == 255 ? MIN[axis] : monkey_pos[axis];
  monkey_pos[axis] = monkey_pos[axis] > MAX[axis]                            ? MAX[axis] : monkey_pos[axis];

  if (monkey_pos[X] == 15) {                              // Si se mueve a la columna 15...
    if (monkey_pos[Y] == bananas_pos) print_message('H'); 
    else                              print_message('D'); 
    return true;                                          // Se acaba la partida
  }
  else {
    print_monkey();                                       // Lo pintamos en la nueva posición
    return false;
  }
}

/* Indica si el mono está atrapado */
bool trapped() {
  return traps_pos[monkey_pos[X]] == monkey_pos[Y];
}

/* Pinta el mono en función de su posición */
void print_monkey() {
  if (trapped()) {
    print_char(monkey_pos[X], monkey_pos[Y], 5);    // atrapado
    print_bananas();                                // actualizamos la posición de los plátanos
  }
  else {
    print_char(monkey_pos[X], monkey_pos[Y], 1);    // libre
  }
}

/* Pinta el tigre donde está el mono y los plátanos en el lado opuesto */
void print_bananas() {
  bananas_pos = !monkey_pos[Y];
  print_char(15, bananas_pos, 4);       // plátanos
  print_char(15, !bananas_pos, 3);      // tigre
}

/* Escribe el carácter en la posición especificada */
void print_char(byte x, byte y, byte c) {
  lcd.setCursor(x, y);
  if (c == 0) lcd.print(" ");
  else        lcd.write(c);
}

/* Escribe el mensaje elegido a pantalla completa */
void print_message(char opt) {
  lcd.clear();
  
  switch (opt) {
    case 'S':
      lcd.print(" # SCALEXTRIC # ");
      lcd.setCursor(0,1);
      lcd.print(" by Sara Aguado ");
      break;
    case 'H':
      lcd.print("   H A P P Y    ");
      lcd.setCursor(0,1);
      lcd.print("  M O N K E Y   ");
      break;
    case 'T':
      lcd.print(" T R A P P E D  ");
      lcd.setCursor(0,1);
      lcd.print("  M O N K E Y   ");
      break;
    case 'D':
      lcd.print("    D I E D     ");
      lcd.setCursor(0,1);
      lcd.print("  M O N K E Y   ");
      break;
    default:
      lcd.print("   E R R O R    ");
      lcd.setCursor(0,1);
      lcd.print("  Press Reset   ");
      break;
  }
}

void press_start() {
  wait_button(button, LOW);  // pulsador
  wait_button(button, HIGH); // depulsado
}

bool press_button(int button) {
  
}

/* Asegura que el botón está pulsado/depulsado */
void wait_button(int button, int value) { 
  while (true) {
    if (digitalRead(button) == value) {
      delay(50);
      if (digitalRead(button) == value) 
        return; /* Necesario por el rebote */
    }
  }
}
