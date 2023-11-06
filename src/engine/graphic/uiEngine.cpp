#include "uiEngine.hpp"

#include <stdexcept>
#include <IMGUI/imgui.h>
#include <IMGUI/imgui_impl_glfw.h>
#include <IMGUI/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

UiEngine::UiEngine(GLFWwindow* w) : window(w) {

    const char* glsl_version = "#version 130";
    
    // // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    
    // // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    CustomStyleColors();

    // Adapt scale of font
    float SCALE = 2.0f;
    ImFontConfig cfg;
    cfg.SizePixels = 25 * SCALE;
    ImGui::GetIO().Fonts->AddFontDefault(&cfg)->Scale = SCALE;
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

UiEngine::~UiEngine(){
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

}

void UiEngine::ImGui_prerender() const noexcept{
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UiEngine::ImGui_postrender() const noexcept{

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    // ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    // glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    // glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    glfwSwapBuffers(window);

    // Get input events
    glfwPollEvents();
}

void UiEngine::CustomStyleColors(){
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_SliderGrab]       = ImVec4(1.f, 0.2f, 0.0f, 1.0f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.f, 0.2f, 0.0f, .5f);
    colors[ImGuiCol_CheckMark]        = ImVec4(1.f, 0.2f, 0.0f, 1.0f);
    colors[ImGuiCol_FrameBg]          = ImVec4(1.f, 0.6f, 0.2f, 0.3f);
    colors[ImGuiCol_FrameBgHovered]   = ImVec4(1.f, 0.6f, 0.2f, 0.25f);
    colors[ImGuiCol_FrameBgActive]    = ImVec4(1.f, 0.5f, 0.1f, 0.35f);
    colors[ImGuiCol_Button]          = ImVec4(1.f, 0.6f, 0.2f, 0.3f);
    colors[ImGuiCol_ButtonHovered]   = ImVec4(1.f, 0.6f, 0.2f, 0.25f);
    colors[ImGuiCol_ButtonActive]    = ImVec4(1.f, 0.5f, 0.1f, 0.35f);
}