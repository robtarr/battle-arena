// Colors
typedef struct color color;
struct color {
  int r,g,b;
};

const color red = {255, 0, 0};
const color green = {0, 255, 0};
const color white = {255, 255, 255};
const color yellow = {255, 255, 0};
const color orange = {255, 100, 0};
const color off = {0, 0, 0};

// Arena
typedef struct side side;
struct side {
  int pin_r, pin_g, pin_b, pin_reset_button;
}

const side left = {11, 10, 9, 4};
const side right = {6, 5, 3, 2};

// Match
typedef struct match match;
struct match {
  bool is_blue_ready, is_red_ready;
  unsigned long start_ts, end_ts;
  bool in_progress;
};

const unsigned long MATCH_WARN_LENGTH = 110000;

match m = {false, false};

bool teams_ready() {
  return ((m.is_blue_ready) && (m.is_red_ready));
}

void make_ready_blue() {
  m.is_blue_ready = true;
}

void make_ready_red() {
  m.is_red_ready = true;
}

void start_match() {
  pulse(3);
  color(green, left);
  color(green, right);
  m.in_progress = true;
  unsigned long now = millis();
  m.start_ts = now;
  m.end_ts = (now + MATCH_WARN_LENGTH);
}

void end_match() {
  pulse(5);
  color(orange, left);
  color(orange, right);
  pulse(5);
  color(red, left);
  color(red, right);
  delay(10000);
  reset_match();
}

void reset_match() {
  m.is_blue_ready = false;
  m.is_red_ready = false;
  m.start_ts = null;
  m.end_ts = null;
  m.in_progress = false;
  color(white, left);
  color(white, right);
}

void pulse(times) {
  // Run pulse effect on the current colors
}

void color(color c, side s) {
  analogWrite(side.pin_r, c.r);
  analogWrite(side.pin_g, c.g);
  analogWrite(side.pin_b, c.b);
}

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);

  // initialize the digital pin as an output.
  pinMode(left.pin_r, OUTPUT);
  pinMode(left.pin_g, OUTPUT);
  pinMode(left.pin_b, OUTPUT);
  pinMode(right.pin_r, OUTPUT);
  pinMode(right.pin_g, OUTPUT);
  pinMode(right.pin_b, OUTPUT);

  pinMode(left.pin_reset_button, INPUT_PULLUP);
  pinMode(right.pin_reset_button, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(left.pin_reset_button), make_ready_red, LOW);
  attachInterrupt(digitalPinToInterrupt(right.pin_reset_button), make_ready_blue, LOW);

  reset_match();
}

void loop() {
  if (m.in_progress) {
    unsigned long now = millis();
    if(now >= m.end_ts) {
      end_match();
    }
  } else {
    if (teams_ready()) {
      start_match();
    }
  }
}
