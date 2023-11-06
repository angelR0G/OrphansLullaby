#include "render.hpp"

#include "../engine/graphic/engine.hpp"
#include "../engine/graphic/uiEngine.hpp"
#include "../components/render.hpp"
#include "../engine/types.hpp"

#include <IMGUI/imgui.h>
#include <IMGUI/imgui_impl_glfw.h>
#include <IMGUI/imgui_impl_opengl3.h>
#include <optional>
#include <cstdio>

#include "../IrrlichtFacade/image.hpp"
#include "../engine/menu.hpp"
#include "../engine/menuController.hpp"

#include "../engine/graphic/sceneTree/sceneMesh.hpp"
#include "../engine/graphic/sceneTree/sceneCamera.hpp"

#include "../manager/gamemanager.hpp"

#define PI 3.1415926

#define FONT_SCALE_MENU (windowWidth/1920)

RenderSystem::RenderSystem(){
    uiController = UiController::Instance();
}

RenderSystem::~RenderSystem(){}

void RenderSystem::render(EntityMan& EM, GraphicEngine& gE, float percentTick, const double dt, roundStats& rs) {

    //Update time round
    if(rs.roundTime>0.0)
        rs.roundTime -= dt;
    
    if(rs.roundTime < 0)
        rs.roundTime = 0;
    

    GameManager* gm = GameManager::Instance();
    int points = (int)GameManager::getPoints();
    playerStats ps = GameManager::getPlayerStats();
    weaponStats ws = GameManager::getWeaponStats();

    int fps = (int)gm->getFPS();

    PlayerBasicValues playerBasic {};
    
    // Updates render objects position with interpolation
    using CList = MetaP::Typelist<RenderComponent, WeaponComponent>;
    EM.foreachORComp<CList>([&](EntityMan::Entity& e){
        auto const& basic = EM.getComponent<BasicComponent>(e);
        if(e.hasTag<PlayerTag>()) {
            playerBasic.posX        = basic.x;
            playerBasic.posY        = basic.y;
            playerBasic.posZ        = basic.z;
            playerBasic.orientation = basic.orientation;
        }
        else if(e.hasComponent<RenderComponent>()) {
            auto& ren = EM.getComponent<RenderComponent>(e);

            float ix = basic.prevx * (1-percentTick) + basic.x * percentTick;
            float iy = basic.prevy * (1-percentTick) + basic.y * percentTick;
            float iz = basic.prevz * (1-percentTick) + basic.z * percentTick;

            float iorientation = basic.prevorientation * (1-percentTick) + basic.orientation * percentTick;

            if(ren.node.getMesh() != nullptr) {
                ren.node.setPosition(ix, iy, iz);
                ren.node.setRotation(basic.verticalOrientation, iorientation, 0);
            }
            if(ren.emitter.getEmitter() != nullptr) {
                ren.emitter.setPosition(ix, iy, iz);
                ren.emitter.setInterpolationValue(percentTick);
            }
        }
    });
    // Update LOD reference position
    MeshNode::setLODReferencePosition(playerBasic.posX, playerBasic.posY, playerBasic.posZ);

    // Draw all render objects
    gE.render();
    
    uiController->ImGui_UI(points, ps, ws, rs, fps, gm->getGoals(), dt, playerBasic);
}

void RenderSystem::update(EntityMan& EM, const float dt, GraphicEngine& gE) {
    // Search player
    float camX{}, camY{}, camZ{};
    float playerX{}, playerY{}, playerZ{}, playerOrientation{};

    using CPList = MetaP::Typelist<BasicComponent>;
    using TPList = MetaP::Typelist<PlayerTag>;
    EM.foreach<CPList, TPList>([&](EntityMan::Entity& e){
        CameraComponent camCmp = EM.getComponent<CameraComponent>(e);

        camCmp.cameraNode.getCameraPosition(&camX, &camY, &camZ);
        particleEmitter::setPlayerReferencePosition({camX, camY, camZ});
    });

    // Update particle emitters
    using CList = MetaP::Typelist<RenderComponent>;
    using TList = MetaP::Typelist<>;
    EM.foreach<CList, TList>([&](EntityMan::Entity& e){
        auto&       ren = EM.getComponent<RenderComponent>(e);

        if(ren.emitter.getEmitter() != nullptr) {
            auto const& bCmp = EM.getComponent<BasicComponent>(e);
            
            // Update emitter transform
            if(e.hasTag<PlayerTag>()) {
                ren.emitter.setPosition(0, 0, 0);

                // If it is the player, save its position and orientation too
                BasicComponent* bCmp {&EM.getComponent<BasicComponent>(e)};
                playerX             = bCmp->x;
                playerY             = bCmp->y;
                playerZ             = bCmp->z;
                playerOrientation   = bCmp->orientation;
            }
            else
                ren.emitter.setPosition(bCmp.x, bCmp.y, bCmp.z);

            ren.emitter.setRotation(0, bCmp.orientation, 0);

            // Update particles
            ren.emitter.update(dt);
        }
    });

    // Update lightning
    gE.updateLightning(playerX, playerY, playerZ, playerOrientation);

    // Update UI
    uiController->update(dt);
}

