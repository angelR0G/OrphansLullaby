#pragma once

#include <irrlicht/irrlicht.h>
#include <cstdint>
#include "key.hpp"

class inputController : public irr::IEventReceiver {
    public:
        bool OnEvent(const irr::SEvent& event);
        void createCursorController(irr::IrrlichtDevice* device_);
        bool isKeyPressed(int id);
        bool getInteractKey();
        void setInteractKey(bool);
        float getMouseX();
        float getMouseY();
        float getOffsetX();
        float getOffsetY();
        bool isMouseLeftPressed();
        bool isMouseRightPressed();
        float getMouseWheel();
        void resetMouseWheel();
        void setScreenSize(std::uint32_t, std::uint32_t);
    private:
        irr::gui::ICursorControl* cursorController{nullptr};
        bool    keys[KEY_COUNT] {};
        bool    mouseLeft{},
                mouseRight{};
        float   mouseWheel{};
        uint16_t   	prevMouseX{},
                	prevMouseY{};
        float   mouseX{0.5f},
                mouseY{0.5f};
        float   offsetX{},
                offsetY{};
        bool    auxUpdateMouse{false};
        bool    interact{false};

        std::uint32_t	screenWidth{1920},
                    	screenHeight{1080};
};