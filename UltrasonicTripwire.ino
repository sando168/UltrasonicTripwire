volatile uint8_t cntT2 = 0;

void power_reduction() {

  // PRR = B10001101;       //TWI:  Off   TIM2:   On   TIM0:  On    TIM1:   Off
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

void init_TIMER2() {
  // check if timer2 works by printing to Serial Monitor
  cli();                 // disable all interrupts
  
  TCCR2A = B00000000;    //COM2A:  N/A    COM2B: N/A    WGM2:  Normal
  TCCR2B = B00000111;    //FOC2A:  N/A    FOC2B: N/A    WGM2:  Normal    CS0: clk/1024
  TCNT2  = 0;
  OCR2A  = 0;
  OCR2B  = 0;
  TIMSK2 = B00000001;    //OCIE2B: N/A   OCIE2A: N/A   TOIE2:  Enabled  
  TIFR2  = B00000000;    //OCF2B:  N/A   OCF2A:  N/A   TOV2:   N/A
  ASSR   = B00000000;
  GTCCR  = B00000000;

  Serial.println(TCCR2A);
  Serial.println(TCCR2B);
  Serial.println(TCNT2);
  Serial.println(OCR2A);
  Serial.println(OCR2B);
  Serial.println(TIMSK2);
  Serial.println(TIFR2);
  Serial.println(ASSR);
  Serial.println(GTCCR);

  sei();                  // enable all interrupts
}

ISR(TIMER2_OVF_vect) {
  cntT2++;
  if (!(cntT2%8)) {
    Serial.println("Tick...");
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Before init.");
  init_TIMER2();
  Serial.println("Init complete.");
}

void loop() {
  // put your main code here, to run repeatedly:
  // TODO: Check for any reset causes
}