void RenderSystem::drawMenu(GraphicEngine& gE, bool ingame){
    menuController* menuC = menuController::Instance();
    auto menuNames  = menuC->getCurrentNames();
    auto menuId     = menuC->getCurrentMenuID();
    int numOpt = menuC->getNumberOptions();

    windowWidth = (float)gE.getWindowSize().width;
    windowHeight = (float)gE.getWindowSize().height;
    float menu_x = windowWidth/2,
          menu_y = windowHeight/4;
    float menu_gap = windowHeight/5;
    bool  text_centered = true;

    float r=1.0f,g=1.0f,b=1.0f,a=1.0f;
    //Selected option variable
    float s=0;

    int menuChangeOption = 0,
        menuChangeSides  = 0;

    float fontScale = FONT_SCALE_MENU;
    
    uiController->drawMenuBk(menuId,ingame);
        
    //Custom text and variable values to each menu screen
    switch (menuId)
    {
    case START_MENU:
        uiController->drawLogo();
        menu_gap = menu_gap*0.8;
        break;
    case OPTIONS_MENU:
    case OPTIONS2_MENU:
    case OPTIONS3_MENU:
        menu_x = windowWidth/6,
        menu_y = windowHeight/6.5;
        menu_gap = windowHeight/15;
        fontScale *= 0.6;

        text_centered = false;

        //Adapt option gap
        if(numOpt>8)
            menu_gap = menu_gap * 1.1*(8/(float)numOpt);
        break;
    case PAUSE_MENU:
        menu_y = windowHeight/10;
        break;
    case CONTROLS_MENU:
        fontScale *= 0.7;
        menu_y = 13*windowHeight/15;
    break;
    case DEFEAT_MENU:
        menu_y = 13*windowHeight/15;
        menu_gap = windowHeight/15;

        uiController->drawText("HAS MUERTO", windowWidth/2, windowHeight/6.5, 0.8, 0, 0, 1, true, fontScale*1.2);
        drawStats(windowWidth/2,2*windowHeight/6.5,fontScale/2,menu_gap);

        fontScale *= 0.7;
    break;
    case WIN_MENU:
        menu_y = 13*windowHeight/15;
        menu_gap = windowHeight/15;

        uiController->drawText("MISIÓN COMPLETADA", windowWidth/2, windowHeight/6.5, 0, 0.8, 0, 1, true, fontScale*1.2);
        drawStats(windowWidth/2,2*windowHeight/6.5,fontScale/2,menu_gap);

        fontScale *= 0.7;
    break;
    default:
        break;
    }

    uiController->startWindow("menu"+std::to_string(menuId));
    if(menuId == OPTIONS_MENU || menuId == OPTIONS2_MENU || menuId == OPTIONS3_MENU){
        ImGui::SetWindowSize(ImVec2(windowWidth*0.85,windowHeight));
        //In option menus: draw side options menus
        menuChangeSides = drawSideMenus(gE, menu_y, r, g, b, a, fontScale, ingame);
    }else ImGui::SetWindowSize(ImVec2(windowWidth,windowHeight));

    //Draw options
    int anyHovered = 0;
    for(size_t i = 0; i<menuC->getCurrentNames().size();i++){
        //Change color on selected option
        if(menuC->getCurrentOption()-1 != (int)i) s=0;
        else                                      s=1;
        if(menuC->getOptionHovered() == (int)i+1) s=2;

        int optT = menuC->getType(i);
        
        float auxfs = fontScale;
        float auxmx = menu_x;
        bool auxtc = text_centered;

        //Change values for first option display in options menu
        if(menuId == OPTIONS_MENU || menuId == OPTIONS2_MENU || menuId == OPTIONS3_MENU){
            if(i==0){
                fontScale = fontScale * 1.4;
                menu_x    = windowWidth/2;
                text_centered = true;
            }else if(i==1) menu_y  = menu_y  * 1.26;
        }

        ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4(0.f, 0.f, 0.f, 0.f));
        //Draw button and if clicked activate button action
        if(uiController->drawButtonInWindowAction(menuNames[i], menu_x, menu_y+menu_gap*i, s, text_centered, fontScale)){
            if(optT==NORMAL_OPTION || optT==CHECKBOX_OPTION){
                if(optT==NORMAL_OPTION) menuChangeOption = i+1;
                else menuC->updateOption(MENU_ENTER,i+1,ingame);
            }
            //If option is slider, do action of moving to the right
            if(optT==SLIDER_OPTION){
                menuC->updateOption(MENU_RIGHT,i+1,ingame);
            }
            menuC->clickSound();

        }
        ImGui::PopStyleColor(3);

        //Select option if hovered
        if(ImGui::IsItemHovered()){
            menuC->setOptionSelected(i+1);
            //Set option hovered for next frames
            menuC->setOptionHovered(i+1);
            anyHovered++;
        }

        fontScale = auxfs;
        menu_x = auxmx;
        text_centered = auxtc;
    }
    if(anyHovered == 0) menuC->setOptionHovered(0);
    

    //Draw option values on options menu screens
    if(menuId == OPTIONS_MENU || menuId == OPTIONS2_MENU || menuId == OPTIONS3_MENU){
        menu_y = menu_y + windowHeight/20;

        drawOptionValues(menu_x, menu_y, menu_gap, fontScale, ingame);
    }
    uiController->endWindow();

    //If activated, change menu at the end of the render
    if(menuChangeOption!=0){
        menuC->updateOption(MENU_ENTER,menuChangeOption,ingame);
    }else if(menuChangeSides!=0){
        if(menuChangeSides==1) menuC->updateOption(MENU_LEFT,1,ingame);
        else if (menuChangeSides==2) menuC->updateOption(MENU_RIGHT,1,ingame);
    }
}


