#pragma once

struct GLFWwindow;
struct GraphicEngine;

struct UiEngine{
    friend GraphicEngine;

    ~UiEngine();

    void ImGui_prerender() const noexcept;
    void ImGui_postrender() const noexcept;

    void CustomStyleColors();
    
    private:
        UiEngine(GLFWwindow*);
        GLFWwindow* window;
};
