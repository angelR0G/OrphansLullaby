#pragma once

#include <cstdint>
#include <memory>
#include "key.hpp"

struct GLFWwindow;
struct GraphicEngine;

class InputController{
    friend GraphicEngine;

    public:
		static InputController* Instance();

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
        void setCursorPosition(GLFWwindow*, float, float);
        void updateInput();
    private:
        InputController();
		inline static std::unique_ptr<InputController> controller {nullptr};

        bool    keys[KEY_COUNT] {};
        bool    mouseLeft{},
                mouseRight{};
        float   mouseWheel{};
        float   prevMouseX{},
                prevMouseY{};
        float   mouseX{0.5f},
                mouseY{0.5f};
        float   offsetX{},
                offsetY{};
        bool    interact{false};

        std::uint32_t	screenWidth{1920},
                    	screenHeight{1080};
        
        // Functions to process events
        static void process_keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void process_mouse(GLFWwindow* window, double xpos, double ypos);
        static void process_mouse_buttons(GLFWwindow* window, int button, int action, int mods);
        static void process_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset);
};