void RenderSystem::drawOptionValues(float menu_x, float menu_y, float menu_gap, float fontScale, int ingame){
    menuController* menuC = menuController::Instance();
    auto menuNames  = menuC->getCurrentNames();
    auto menuId     = menuC->getCurrentMenuID();
    float r=1.0f,g=1.0f,b=1.0f,a=1.0f;

    int s = 0;
    int anyHovered = 0;
    for(size_t i = 0; i<menuC->getCurrentNames().size();i++){
        //Select color
        if(menuC->getCurrentOption()-1 != (int)i) r=1.0f,g=1.0f,b=1.0f,a=1.0f,s=0;
        else                     r=1.0f,g=0.2f,b=0.0f,a=1.0f,s=1;

        if(menuC->getType(i)==SLIDER_OPTION || menuC->getType(i)==CHECKBOX_OPTION){
            ImGui::SetWindowFontScale(fontScale);
            ImVec2 textSize = ImGui::CalcTextSize(menuNames[i]);
            ImVec2 charSize = ImGui::CalcTextSize("A");

            //Calculate format of number shown on slider
            float fraction, whole;
            fraction = std::modf(menuC->getValue(i), &whole);

            const char* format = "%.0f";

            if(fraction!=0){
                format = "%.2f";
            }

            char res[100];

            strcpy(res,"##");//Not show label
            strcat(res,menuNames[i]);

            ImGui::PushItemWidth(-1);//Adapt width to window

            ImGui::SetWindowFontScale(fontScale/2);
            ImGui::SetCursorPos(ImVec2(menu_x+textSize.x+charSize.x, menu_y+menu_gap*i-textSize.y/1.5));

            //Draw slider
            if(menuC->getType(i)==SLIDER_OPTION){
                //Draw name of level of option chosen on certain cases
                if(menuId==OPTIONS3_MENU && (i == 3 || i == 6)){
                    size_t curVal = (int)menuC->getValue(i);
                    std::vector<const char*> subNames {""};
                    const char* subOption {"a"};
                    ImVec2 textSize2 = ImGui::CalcTextSize(subOption);

                    //Selected suboptions names vector
                    if(i == 3) subNames = particleAmountLevels;
                    else if(i == 6) subNames = shadowQualityLevels;

                    //Get suboption selected
                    if(curVal <= subNames.size()-1){
                        subOption = subNames[curVal];
                        textSize2 = ImGui::CalcTextSize(subOption);
                    }

                    //Draw button and if clicked activate button action
                    if(uiController->drawButtonInWindowAction(subOption, menu_x+textSize.x+charSize.x, menu_y+menu_gap*i-textSize.y/1, s, false, fontScale)){
                        menuC->updateOption(MENU_RIGHT,i+1,ingame);
                        menuC->clickSound();
                    }
                }else{
                    float* vPointer = menuC->getValuePointer(i);
                    float vMin = menuC->getMinValue(i), vMax = menuC->getMaxValue(i);

                    //Draw slider and if modified update value
                    if(ImGui::SliderFloat(res, vPointer, vMin, vMax, format)){

                        //Check if value is beyond boundaries
                        if(*vPointer > vMax) *vPointer = vMax;
                        if(*vPointer < vMin) *vPointer = vMin;

                        menuC->setValue(i,*menuC->getValuePointer(i));
                        menuC->clickSound();
                    }
                }
            //Draw checkbox
            }else if(menuC->getType(i)==CHECKBOX_OPTION){
                bool boolean = false;
                bool auxVal = (bool)(int)menuC->getValue(i);
                if(menuC->getValue(i)) boolean = true;

                //Draw checkbox and if clicked activate action
                if(ImGui::Checkbox(res, &auxVal)){
                    menuC->updateOption(MENU_ENTER,i+1,ingame);
                    menuC->clickSound();

                }
            }

            //Select option if hovered
            if(ImGui::IsItemHovered()){
                menuC->setOptionSelected(i+1);
                menuC->setSideHover(i+1);
                anyHovered++;
            }

            ImGui::PopItemWidth();
        }
    }
    if(anyHovered==0) menuC->setSideHover(0);
}

