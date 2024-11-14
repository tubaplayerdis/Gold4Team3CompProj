#include "UISystem.h"

bool UISystem::doRender = false;

void UISystem::toggleUI() {
    doRender = !doRender;
}

int UISystem::renderLoop() {
    return 1;
}