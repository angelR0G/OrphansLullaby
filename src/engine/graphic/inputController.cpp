#include "inputController.hpp"

#include "engine.hpp"
#include <GLFW/glfw3.h>

InputController* InputController::Instance(){
    if(controller.get() == 0){
        controller = std::unique_ptr<InputController>(new InputController());
    }
    return controller.get();
}

InputController::InputController(){}

void InputController::updateInput() {
    glfwPollEvents();
}

void InputController::process_keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //Save key new state
    bool keyState {true};
    if(action == GLFW_RELEASE)
        keyState = false;

    int id{-1};

    // Check pressed key
    if      (key == GLFW_KEY_W)
        id = KEY_W;
    else if (key == GLFW_KEY_A)
        id = KEY_A;
    else if (key == GLFW_KEY_S)
        id = KEY_S;
    else if (key == GLFW_KEY_D)
        id = KEY_D;
    else if (key == GLFW_KEY_R)
        id = KEY_R;
    else if (key == GLFW_KEY_LEFT_SHIFT)
        id = SHIFT;
    else if (key == GLFW_KEY_ESCAPE)
        id = ESCAPE;
    else if (key == GLFW_KEY_E)
        id = KEY_E;
    else if (key == GLFW_KEY_1)
        id = KEY_1;
    else if (key == GLFW_KEY_2)
        id = KEY_2;
    else if (key == GLFW_KEY_SPACE)
        id = SPACE;
    else if (key == GLFW_KEY_LEFT)
        id = LEFT;
    else if (key == GLFW_KEY_UP)
        id = UP;
    else if (key == GLFW_KEY_RIGHT)
        id = RIGHT;
    else if (key == GLFW_KEY_DOWN)
        id = DOWN;
    else if (key == GLFW_KEY_ENTER)
        id = ENTER;
    else if (key == GLFW_KEY_J)
        id = KEY_J;
    else if (key == GLFW_KEY_I)
        id = KEY_I;

    // Update interact value
    if (id == KEY_E && controller->keys[KEY_E] == false && keyState)    controller->interact = true;
    else                                                                controller->interact = false;
    
    // Update key state
    if (id >= 0) controller->keys[id] = keyState;
}

void InputController::process_mouse(GLFWwindow* window, double xpos, double ypos) {
    // Save previous mouse position
    controller->prevMouseX = controller->mouseX;
    controller->prevMouseY = controller->mouseY;
    
    // Calculate offset
    controller->offsetX += xpos - controller->prevMouseX;
    controller->offsetY += ypos - controller->prevMouseY;

    // Check if cursor is close to a screen border
    if((xpos < 0 || xpos >= controller->screenWidth ||
        ypos < 0 || ypos >= controller->screenHeight)) {

        // Set the cursor in the center of the screen
        controller->setCursorPosition(window, 0.5, 0.5);
    }
    else {
        // Update cursor position
        controller->mouseX = xpos;
        controller->mouseY = ypos;
    }
}

void InputController::process_mouse_buttons(GLFWwindow* window, int button, int action, int mods) {
    //Save button new state
    bool keyState {true};
    if(action == GLFW_RELEASE)
        keyState = false;

    // Check pressed button
    if      (button == GLFW_MOUSE_BUTTON_LEFT)
        controller->mouseLeft   = keyState;
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
        controller->mouseRight  = keyState;
}

void InputController::process_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset) {
    // Save wheel scroll
    controller->mouseWheel = yoffset;
}

void InputController::setCursorPosition(GLFWwindow* window, float x, float y) {
    if(window == nullptr) return;

    // For preventing bad behaviour, poll all input events first
    updateInput();

    // Update cursor values
    mouseX      = screenWidth * x;
    mouseY      = screenHeight * y;
    prevMouseX  = mouseX;
    prevMouseY  = mouseY;

    // Move cursor in the window
    glfwSetCursorPos(window, mouseX, mouseY);

    // Reset offset values
    offsetX = 0;
    offsetY = 0;
}

bool InputController::isKeyPressed(int id) {
    if(id >= 0 && id<KEY_COUNT) {
        return keys[id];
    }

    return false;
}

bool InputController::getInteractKey() {
    return interact;
}

void InputController::setInteractKey(bool value) {
    interact = value;
}

float InputController::getMouseX() {
    return mouseX;
}

float InputController::getMouseY() {
    return mouseY;
}

float InputController::getOffsetX() {
    float aux   = offsetX;
    offsetX     = 0.0;
    return aux;
}

float InputController::getOffsetY() {
    float aux   = offsetY;
    offsetY     = 0.0;
    return aux;
}

bool InputController::isMouseLeftPressed() {
    return mouseLeft;
}

bool InputController::isMouseRightPressed() {
    return mouseRight;
}

float InputController::getMouseWheel() {
    return mouseWheel;
}

void InputController::resetMouseWheel() {
    mouseWheel = 0.f;
}

void InputController::setScreenSize(std::uint32_t w, std::uint32_t h) {
    screenWidth     = w;
    screenHeight    = h;
    
    // Update initial mouse position
    mouseX      = screenWidth/2;
    mouseY      = screenHeight/2;
    prevMouseX  = mouseX;
    prevMouseY  = mouseY;
}