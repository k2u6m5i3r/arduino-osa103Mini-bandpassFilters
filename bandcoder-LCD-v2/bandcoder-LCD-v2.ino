#define F_CPU 16000000UL

//#include <avr/io.h>
//#include <stdint.h>
//#include <avr/interrupt.h>

#include <LiquidCrystal.h>

// добавил вывод для частоты 0-2MHz
#define D7 7 
// вывод D2 2-4MHz
#define A 2
// вывод D3 4-8MHz
#define B 3
// вывод D4 8-16MHz
#define C 4
// вывод D5 16-32MHz
#define D 5
// вывод D6 >32MHz
#define E 6
// то что передача на ноге D8 или в коде PB0
// смотреть в функции show_tx()

const uint32_t freqs[] = { // верхние границы диапазонов
    2000000 //0M..2M
  , 4000000 //2М..4М
  , 8000000 //4М..8М
  ,16000000 //8М..16М
  ,32000000 //16М.32М
  ,0xFFFFFFFFUL // если выше - включаем bypass
};

uint8_t x; // индекс принятого байта в буфере
uint8_t buf[5]; // буфер приёма
uint16_t tmr; // предыдущее состояние таймера

#define FREQ (*(uint32_t*)buf)
#define RXTX (buf[4])
#define TXFLAG 0x80

uint32_t freqMy = 0x0;// переменная для хранения полученого числа
uint32_t freqShift = 500; // смещение для корректноцц работы переключения фильтра

boolean bufferComplete; // данные пришли?!

// Дисплей
// Установка дисплея
LiquidCrystal lcd(4, 5, 6, 7, 8, 9); // I used an odd pin combination because I need pin 2 and 3 for the interrupts.
void setup() {
// LCD 1602 инициализация
  lcd.begin(16, 2);                    // настройка дисплея
  // показать банер на дисплее
  showLCDBaner();
  Serial.begin(1e6);// инициализация со скоростью 1М 
  
//  pinMode(A, OUTPUT);
//  pinMode(B, OUTPUT);
//  pinMode(C, OUTPUT);
//  pinMode(D, OUTPUT);
//  pinMode(E, OUTPUT);
//  pinMode(D7, OUTPUT);
  
//  digitalWrite(A, LOW);
//  digitalWrite(B, LOW);
//  digitalWrite(C, LOW);
//  digitalWrite(D, LOW);
//  digitalWrite(E, LOW);
//  digitalWrite(D7, /* LOW */ /* HIGH */);

      lcd.setCursor(0, 0);
      lcd.print("              ");
      lcd.setCursor(0, 0);
      lcd.print(" All LOW  ");
      lcd.setCursor(0, 1);
      lcd.print("              "); 
}

void loop(){
  x=0;
  serialEvent();
  if (bufferComplete) {  //если есть доступные данные    
    for (uint8_t i = 0; i < sizeof(freqs) / sizeof(freqs[0]); i++){
      //lcd.print(buf[0],HEX);lcd.print(" ");lcd.print(buf[1],HEX);lcd.print(" ");lcd.print(buf[2],HEX);lcd.print(" ");lcd.print(buf[3],HEX);lcd.print(" ");lcd.print(buf[4],HEX);
      lcd.setCursor(0, 1);
      lcd.print("          ");
      lcd.setCursor(0, 1);
      freqMy=0x0;
      freqMy = buf[3];
      freqMy = freqMy  << 8;
      freqMy = freqMy | buf[2];
      freqMy = freqMy  << 8;
      freqMy = freqMy | buf[1];
      freqMy = freqMy  << 8;
      freqMy = freqMy | buf[0];
      lcd.print(freqMy,DEC);
      if ( (freqMy + freqShift) <= freqs[i]){
        switch_bpf(i);
        break;
      }
      bufferComplete = false;
    }
  }
  show_tx(); // покажем работу на передачу
}
void serialEvent() {
  while (Serial.available()) {
    buf[x++] = (uint8_t)Serial.read();
    if(x == 4){
     bufferComplete = true;
     x = 0;
    }
  }
}
//Decoder
void switch_bpf(uint8_t n){
  if (n < 5){
    lcd.setCursor(0, 0);
    lcd.print("                ");
    if (n == 0){
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("0-4 MHz");
//      digitalWrite(A, HIGH);
//      digitalWrite(B, LOW);
//      digitalWrite(C, LOW);
//      digitalWrite(D, LOW);
//      digitalWrite(E, LOW);
//      digitalWrite(D7, /* LOW */ /* HIGH */);
    }
    if (n == 1){
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("4-8 MHz");
//      digitalWrite(A, LOW);
//      digitalWrite(B, HIGH);
//      digitalWrite(C, LOW);
//      digitalWrite(D, LOW);
//      digitalWrite(E, LOW);
//      digitalWrite(D7, /* LOW */ /* HIGH */);
    }
    if (n == 2){
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("8-16 MHz");
//      digitalWrite(A, LOW);
//      digitalWrite(B, LOW);
//      digitalWrite(C, HIGH);
//      digitalWrite(D, LOW);
//      digitalWrite(E, LOW);
//      digitalWrite(D7, /* LOW */ /* HIGH */);
    }
    if (n == 3){
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("16-32 MHz");
//      digitalWrite(A, LOW);
//      digitalWrite(B, LOW);
//      digitalWrite(C, LOW);
//      digitalWrite(D,HIGH);
//      digitalWrite(E, LOW);
//      digitalWrite(D7, /* LOW */ /* HIGH */);
    }
    if (n == 4){
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print(">32 MHz all LOW");
//      digitalWrite(A, LOW);
//      digitalWrite(B, LOW);
//      digitalWrite(C, LOW);
//      digitalWrite(D, LOW);
//      digitalWrite(E, HIGH);
//      digitalWrite(D7, /* LOW */ /* HIGH */);
    }
  }
}
//*** Переключение RX/TX ***//
inline void show_tx(void){
  if (RXTX & TXFLAG) {
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print("peredacha");
    //PORTB |= (1 << PB0);
  }else{
    //PORTB &= ~(1 << PB0);
  }
}
void showLCDBaner(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("                ");
  delay(500);
}
