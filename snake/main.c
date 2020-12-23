#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define CANVAS_WIDTH 240
#define CANVAS_HEIGHT 136

const Vector2 ZERO_VEC = {0, 0};

Rectangle GetCanvasTarget() {
  float sh = (float)GetScreenHeight();
  float sw = (float)GetScreenWidth();
  float scale = fminf(sh / CANVAS_HEIGHT, sw / CANVAS_WIDTH);
  Rectangle rec = {0, 0, CANVAS_WIDTH * scale, CANVAS_HEIGHT * scale};
  return rec;
}

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(CANVAS_WIDTH, CANVAS_HEIGHT, "Snake");
  SetTargetFPS(30);

  RenderTexture2D canvas = LoadRenderTexture(CANVAS_WIDTH, CANVAS_HEIGHT);
  Rectangle canvas_field = {0, 0, (float)canvas.texture.width,
                            (float)canvas.texture.height};
  SetTextureFilter(canvas.texture, FILTER_POINT);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(GREEN);

    BeginTextureMode(canvas);
    ClearBackground(BLACK);
    DrawRectangle(10, 10, 16, 16, RED);
    EndTextureMode();

    DrawTexturePro(canvas.texture, canvas_field, GetCanvasTarget(), ZERO_VEC,
                   0.0f, WHITE);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}