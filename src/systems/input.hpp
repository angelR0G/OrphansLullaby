#pragma once

#include "../engine/types.hpp"
#include "../engine/graphic/inputController.hpp"

struct InputSystem {
    uint8_t update(EntityMan& EM, float);
    uint8_t keyPress(unsigned int);
    std::pair<int, int> updateMenu(int numopt, int curopt);
    InputController* getController();
    void initializeInput();

    private:
        InputController* controller;
        bool interactPressed{false};
};