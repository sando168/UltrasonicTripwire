#define TRIG_PIN 2
#define ECHO_PIN 8

#define IC1_EDGESEL_MSK B01000000
#define IC1_POSEDGE_MSK B01000000

void init_GPIO() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void power_reduction() {
  // PRR = B10001101;       //TWI:  Off   TIM2:   On   TIM0:  On    TIM1:   On
  //                         //SPI:  Off   USART:  On   ADC:   Off
  // Serial.println(PRR);

  // TWI:   Disabled
  // TIM2:  Enabled
  // TIM0:  Enabled
  // TIM1:  Enabled
  // SPI:   Disabled
  // USART: Enabled
  // ADC:   Enabled
}

void init_TIMER1() {
  cli();

  TCCR1A = B00000000;    //COM1A:  N/A        COM1B: N/A      WGM1:  Normal
  TCCR1B = B11000101;    //ICNC1:  Enabled    ICES1: +Edge    WGM1:  Normal    CS1: clk/1024
  TCCR1C = B00000000;    //FOC1A:  N/A        FOC1B: N/A
  TCNT1  = 0;
  OCR1A  = 0;
  OCR1B  = 0;
  ICR1   = 0;
  TIMSK1 = B00100000;    //ICIE1:  Enabled  OCIE1B: N/A   OCIE1A: N/A   TOIE1:  N/A
  TIFR1  = B00000000;    //OCF1B:  N/A      OCF1A:  N/A   TOV1:   N/A

  sei();
}

void init_TIMER2() {
  cli();                 // disable all interrupts
  
  TCCR2A = B00000000;    //COM2A:  N/A    COM2B: N/A    WGM2:  Normal
  TCCR2B = B00000111;    //FOC2A:  N/A    FOC2B: N/A    WGM2:  Normal    CS2: clk/1024
  TCNT2  = 0;
  OCR2A  = 0;
  OCR2B  = 0;
  TIMSK2 = B00000001;    //OCIE2B: N/A   OCIE2A: N/A   TOIE2:  Enabled  
  TIFR2  = B00000000;    //OCF2B:  N/A   OCF2A:  N/A   TOV2:   N/A
  ASSR   = B00000000;
  GTCCR  = B00000000;

  sei();                  // enable all interrupts
}

volatile uint16_t uiPrevICR1;
volatile uint16_t uiEchoCNT;
ISR(TIMER1_CAPT_vect) {

  if ( TCCR1B & IC1_POSEDGE_MSK ) {
    // Store TIM1 counter value
    uiPrevICR1 = ICR1;
  } else {
    // Calculate ECHO pulse period
    uiEchoCNT =  (ICR1 - uiPrevICR1 - 1);
  }
  Serial.println(uiEchoCNT);

  // Toggle IC Edge
  TCCR1B ^= IC1_EDGESEL_MSK;
}

volatile uint8_t ucCntT2 = 0;
ISR(TIMER2_OVF_vect) {
  ucCntT2++;
  // Sample Rate: Every ~ 0.13 sec
  if ( !(ucCntT2%8) ) {
    // Trigger Ultrasonic sensor with a 10us pulse.
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
  }
}

void setup() {
  // Initialize UART comms to 115200 baud
  Serial.begin(115200);
  // Print reset cause register
  Serial.println(MCUSR);
  // Initialize peripherals
  init_GPIO();
  init_TIMER1();
  init_TIMER2();
}

void loop() {

}
