#include "input.hpp"
#include <cmath>
#include "../engine/menuaction.hpp"
#include "../components/input.hpp"

#include "../engine/graphic/engine.hpp"

uint8_t InputSystem::update(EntityMan& EM, float deltaTime) {
    uint8_t state{0};

    // Make sure all input events are saved
    controller->updateInput();

    using CList = MetaP::Typelist<InputComponent,MovementComponent,CameraComponent,WeaponComponent>;
    using TList = MetaP::Typelist<>;
    EM.foreach<CList, TList>([&](Entity& e){
        BasicComponent* basic   = &EM.getComponent<BasicComponent>(e);
        MovementComponent* mov  = &EM.getComponent<MovementComponent>(e);
        CameraComponent* cam    = &EM.getComponent<CameraComponent>(e);
        InputComponent* input   = &EM.getComponent<InputComponent>(e);
        WeaponComponent* weap   = &EM.getComponent<WeaponComponent>(e);

        mov->movOrientation     = 0.0;
        bool movForward         = false;
        bool movLateral         = false;

        if(controller->isKeyPressed(KEY_W)) {
            movForward = !movForward;
        }
        if(controller->isKeyPressed(KEY_S)) {
            movForward = !movForward;
            if(movForward) mov->movOrientation += 0.5;
        }
        if(controller->isKeyPressed(KEY_A)) {
            movLateral = !movLateral;
            if (movForward) {
                if (mov->movOrientation > 0.25) mov->movOrientation += 0.125;
                else                            mov->movOrientation -= 0.125;
            }
            else {
                mov->movOrientation -= 0.25;
            }
        }
        if(controller->isKeyPressed(KEY_D)) {
            movLateral = !movLateral;
            if (movForward) {
                if (mov->movOrientation > 0.25) mov->movOrientation -= 0.125;
                else                            mov->movOrientation += 0.125;
            }
            else {
                mov->movOrientation += 0.25;
            }
        }
        if(controller->isKeyPressed(KEY_J)){
            GraphicEngine* engine3D = GraphicEngine::Instance();
            engine3D->captureMouse(!engine3D->getCaptureMouse());
        }

        if (movForward || movLateral)   mov->moving = true;
        else                            mov->moving = false;

        if(input->alternateRunning) {
            // Press button to alternate between run and walk
            if(controller->isKeyPressed(SHIFT))  {
                // If button is not being held, change running state
                if(!input->alternateRunningPressed) {
                    mov->tryToRun = !mov->tryToRun;
                    input->alternateRunningPressed = true;
                }
            }
            else
                input->alternateRunningPressed = false;
        }
        else {
            // Run while pressing button
            if(controller->isKeyPressed(SHIFT))  mov->tryToRun = true;
            else                                mov->tryToRun = false;
        }

        // Camera movement
        GraphicEngine* engine3D = GraphicEngine::Instance();
        // Don't update camera if mouse isn't captured
        //if(engine3D->getCaptureMouse()){
            float yaw   = controller->getOffsetX();
            float pitch = controller->getOffsetY();
            if (yaw != 0)    cam->cameraNode.addYaw(yaw * input->mouseHorizontalSensitivity);
            if (pitch != 0)  cam->cameraNode.addPitch(pitch * input->mouseVerticalSensitivity);

            basic->orientation = cam->cameraNode.getYaw();
        //}

        // Changing weapon
        if(controller->isKeyPressed(KEY_1))  	weap->getFirstWeapon    = true;
        if(controller->isKeyPressed(KEY_2))  	weap->getSecondWeapon   = true;
        if(controller->getMouseWheel() != 0) 	weap->changeWeapon      = true;

        // Shooting
        WeaponData* weapon = weap->activeWeapon;
        if(controller->isMouseLeftPressed()) 	weapon->weaponState |= WEAPON_FIRING; 
        else                                	weapon->weaponState &= ~WEAPON_FIRING;

		// Reload
        if(controller->isKeyPressed(KEY_R) && weapon->reloadElapsed >= weapon->reloadTime)  
			weapon->weaponState |= WEAPON_RELOAD;

        //Interact with an object
        if(controller->getInteractKey()) {
            if(!interactPressed)
                input->interact = true;

            interactPressed = true;
        }
        else {
            input->interact = false;
            interactPressed = false;
        }

        // Quit game
        if(controller->isKeyPressed(ESCAPE)) state = 1;

        // Debug
        if(controller->isKeyPressed(KEY_I)) state = 10;

        // Reset controller values
        controller->resetMouseWheel();
    });

    return state;
}

uint8_t InputSystem::keyPress(unsigned int key) {
    uint8_t state{0};
    // Quit game
    if(controller->isKeyPressed(key)) state = 1;

    return state;
}

//Returns action and option in menu
std::pair<int, int> InputSystem::updateMenu(int numopt, int curopt) {
    int action{0};
    int option{curopt};

    // Back
    if(controller->isKeyPressed(ESCAPE)){
        action = MENU_BACK;
    // Enter Option
    }else if(controller->isKeyPressed(SPACE) || controller->isKeyPressed(ENTER)){
        action = MENU_ENTER;
    }else{
        // Option above
        if(controller->isKeyPressed(KEY_W) || controller->isKeyPressed(UP)) {
            option = curopt - 1;
        }
        // Option below
        if(controller->isKeyPressed(KEY_S) || controller->isKeyPressed(DOWN)) {
            option = curopt + 1;
        }
        // Option to left
        if(controller->isKeyPressed(KEY_A) || controller->isKeyPressed(LEFT)) {
            action = MENU_LEFT;
        }
        // Option to right
        if(controller->isKeyPressed(KEY_D) || controller->isKeyPressed(RIGHT)) {
            action = MENU_RIGHT;
        }
        if(option<1)
            option = numopt;
        if(option>numopt)
            option = 1;
    }
    
    return {action, option};
}

InputController* InputSystem::getController() {
    return controller;
}

void InputSystem::initializeInput() {
    controller = InputController::Instance();
}