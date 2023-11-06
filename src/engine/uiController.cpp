#include "uiController.hpp"
#include "graphic/engine.hpp"
#include "../IrrlichtFacade/image.hpp"
#include "../manager/gamemanager.hpp"
#include <cmath>

#include "menu.hpp"

#define UI_IMAGES(file) "media/ui/" file

#define WHITE_IMAGE         0
#define ORPHANSLOGO_IMAGE   1
#define CONTROLS_IMAGE      2
#define HEALTHBAR_IMAGE     3
#define POINTSTAIN_IMAGE    4
#define CROSSHAIR_IMAGE     5
#define HITMARK_IMAGE       6
#define HITMARKS_IMAGE      7
#define AMMOBK_IMAGE        8
#define POWERUP_IMAGE       9
#define POWERUP_INMUNITY_IMAGE      10
#define POWERUP_TANK_IMAGE          11
#define POWERUP_SPEED_IMAGE         12
#define POWERUP_STEADY_AIM_IMAGE    14
#define INFINITE_IMAGE      13
#define HEART0_IMAGE        15
#define HEART1_IMAGE        16
#define HEART2_IMAGE        17
#define HEART3_IMAGE        18
#define HEART4_IMAGE        19
#define SPLATTER_IMAGE      20
#define LOADING_SCREEN_IMAGE 21
#define DAMAGE_MARK_IMAGE   22
#define MAIN_MENU_IMAGE     23
#define OPTIONS_MENU_IMAGE  24
#define CREDITS_MENU_IMAGE  25
#define PAUSE_MENU_IMAGE    26

#define FONT_SCALE_HUD      (windowWidth/1920)*0.65
#define FONT_SCALE_HUD2     FONT_SCALE_HUD*1.2
// HUD images position
// PowerUps
    #define POWERUPS_X          windowWidth  * 7/100
    #define POWERUPS_Y          windowHeight * 72/100
    #define POWERUPS_Y_INC      windowHeight * -14/100
    #define POWERUPS_START_X    windowWidth  * 50/100
    #define POWERUPS_START_Y    windowHeight * 80/100
// Goals
    #define GOALS_X             windowWidth  * 5/100
    #define GOALS_Y             windowHeight * 5/100
    #define GOALS_LINE_HEIGHT   windowHeight * 2.5/100
// Damage marks
    #define DAMAGE_MARK_OFFSET      windowHeight * 30/100
    #define DAMAGE_MARK_TIME_OFFSET windowHeight * 10/100
    #define DAMAGE_MARK_DUR         1.5f
    #define DAMAGE_MARK_FADE_OUT    0.4f

// Corner progress bar
    #define CORNER_PROGRESS_BAR_X       windowWidth  * 77/100
    #define CORNER_PROGRESS_BAR_Y       windowHeight * 87/100
    #define CORNER_PROGRESS_BAR_W       windowWidth  * 15/100
    #define CORNER_PROGRESS_BAR_H       windowHeight * 2/100
    #define CORNER_PROGRESS_BAR_INNER_X windowWidth  * 77.5/100
    #define CORNER_PROGRESS_BAR_INNER_Y windowHeight * 87.5/100
    #define CORNER_PROGRESS_BAR_INNER_W windowWidth  * 14/100
    #define CORNER_PROGRESS_BAR_INNER_H windowHeight * 1/100

UiController* UiController::Instance(){
    if(uiController.get() == 0){
        uiController = std::unique_ptr<UiController>(new UiController());
    }
    return uiController.get();
}

UiController::UiController(){
    createImages();

    // Save initial window size
    auto windowSize {GraphicEngine::Instance()->getWindowSize()};
    windowWidth     = windowSize.width;
    windowHeight    = windowSize.height;

    // Load font
    float SCALE = 4.0f;
    ImFontConfig cfg;
    cfg.SizePixels = 25 * SCALE;
    font1 = ImGui::GetIO().Fonts->AddFontFromFileTTF("media/fonts/N8ghtmare.ttf",cfg.SizePixels);
    IM_ASSERT(font1 != NULL);
}

UiController::~UiController(){
    uiImages.clear();
}

void UiController::setWindowSize(float width, float height) {
    windowWidth     = width;
    windowHeight    = height;
}

