#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

// вывод D2 0-4MHz
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
    4000000 //0М..2М..4М
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
uint32_t freqShift = 0x0; // смещение для корректноцц работы переключения фильтра, лучше не использовать

void setup() {
  uart_init(); 
  timer_init();
  tmr = TCNT1;
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  // начальная  инициализация, поморгать светодиодами
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  delay(500);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
//  delay(500);
//  digitalWrite(A, HIGH);
//  digitalWrite(B, HIGH);
//  digitalWrite(C, HIGH);
//  digitalWrite(D, HIGH);
//  digitalWrite(E, HIGH);
//  delay(500);
//  digitalWrite(A, LOW);
//  digitalWrite(B, LOW);
//  digitalWrite(C, LOW);
//  digitalWrite(D, LOW);
//  digitalWrite(E, LOW);
}

void loop(){
  x = 0;
  while (x < 5){
    buf[x++] = uart_receive();
    if (TCNT1 - tmr > 600) // если на приём ушло много времени,
    x = 1; // ... то это первый байт пакета
    tmr = TCNT1; // обновим таймер
  }
  for (uint8_t i = 0; i < sizeof(freqs) / sizeof(freqs[0]); i++){
    freqMy=0x0;
    freqMy = buf[3];
    freqMy = freqMy  << 8;
    freqMy = freqMy | buf[2];
    freqMy = freqMy  << 8;
    freqMy = freqMy | buf[1];
    freqMy = freqMy  << 8;
    freqMy = freqMy | buf[0];
    if ( (freqMy + freqShift) <= freqs[i]){
      switch_bpf(i);
      break;
    }
  }
  show_tx(); // покажем работу на передачу
}
//Decoder
void switch_bpf(uint8_t n){
  if (n < 5){
    if (n == 0){
      digitalWrite(A, HIGH);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      digitalWrite(E, LOW);
    }
    if (n == 1){
      digitalWrite(A, LOW);
      digitalWrite(B, HIGH);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      digitalWrite(E, LOW);
    }
    if (n == 2){
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, HIGH);
      digitalWrite(D, LOW);
      digitalWrite(E, LOW);
    }
    if (n == 3){
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D,HIGH);
      digitalWrite(E, LOW);
    }
    if (n == 4){
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      digitalWrite(E, HIGH);
    }
  }
}
// инициализация COM-порта апаратного
inline void uart_init(void){
  UBRR0 = 0; // 1Mbps
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 бит данных, 1 стоп-бит, без контроля чётности
  UCSR0B = (1 << RXEN0); // разрешить прием данных
}
//*** Инициализация таймера 1, частота ~16 КГц ***//
inline void timer_init(void){
  //TCCR1A = 0; // режим CTC
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // fOSC/1024
  //OCR1A = 0xFFFF; // считать до упора
}
//*** Прием данных по UART ***//
unsigned char uart_receive(void){
  while (!(UCSR0A & (1 << RXC0)));
  return UDR0;
}
//*** Переключение RX/TX ***//
inline void show_tx(void){
  if (RXTX & TXFLAG){
    PORTB |= (1 << PB0);
  }else{
    PORTB &= ~(1 << PB0);
  }
}
