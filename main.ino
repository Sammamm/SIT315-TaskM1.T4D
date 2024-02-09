//**************************************************//
//  the comments underline the updates in the code  //
//**************************************************//

const uint8_t BTN_PIN = 2;
const uint8_t LED_PIN = 13;
const uint8_t MOTION_PIN = 3;

// define motion pin for external interrupts
const uint8_t MOTION_PIN_EXT = 5;

// temp sensor is conntected to analog pin 0
const uint8_t TEMP_PIN = 0;

// initiate a comparer match variable
int timer1_compare_match;

uint8_t buttonPrevState = LOW;
uint8_t ledState = LOW;

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(MOTION_PIN, INPUT);

  // set the pin to read input
  pinMode(MOTION_PIN_EXT, INPUT_PULLUP);


  pinMode(LED_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BTN_PIN), toggleBtn, CHANGE);
  attachInterrupt(digitalPinToInterrupt(MOTION_PIN), toggleMotion, CHANGE);

  // activate the PORTD by setting it to 1.
  PCICR |= B00000100;

  // set the MOTION_PIN_EXT as output by bitwise operations
  //0 - output
  //1 - input
  PCMSK2 |= (1 << MOTION_PIN_EXT);


  // Disable interups
  noInterrupts();

  // Intiate timer 1
  TCCR1A = 0;
  TCCR1B = 0;

  // calculated compare value for timer to tick after every second
  timer1_compare_match = 31249;

  TCNT1 = timer1_compare_match;


  // Set prescaler to 256
  TCCR1B |= (1 << CS12);

  // Enable timer interrupt for compare mode
  TIMSK1 |= (1 << OCIE1A);

  interrupts();
  // Enable interrupts

  Serial.begin(9600);
}

uint8_t buttonState;
uint8_t motionValue;

void loop() {
}

ISR(TIMER1_COMPA_vect) {
  // Preload timer with compare value
  TCNT1 = timer1_compare_match;

  // calculate temprature
  int reading = analogRead(TEMP_PIN);

  float voltage = reading * 4.68;
  voltage /= 1024.0;
  float temp = (voltage - 0.5) * 100;

  // print temprature to serial moniter
  Serial.print("Temprature: ");
  Serial.print(temp);
  Serial.println(" C");
}


// set up ISR (Interrupt Service Routine) for PORT D
ISR(PCINT2_vect) {
  // reads the motion value
  motionValue = digitalRead(MOTION_PIN_EXT);

  // motion detected => LED ON
  if (motionValue == true) {
    digitalWrite(LED_PIN, true);
    Serial.println("LED ON");
  }
  // no motion => LED OFF
  else {
    digitalWrite(LED_PIN, false);
    Serial.println("LED OFF");
  }
}


void toggleBtn() {
  buttonState = digitalRead(BTN_PIN);

  if (buttonState != buttonPrevState) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    Serial.println("LED SWITCHED");
  }
}

void toggleMotion() {
  motionValue = digitalRead(MOTION_PIN);

  if (motionValue == true) {
    digitalWrite(LED_PIN, true);
    Serial.println("LED ON");
  } else {
    digitalWrite(LED_PIN, false);
    Serial.println("LED OFF");
  }
}
