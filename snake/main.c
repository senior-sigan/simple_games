/**
 * SNAKE GAME!!
 *
 * TODO:
 * - add walls
 * - add different rooms with walls configurations
 * - show score on game over screen
 * - add second type of food
 */

#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define CANVAS_WIDTH 240
#define CANVAS_HEIGHT 120

#define CELL_WIDTH 10   // in pixels
#define CELL_HEIGHT 10  // in pixels

#define EAT_WIDTH 6
#define EAT_HEIGHT 6

#define FIELD_WIDTH 24   // (CANVAS_WIDTH / CELL_WIDTH)
#define FIELD_HEIGHT 12  // (CANVAS_HEIGHT / CELL_HEIGHT)

const short LVL[FIELD_HEIGHT][FIELD_WIDTH] = {
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1};

const Vector2 ZERO_VEC = {0, 0};
RenderTexture2D canvas;

typedef struct SnakeNode_ {
  int32_t lifetime;
  bool eat_inside;
} SnakeNode;

typedef struct Snake_ {
  int dir_x;  // 1 - right, -1 - left, 0 - nowhere
  int dir_y;  // 1 - up , -1 - down, 0 - nowhere
  int next_dir_x;
  int next_dir_y;
  int length;
  int pos_x;
  int pos_y;
  float speed;                 // in seconds till next movement
  float last_step_updated_at;  // time since last step update
  bool has_changed_dir;
  bool has_eaten;
} Snake;

typedef struct Food_ {
  int x;
  int y;
} Food;

void GoToMenu();
void GoToGame();

bool is_game_over = false;
Food food = {-1, -1};
// Eat eat_super = {-1, -1};
SnakeNode snake_body[FIELD_WIDTH][FIELD_HEIGHT];
Snake snake;

void (*Draw)(void);
void (*Update)(void);

void InitSnake() {
  for (int i = 0; i < snake.length; i++) {
    int x = snake.pos_x - snake.dir_x * i;
    int y = snake.pos_y - snake.dir_y * i;
    snake_body[x][y].lifetime = snake.length - i;
  }
}

void SpawnFood() {
  // TODO: replace with deterministic way
  int x;
  int y;
  do {
    x = GetRandomValue(0, FIELD_WIDTH - 1);
    y = GetRandomValue(0, FIELD_HEIGHT - 1);
  } while (snake_body[x][y].lifetime > 0);
  food.x = x;
  food.y = y;
}

void Setup() {
  snake.dir_x = 1;
  snake.dir_y = 0;
  snake.next_dir_x = 1;
  snake.next_dir_y = 0;
  snake.length = 2;
  snake.pos_x = FIELD_WIDTH / 2;
  snake.pos_y = FIELD_HEIGHT / 2;
  snake.last_step_updated_at = 0;
  snake.has_eaten = false;
  is_game_over = false;
  for (int i = 0; i < FIELD_WIDTH; i++) {
    for (int j = 0; j < FIELD_HEIGHT; j++) {
      snake_body[i][j].lifetime = 0;
      snake_body[i][j].eat_inside = false;
    }
  }

  InitSnake();
  SpawnFood();
}

void DrawWallBody(int i, int j) {
  if (LVL[j][i] == 0) {
    return;
  }

  DrawRectangle(i * CELL_WIDTH, j * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                WHITE);
}

void DrawSnakeBody(int i, int j) {
  if (snake_body[i][j].lifetime < 1) {
    return;
  }

  DrawRectangle(i * CELL_WIDTH, j * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                DARKBROWN);
}

void DrawSnakeHead() {
  DrawRectangle(snake.pos_x * CELL_WIDTH, snake.pos_y * CELL_HEIGHT, CELL_WIDTH,
                CELL_HEIGHT, BROWN);
}

void DrawGameMatrix() {
  for (int i = 0; i < FIELD_WIDTH; i++) {
    for (int j = 0; j < FIELD_HEIGHT; j++) {
      DrawSnakeBody(i, j);
      DrawWallBody(i, j);
    }
  }
  DrawSnakeHead();
}

void DrawFood() {
  int dw = (CELL_WIDTH - EAT_WIDTH) / 2;
  int dh = (CELL_HEIGHT - EAT_HEIGHT) / 2;
  DrawRectangle(food.x * CELL_WIDTH + dw, food.y * CELL_HEIGHT + dh, EAT_HEIGHT,
                EAT_WIDTH, MAROON);
}

void CheckFood() {
  if (snake.pos_x == food.x && snake.pos_y == food.y) {
    snake.length++;
    snake.has_eaten = true;
    SpawnFood();
  }
}

void CheckPopka() {
  if (snake_body[snake.pos_x][snake.pos_y].lifetime > 0) {
    is_game_over = true;
  }
}

