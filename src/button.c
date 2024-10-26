#include "button.h"
#include <raylib.h>

Button NewButton(Rectangle rec, char *text, void (*action)(void)) {
    return (Button){rec, text, action, false};
}

void HandleButtonCollision(Button *b, Vector2 mousePos) {
    b->isHover = CheckCollisionPointRec(mousePos, b->rec);
}

void HandleButtonAction(Button *b) {
    if (!b->isHover)
        return;
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return;

    b->action();
}

void RenderButton(Button *b) {
    if (b->isHover)
        DrawRectangleRec(b->rec, LIGHTGRAY);
    else
        DrawRectangleRec(b->rec, GRAY);

    DrawText(b->text, b->rec.x + 20, b->rec.y + 20, 20, BLACK);
}
