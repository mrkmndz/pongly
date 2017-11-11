int pos_pins[] = {2, 3, 4, 5, 6, 7};
int neg_pins[] = {8, 9, 10, 11, 12, 13};
#define NUM_PINS 6

void set_pins_as_output(int *pins, size_t len) {
  for (int i = 0; i < len; i++) {
    pinMode(pins[i], OUTPUT);
  }
}

void set_pins_low(int *pins, size_t len) {
  for (int i = 0; i < len; i++) {
    digitalWrite(pins[i], LOW);
  }
}

void set_pins_high(int *pins, size_t len) {
  for (int i = 0; i < len; i++) {
    digitalWrite(pins[i], HIGH);
  }
}

void setup() {
  set_pins_as_output(pos_pins, NUM_PINS);
  set_pins_as_output(neg_pins, NUM_PINS);
  set_pins_high(pos_pins, NUM_PINS);
  set_pins_high(neg_pins, NUM_PINS);
  Serial.begin(115200);
  Serial.setTimeout(100);
  loop_through_all(50);
}

void loop_through_all(size_t time_delay) {
  for (int i = 0; i < NUM_PINS; i++) {
    digitalWrite(pos_pins[i], LOW);
    for (int j = 0; j < NUM_PINS; j++) {
      digitalWrite(neg_pins[j], LOW);
      delay(time_delay);
      digitalWrite(neg_pins[j], HIGH);
    }
    digitalWrite(pos_pins[i], HIGH);
  }
}

void display(byte pattern[6][6])
{
  for (int row = 0; row < NUM_PINS; row++) {
    for (int col = 0; col < NUM_PINS; col++) {
      int state = (pattern[col][row] > 0) ? LOW : HIGH;
      digitalWrite(neg_pins[col], state);
    }
    digitalWrite(pos_pins[row], LOW);
    delayMicroseconds(100);
    digitalWrite(pos_pins[row], HIGH);
  }
}

const byte blank[6][6] = {{0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0}};

byte m[6][6] = {{0, 1, 0, 0, 1, 0},
                {0, 1, 1, 1, 1, 0},
                {0, 1, 0, 0, 1, 0},
                {0, 1, 0, 0, 1, 0},
                {0, 1, 0, 0, 1, 0},
                {0, 0, 0, 0, 0, 0}};


byte a[6][6] = {{0, 1, 1, 1, 1, 0},
                {0, 1, 0, 0, 1, 0},
                {0, 1, 1, 1, 1, 0},
                {0, 1, 0, 0, 1, 0},
                {0, 1, 0, 0, 1, 0},
                {0, 0, 0, 0, 0, 0}};

byte r[6][6] = {{0, 1, 1, 1, 1, 0},
                {0, 1, 0, 0, 1, 0},
                {0, 1, 1, 1, 0, 0},
                {0, 1, 0, 1, 0, 0},
                {0, 1, 0, 0, 1, 0},
                {0, 0, 0, 0, 0, 0}};

byte k[6][6] = {{0, 1, 0, 0, 1, 0},
                {0, 1, 0, 1, 0, 0},
                {0, 1, 1, 1, 0, 0},
                {0, 1, 0, 1, 0, 0},
                {0, 1, 0, 0, 1, 0},
                {0, 0, 0, 0, 0, 0}};

int* addVecs(int* v1, int* v2) {
  int res[2] = {v1[0] + v2[0], v1[1], v2[1]};
  return res;
}

void procede() {
  int next_pos[2] = addVecs(ball_pos, ball_vel);
  int new_vel[2] = memcpy(ball_vel);

  // First, x-coordinate
  if (next_pos[0] == 0 || next_pos[0] == 5) {
    // Need to add actual check for collision!
    new_vel[0] *= -1;
  }

  // Second, y-coordinate
  if (next_pos[1] < 0 || next_pos[1] > 5) {
    new_vel[1] *= -1;
  }

  ball_vel = memcpy(new_vel);
  ball_pos = addVecs(ball_pos, ball_vel);
}

byte* getPattern() {
  byte pattern[6][6] = blank;
  for (int i = 0; i < p1_length; i++) {
    pattern[0][p1_pos + i] = 1;
  }
  for (int i = 0; i < p2_length; i++) {
    pattern[5][p2_pos + i] = 1;
  }
  pattern[ball_pos[0]][ball_pos[1]] = 1;
  return pattern;
}

// int rep = 0;
/* States are: 
  0: menu
  1: playing pong game
*/
int state = 0;
int p1_length = 6;
int p2_length = 6;
int p1_pos = 0;
int p2_pos = 0;
int game_start = 0;
int ball_pos[2];
int ball_vel[2];
int ball_speed;
void loop() {
  if (state == 0) {
    if (true) {
      state = 1;
      ball_pos = {1, 0};
      ball_vel = {1, 1};
      ball_speed = 1000;
      game_start = millis(); 
    }
  }

  else if (state == 1) {
    if ((millis() - game_start % ball_speed == 0) {
      procede();
    }
    display(getPattern());
  }
  // int state = rep % 4;
  // switch (state) {
  //   case 0:
  //   display(m);
  //   break;
  //   case 1:
  //   display(a);
  //   break;
  //   case 2:
  //   display(r);
  //   break;
  //   case 3:
  //   display(k);
  //   break;
  // }
  // if (micros() % 10000 == 0) {
  //   Serial.println("switch");
  //   rep++;
  // }
}
