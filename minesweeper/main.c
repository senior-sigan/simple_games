#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

// + отрисовать поле
// + сгенерировать поле random
// + возня с мышкой
// - обрабатывать нажатия левой и правой кнопки мыши
// - алгоритм открытия только нужных частей поля
// - обработать БДЫЩЬ когда наступили на мину
// - обработать game win
// - рисовать HUD - количество мин, время

#define CANVAS_WIDTH 240
#define CANVAS_HEIGHT 120

#define EMPTY_CELL_MARKER 0
#define MINE_MARKER -1

#define CLOSED_CELL_MARKER 0
#define OPENNED_CELL_MARKER 1
#define FLAG_CELL_MARKER 2

typedef struct Point_ {
  int x;
  int y;
} Point;

int cell_font_size = 0;
int n_mines = 0;
int cell_size = 0;
int n_cells_height = 0;
int n_cells_width = 0;

int* field = NULL;
int* field_state = NULL;

Point mouse;

const Vector2 ZERO_VEC = {0, 0};
RenderTexture2D canvas;

int Clampi(int value, int lo, int hi) {
  return value > hi ? hi : value < lo ? lo : value;
}

Point ToPoint(int pos) {
  Point point;
  point.x = pos % n_cells_width;
  point.y = pos / n_cells_width;
  return point;
}

int ToPos(Point point) {
  return point.x + point.y * n_cells_width;
}

int ToPos2(int i, int j) {
  return i + j * n_cells_width;
}

Rectangle GetCanvasTarget() {
  float sh = (float)GetScreenHeight();
  float sw = (float)GetScreenWidth();
  float scale = fminf(sh / CANVAS_HEIGHT, sw / CANVAS_WIDTH);
  Rectangle rec = {0, 0, CANVAS_WIDTH * scale, CANVAS_HEIGHT * scale};
  return rec;
}

void SetHint(Point point) {
  if (point.x < 0 || point.x >= n_cells_width || point.y < 0 ||
      point.y >= n_cells_height) {
    return;
  }
  if (field[ToPos(point)] != MINE_MARKER) {
    field[ToPos(point)]++;
  }
}

void SetHintsAround(int pos) {
  Point p = ToPoint(pos);
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      Point point = p;
      point.x += i;
      point.y += j;
      SetHint(point);
    }
  }
}

void GenerateMines() {
  int mines_to_generate = n_mines;
  while (mines_to_generate > 0) {
    int pos = GetRandomValue(0, n_cells_height * n_cells_width - 1);
    if (field[pos] != MINE_MARKER) {
      field[pos] = MINE_MARKER;
      mines_to_generate--;
      SetHintsAround(pos);
    }
  }
}

void CleanUp() {
  if (field != NULL) {
    free(field);
  }
}

void Setup() {
  n_mines = 10;
  n_cells_height = 9;
  n_cells_width = 9;
  cell_size = 12;
  cell_font_size = 10;
  CleanUp();
  field = (int*)calloc(n_cells_height * n_cells_width, sizeof(int));
  GenerateMines();
}

void DrawCell(int i, int j) {
  int pos = ToPos2(i, j);
  int state = field[pos];
  if (state == MINE_MARKER) {
    DrawRectangle(i * cell_size, j * cell_size, cell_size, cell_size, PINK);
    return;
  }
  char str[4];
  sprintf(str, "%d", state);
  DrawText(str, i * cell_size + 1, j * cell_size + 1, cell_font_size, PINK);
}

void Draw() {
  int color_idx = 0;
  for (int i = 0; i < n_cells_width; i++) {
    for (int j = 0; j < n_cells_height; j++) {
      color_idx++;
      Color color;
      if (color_idx % 2 == 0) {
        color = GREEN;
      } else {
        color = LIME;
      }
      DrawRectangle(i * cell_size, j * cell_size, cell_size, cell_size, color);

      DrawCell(i, j);
    }
  }
}

/**
 * Convert canvas coordinates into field coordinates.
 * Canvas coords inside [0..CANVAS_WIDTH]..[0..CANVAS_HEIGHT]
 * Field Coordinates [0..n_cells_width]..[0..n_cells_height]
 *
 * @param real_point
 * @return
 */
Point ToFieldCoord(Point real_point) {
  return real_point;  // fixme
}

void Update() {
  if (IsKeyPressed(KEY_R)) {
    Setup();
    return;
  }

  // - взять ячейку по координатам
  // - это бомба?
  // - это поле открыто?
  Point p = ToFieldCoord(mouse);
  // TODO: skip this mouse pos. It's outside of the field.

  int idx = ToPos(p);

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (field[idx] == MINE_MARKER) {
      // TODO: BUM
    } else {
      if (field_state[idx] == OPENNED_CELL_MARKER) {
        // do nothing
      } else {
        // open cells around this cell
      }
    }
  }

  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    if (field_state[idx] == FLAG_CELL_MARKER) {
      field_state[idx] = FLAG_CELL_MARKER;
    } else {
      field_state[idx] = CLOSED_CELL_MARKER;
    }
  }
}

void TranslateMousePos(Rectangle canvas_target) {
  mouse.x = GetMouseX();
  mouse.y = GetMouseY();

  mouse.x = (int)(CANVAS_WIDTH * (float)mouse.x / canvas_target.width -
                  canvas_target.x);
  mouse.y = (int)(CANVAS_HEIGHT * (float)mouse.y / canvas_target.height -
                  canvas_target.y);

  mouse.x = Clampi(mouse.x, 0, CANVAS_WIDTH);
  mouse.y = Clampi(mouse.y, 0, CANVAS_HEIGHT);
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
  Rectangle canvas_target = GetCanvasTarget();
  DrawTexturePro(canvas.texture, canvas_field, canvas_target, ZERO_VEC, 0.0f,
                 WHITE);
  TranslateMousePos(canvas_target);
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

  while (!WindowShouldClose()) {
    GameLoop();
  }

  CloseWindow();
  CleanUp();

  return 0;
}