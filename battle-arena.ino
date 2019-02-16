// Colors
typedef struct color color;
struct color {
  int r,g,b;
};

const color red = {255, 0, 0};
const color blue = {0, 0, 255};
const color green = {0, 255, 0};
const color white = {255, 255, 255};
const color yellow = {255, 255, 0};
const color orange = {255, 100, 0};
const color off = {0, 0, 0};

// Arena
typedef struct side side;
struct side {
  int pin_r, pin_g, pin_b, pin_reset_button;
};
bool discoMode = false;

const side left = {11, 10, 9, 2};
const side right = {6, 5, 4, 3};

// Match
typedef struct match match;
struct match {
  bool is_blue_ready, is_red_ready;
  unsigned long start_ts, end_ts;
  bool in_progress;
};

// const unsigned long MATCH_WARN_LENGTH = 110000; // tournament length
const unsigned long MATCH_WARN_LENGTH = 35000;  // demo length

match m = {false, false};

bool teams_ready() {
  return ((m.is_blue_ready) && (m.is_red_ready));
}

void set_color(color c, side s) {
  analogWrite(s.pin_r, c.r);
  analogWrite(s.pin_g, c.g);
  analogWrite(s.pin_b, c.b);
}

void make_ready_blue() {
  if (!m.is_blue_ready) {
    Serial.println("Blue ready");
  }
  m.is_blue_ready = true;
  set_color(blue, left);
}

void make_ready_red() {
  if (!m.is_red_ready) {
    Serial.println("Red ready");
  }
  m.is_red_ready = true;
  set_color(red, right);
}

void start_match() {
  Serial.println("Start match!");
  pulse(3, blue, red);

  pulse(3, green, green);

  set_color(green, left);
  set_color(green, right);

  m.in_progress = true;
  unsigned long now = millis();
  m.start_ts = now;
  m.end_ts = (now + MATCH_WARN_LENGTH);
}

void end_match() {
  Serial.println("End Match");
  pulse(5, yellow, yellow);

  pulse(5, orange, orange);

  set_color(red, left);
  set_color(red, right);
  delay(10000);
  reset_match();
};

void reset_match() {
  Serial.println("Reset Match");
  m.is_blue_ready = false;
  m.is_red_ready = false;
  m.start_ts = NULL;
  m.end_ts = NULL;
  m.in_progress = false;
  set_color(white, left);
  set_color(white, right);
}

void pulse(int times, color l, color r) {
  for(int pulses = 0; pulses <= times; pulses++) {
    float i, out;

    for (i = (3.14/2); i < (3.14*1.5); i = i + 0.01) {
      out = sin(i) * 0.5 + 0.5;

      analogWrite(left.pin_r, l.r * out);
      analogWrite(left.pin_g, l.g * out);
      analogWrite(left.pin_b, l.b * out);

      analogWrite(right.pin_r, r.r * out);
      analogWrite(right.pin_g, r.g * out);
      analogWrite(right.pin_b, r.b * out);

      delay(2);
    }
  }
}

void disco() {
  unsigned int color[3];
  color[0] = 255;
  color[1] = 0;
  color[2] = 0;

  for (int dec = 0; dec < 3; dec += 1) {
    int inc = dec == 2 ? 0 : dec + 1;

    for (float in = 0; in < 6.283; in = in + 0.01) {
      color[dec] -= 1;
      color[inc] += 1;

      analogWrite(left.pin_r, color[0]);
      analogWrite(left.pin_g, color[1]);
      analogWrite(left.pin_b, color[2]);

      analogWrite(right.pin_r, color[1]);
      analogWrite(right.pin_g, color[2]);
      analogWrite(right.pin_b, color[0]);

      delay(2);
    }
  }
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
  String cmd = Serial.readString();
  if (cmd == "reset") {
    reset_match();
  } else if (cmd == "start") {
    make_ready_red();
    make_ready_blue();
  } else if (cmd == "disco") {
    discoMode = !discoMode;

    if (discoMode) {
      Serial.println("Disco Mode enabled!");
    } else {
      Serial.println("Goodnight Travolta");
    }
  }

  if (m.in_progress) {
    unsigned long now = millis();
    if (discoMode) {
      disco();
    }
    if(now >= m.end_ts) {
      end_match();
    }
  } else {
    if (teams_ready()) {
      start_match();
    }
  }
}




