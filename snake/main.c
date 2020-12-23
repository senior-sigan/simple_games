#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CANVAS_WIDTH 240
#define CANVAS_HEIGHT 136

#define BODY_WIDTH 10   // in pixels
#define BODY_HEIGHT 10  // in pixels

const int FIELD_WIDTH = CANVAS_WIDTH / BODY_WIDTH;
const int FIELD_HEIGHT = CANVAS_HEIGHT / BODY_HEIGHT;

const Vector2 ZERO_VEC = {0, 0};

typedef struct SnakeNode_ {
  int32_t lifetime;
  bool eat_inside;
} SnakeNode;

typedef struct Snake_ {
  int dir_x;  // 1 - right, -1 - left, 0 - nowhere
  int dir_y;  // 1 - up , -1 - down, 0 - nowhere
  int length;
  int pos_x;
  int pos_y;
  float speed;                 // in seconds till next movement
  float last_step_updated_at;  // time since last step update
} Snake;

SnakeNode snake_body[FIELD_WIDTH][FIELD_HEIGHT];
Snake snake = {.dir_x = 1,
               .dir_y = 0,
               .length = 2,
               .pos_x = FIELD_WIDTH / 2,
               .pos_y = FIELD_HEIGHT / 2,
               .speed = 0.2f,
               .last_step_updated_at = 0};

void InitSnake() {
  for (int i = 0; i < snake.length; i++) {
    int x = snake.pos_x - snake.dir_x * i;
    int y = snake.pos_y - snake.dir_y * i;
    snake_body[x][y].lifetime = snake.length - i;
  }
}

void Setup() {
  for (int i = 0; i < FIELD_WIDTH; i++) {
    for (int j = 0; j < FIELD_HEIGHT; j++) {
      snake_body[i][j].lifetime = 0;
      snake_body[i][j].eat_inside = false;
    }
  }

  InitSnake();
}

void DrawSnakeBody(int i, int j) {
  DrawRectangle(i * BODY_WIDTH, j * BODY_HEIGHT, BODY_WIDTH, BODY_HEIGHT,
                DARKBROWN);
}

void DrawSnakeHead() {
  DrawRectangle(snake.pos_x * BODY_WIDTH, snake.pos_y * BODY_HEIGHT, BODY_WIDTH,
                BODY_HEIGHT, BROWN);
}

void DrawSnake() {
  for (int i = 0; i < FIELD_WIDTH; i++) {
    for (int j = 0; j < FIELD_HEIGHT; j++) {
      if (snake_body[i][j].lifetime > 0) {
        DrawSnakeBody(i, j);
      }
    }
  }
  DrawSnakeHead();
}

void MoveSnake() {
  // TODO: move snake smooooothly
  snake.last_step_updated_at += GetFrameTime();
  if (snake.last_step_updated_at >= snake.speed) {
    snake.last_step_updated_at =0;
    // TODO: do -snake.speed but inside a loop
  } else {
    return;
  }

  for (int i = 0; i < FIELD_WIDTH; i++) {
    for (int j = 0; j < FIELD_HEIGHT; j++) {
      if (snake_body[i][j].lifetime > 0) {
        snake_body[i][j].lifetime--;
      }
    }
  }

  snake.pos_x += snake.dir_x;
  snake.pos_y += snake.dir_y;

  if (snake.pos_x >= FIELD_WIDTH) {
    snake.pos_x = 0;
    // TODO: check bite popka
  }
  if (snake.pos_x < 0) {
    snake.pos_x = FIELD_WIDTH - 1;
    // TODO: check bite popka
  }
  if (snake.pos_y >= FIELD_HEIGHT) {
    snake.pos_y = 0;
    // TODO: check bite popka
  }
  if (snake.pos_y < 0) {
    snake.pos_y = FIELD_HEIGHT - 1;
    // TODO: check bite popka
  }

  snake_body[snake.pos_x][snake.pos_y].lifetime = snake.length;
}

void Draw() {
  DrawSnake();
}

void ControlSnake() {
  if ((snake.dir_y == 0) && (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))) {
    snake.dir_y = 1;
    snake.dir_x = 0;
  }
  if ((snake.dir_y == 0) && (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))) {
    snake.dir_y = -1;
    snake.dir_x = 0;
  }
  if ((snake.dir_x == 0) && (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))) {
    snake.dir_y = 0;
    snake.dir_x = -1;
  }
  if ((snake.dir_x == 0) && (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))) {
    snake.dir_y = 0;
    snake.dir_x = 1;
  }
}

void Update() {
  MoveSnake();
  ControlSnake();
}

Rectangle GetCanvasTarget() {
  float sh = (float)GetScreenHeight();
  float sw = (float)GetScreenWidth();
  float scale = fminf(sh / CANVAS_HEIGHT, sw / CANVAS_WIDTH);
  Rectangle rec = {0, 0, CANVAS_WIDTH * scale, CANVAS_HEIGHT * scale};
  return rec;
}

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(CANVAS_WIDTH * 2, CANVAS_HEIGHT * 2, "Snake");
  SetTargetFPS(30);

  RenderTexture2D canvas = LoadRenderTexture(CANVAS_WIDTH, CANVAS_HEIGHT);
  Rectangle canvas_field = {0, 0, (float)canvas.texture.width,
                            (float)canvas.texture.height};
  SetTextureFilter(canvas.texture, FILTER_POINT);

  Setup();
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(GREEN);

    BeginTextureMode(canvas);
    ClearBackground(BLACK);
    Draw();
    EndTextureMode();

    DrawTexturePro(canvas.texture, canvas_field, GetCanvasTarget(), ZERO_VEC,
                   0.0f, WHITE);
    EndDrawing();
    Update();
  }

  CloseWindow();

  return 0;
}