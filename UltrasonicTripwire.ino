#define TRIG_PIN 2
#define ECHO_PIN 8
#define DISTANCE_UNITS  INCHES

#define METERS          343         // Speed of sound:  m/s
#define CENTIMETERS     343 * 100   // Speed of sound: cm/s
#define FEET            1100        // Speed of sound: ft/s
#define INCHES          1100 * 12   // Speed of sound: in/s

#define IC1_EDGESEL_MSK B01000000
#define IC1_POSEDGE_MSK B01000000

void init_GPIO() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void init_PERIPHERALS_OFF() {
  PRR = B10000101;       //TWI:  Off   TIM2:   On   TIM0:  On    TIM1:   On
                         //SPI:  Off   USART:  On   ADC:   Off
}

void init_TIMER1() {
  cli();                 // Disable all interrupts

  TCCR1A = B00000000;    //COM1A:  N/A        COM1B: N/A      WGM1:  Normal
  TCCR1B = B11000101;    //ICNC1:  Enabled    ICES1: +Edge    WGM1:  Normal    CS1: clk/1024
  TCCR1C = B00000000;    //FOC1A:  N/A        FOC1B: N/A
  TCNT1  = 0;
  OCR1A  = 0;
  OCR1B  = 0;
  ICR1   = 0;
  TIMSK1 = B00100000;    //ICIE1:  Enabled  OCIE1B: N/A   OCIE1A: N/A   TOIE1:  N/A
  TIFR1  = B00000000;    //OCF1B:  N/A      OCF1A:  N/A   TOV1:   N/A

  sei();                 // Enable all interrupts
}

void init_TIMER2() {
  cli();                 // Disable all interrupts
  
  TCCR2A = B00000000;    //COM2A:  N/A    COM2B: N/A    WGM2:  Normal
  TCCR2B = B00000111;    //FOC2A:  N/A    FOC2B: N/A    WGM2:  Normal    CS2: clk/1024
  TCNT2  = 0;
  OCR2A  = 0;
  OCR2B  = 0;
  TIMSK2 = B00000001;    //OCIE2B: N/A   OCIE2A: N/A   TOIE2:  Enabled  
  TIFR2  = B00000000;    //OCF2B:  N/A   OCF2A:  N/A   TOV2:   N/A
  ASSR   = B00000000;
  GTCCR  = B00000000;

  sei();                 // Enable all interrupts
}

volatile uint16_t uiPrevICR1;
volatile uint16_t uiEchoCNT;
volatile bool bConvertDist;
ISR(TIMER1_CAPT_vect) {

  if ( TCCR1B & IC1_POSEDGE_MSK ) {
    // Store TIM1 counter value
    uiPrevICR1 = ICR1;
  } else {
    // Calculate ECHO pulse period
    uiEchoCNT =  (ICR1 - uiPrevICR1 - 1);
    bConvertDist = 1;
  }
  // Toggle IC Edge
  TCCR1B ^= IC1_EDGESEL_MSK;
}

volatile uint8_t ucCntT2 = 0;
ISR(TIMER2_OVF_vect) {
  ucCntT2++;
  // Sample Rate: 16MHz / 1024 / 255 / 3 = ~20 Hz
  if ( !(ucCntT2%3) ) {
    // Trigger Ultrasonic sensor with a 10us pulse.
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
  }
}

void setup() {

  // TODO: Remove all serial comms. Causes micro to freeze.

  // Initialize UART comms to 115200 baud
  Serial.begin(115200);
  // Print reset cause register
  Serial.println('\n');
  Serial.println(MCUSR);

  // Initialize peripherals
  init_GPIO();
  init_TIMER1();
  init_TIMER2();
  init_PERIPHERALS_OFF();

  Serial.println("Start of program.");
}

float fEchoPeriod;
float fDistance;
void loop() {

  // Convert Echo reading to Distance
  if ( bConvertDist ) {
    fEchoPeriod = (64 * uiEchoCNT) / (float) 1000000;
    fDistance = fEchoPeriod * DISTANCE_UNITS / (float) 2;
    Serial.println(fDistance);
    bConvertDist = 0;
  }

}