void MoveSnake() {
  // TODO: move snake smooooothly
  snake.last_step_updated_at += GetFrameTime();
  if (snake.last_step_updated_at >= snake.speed) {
    snake.last_step_updated_at = 0;
    // TODO: do -snake.speed but inside a loop
  } else {
    return;
  }

  if (snake.has_eaten) {
    snake.has_eaten = false;
  } else {
    for (int i = 0; i < FIELD_WIDTH; i++) {
      for (int j = 0; j < FIELD_HEIGHT; j++) {
        if (snake_body[i][j].lifetime > 0) {
          snake_body[i][j].lifetime--;
        }
      }
    }
  }

  snake.dir_x = snake.next_dir_x;
  snake.dir_y = snake.next_dir_y;
  snake.pos_x += snake.dir_x;
  snake.pos_y += snake.dir_y;

  if (snake.pos_x >= FIELD_WIDTH) {
    snake.pos_x = 0;
  }
  if (snake.pos_x < 0) {
    snake.pos_x = FIELD_WIDTH - 1;
  }
  if (snake.pos_y >= FIELD_HEIGHT) {
    snake.pos_y = 0;
  }
  if (snake.pos_y < 0) {
    snake.pos_y = FIELD_HEIGHT - 1;
  }

  CheckPopka();
  snake_body[snake.pos_x][snake.pos_y].lifetime = snake.length;
}

void DrawGame() {
  DrawFood();
  DrawGameMatrix();

  if (is_game_over) {
    DrawText("POTRACHENO", 2, CANVAS_HEIGHT / 2, 20, MAGENTA);
    DrawText("PRESS F TO RESTART", 2, CANVAS_HEIGHT / 2 + 20, 12, MAGENTA);
  }
}

void ControlSnake() {
  if ((snake.dir_y == 0) && (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))) {
    snake.next_dir_y = -1;
    snake.next_dir_x = 0;
    return;
  }
  if ((snake.dir_y == 0) && (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))) {
    snake.next_dir_y = 1;
    snake.next_dir_x = 0;
    return;
  }
  if ((snake.dir_x == 0) && (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))) {
    snake.next_dir_y = 0;
    snake.next_dir_x = -1;
    return;
  }
  if ((snake.dir_x == 0) && (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))) {
    snake.next_dir_y = 0;
    snake.next_dir_x = 1;
    return;
  }
}

void UpdateGame() {
  if (is_game_over) {
    if (IsKeyPressed(KEY_F)) {
      GoToMenu();
    }
    return;
  }
  if (IsKeyPressed(KEY_R)) {
    GoToMenu();
    return;
  }

  MoveSnake();
  CheckFood();
  ControlSnake();
}

Rectangle GetCanvasTarget() {
  float sh = (float)GetScreenHeight();
  float sw = (float)GetScreenWidth();
  float scale = fminf(sh / CANVAS_HEIGHT, sw / CANVAS_WIDTH);
  Rectangle rec = {0, 0, CANVAS_WIDTH * scale, CANVAS_HEIGHT * scale};
  return rec;
}

void DrawMenu() {
  DrawText("SELECT DIFFICULTY", 4, 4, 20, MAGENTA);
  DrawText("press key", 4, 20, 12, MAGENTA);
  DrawText("A", 4, 32, 20, DARKBROWN);
  DrawText("B", 32, 32, 40, DARKBROWN);
  DrawText("C", 98, 32, 60, DARKBROWN);
}

void UpdateMenu() {
  if (IsKeyPressed(KEY_A)) {
    snake.speed = 0.2f;
  } else if (IsKeyPressed(KEY_B)) {
    snake.speed = 0.1f;
  } else if (IsKeyPressed(KEY_C)) {
    snake.speed = 0.05f;
  } else {
    return;
  }

  GoToGame();
}

void GoToMenu() {
  Draw = DrawMenu;
  Update = UpdateMenu;
}

void GoToGame() {
  Draw = DrawGame;
  Update = UpdateGame;
  Setup();
}

void GameLoop() {
  BeginDrawing();
  ClearBackground(GREEN);

  BeginTextureMode(canvas);
  ClearBackground(BLACK);
  Draw();
  EndTextureMode();

  Rectangle canvas_field = {0, 0, (float)canvas.texture.width,
                            -(float)canvas.texture.height};
  DrawTexturePro(canvas.texture, canvas_field, GetCanvasTarget(), ZERO_VEC,
                 0.0f, WHITE);
  EndDrawing();
  Update();
}

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(CANVAS_WIDTH * 2, CANVAS_HEIGHT * 2, "Snake");
  SetTargetFPS(30);
  SetTextureFilter(GetFontDefault().texture, FILTER_POINT);
  canvas = LoadRenderTexture(CANVAS_WIDTH, CANVAS_HEIGHT);
  SetTextureFilter(canvas.texture, FILTER_POINT);

  Setup();
  Draw = DrawMenu;
  Update = UpdateMenu;

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(GameLoop, 0, 1);
#else
  while (!WindowShouldClose()) {
    GameLoop();
  }
#endif

  CloseWindow();

  return 0;
}