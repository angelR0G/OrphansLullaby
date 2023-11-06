#include "inputController.hpp"

#define MOUSE_X_BORDER 200
#define MOUSE_Y_BORDER 180

bool inputController::OnEvent(const irr::SEvent& event) {
    using namespace irr;
    if (event.EventType == EET_KEY_INPUT_EVENT) {
        int id = -1;
        switch (event.KeyInput.Key) {
            case KEY_KEY_W:
                id = KEY_W;
                break;
            case KEY_KEY_A:
                id = KEY_A;
                break;
            case KEY_KEY_S:
                id = KEY_S;
                break;
            case KEY_KEY_D:
                id = KEY_D;
                break;
            case KEY_KEY_R:
                id = KEY_R;
                break;
            case KEY_LSHIFT:
                id = SHIFT;
                break;
            case KEY_ESCAPE:
                id = ESCAPE;
                break;
            case KEY_KEY_E:
                id = KEY_E;
                break;
            case KEY_KEY_1:
                id = KEY_1;
                break;
            case KEY_KEY_2:
                id = KEY_2;
                break;
            case KEY_SPACE:
                id = SPACE;
                break;
            case KEY_LEFT:
                id = LEFT;
                break;
		    case KEY_UP:
                id = UP;
                break;
		    case KEY_RIGHT:
                id = RIGHT;
                break;
		    case KEY_DOWN:
                id = DOWN;
                break;
            case KEY_RETURN:
                id = ENTER;
                break;
            default:
                break;
        }

        if(id == KEY_E && keys[KEY_E] == false && event.KeyInput.PressedDown)   interact = true;
        else                                                                    interact = false;

        if (id >= 0) keys[id] = event.KeyInput.PressedDown;
    }
    else if (event.EventType == EET_MOUSE_INPUT_EVENT) {
        switch(event.MouseInput.Event) {
            case EMIE_LMOUSE_PRESSED_DOWN:
                mouseLeft = true;
                break;

            case EMIE_LMOUSE_LEFT_UP:
                mouseLeft = false;
                break;

            case EMIE_RMOUSE_PRESSED_DOWN:
                mouseRight = true;
                break;

            case EMIE_RMOUSE_LEFT_UP:
                mouseRight = false;
                break;

            case EMIE_MOUSE_WHEEL:
                mouseWheel = event.MouseInput.Wheel;
                break;

            case EMIE_MOUSE_MOVED:
                if (auxUpdateMouse) {
                    // Save previous mouse position
                    prevMouseX = mouseX;
                    prevMouseY = mouseY;
                    
                    // Get new mouse position
                    float auxX = event.MouseInput.X;
                    float auxY = event.MouseInput.Y;

                    // Check if cursor is close to a screen border
                    if((auxX <= MOUSE_X_BORDER || auxX >= screenWidth-MOUSE_X_BORDER ||
                        auxY <= MOUSE_Y_BORDER || auxY >= screenHeight-MOUSE_Y_BORDER) 
                        && cursorController != nullptr) {
                        // Prevent any mouse movement event
                        auxUpdateMouse = false;

                        // Set the cursor in the center
                        cursorController->setPosition(0.5f, 0.5f);
                        mouseX = screenWidth/2;
                        mouseY = screenHeight/2;
                        prevMouseX = mouseX;
                        prevMouseY = mouseY;
                    }
                    else {
                        mouseX = auxX;
                        mouseY = auxY;

                        // Calculate offset
                        offsetX += mouseX - prevMouseX;
                        offsetY += mouseY - prevMouseY;
                    }
                }
                else {auxUpdateMouse = true;}

                break;
            default:
                break;
        }
    }

    return false;
}

void inputController::createCursorController(irr::IrrlichtDevice* device_) {
    if(cursorController == nullptr) {
        cursorController = device_->getCursorControl();
        cursorController->setPosition(0.5f, 0.5f);
    }
}

bool inputController::isKeyPressed(int id) {
    if(id >= 0 && id<KEY_COUNT) {
        return keys[id];
    }

    return false;
}

bool inputController::getInteractKey() {
    return interact;
}

void inputController::setInteractKey(bool value) {
    interact = value;
}

float inputController::getMouseX() {
    return mouseX;
}

float inputController::getMouseY() {
    return mouseY;
}

float inputController::getOffsetX() {
    float aux   = offsetX;
    offsetX     = 0.0;
    return aux;
}

float inputController::getOffsetY() {
    float aux   = offsetY;
    offsetY     = 0.0;
    return aux;
}

bool inputController::isMouseLeftPressed() {
    return mouseLeft;
}

bool inputController::isMouseRightPressed() {
    return mouseRight;
}

float inputController::getMouseWheel() {
    return mouseWheel;
}

void inputController::resetMouseWheel() {
    mouseWheel = 0.f;
}

void inputController::setScreenSize(std::uint32_t w, std::uint32_t h) {
    screenWidth     = w;
    screenHeight    = h;
    
    // Update initial mouse position
    mouseX = screenWidth/2;
    mouseY = screenHeight/2;
    prevMouseX = mouseX;
    prevMouseY = mouseY;
}