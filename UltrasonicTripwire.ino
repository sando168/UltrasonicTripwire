#define ECHO_PIN 8
#define TEST_PIN LED_BUILTIN

#define IC1_EDGESEL_MSK B01000000;

void init_GPIO() {
  pinMode(TEST_PIN, OUTPUT);
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

ISR(TIMER1_CAPT_vect) {
  // Toggle IC Edge
  TCCR1B ^= IC1_EDGESEL_MSK;
  // Print Timer 1 value stored in ICR1
  Serial.println(ICR1);
}

volatile uint8_t cntT2 = 0;
volatile bool pinState;
ISR(TIMER2_OVF_vect) {
  cntT2++;
  // If 1 sec has passed,
  if ( !(cntT2%64) ) {
    // Toggle TEST_PIN to verify IC1 is working
    pinState = !pinState;
    digitalWrite(TEST_PIN, pinState);
  }
}

void setup() {
  // Initialize UART comms to 115200 baud
  Serial.begin(115200);
  // Print reset cause register
  Serial.println(MCUSR);
  // Initialize peripherals
  init_GPIO();
  Serial.println("Before init.");
  init_TIMER1();
  Serial.println("Init complete.");
  init_TIMER2();
}

void loop() {

}
