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
  pinMode(A3, INPUT);
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

void addVecs(byte* v1, byte* v2, byte* sum) {
  byte* a = v1[0] + v2[0];
  byte* b = v1[1] + v2[1];
  memcpy(sum, a, sizeof(byte));
  memcpy(sum[1], b, sizeof(byte));
}

void procede(byte p1p, byte p1l, byte p2p, byte p2l, byte* bp, byte* bv) {
  byte* next_pos;
  addVecs(bp, bv, next_pos);
  byte new_vel[2];
  memcpy(new_vel, bv, sizeof(byte) * 2);

  // First, x-coordinate
  if (next_pos[0] == 0 || next_pos[0] == 5) {
    // Need to add actual check for collision!
    // Will make use of paddle positions
    new_vel[0] *= -1;
  }

  // Second, y-coordinate
  if (next_pos[1] < 0 || next_pos[1] > 5) {
    new_vel[1] *= -1;
  }

  memcpy(bv, new_vel, sizeof(byte) * 2);
  addVecs(bp, bv, bp);
}

byte* getPattern(byte p1p, byte p1l, byte p2p, byte p2l, byte* bp) {
  byte pattern[36];
  memcpy(pattern, blank, sizeof(byte) * 36);
  for (int i = 0; i < p1l; i++) {
    pattern[p1p + i] = 1;
  }
  for (int i = 0; i < p2l; i++) {
    pattern[6 * 5 + p2p + i] = 1;
  }
  pattern[6 * bp[0] + bp[1]] = 1;
  return pattern;
}

//byte[6][6] toArray(byte* ptr) {
//  byte arr[6][6];
//  for (int r = 0; r < 6; r++) {
//    for (int c = 0; c < 6; c++) {
//      arr[r][c] = ptr[6*r + c];
//    }
//  }
//  return arr;
//}

// int rep = 0;
/* States are: 
  0: menu
  1: playing pong game
*/
int state = 0;
byte p1_length = 6;
byte p2_length = 6;
byte p1_pos = 0;
byte p2_pos = 0;
int game_start = 0;
byte ball_pos[2];
byte ball_vel[2];
int ball_speed;
void ben_loop() {
  if (state == 0) {
    if (true) {
      state = 1;
      ball_pos[0] = 1;
      ball_pos[1] = 0;
      ball_vel[0] = 1;
      ball_vel[1] = 1;
//      ball_vel = {1, 1};
      ball_speed = 1000;
      game_start = millis(); 
    }
  }

  else if (state == 1) {
    if ((millis() - game_start) % ball_speed == 0) {
      procede(p1_pos, p1_length, p2_pos, p2_length, ball_pos, ball_vel);
    }
    byte* pattern = getPattern(p1_pos, p1_length, p2_pos, p2_length, ball_pos);
    display((byte[6][6])pattern);
  }
}

byte get_pos(int pin, byte paddle_size) {
  float val = analogRead(pin);    // read the input pin
  float max_val = 400;
  int range = 6 - paddle_size;
  int pos = (val/max_val) * range;
  return (pos > range) ? range : pos;
}

void loop() {
  byte pattern[6][6];
  for (int i = 0; i<6; i++) {
    for (int j = 0; j<6; j++) {
      pattern[i][j]=0;
    }
  }
  int Apos = get_pos(A3, 2);
  pattern[Apos][0] = 1;
  pattern[Apos + 1][0] = 1;
  int Bpos = get_pos(A0, 2);
  pattern[Bpos][5] = 1;
  pattern[Bpos + 1][5] = 1;
  display(pattern);
}
