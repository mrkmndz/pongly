int pos_pins[] = {2, 3, 4, 5, 6, 7};
int neg_pins[] = {8, 9, 10, 11, 12, 13};
#define NUM_PINS 6

typedef struct vec_struct {
  int x;
  int y;
} vec_t; 

typedef struct ball_state_struct {
  vec_t position;
  vec_t velocity;
} ball_state_t;

typedef struct player_state_struct {
  int position;
  int last_position;
  int direction;
  int pin;
  unsigned long next_sense;
  int paddle_size;
} player_state_t;
  

typedef struct game_state_struct {
  ball_state_t ball_state;
  player_state_t p1_state;
  player_state_t p2_state;
  bool finished;
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

player_state_struct get_init_player() {
  player_state_struct pstate;
  pstate.position = 0;
  pstate.last_position = 0;
  pstate.direction = 0;
  pstate.paddle_size = 3;
  pstate.next_sense = millis();
  return pstate;
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

  state.ball_state.position.x = 2;
  state.ball_state.position.y = 2;
  state.ball_state.velocity.x = 0;
  state.ball_state.velocity.y = 1;
  state.p1_state = get_init_player();
  state.p1_state.pin = A3;
  state.p2_state = get_init_player();
  state.p2_state.pin = A0;
  state.finished = false;

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

void move_ball(game_state_t *state) {
  ball_state_t b = state->ball_state;
  state->ball_state.position = addVecs(b.position, b.velocity);
}

void block(game_state_t *state) {
  int y = state->ball_state.position.y;
  player_state_t *target = (y == 0) ? &state->p1_state : &state->p2_state;
  int paddle_pos = target->position;
  int x = state->ball_state.position.x;
  if (x < paddle_pos || x >= paddle_pos + target->paddle_size) {
    state->finished = true;
    target->paddle_size = 0;
    return;
  }
  state->ball_state.velocity.y *= -1;
  state->ball_state.velocity.x += target->direction;
  if (state->ball_state.velocity.x >= 1) {
    state->ball_state.velocity.x = 1;
  } else if (state->ball_state.velocity.x <= -1) {
    state->ball_state.velocity.x = -1;
  } else {
    move_ball(state);
  }
  move_ball(state);
}

game_state_t proceed(game_state_t state) {
  move_ball(&state);

  int x = state.ball_state.position.x;
  if (x < 0) {
    Serial.print("off low");
    state.ball_state.velocity.x = 1;
    state.ball_state.position.x = 1;
  } else if (x > 5) {
    Serial.print("off high");
    state.ball_state.velocity.x = -1;
    state.ball_state.position.x = 4;
  }

  switch (state.ball_state.position.y) {
    case 0:
    case 5:
      block(&state);
  }

  return state;
}

void print_state(game_state_t state) {
  byte pattern[6][6];
  for (int i = 0; i<6; i++) {
    for (int j = 0; j<6; j++) {
      pattern[i][j]=0;
    }
  }
  for (int i = 0; i < state.p1_state.paddle_size; i++) {
    pattern[state.p1_state.position + i][0] = 1;
  }
  for (int i = 0; i < state.p2_state.paddle_size; i++) {
    pattern[state.p2_state.position + i][5] = 1;
  }
  ball_state_t ball_state = state.ball_state;
  pattern[ball_state.position.x][ball_state.position.y] = 1;
  display(pattern);
}

void update_player(player_state_t *state) {
  float val = analogRead(state->pin);    // read the input pin
  float max_val = 400;
  int range = 6 - state->paddle_size;
  int pos = (val/max_val) * range;
  state->position = (pos > range) ? range : pos;
  if (millis() > state->next_sense) {
    state->next_sense += 60;
    if (state->position == state->last_position) {
      state->direction = 0;
    } else if (state->position > state->last_position) {
      state->direction = 1;
    } else {
      state->direction = -1;
    }
    state->last_position = state->position;
  }
}

void loop() {
  static unsigned long next_frame = millis();
  update_player(&state.p1_state);
  update_player(&state.p2_state);
  if (millis() > next_frame && !state.finished) {
    next_frame += 200;
    state = proceed(state);
  }
  print_state(state);
}