void UiController::createImages(){
    uiImages.clear();
    uiImages.push_back(ImageNode(UI_IMAGES("white.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("menu/orphanslogo.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("controles.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("hearthealthbar.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("pointstain.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("crosshair/ch.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("crosshair/hm.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("crosshair/hms.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("ammobk.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("powerupicon.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("powerupinmunity.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("poweruptank.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("powerupspeed.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("infinite.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("powerupaim.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("heart0.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("heart1.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("heart2.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("heart3.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("heart4.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("plasmasplatter.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("loading_screen.jpg")));
    uiImages.push_back(ImageNode(UI_IMAGES("damageMark.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("menu/mainmenu.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("menu/optionsmenu.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("menu/creditsmenu.png")));
    uiImages.push_back(ImageNode(UI_IMAGES("menu/pausemenu.png")));
}


void UiController::ImGui_UI(int points, playerStats ps, weaponStats ws, roundStats rs, int fps, Goals* goals, const double dt, const PlayerBasicValues& pb){
    drawTextCenterIMGUI();

    ImVec4 defHUDcolor = {1.0f, 1.0f, 1.0f, 1.0f};
    ImVec4 ammoHUDcolor = defHUDcolor;
    ImVec4 ammoXHUDcolor = ammoHUDcolor;

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoBackground;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;

    bool * open_ptr = nullptr;

    ImGui::PushFont(font1);
    ImGui::PushStyleColor(ImGuiCol_WindowBg,{1.0f, 1.0f, 1.0f, 0.0f});
    ImGui::Begin("HUD", open_ptr,  window_flags);
    ImGui::SetWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::SetWindowPos(ImVec2(0,0));
    ImGui::SetWindowFontScale(FONT_SCALE_HUD);
    const char* text = std::to_string((int)ws.ammo).c_str();
    ImVec2 textSize  = ImGui::CalcTextSize(text);

    //Draw ammo background
    float ab_x = 89*windowWidth/100,
          ab_y = 83*windowHeight/100;
    float scaleab = (windowWidth / 1920) * 0.8;
    std::pair<unsigned int, unsigned int> ab_dim = uiImages[AMMOBK_IMAGE].getTextureSize();
    float   ab_w = ab_dim.first * scaleab,
            ab_h = ab_dim.second * scaleab;
    uiImages[AMMOBK_IMAGE].setSize(std::pair<float, float>{ab_w, ab_h});
    uiImages[AMMOBK_IMAGE].setPosition(std::pair<float, float>{ab_x, ab_y});
    uiImages[AMMOBK_IMAGE].drawImage();

    // Draw weapon ammo

      // Weapon reloading
    if(ws.reloadElapsed<ws.reloadTime){
        ammoHUDcolor.w = {0.5f};
    }

    ammoXHUDcolor = ammoHUDcolor;

    // Change color of ammo on low ammo
    float lowAmmoDiv = 3;
    if(ws.ammo<=ws.magSize/lowAmmoDiv){
        float lowAmmoIndex = (float)ws.ammo / ((float)ws.magSize/(lowAmmoDiv/2));
        ammoHUDcolor = {1.0f, lowAmmoIndex, lowAmmoIndex, 1.0f};
    }

    // Total ammo left
    
    float in_w, in_h;
    float widthAmmoextra;

    if(ws.infiniteAmmo){
        std::pair<float, float> in_dim = uiImages[INFINITE_IMAGE].getTextureSize();
        in_w = in_dim.first * 0.1;
        in_h = in_dim.second * 0.1;
        widthAmmoextra = in_w;
    }else{
        int ammoextra = (int)ws.totalAmmo;

        text = std::to_string(ammoextra).c_str();
        textSize = ImGui::CalcTextSize(text);
        widthAmmoextra = textSize.x;
    }
    

    ImGui::SetCursorPos(ImVec2(ab_x+ab_w/2-ab_w/32-widthAmmoextra/2,ab_y+3.3*ab_h/8));

    if(ws.infiniteAmmo){

        uiImages[INFINITE_IMAGE].setSize(std::pair<unsigned int, unsigned int>{in_w, in_h});
        uiImages[INFINITE_IMAGE].setPosition(std::pair<unsigned int, unsigned int>{ab_x+ab_w/2-ab_w/32-widthAmmoextra/2, ab_y+4.2*ab_h/8});
        uiImages[INFINITE_IMAGE].setColor(std::array<float, 4>{ammoXHUDcolor.x, ammoXHUDcolor.y, ammoXHUDcolor.z, ammoXHUDcolor.w});
        uiImages[INFINITE_IMAGE].drawImage();
        // image->drawImageCustom(INFINITE_IMAGE, ab_x+ab_w/2-ab_w/32-widthAmmoextra/2, ab_y+4.2*ab_h/8,
        //  ammoXHUDcolor.x, ammoXHUDcolor.y, ammoXHUDcolor.z, ammoXHUDcolor.w, in_w, in_h);
    }
    else
        ImGui::TextColored(ammoXHUDcolor, text);
        
    
    ////////////////////////////////////////////////
    // Draw crosshair elements

    //Determine which hitmark to draw on hit
    auto drawHitMark = -1;
    if(ws.hitEnemy > 0.001){
        drawHitMark = HITMARK_IMAGE;
        if(ws.collisionType == 2) drawHitMark = HITMARKS_IMAGE;
    }

    //Draw crosshair
    float scalepch =  windowWidth* 0.03;
    float opacitych = 0.65f;
    
    float ch_x = windowWidth/2 -scalepch/2,
          ch_y = windowHeight/2-scalepch/2;

    uiImages[CROSSHAIR_IMAGE].setSize(std::pair<float, float>{scalepch, scalepch});
    uiImages[CROSSHAIR_IMAGE].setPosition(std::pair<float, float>{ch_x, ch_y});
    uiImages[CROSSHAIR_IMAGE].setColor(std::array<float, 4>{1.0f, 1.0f, 1.0f, opacitych});
    uiImages[CROSSHAIR_IMAGE].drawImage();
    //image->drawImageCustom(CROSSHAIR_IMAGE, ch_x, ch_y, 1.0f, 1.0f, 1.0f, opacitych, scalepch, scalepch);

    // Draw hit mark
    if(drawHitMark!=-1){
        uiImages[drawHitMark].setSize(std::pair<float, float>{scalepch, scalepch});
        uiImages[drawHitMark].setPosition(std::pair<float, float>{ch_x, ch_y});
        uiImages[drawHitMark].setColor(std::array<float, 4>{1.0f, 1.0f, 1.0f, opacitych});
        uiImages[drawHitMark].drawImage();
        //image->drawImageCustom(drawHitMark, ch_x, ch_y, 1.0f, 1.0f, 1.0f, opacitych, scalepch, scalepch);

    }
    ////////////////////////////////////////////////
    
    //Position of HUD elements
    float scoreNbarsX =   3*windowWidth/100,
          scoreNbarsY =   81*windowHeight/100,
          scoreNbarsGap = textSize.y;
    

    //Draw points' stain background
    float scaleps = (windowWidth / 1920) * 0.8;
    std::pair<float, float> ps_dim = uiImages[POINTSTAIN_IMAGE].getTextureSize();
    float   ps_w = ps_dim.first * scaleps,
            ps_h = ps_dim.second * scaleps;
    uiImages[POINTSTAIN_IMAGE].setSize(std::pair<float, float>{ps_w, ps_h});
    uiImages[POINTSTAIN_IMAGE].setPosition(std::pair<float, float>{scoreNbarsX, scoreNbarsY-2*windowHeight/100});
    uiImages[POINTSTAIN_IMAGE].setColor(std::array<float, 4>{1.0f, 1.0f, 1.0f, 1.f});
    uiImages[POINTSTAIN_IMAGE].drawImage();
    //image->drawImageCustom(POINTSTAIN_IMAGE, scoreNbarsX, scoreNbarsY-2*windowHeight/100,1.0f,1.0f,1.0f,1.0f, ps_w, ps_h);

    //Draw points

    text = std::to_string(points).c_str();
    textSize = ImGui::CalcTextSize(text);
    
    ImGui::SetCursorPos(ImVec2(scoreNbarsX+ps_w/2-textSize.x/2,scoreNbarsY-textSize.y/3.2));
    ImGui::TextColored(defHUDcolor,"%i", points);

    ///////////////////////////////////////////
    //Draw healthbar
    float   hb_x = scoreNbarsX,
            hb_y = scoreNbarsY+scoreNbarsGap,
            health = ps.health,
            infection = ps.infection;

    float scalehb = (windowWidth / 1920) * 1.0;
    std::pair<float, float> hb_dim = uiImages[HEALTHBAR_IMAGE].getTextureSize();
    float hb_w = hb_dim.first * scalehb,
          hb_h = hb_dim.second * scalehb;
    //Draw healthbar image
    uiImages[HEALTHBAR_IMAGE].setSize(std::pair<float, float>{hb_w, hb_h});
    uiImages[HEALTHBAR_IMAGE].setPosition(std::pair<float, float>{hb_x, hb_y});
    uiImages[HEALTHBAR_IMAGE].setColor(std::array<float, 4>{1.0f, 1.0f, 1.0f, 1.f});
    uiImages[HEALTHBAR_IMAGE].drawImage();
    //image->drawImageCustom(HEALTHBAR_IMAGE, hb_x, hb_y,1.0f,1.0f,1.0f,1.0f, hb_w, hb_h);

    //Draw health
    float hb_health_totalw = (60.5*hb_w/100);
    float hb_health_x = hb_x+31.5*hb_w/100,
          hb_health_y = hb_y+21.25*hb_h/100;
    float hb_health_w = hb_health_totalw*(health/100),
          hb_health_h = 18.7*hb_h/100;
    uiImages[WHITE_IMAGE].setSize(std::pair<float, float>{hb_health_w, hb_health_h});
    uiImages[WHITE_IMAGE].setPosition(std::pair<float, float>{hb_health_x, hb_health_y});
    uiImages[WHITE_IMAGE].setColor(std::array<float, 4>{0.8f, 0.0f, 0.0f, 1.0f});
    uiImages[WHITE_IMAGE].drawImage();
    //image->drawImageCustom(WHITE_IMAGE, hb_health_x, hb_health_y, 0.8f, 0.0f, 0.0f, 1.0f, hb_health_w, hb_health_h);

    //Level of infection
    infectionValue = trunc(infection);
    //int currentHeart = HEART0_IMAGE + infectionValue;
    int currentHeart = HEART0_IMAGE;
    if(infectionValue == 1) currentHeart = HEART1_IMAGE;
    if(infectionValue == 2) currentHeart = HEART2_IMAGE;
    if(infectionValue == 3) currentHeart = HEART3_IMAGE;
    if(infectionValue == 4) currentHeart = HEART4_IMAGE;

    //Quantity of infection inside a level
    float infectionInsideLevel = infection;
    while(infectionInsideLevel >= 1) --infectionInsideLevel;

    //Draw current heart of level of infection
    std::pair<float, float> heart_dim = uiImages[currentHeart].getTextureSize();
    float heart_w = heart_dim.first * scalehb * 0.13,
          heart_h = heart_dim.second * scalehb * 0.13;
    float heart_x = hb_x+9*hb_w/100,
          heart_y = hb_y+13*hb_h/100;

    //Draw healthbar image
    uiImages[currentHeart].setSize(std::pair<float, float>{heart_w, heart_h});
    uiImages[currentHeart].setPosition(std::pair<float, float>{heart_x, heart_y});
    uiImages[currentHeart].setColor(std::array<float, 4>{1.0f, 1.0f, 1.0f, 1.f});
    uiImages[currentHeart].drawImage();

    //Draw definitive infection
    float hb_definf_x = hb_health_x+hb_health_totalw-hb_health_totalw*(infectionValue/10),
          hb_definf_y = hb_health_y;
    float hb_definf_w = hb_health_totalw*(infectionValue/10),
          hb_definf_h = hb_health_h;
    uiImages[WHITE_IMAGE].setSize(std::pair<float, float>{hb_definf_w, hb_definf_h});
    uiImages[WHITE_IMAGE].setPosition(std::pair<float, float>{hb_definf_x, hb_definf_y});
    uiImages[WHITE_IMAGE].setColor(std::array<float, 4>{0.1f, 0.0f, 0.1f, 1.0f});
    uiImages[WHITE_IMAGE].drawImage();
    //image->drawImageCustom(WHITE_IMAGE, hb_definf_x, hb_definf_y, 0.1f, 0.0f, 0.1f, 1.0f, hb_definf_w, hb_definf_h);

    //Draw infection
    float hb_inf_x = hb_health_x,
          hb_inf_y = hb_y+62.65*hb_h/100;
    float hb_inf_w = (28*hb_w/100) * infectionInsideLevel,
          hb_inf_h = 18*hb_h/100;
    uiImages[WHITE_IMAGE].setSize(std::pair<float, float>{hb_inf_w, hb_inf_h});
    uiImages[WHITE_IMAGE].setPosition(std::pair<float, float>{hb_inf_x, hb_inf_y});
    uiImages[WHITE_IMAGE].setColor(std::array<float, 4>{0.55f, 0.2f, 0.65f, 1.0f});
    uiImages[WHITE_IMAGE].drawImage();

    //Draw powerup icon
    size_t powerupsDrawn {0};
    for(size_t i{0}; i<ps.specialEffect.size(); ++i) {
        if(ps.specialEffect[i] != SpecialEffect::No_Effect) {
            drawPowerUp(ps.specialEffect[i], powerupsDrawn, ps.specialEffectHealth[i], ps.effectsDisplayTime[i]);
            ++powerupsDrawn;
        }
    }

    ///////////////////////////////////////////

    drawMainGoal(goals);

    drawDamageMarks(pb);

    ImGui::End();
    ImGui::PopStyleColor(1);

    //Second window, for different font scale
    ImGui::PushStyleColor(ImGuiCol_WindowBg,{1.0f, 1.0f, 1.0f, 0.0f});
    ImGui::Begin("HUD2", open_ptr,  window_flags);
    ImGui::SetWindowSize(ImVec2(windowWidth,windowHeight));
    ImGui::SetWindowPos(ImVec2(0,0));
    ImGui::SetWindowFontScale(FONT_SCALE_HUD2);

    // Draw weapon loaded ammo
    
    text = std::to_string((int)ws.ammo).c_str();
    textSize = ImGui::CalcTextSize(text);

    ImGui::SetCursorPos(ImVec2(ab_x+ab_w/2-ab_w/32-textSize.x/2,ab_y+0.6*ab_h/8));
    ImGui::TextColored(ammoHUDcolor,"%i", (int)ws.ammo);
    

    ImGui::End();
    ImGui::PopStyleColor(1);

    if(rs.roundTime > 0){
        float   maxDur = ROUND_DISPLAY_TIME/8,
                changeDur = ROUND_DISPLAY_TIME/8;
        float   maxTime     = ROUND_DISPLAY_TIME-maxDur,
                changeTime  = maxTime-changeDur,
                fadeOutTime = ROUND_DISPLAY_TIME/3;

        float opacityw = 1.0f;
        //Fade out when time is low
        if(rs.roundTime <= fadeOutTime)
            opacityw *= rs.roundTime/fadeOutTime;

        //Third window, for different font scale
        ImGui::PushStyleColor(ImGuiCol_WindowBg,{1.0f, 1.0f, 1.0f, 0.0f});
        ImGui::Begin("HUD3", open_ptr,  window_flags);
        ImGui::SetWindowSize(ImVec2(windowWidth,windowHeight));
        ImGui::SetWindowPos(ImVec2(0,0));

        //Size scale coefficient
        float scalerQmin = 1.0,        //Normal size
              scalerQmax = 3.0,        //Start size
              scalerQ = scalerQmin;

        //Size scale calculation
        if(rs.roundTime>=maxTime){
            //At start, maxscale
            scalerQ = scalerQmax;
        }else if(rs.roundTime>=changeTime){
            //During change time, average scale
            float durQ = (maxTime - rs.roundTime)/changeDur;
            scalerQ = scalerQmax - (scalerQmax-scalerQmin)*durQ;
        }

        ImGui::SetWindowFontScale(FONT_SCALE_HUD*2.3*scalerQ);
        
        text = std::to_string(rs.wave).c_str();
        textSize = ImGui::CalcTextSize(text);

        //Normal location
        float hb_r_xmin = windowWidth/2-textSize.x/2,
              hb_r_ymin = windowHeight/100;
        //Start location
        float hb_r_xmax = windowWidth/2-textSize.x/2,
              hb_r_ymax = windowHeight/4-textSize.y/2;
        float hb_r_x = hb_r_xmin,
              hb_r_y = hb_r_ymin;

        //Location calculation
        if(rs.roundTime>=maxTime){
            //At start, maxlocation
            hb_r_x = hb_r_xmax;
            hb_r_y = hb_r_ymax;
        }else if(rs.roundTime>=changeTime){
            //During change time, average location
            float durQ = (maxTime - rs.roundTime)/changeDur;
            hb_r_x = hb_r_xmax - (hb_r_xmax-hb_r_xmin) * durQ;
            hb_r_y = hb_r_ymax - (hb_r_ymax-hb_r_ymin) * durQ;
        }

        // Draw wave
        ImGui::SetCursorPos(ImVec2(hb_r_x,hb_r_y));
        ImGui::TextColored(ImVec4(0.55f, 0.2f, 0.65f, opacityw),"%i", rs.wave);
        

        ImGui::End();
        ImGui::PopStyleColor(1);
    }
    
    //On raise of infection, start displaying infection warning
    if(infectionValueOld != -1 && infectionValue==infectionValueOld+1){
        infectionValueTime = INFECTION_DISPLAY_TIME;

        //Clear previous values of splatter of warning infection
        splatterspec_w.clear();
        splatterspec_h.clear();
        splatterspec_x.clear();
        splatterspec_y.clear();

        splatterAmount = 0;
    }
    if(infectionValueTime>0){
        drawInfectionWarning(windowWidth/32,windowHeight/32);
        infectionValueTime-=dt;
    }

    infectionValueOld = infectionValue;

    ImGui::PopFont();
}

void UiController::drawMenuBk(int menuid, bool ingame){
    int pos_image = -1;
    if(!ingame){
        if(START_MENU==menuid) pos_image = MAIN_MENU_IMAGE;
        else if(OPTIONS_MENU==menuid||OPTIONS2_MENU==menuid||OPTIONS3_MENU==menuid) pos_image = OPTIONS_MENU_IMAGE;
        else if(CREDITS_MENU==menuid) pos_image = CREDITS_MENU_IMAGE;
    }else pos_image = PAUSE_MENU_IMAGE;
    if(CONTROLS_MENU==menuid) pos_image = CONTROLS_IMAGE;

    if(pos_image==-1) drawBackground("BK",0.1f, 0.1f, 0.1f, 1.0f);
    else drawImageFull(pos_image, true);
}

void UiController::drawImageFull(int pos_image, bool fullWindow){    
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoBackground;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoDecoration;

    bool * open_ptr = nullptr;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg,{1.0f, 1.0f, 1.0f, 0.0f});
    ImGui::Begin("HUD", open_ptr,  window_flags);
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImVec2(windowWidth+80,windowHeight+80));
    //ImageNode::Instance()->drawImage(pos_image, fullWindow);
    uiImages[pos_image].setSize(std::pair<float, float>{windowWidth, windowHeight});
    uiImages[pos_image].setPosition(std::pair<float, float>{0, 0});
    uiImages[pos_image].drawImage();
    ImGui::End();
    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(1);
}

void UiController::drawBackground(const char* string, float r, float g, float b, float a){    
    startWindow(string);
    ImGui::SetWindowSize(ImVec2(windowWidth+80,windowHeight+80));
    ImGui::SetWindowPos(ImVec2(-40, -40));

    //Draw bk
    float bk_x = 0,
          bk_y = 0;
    float bk_w = windowWidth+80,
          bk_h = windowHeight+80;
    uiImages[WHITE_IMAGE].setSize(std::pair<float, float>{bk_w, bk_h});
    uiImages[WHITE_IMAGE].setPosition(std::pair<float, float>{bk_x, bk_y});
    uiImages[WHITE_IMAGE].setColor(std::array<float, 4>{ r, g, b, a});
    uiImages[WHITE_IMAGE].drawImage();
    //image->drawImageCustom(WHITE_IMAGE, bk_x, bk_y, r, g, b, a, bk_w, bk_h);

    endWindow();
}

void UiController::drawText(const char* string, float x, float y, float r, float g, float b, float a, bool centered, float fontScale){
    
    startWindow(string);
    ImGui::SetWindowSize(ImVec2(windowWidth,windowHeight));
    ImGui::SetWindowPos(ImVec2(0,0));
    ImGui::SetWindowFontScale((windowWidth/1920)*fontScale);
    ImVec2 textSize = ImGui::CalcTextSize(string);

    float centering = 0;
    if(centered) centering = textSize.x/2;

    ImGui::SetCursorPos(ImVec2(x-centering,y));
    ImGui::TextColored(ImVec4(r,g,b,a),string);

    endWindow();
}

void UiController::drawTextInWindow(const char* string, float x, float y, float r, float g, float b, float a, bool centered, float fontScale){
    
    //ImGui::SetWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::SetWindowPos(ImVec2(0,0));
    ImGui::SetWindowFontScale((windowWidth/1920)*fontScale);
    ImVec2 textSize = ImGui::CalcTextSize(string);

    float centering = 0;
    if(centered) centering = textSize.x/2;

    ImGui::SetCursorPos(ImVec2(x-centering,y));
    ImGui::TextColored(ImVec4(r,g,b,a),string);
}

bool UiController::drawButtonInWindow(const char* string, float x, float y, float r, float g, float b, float a, bool centered, float fontScale){
    bool res = false;

    //ImGui::SetWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::SetWindowPos(ImVec2(0,0));
    ImGui::SetWindowFontScale((windowWidth/1920)*fontScale);
    ImVec2 textSize = ImGui::CalcTextSize(string);

    float centering = 0;
    if(centered) centering = textSize.x/2;

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r,g,b,a));

    ImGui::SetCursorPos(ImVec2(x-centering,y));
    res = ImGui::Button(string);

    ImGui::PopStyleColor(1);

    return res;
}

//Action
//0 Normal
//1 Active/Selected
bool UiController::drawButtonInWindowAction(const char* string, float x, float y, int action, bool centered, float fontScale){
    float r=1.0f,g=1.0f,b=1.0f,a=1.0f;

    // On selection
    if(action==1||action==2){
        //Change color
        if(action==1) r=0.7f,g=0.2f,b=0.0f,a=1.0f;
        if(action==2) r=1.0f,g=0.1f,b=0.0f,a=1.0f;

        //Draw shadow
        drawTextInWindow(string, x-1, y-1, 0, 0, 0, 1, centered, fontScale);
    }

    return drawButtonInWindow(string, x, y, r, g, b, a, centered, fontScale);
}

void UiController::drawCinematicProgressBar(float value) {
    startWindow("progressBar");

    // Draw background
    drawRectangle(  CORNER_PROGRESS_BAR_X, CORNER_PROGRESS_BAR_Y, 
                    CORNER_PROGRESS_BAR_W, CORNER_PROGRESS_BAR_H, 
                    1.0, 1.0, 1.0, 1.0);
    // Draw progress bar
    drawRectangle(  CORNER_PROGRESS_BAR_INNER_X, CORNER_PROGRESS_BAR_INNER_Y, 
                    CORNER_PROGRESS_BAR_INNER_W * value, CORNER_PROGRESS_BAR_INNER_H, 
                    0.8, 0.0, 0.0, 1.0);
                    
    endWindow();
}

void UiController::drawLogo(){
    startWindow("logo");
    ImGui::SetWindowSize(ImVec2(windowWidth,windowHeight));
    ImGui::SetWindowPos(ImVec2(0,0));

    //Draw logo
    float scalelogo = (windowWidth / 1920) * 0.4;
    std::pair<float, float> logo_dim = uiImages[ORPHANSLOGO_IMAGE].getTextureSize();
    float logo_w = logo_dim.first * scalelogo,
          logo_h = logo_dim.second * scalelogo;
    float logo_x = windowWidth/2 - logo_w/2,
          logo_y = windowHeight/8 - logo_h/2;
    uiImages[ORPHANSLOGO_IMAGE].setSize(std::pair<float, float>{logo_w, logo_h});
    uiImages[ORPHANSLOGO_IMAGE].setPosition(std::pair<float, float>{logo_x, logo_y});
    uiImages[ORPHANSLOGO_IMAGE].setColor(std::array<float, 4>{ 1.0f, 1.0f, 1.0f, 1.0f});
    uiImages[ORPHANSLOGO_IMAGE].drawImage();
    //image->drawImageCustom(ORPHANSLOGO_IMAGE, logo_x, logo_y, 1.0f, 1.0f, 1.0f, 1.0f, logo_w, logo_h);

    endWindow();
}

std::string UiController::wrapString(const std::string& str, size_t wrapSize, size_t& lines) {
    lines = 1;
    if(wrapSize == 0) return str;

    std::string wrappedString {""};
    std::string referenceString {str};

    // Loop until string is wrapped
    while(referenceString.length() > wrapSize) {
        // Find first space before wrap size
        auto pos {referenceString.rfind(' ', wrapSize)};

        if(pos == std::string::npos) {
            // Space not found, look beyond wrap limit
            pos = referenceString.find(' ');
        }

        if(pos != std::string::npos) {
            // Space found, wrap text in that position
            wrappedString += referenceString.substr(0, pos) + "\n";
            referenceString = referenceString.substr(pos + 1);
            ++lines;
        }
        else break;
    }

    // Copy remaining part
    wrappedString += referenceString;

    return wrappedString;
}

void UiController::drawLoadingScreen(){
    drawImageFull(LOADING_SCREEN_IMAGE,true);
}

void UiController::drawPowerUp(SpecialEffect effect, size_t offset, float effectHealth, float dispTime) {
    //Times to change powerup size and location
    float   transitionTime  {0.5},          //Duration of start size and location
            transitionFactor{0};
    //Size scale coefficient
    const float     scaleInit   {0.7},        //Normal size
                    scaleFinal  {0.3};        //Start size
    float           scaleFactor {scaleInit};

    //Calculate transition value
    if (dispTime > 0) {
        if(dispTime >= transitionTime)  transitionFactor = 1;
        else                            transitionFactor = dispTime/transitionTime;
    }
    
    //Calculate size of powerup image
    scaleFactor = scaleFinal - (scaleFinal-scaleInit) * transitionFactor;
    float scale = (windowWidth / 1920) * scaleFactor;
    std::pair<float, float> image_dim = uiImages[POWERUP_IMAGE].getTextureSize();

    float   image_width     = image_dim.first * scale,
            image_height    = image_dim.second * scale;

    // Calculate location of image
    float   image_final_y {POWERUPS_Y + POWERUPS_Y_INC * offset};
    float   image_x {(POWERUPS_X     - (POWERUPS_X - POWERUPS_START_X)       * transitionFactor) - image_width/2},
            image_y {(image_final_y  - (image_final_y - POWERUPS_START_Y)    * transitionFactor) - image_height/2};

    //Draw powerup background icon
    uiImages[POWERUP_IMAGE].setSize(std::pair<float, float>{image_width, image_height});
    uiImages[POWERUP_IMAGE].setPosition(std::pair<float, float>{image_x, image_y});
    uiImages[POWERUP_IMAGE].setColor(std::array<float, 4>{1.0f,1.0f,1.0f,1.0f});
    uiImages[POWERUP_IMAGE].drawImage();

    int puImageId {POWERUP_INMUNITY_IMAGE};
    if(effect == SpecialEffect::Inmunity)
        puImageId = POWERUP_INMUNITY_IMAGE;
    else if(effect == SpecialEffect::Tank)
        puImageId = POWERUP_TANK_IMAGE;
    else if(effect == SpecialEffect::Speed)
        puImageId = POWERUP_SPEED_IMAGE;
    else if(effect == SpecialEffect::Steady_Aim)
        puImageId = POWERUP_STEADY_AIM_IMAGE;
    
    //Draw powerup icon
    uiImages[puImageId].setSize(std::pair<float, float>{image_width, image_height});
    uiImages[puImageId].setPosition(std::pair<float, float>{image_x, image_y});
    uiImages[puImageId].setColor(std::array<float, 4>{1.0f,1.0f,1.0f,1.0f});
    uiImages[puImageId].drawImage();

    //Draw powerup health
    if(transitionFactor < 0.001) {
        // Interpolate color depending on health
        float factor {effectHealth/EFFECTS_INIT_HEALTH};
        float   r1{0.8f},   g1{0.f},    b1{0.f},
                r2{0.55f},  g2{0.2f},   b2{0.65f};
        float   colorR {(r2-r1)*factor + r1},
                colorG {(g2-g1)*factor + g1},
                colorB {(b2-b1)*factor + b1};

        // Draw health
        drawRectangle(image_x, image_y, image_width*factor, image_height*0.1, colorR, colorG, colorB, 1.f);
    }
    
}

void UiController::drawMainGoal(Goals* goals) {
    // Get goal type and check there is a goal
    GoalTypes type {goals->getType()};
    if(type == GoalTypes::NoGoal) return;

    size_t lines {};
    drawTextInWindow(   wrapString(goals->getDescription(), 50, lines).c_str(), 
                        GOALS_X, GOALS_Y, 1.0f, 1.0f, 1.0f, 1.0f, false, 0.25f);
    
    // Show goal progress depending on goal type
    if(type == GoalTypes::ContinuousProgress) {
        ++lines;

        // Draw a progress bar
        auto val {goals->getCurrentProgress()};
        float maxWidth  {windowWidth * 0.1f};
        float maxHeight {windowHeight * 0.016f};

        // Draw progress bar background
        drawRectangle(GOALS_X, GOALS_Y + GOALS_LINE_HEIGHT * lines, maxWidth, maxHeight, 0.0, 0.0, 0.0, 0.8);
        size_t progress {static_cast<size_t>(std::floor(std::get<float>(val.first)*10.f/std::get<float>(val.second)))};

        // Calculate progress piece values
        maxWidth        = windowWidth * 0.008;
        maxHeight       = windowHeight * 0.01;
        float xPadding  { windowWidth * 0.0015f };
        float yPadding  { windowHeight * 0.003f };
        float red       { 1.0f };

        // When goal is finishing, change progress bar color
        if(progress >= 9) {
            red -= (std::get<float>(val.first)/std::get<float>(val.second) - 0.9) * 10.f;
        }

        // Draw progress by pieces
        for(size_t i{0}; i<progress; ++i) {
            // Draw a rectangle
            drawRectangle(  GOALS_X + i*(maxWidth + 2*xPadding) + xPadding, GOALS_Y + GOALS_LINE_HEIGHT * lines + yPadding, 
                            maxWidth, maxHeight, 
                            red, 1.0f-red, 0.0, 1.0);
        }
    }
    else if(type == GoalTypes::Count) {
        // Draw objective and current value
        auto val {goals->getCurrentProgress()};
        int current     {std::get<int>(val.first)}, 
            objective   {std::get<int>(val.second)};
        std::string progress {std::to_string(current) + "/" + std::to_string(objective)};

        drawTextInWindow(   progress.c_str(),
                            GOALS_X, GOALS_Y + GOALS_LINE_HEIGHT * lines, 1.0f, 1.0f, 1.0f, 1.0f, false, 0.25f);
    }
}

void UiController::drawRectangle(float posX, float posY, float width, float height, float r, float g, float b, float a) {
    uiImages[WHITE_IMAGE].setSize(std::pair<float, float>{width, height});
    uiImages[WHITE_IMAGE].setPosition(std::pair<float, float>{posX, posY});
    uiImages[WHITE_IMAGE].setColor(std::array<float, 4>{r, g, b, a});
    uiImages[WHITE_IMAGE].drawImage();
}

void UiController::drawDamageMarks(const PlayerBasicValues& playerBasic) {
    for(size_t i{0}; i<damageMarks.size(); ++i) {
        // Calculate draw position
        float angle {damageMarks[i].getDrawAngle(playerBasic.posX, playerBasic.posZ, playerBasic.orientation)};

        // Draw a mark
        float timeOffset {(1 - damageMarks[i].duration/DAMAGE_MARK_DUR) * DAMAGE_MARK_TIME_OFFSET + DAMAGE_MARK_OFFSET};
        uiImages[DAMAGE_MARK_IMAGE].setPosition(std::pair<float, float>{
                                                windowWidth/2.f     + sin(angle)    * timeOffset, 
                                                windowHeight/2.f    + -cos(angle)   * timeOffset});
        uiImages[DAMAGE_MARK_IMAGE].setColor({1.0f, 1.0f, 1.0f, std::min(1.0f, damageMarks[i].duration/DAMAGE_MARK_FADE_OUT)});
        uiImages[DAMAGE_MARK_IMAGE].drawRotatedImage(angle);
    }
}

void UiController::drawInfectionWarning(float x, float y){
    float fadeOutTime = INFECTION_DISPLAY_TIME/3;

    float opacityw = 1.0f;
    //Fade out when time is low
    if(infectionValueTime <= fadeOutTime)
        opacityw *= infectionValueTime/fadeOutTime;

    //Fourth window
    startWindow("HUD4");
    ImGui::SetWindowSize(ImVec2(windowWidth,windowHeight));
    ImGui::SetWindowPos(ImVec2(0,0));
    ImGui::SetWindowFontScale(FONT_SCALE_HUD);


    //Draw splatter
    float scalesplatter = (windowWidth / 1920) * 1.75;
    std::pair<float, float> spl_dim = uiImages[SPLATTER_IMAGE].getTextureSize();
    float splatter_w = spl_dim.first * scalesplatter,
          splatter_h = spl_dim.second * scalesplatter;
    int splatter_x = x,
          splatter_y = y;
    
    //Set random splatter amount
    if(splatterAmount < 5)
        splatterAmount = 5 + rand()%(11-1); //rand 1 to 10

    for(int i = 0; i<splatterAmount;++i){
        if(infectionValueTime == INFECTION_DISPLAY_TIME){
            int scaleVar  = 45;
            float randscale = (float)(100+rand()%(scaleVar-1-0))/(float)100;

            splatterspec_w.push_back(splatter_w * randscale);
            splatterspec_h.push_back(splatter_h * randscale);

            //Splatter up
            if(i<splatterAmount/3){
                splatterspec_x.push_back(splatter_x + rand()%(((int)windowWidth-splatter_x+1-(int)splatterspec_w[i])-50));
                splatterspec_y.push_back(splatter_y + rand()%(int)windowHeight/16+1); //+ rand()%(((int)windowHeight-splatter_y+1-((int)windowHeight/2+(int)splatterspec_h[i]))-50));
            //Splatter down
            }else if(i<2*splatterAmount/3){
                splatterspec_x.push_back(splatter_x*4 + rand()%(((int)windowWidth-splatter_x*4+1-(int)splatterspec_w[i])-50));
                splatterspec_y.push_back(windowHeight-splatter_y-splatterspec_h[i]+ rand()%(int)windowHeight/16+1);
            //Splatter left
            }else if(i<5*splatterAmount/6){
                splatterspec_x.push_back(splatter_x + rand()%(int)windowWidth/16+1);
                splatterspec_y.push_back(splatter_y + rand()%(int)12*windowHeight/16+1);
            //Splatter right
            }else{
                splatterspec_x.push_back(windowWidth-splatter_x-splatterspec_w[i] + rand()%(int)windowWidth/16+1);
                splatterspec_y.push_back(splatter_y + rand()%(int)12*windowHeight/16+1);
            }
        }
        
        uiImages[SPLATTER_IMAGE].setSize(std::pair<float, float>{splatterspec_w[i], splatterspec_h[i]});
        uiImages[SPLATTER_IMAGE].setPosition(std::pair<float, float>{splatterspec_x[i], splatterspec_y[i]});
        uiImages[SPLATTER_IMAGE].setColor(std::array<float, 4>{ 1.0f, 1.0f, 1.0f, opacityw});
        uiImages[SPLATTER_IMAGE].drawImage();
    }

    endWindow();
}

void UiController::startWindow(std::string name){
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoBackground;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoDecoration;

    bool * open_ptr = nullptr;

    ImGui::PushFont(font1);
    ImGui::PushStyleColor(ImGuiCol_WindowBg,{1.0f, 1.0f, 1.0f, 0.0f});
    ImGui::Begin(name.c_str(), open_ptr,  window_flags);
}

void UiController::endWindow(){
    ImGui::End();
    ImGui::PopStyleColor(1);
    ImGui::PopFont();
}

//Draw the text from the interactuable items in the game
void UiController::drawTextCenterIMGUI(){
    //Get the text from the item
    Text uiText = GameManager::Instance()->getUiText();

    if(uiText.active){
        startWindow("uiText");

        //Initialize window to draw the text
        GraphicEngine* ge = GraphicEngine::Instance();
        ImGui::SetWindowSize(ImVec2(windowWidth,windowHeight));
        ImGui::SetWindowPos(ImVec2(0,0));
        ImVec2 textSize = ImGui::CalcTextSize(uiText.text.c_str());

        //Get text color
        ImVec4 colorI {uiText.color[0], uiText.color[1], uiText.color[2], uiText.color[3]};
        
        //Calculate window relative position for the text
        ImGui::SetCursorPos(ImVec2(((windowWidth+uiText.width)/2) - (textSize.x/2), ((windowHeight+uiText.height)/2) - (textSize.y/2)));
        ImGui::SetWindowFontScale((windowWidth/1920)*uiText.sizeText);

        //Draw text
        ImGui::TextColored(colorI,"%s", uiText.text.c_str());

        endWindow();
    }
}

void UiController::addDamageMark(float x, float z) {
    damageMarks.emplace_back(&uiImages[DAMAGE_MARK_IMAGE], x, z, DAMAGE_MARK_DUR);
}

void UiController::update(float dt) {
    updateDamageMarks(dt);
}

void UiController::updateDamageMarks(float dt) {
    for(auto mark {damageMarks.begin()}; mark < damageMarks.end(); ++mark) {
        // Update display time
        mark->duration -= dt;

        if(mark->duration <= 0) {
            // Remove damage mark
            *mark = damageMarks.back();

            damageMarks.pop_back();
            --mark;
        }
    }
}

//Reset data
void UiController::reset(){
    infectionValue = -1,
    infectionValueOld = -1;

    infectionValueTime = 0;

    splatterspec_w.clear();
    splatterspec_h.clear();
    splatterspec_x.clear();
    splatterspec_y.clear();

    splatterAmount = 0;

    damageMarks.clear();
}