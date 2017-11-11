int pos_pins[] = {2, 3, 4, 5, 6, 7};
int neg_pins[] = {8, 9, 10, 11, 12, 13};
#define NUM_PINS 6

typedef struct vec_struct {
  byte x;
  byte y;
} vec_t; 

typedef struct ball_state_struct {
  vec_t position;
  vec_t velocity;
} ball_state_t;

typedef struct game_state_struct {
  ball_state_t ball_state;
  byte paddle_lengths;
  byte p1_position;
  byte p2_position;
} game_state_t;


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

game_state_t state;
void setup() {
  pinMode(A3, INPUT);
  set_pins_as_output(pos_pins, NUM_PINS);
  set_pins_as_output(neg_pins, NUM_PINS);
  set_pins_high(pos_pins, NUM_PINS);
  set_pins_high(neg_pins, NUM_PINS);
  Serial.begin(115200);
  Serial.setTimeout(100);
  loop_through_all(50);

  state.ball_state.position.x = 3;
  state.ball_state.position.y = 4;
  state.ball_state.velocity.x = 1;
  state.ball_state.velocity.y = 1;
  state.p1_position = 0;
  state.p2_position = 0;
  state.paddle_lengths = 2;

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

vec_t addVecs(vec_t v1, vec_t v2) {
  vec_t sum;
  sum.x = v1.x + v2.x;
  sum.y = v1.y + v2.y;
  return sum;
}

game_state_t proceed(game_state_t state) {
  ball_state_t ball_state = state.ball_state;
  vec_t next_ball_position = addVecs(ball_state.position, ball_state.velocity);

  vec_t next_ball_velocity;

  byte next_x = next_ball_position.x;
  if (next_x == 0 || next_x == 5) {
    next_ball_velocity.x = ball_state.velocity.x * -1;
  } else {
    next_ball_velocity.x = ball_state.velocity.x;
  }

  byte next_y = next_ball_position.y;
  if (next_y == 0 || next_y == 5) {
    next_ball_velocity.y = ball_state.velocity.y * -1;
  } else {
    next_ball_velocity.y = ball_state.velocity.y;
  }
  
  state.ball_state.position = next_ball_position;
  state.ball_state.velocity = next_ball_velocity;
}

void print_state(game_state_t state) {
  byte pattern[6][6];
  for (int i = 0; i<6; i++) {
    for (int j = 0; j<6; j++) {
      pattern[i][j]=0;
    }
  }
  for (int i = 0; i < state.paddle_lengths; i++) {
    pattern[state.p1_position + i][0] = 1;
  }
  for (int i = 0; i < state.paddle_lengths; i++) {
    pattern[state.p2_position + i][5] = 1;
  }
  ball_state_t ball_state = state.ball_state;
  pattern[ball_state.position.x][ball_state.position.y] = 1;
  display(pattern);
}

byte get_pos(int pin, byte paddle_size) {
  float val = analogRead(pin);    // read the input pin
  float max_val = 400;
  int range = 6 - paddle_size;
  int pos = (val/max_val) * range;
  return (pos > range) ? range : pos;
}

void loop() {
  static int game_start = millis();
  state.p1_position = get_pos(A3, state.paddle_lengths);
  state.p2_position = get_pos(A0, state.paddle_lengths);
  if (millis() - game_start % 100 == 0) {
    state = proceed(state);
  }
  print_state(state);
}
