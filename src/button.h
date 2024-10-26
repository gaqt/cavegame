#pragma once

#include <raylib.h>

typedef struct {
    Rectangle rec;
    char *text;
    void (*action)(void);
    bool isHover;
} Button;

Button NewButton(Rectangle rec, char *text, void (*action)(void));
void HandleButtonCollision(Button *b, Vector2 mousePos);
void HandleButtonAction(Button *b);
void RenderButton(Button *b);
