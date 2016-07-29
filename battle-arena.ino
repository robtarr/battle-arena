int red_1 = 11;
int green_1 = 10;
int blue_1 = 9;
int button_1 = 4;
bool ready_1 = false;

int red_2 = 6;
int green_2 = 5;
int blue_2 = 3;
int button_2 = 2;
bool ready_2 = false;

unsigned long start_time = 0;
unsigned long end_time = 0;
unsigned long end_warning_time = 0;
unsigned long reset_time = 0;
unsigned long now;

const unsigned long BOTH_READY = 5000;
const unsigned long MATCH_LENGTH = 120000;
const unsigned long END_WARNING = 10000;

const unsigned long RESET = 5000;

void red(int r, int g, int b) {
  analogWrite(r, 255);
  analogWrite(g, 0);
  analogWrite(b, 0);
}

void green(int r, int g, int b) {
  analogWrite(r, 0);
  analogWrite(g, 255);
  analogWrite(b, 0);
}

void blue(int r, int g, int b) {
  analogWrite(r, 0);
  analogWrite(g, 0);
  analogWrite(b, 255);
}

void white(int r, int g, int b) {
  analogWrite(r, 255);
  analogWrite(g, 255);
  analogWrite(b, 255);
}

void yellow(int r, int g, int b) {
  analogWrite(r, 255);
  analogWrite(g, 255);
  analogWrite(b, 0);
}

void orange(int r, int g, int b) {
  analogWrite(r, 255);
  analogWrite(g, 100);
  analogWrite(b, 0);
}

void off(int r, int g, int b) {
  analogWrite(r, 0);
  analogWrite(g, 0);
  analogWrite(b, 0);
}

void all_ready() {
  for (int i = 0; i < 3; i++) {
    for (int c = 256; c > 0; c--) { 
      analogWrite(blue_1, c);
      analogWrite(red_2, c);
      delay(5);
    }
  }
}

// the setup routine runs once when you press reset:
void setup() {
  analogWrite(A0, HIGH);
  analogWrite(A1, HIGH);
  
  // initialize the digital pin as an output.
  pinMode(red_1, OUTPUT);
  pinMode(green_1, OUTPUT);
  pinMode(blue_1, OUTPUT);
  pinMode(red_2, OUTPUT);
  pinMode(green_2, OUTPUT);
  pinMode(blue_2, OUTPUT);

  pinMode(button_1, INPUT_PULLUP);
  pinMode(button_2, INPUT_PULLUP);

  white(red_1, green_1, blue_1);
  white(red_2, green_2, blue_2);

  Serial.begin(9600);
  Serial.println("Starting");
}


void start() {
  Serial.println("Go!!!");
  green(red_2, green_2, blue_2);
  green(red_1, green_1, blue_1);
  start_time = 0;

  end_time = millis() + MATCH_LENGTH;
  end_warning_time = end_time - END_WARNING;
}

void end_match() {
  Serial.println("Time");
  red(red_1, green_1, blue_1);
  red(red_2, green_2, blue_2);
  end_time = 0;

  reset_time = millis() + RESET;
}

void end_warning() {
  Serial.println("Match almost over");
  end_warning_time = 0;
  
  for (int i = 0; i < 5; i++) {
    green(red_1, green_1, blue_1);
    green(red_2, green_2, blue_2);
    delay(800);
    off(red_1, green_1, blue_1);
    off(red_2, green_2, blue_2);
    delay(200);
  } 
  for (int i = 0; i < 5; i++) {
    orange(red_1, green_1, blue_1);
    orange(red_2, green_2, blue_2);
    delay(800);
    off(red_1, green_1, blue_1);
    off(red_2, green_2, blue_2);
    delay(200);
  }
}

void reset() {
  Serial.println("Reset");
  white(red_1, green_1, blue_1);
  white(red_2, green_2, blue_2);
  ready_1 = false;
  ready_2 = false;
  reset_time = 0;
}

// the loop routine runs over and over again forever:
void loop() {
  if (digitalRead(button_1) == LOW) {
    ready_1 = true;
    blue(red_1, green_1, blue_1);
    Serial.println("Ready Player 1");
  }

  if (digitalRead(button_2) == LOW) {
    ready_2 = true;
    red(red_2, green_2, blue_2);
    Serial.println("Ready Player 2");
  }

  if (ready_1 && ready_2) {
    ready_1 = false;
    ready_2 = false;
    start_time = millis() + BOTH_READY;
    delay(1000);
    all_ready();
  }

  now = millis();
  delay(1000);

  if (start_time != 0 && now >= start_time) { start(); }
  if (end_warning_time != 0 && now >= end_warning_time) { end_warning(); }
  if (end_time != 0 && now >= end_time) { end_match(); }
  if (reset_time != 0 && now >= reset_time) { reset(); }
}
