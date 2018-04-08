/*
  A seven segement display counter

  Counts the number of times a button is pressed and displays this number on a 7 segment display

  Nicolas Massé
*/

/*
 * The button is connected to PIN 2.
 * 
 * To prevent any rebound issue, filter the button signal with a low pass filter (R = 240 ohms, C = 6.8 micro farad)
 */
#define BUTTON_PIN 2

/*
 * The seven segment display has a common cathode for each digit
 */
#define CATHODE_1000_PIN 1
#define CATHODE_100_PIN 13
#define CATHODE_10_PIN 3
#define CATHODE_1_PIN 4
// The splitter is the colon between the second and third digit
#define CATHODE_SPLITTER_PIN 0

/*
 * The anodes will light up a segment when it is LOW and the cathode is HIGH.
 * 
 * The anode numbering refers to the standard numbering:
 * https://en.wikipedia.org/wiki/Seven-segment_display
 */
#define ANODE_A_PIN 11
#define ANODE_B_PIN 10
#define ANODE_C_PIN 9
#define ANODE_D_PIN 8
#define ANODE_E_PIN 7
#define ANODE_F_PIN 6
#define ANODE_G_PIN 5

/*
 * The number of milliseconds to wait in order to let the retinal persistence do its job. 
 * See https://en.wikipedia.org/wiki/Persistence_of_vision
 */
#define DELAY 5

/*
 * Set this in order to see how the display multiplexing work
 */
//#define DEBUG

#ifdef DEBUG
#define DELAY_DEBUG delay(50);
#define DELAY 500
#else
#define DELAY_DEBUG 
#endif

// The state of each anode for each number between 0 and 9
#define ANODE_A_STATE(x) (x != 4 && x != 1 ? LOW : HIGH)
#define ANODE_B_STATE(x) (x != 5 && x != 6 ? LOW : HIGH)
#define ANODE_C_STATE(x) (x != 2 ? LOW : HIGH)
#define ANODE_D_STATE(x) (x != 1 && x != 4 && x != 7 ? LOW : HIGH)
#define ANODE_E_STATE(x) (x != 0 && x != 2 && x != 6 && x != 8 ? HIGH : LOW)
#define ANODE_F_STATE(x) (x != 1 && x != 2 && x != 3 && x != 7 ? LOW : HIGH)
#define ANODE_G_STATE(x) (x != 0 && x != 1 && x != 7 ? LOW : HIGH)

// All the cathodes
const int cathodes[] = {CATHODE_1000_PIN, CATHODE_100_PIN, CATHODE_10_PIN, CATHODE_1_PIN, CATHODE_SPLITTER_PIN};

// All the anodes
const int anodes[] = {ANODE_A_PIN, ANODE_B_PIN, ANODE_C_PIN, ANODE_D_PIN, ANODE_E_PIN, ANODE_F_PIN, ANODE_G_PIN};

void setup() {
  // cathodes as output with default to LOW
  for (int i = 0; i < sizeof(cathodes)/sizeof(cathodes[0]); i++) {
    pinMode(cathodes[i], OUTPUT);
    digitalWrite(cathodes[i], LOW);
  }
  // anodes as output with default to HIGH
  for (int i = 0; i < sizeof(anodes)/sizeof(anodes[0]); i++) {
    pinMode(anodes[i], OUTPUT);
    digitalWrite(anodes[i], HIGH);
  }
  
  // Enable the button and register an interrupt
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_interrupt, RISING);
}

// the counter is later incremented by an interrupt
volatile int counter = 0;


void button_interrupt() {
  counter++;
}

void loop() {
  int n = counter;

  // Base 10 conversion
  int n1 = n % 10;
  int n10 = ((n - n1) % 100) / 10;
  int n100 = ((n - n1 - n10 * 10) % 1000) / 100;
  int n1000 = ((n - n1 - n10 * 10 - n100 * 100) % 10000) / 1000;

  int digits[] = { n1000, n100, n10, n1 };
  for (int c = 0; c < 4; c++) {
    int digit = digits[c];
    digitalWrite(cathodes[c], HIGH);
    DELAY_DEBUG
    digitalWrite(ANODE_A_PIN, ANODE_A_STATE(digit));
    DELAY_DEBUG
    digitalWrite(ANODE_B_PIN, ANODE_B_STATE(digit));
    DELAY_DEBUG
    digitalWrite(ANODE_C_PIN, ANODE_C_STATE(digit));
    DELAY_DEBUG
    digitalWrite(ANODE_D_PIN, ANODE_D_STATE(digit));
    DELAY_DEBUG
    digitalWrite(ANODE_E_PIN, ANODE_E_STATE(digit));
    DELAY_DEBUG
    digitalWrite(ANODE_F_PIN, ANODE_F_STATE(digit));
    DELAY_DEBUG
    digitalWrite(ANODE_G_PIN, ANODE_G_STATE(digit));
    delay(DELAY);
    digitalWrite(cathodes[c], LOW);
    DELAY_DEBUG
  }
}