//Return if side menu clicked
int RenderSystem::drawSideMenus(GraphicEngine& gE, float y, float r, float g, float b, float a, float fontScale, int ingame){
    menuController* menuC = menuController::Instance();

    int res = 0;

    int menuLid = menuC->sidesOptionsMenu(MENU_LEFT);
    const char* menuL = menuC->getFirstName(menuLid);
    if(uiController->drawButtonInWindowAction(menuL, windowWidth/4, y, 0, true, fontScale/1.2)){
        res = 1;
    }
    //uiController->drawText(menuL, windowWidth/4, y, r, g, b, a, true, fontScale/1.2);
    if(ImGui::IsItemHovered()){
        menuC->setOptionSelected(1);
    }

    int menuRid = menuC->sidesOptionsMenu(MENU_RIGHT);
    const char* menuR = menuC->getFirstName(menuRid);
    if(uiController->drawButtonInWindowAction(menuR, 3*windowWidth/4, y, 0, true, fontScale/1.2)){
        res = 2;
    }
    //uiController->drawText(menuR, 3*windowWidth/4, y, r, g, b, a, true, fontScale/1.2);
    if(ImGui::IsItemHovered()){
        menuC->setOptionSelected(1);
    }

    return res;
}

void RenderSystem::drawLoadingScreen(GraphicEngine& gE){
    uiController->drawLoadingScreen();
}

UiController* RenderSystem::getUiController() noexcept {
    return uiController;
}

void RenderSystem::reset(){
    uiController->reset();
}

void RenderSystem::drawStats(float x, float y, float fontScale, float gap){
    menuController* menuC = menuController::Instance();
    float r = 1, g = 1, b = 1, a = 1;
    auto stats = menuC->getLastGameStats();
    std::vector<std::string> join;
    std::vector<std::string> text {"MINUTOS VIVO","PUNTOS TOTALES","OLEADAS COMPLETADAS","ENEMIGOS ELIMINADOS","ENEMIGOS ESPECIALES ELIMINADOS"};
    std::vector<std::string> value {std::to_string(stats.minutes),std::to_string(stats.points)
                        ,std::to_string(stats.wave),std::to_string(stats.kills),std::to_string(stats.specialKills)};

    for(size_t i = 0; i<text.size();i++){
        join.push_back(text[i]+": "+value[i]);
    }

    uiController->drawText("ESTADÍSTICAS",x,y+gap*0.5, r,g,b,a, true, fontScale*1.5);
    uiController->drawText(join[0].c_str(),x,y+gap*2, r,g,b,a, true, fontScale);
    uiController->drawText(join[1].c_str(),x,y+gap*3, r,g,b,a, true, fontScale);
    uiController->drawText(join[2].c_str(),x,y+gap*4, r,g,b,a, true, fontScale);
    uiController->drawText(join[3].c_str(),x,y+gap*5, r,g,b,a, true, fontScale);
    uiController->drawText(join[4].c_str(),x,y+gap*6, r,g,b,a, true, fontScale);
}