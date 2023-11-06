#include "menuController.hpp"

#include <chrono>
#include "../engine/graphic/engine.hpp"
#include "../engine/graphic/uiEngine.hpp"
#include "../IrrlichtFacade/image.hpp"
#include "../manager/gamemanager.hpp"

using c_clock = std::chrono::steady_clock;
#define UPDATE_MENU_TIME 0.13
#define SLEEP_MENU_TIME 0.002
#define OPTIONS_PATH "options.txt"

// Cinematics parameters
#define CINEMATIC_VIDEO_LOGO    "media/cinematics/logo/LogoFin000.jpg"
#define CINEMATIC_SOUND_LOGO    "event:/Cinematicas/Logo Pandaemic"
#define CINEMATIC_SPEED_LOGO    60.f
#define CINEMATIC_VIDEO_INTRO   "media/cinematics/cinematica_inicial/General000.jpg"
#define CINEMATIC_SOUND_INTRO   "event:/Cinematicas/Cinematica Inicial"
#define CINEMATIC_SPEED_INTRO   13.f
#define CINEMATIC_VIDEO_END     "media/cinematics/cinematica_final/Cinematica_Final000.jpg"
#define CINEMATIC_SOUND_END     "event:/Cinematicas/Cinematica Final"
#define CINEMATIC_SPEED_END     12.f
#define CINEMATIC_VIDEO_CREDITS "media/cinematics/creditos/Creditos000.jpg"
#define CINEMATIC_SOUND_CREDITS "event:/Música/Musica Creditos"
#define CINEMATIC_SPEED_CREDITS 15.f

menuController* menuController::Instance(){
    if(pmenuController.get() ==  0){
        pmenuController = std::unique_ptr<menuController>(new menuController());
    }

    return pmenuController.get();
}

//Constructor
menuController::menuController(){
    initialize();

    //Get a pinter to ui instance
    uiController = UiController::Instance();

    // Get a pointer to graphic engine
    graphicEngine = GraphicEngine::Instance();
}

//Destructor
menuController::~menuController(){
}

void menuController::initialize(){
    //Read options document
    readOptionsDocument();
    //Check if values are valid
    valueValidation();

    menu startMenu{START_MENU,4,
                    {"JUGAR","OPCIONES","CRÉDITOS","SALIR"}};
    //id_menu,optionsNumber,optionsNames,optionsTypes,optionsValues,optionsMaxValues,optionsMinValues,optionsGaps

    //Sound
    menu optionsMenu{OPTIONS_MENU,7,
                    {"SONIDO",mc.optionsName[0],mc.optionsName[1],mc.optionsName[2],mc.optionsName[3],mc.optionsName[4],"VOLVER"},
                    {NORMAL_OPTION,SLIDER_OPTION,SLIDER_OPTION,SLIDER_OPTION,SLIDER_OPTION,SLIDER_OPTION,NORMAL_OPTION},
                    {0,mc.soundTotal,mc.soundMusic,mc.soundVoices,mc.soundAmbient,mc.soundEffects,0},
                    {0,mc.opValueMax[0],mc.opValueMax[1],mc.opValueMax[2],mc.opValueMax[3],mc.opValueMax[4],0},
                    {0,0,0,0,0,0,0},
                    {0,5,5,5,5,5,0}};

    //Control
    menu options2Menu{OPTIONS2_MENU,6,
                    {"CONTROLES",mc.optionsName[5],mc.optionsName[6],mc.optionsName[7],"VER CONTROLES","VOLVER"},
                    {NORMAL_OPTION,SLIDER_OPTION,SLIDER_OPTION,CHECKBOX_OPTION,NORMAL_OPTION,NORMAL_OPTION},
                    {0,mc.sensibilityX,mc.sensibilityY,mc.sprintAlternate,0,0},
                    {0,mc.opValueMax[5],mc.opValueMax[6],mc.opValueMax[7],0,0},
                    {0,0,0,0,0,0},
                    {0,5,5,1,0,0}};

    //Graphic
    menu options3Menu{OPTIONS3_MENU,9,
                    {"GRÁFICOS",mc.optionsName[8],mc.optionsName[9],mc.optionsName[10],mc.optionsName[11],mc.optionsName[12],mc.optionsName[13],mc.optionsName[14],"VOLVER"},
                    {NORMAL_OPTION,SLIDER_OPTION,CHECKBOX_OPTION,SLIDER_OPTION,CHECKBOX_OPTION,CHECKBOX_OPTION,SLIDER_OPTION,SLIDER_OPTION,NORMAL_OPTION},
                    {0,mc.fov,mc.particles,mc.particlesQuantity,mc.shadows,mc.shadowsSmooth,mc.shadowsQuality,mc.renderDistance,0},
                    {0, mc.opValueMax[8],mc.opValueMax[9],mc.opValueMax[10],mc.opValueMax[11],mc.opValueMax[12],mc.opValueMax[13],mc.opValueMax[14],0},
                    {0, mc.opValueMin[8],0,0,0,0,0,mc.opValueMin[14],0},
                    {0,2.5,1,1,1,1,1,1,0}};
    menu creditsMenu{CREDITS_MENU,1,{"VOLVER"}};
    menu pauseMenu{PAUSE_MENU,3,
                    {"REANUDAR","OPCIONES","MENÚ PRINCIPAL"}};
    menu controlsMenu{CONTROLS_MENU,1,{"VOLVER"}};
    menu defeatMenu{DEFEAT_MENU,1,{"MENÚ PRINCIPAL"}};
    menu winMenu{WIN_MENU,1,{"MENÚ PRINCIPAL"}};
    menus.push_back(startMenu);
    menus.push_back(optionsMenu);
    menus.push_back(options2Menu);
    menus.push_back(options3Menu);
    menus.push_back(creditsMenu);
    menus.push_back(pauseMenu);
    menus.push_back(controlsMenu);
    menus.push_back(defeatMenu);
    menus.push_back(winMenu);

    currentMenu = menus[0];
    numberOptions = currentMenu.totalOptions;
    optionSelected = currentMenu.optionSelected;
    id = currentMenu.id;

    sys_input.initializeInput();
}

void menuController::readOptionsDocument(){
    optionsFile.open (OPTIONS_PATH);
    std::string line;
    size_t pos = 0, lines = 0;

    if(optionsFile.is_open())
    while(optionsFile){
        std::getline (optionsFile, line);
        std::string optionNameData;
        std::string optionValueData;
        // Find prefix and sufix between the value
        std::size_t prefix = line.find('<');
        std::size_t sufix = line.find('>');
        // Check if both characters exist in string
        if( prefix != std::string::npos && sufix != std::string::npos)
        {
            // Get name of option
            optionNameData = line.substr(0, prefix - 1);
            // Get value of option
            optionValueData = line.substr(prefix + 1, sufix - prefix - 1);

            /*
            // Check if option in document exist in game and change it
            bool found = false;
            for(size_t i = 0; i<mc.optionsValue.size() && !found;i++){
                if(optionNameData == mc.optionsName[i]){
                    *mc.optionsValue[i] = stof(optionValueData);
                    found = true;
                }
            }
            */
            //Change value in order
            if(mc.optionsValue.size()>pos)
                *mc.optionsValue[pos] = stof(optionValueData);
            pos++;
        }
        lines++;
    }

    // Delete file if content is invalid
    if(pos!=mc.optionsValue.size()) std::remove(OPTIONS_PATH);

    optionsFile.close();
}

void menuController::writeOptionsDocument(){
    // Write or replace options file
    optionsFile.open (OPTIONS_PATH,std::ofstream::out | std::ofstream::trunc);
    for(size_t i = 0; i<mc.optionsValue.size();i++)
        optionsFile<< mc.optionsName[i] << "<" << *mc.optionsValue[i] <<">\n";
    optionsFile.close();
}

// Validate value of option
// If value is invalid, restore to default
void menuController::valueValidation(){
    bool badValue = false;
    for(size_t i = 0; i<mc.optionsValue.size();i++)
        if(*mc.optionsValue[i]>mc.opValueMax[i]||*mc.optionsValue[i]<mc.opValueMin[i])
            badValue = true;

    if(badValue){
        for(size_t i = 0; i<mc.optionsValue.size();i++)
            *mc.optionsValue[i] = mc.opValueDefault[i];
    }
}

int menuController::update(int action, int option, bool ingame){
    optionSelected = option;
    
    if(stateChange==0)
        updateOption(action, option, ingame);

    //Reset value
    int auxRes = stateChange;
    stateChange = 0;

    return auxRes;
}

int menuController::updateOption(int action, int option, bool ingame){
    int res = 0;

    switch (id)
    {
    case START_MENU:
        switch (action)
        {
        case MENU_BACK:
            if(!escapeValid) break;

            res+=10;
            break;
        case MENU_ENTER:
            if(!spaceValid) break;

            switch (option)
            {
            case 1:
                ++res;
                break;
            case 2:
                changeToMenu(menus[OPTIONS_MENU]);
                break;
            case 3:
                ++res;
                changeToMenu(menus[CREDITS_MENU]);
                break;
            case 4:
                res+=10;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        break;
    case PAUSE_MENU:
        switch (action)
        {
        case MENU_BACK:
            if(!escapeValid) break;

            ++res;
            break;
        case MENU_ENTER:
            if(!spaceValid) break;

            switch (option)
            {
            case 1:
                ++res;
                break;
            case 2:
                changeToMenu(menus[OPTIONS_MENU]);
                break;
            case 3:
                res+=10;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        break;
    case OPTIONS_MENU:
    case OPTIONS2_MENU:
    case OPTIONS3_MENU:
        switch (action)
            {
            case MENU_BACK:
                if(!escapeValid) break;

                changeToMenu(menus[mainMenu]);
                break;
            case MENU_ENTER:
                if(!spaceValid) break;

                if(option==1) alternateOptionsMenu(MENU_RIGHT);
                if(id==OPTIONS2_MENU && option==numberOptions-1) changeToMenu(menus[CONTROLS_MENU]);
                if(option==numberOptions) changeToMenu(menus[mainMenu]);
                else{
                    //Alternate checkbox value
                    if(currentMenu.optionType[option-1] == CHECKBOX_OPTION){
                        currentMenu.optionValue[option-1]=!currentMenu.optionValue[option-1];
                        updateMenuConfigOption(currentMenu.optionValue[option-1]);
                    //On slider, increment value
                    }else if(currentMenu.optionType[option-1] == SLIDER_OPTION){
                        float value     = currentMenu.optionValue[option-1];
                        float valueMax  = currentMenu.optionValueMax[option-1];
                        float leap      = currentMenu.optionGap[option-1];

                        float valueMin  = currentMenu.optionValueMin[option-1];

                        if(value+leap<=valueMax) value+= leap;
                        else                     value= valueMin;
                        currentMenu.optionValue[option-1] = value;

                        updateMenuConfigOption(value);
                    }
                }
                break;
            case MENU_LEFT:
                if(option==1) alternateOptionsMenu(MENU_LEFT);
                else{
                    if(currentMenu.optionType[option-1] == SLIDER_OPTION){
                        
                        float value     = currentMenu.optionValue[option-1];
                        float valueMin  = currentMenu.optionValueMin[option-1];
                        float leap      = currentMenu.optionGap[option-1];

                        float valueMax  = currentMenu.optionValueMax[option-1];

                        if(value-leap>=valueMin) value-= leap;
                        else                     value= valueMax;
                        currentMenu.optionValue[option-1] = value;

                        updateMenuConfigOption(value);
                    }
                }
                break;
            case MENU_RIGHT:
                if(option==1) alternateOptionsMenu(MENU_RIGHT);
                else{
                    if(currentMenu.optionType[option-1] == SLIDER_OPTION){
                        float value     = currentMenu.optionValue[option-1];
                        float valueMax  = currentMenu.optionValueMax[option-1];
                        float leap      = currentMenu.optionGap[option-1];

                        float valueMin  = currentMenu.optionValueMin[option-1];

                        if(value+leap<=valueMax) value+= leap;
                        else                     value= valueMin;
                        currentMenu.optionValue[option-1] = value;

                        updateMenuConfigOption(value);
                    }
                }
                break;
            default:
                break;
            }
        break;
    case CREDITS_MENU:
            switch (action)
            {
            case MENU_BACK:
                if(!escapeValid) break;
                [[fallthrough]];
            case MENU_ENTER:
                if(!spaceValid) break;

                changeToMenu(menus[mainMenu]);
                break;
            default:
                break;
            }
    break;
    case CONTROLS_MENU:
         switch (action)
            {
            case MENU_BACK:
                if(!escapeValid) break;
                [[fallthrough]];
            case MENU_ENTER:
                if(!spaceValid) break;

                changeToMenu(menus[OPTIONS2_MENU]);
                break;
            default:
                break;
            }
    break;
    case DEFEAT_MENU:
    case WIN_MENU:
            switch (action)
            {
            case MENU_BACK:
                if(!escapeValid) break;
                [[fallthrough]];
            case MENU_ENTER:
                if(!spaceValid) break;

                res+=10;
                break;
            default:
                break;
            }
    break;
    default:
        break;
    }
    stateChange = res;
    return stateChange;
}

void menuController::updateMenuConfigOption(float nvalue){
    updateMenuConfigOption(nvalue, optionSelected);
};

void menuController::updateMenuConfigOption(float nvalue, int optSel){
    switch (id)
    {
    case OPTIONS_MENU://Sound
        switch (optSel)
            {
            case 2:
                mc.soundTotal = nvalue;
                break;
            case 3:
                mc.soundMusic = nvalue;
                break;
            case 4:
                mc.soundVoices = nvalue;
                break;
            case 5:
                mc.soundAmbient = nvalue;
                break;
            case 6:
                mc.soundEffects = nvalue;
                break;
            default:
                break;
            }
        break;
    case OPTIONS2_MENU://Control
        switch (optSel)
            {
            case 2:
                mc.sensibilityX = nvalue;
                break;
            case 3:
                mc.sensibilityY = nvalue;
                break;
            case 4:
                mc.sprintAlternate = nvalue;
                break;
            default:
                break;
            }
        break;
    case OPTIONS3_MENU://Graphic
        switch (optSel)
            {
            case 2:
                mc.fov = nvalue;
                break;
            case 3:
                mc.particles = nvalue;
                break;
            case 4:
                mc.particlesQuantity = nvalue;
                break;
            case 5:
                mc.shadows = nvalue;
                break;
            case 6:
                mc.shadowsSmooth = nvalue;
                break;
            case 7:
                mc.shadowsQuality = nvalue;
                break;
            case 8:
                mc.renderDistance = nvalue;
                break;
            default:
                break;
            }
        break;
    default:
        break;
    }

    if(ingame){
        GameManager* gm = GameManager::Instance();
        if(gm!=nullptr)
            gm->updateConfigGame();
    }else{
        updateConfig();
    }

    //Write options document
    writeOptionsDocument();
};

void menuController::updateConfig(){
    menuConfig config  = mc;

    //Check config values before applying
    valueValidation();

    if(graphicEngine!=nullptr){
        //Fov
        graphicEngine->setFov(config.fov);

        //Particles
        graphicEngine->setParticlesQuality((bool)config.particles,config.particlesQuantity);

        //Shadows
        graphicEngine->enableShadows((bool)config.shadows);
        graphicEngine->setSmoothShadows((bool)config.shadowsSmooth);
        graphicEngine->setShadowQuality(config.shadowsQuality);

        //Render distance
        graphicEngine->setRenderDistance(config.renderDistance/100);
    }

    if(soundEngine!=nullptr){
        soundEngine->setVCAVolume("vca:/VCA-Master",config.soundTotal/100);
        soundEngine->setVCAVolume("vca:/VCA-Efectos",config.soundEffects/100);
        soundEngine->setVCAVolume("vca:/VCA-Musica",config.soundMusic/100);
        soundEngine->setVCAVolume("vca:/VCA-Dialogos",config.soundVoices/100);
        soundEngine->setVCAVolume("vca:/VCA-Ambiente",config.soundAmbient/100);
    }
}


void menuController::alternateOptionsMenu(int move){
    //OPTIONS3_MENU OPTIONS_MENU OPTIONS2_MENU
    auto changeto = 0;
    switch (currentMenu.id)
    {
    case OPTIONS_MENU:
        if(move==MENU_LEFT) changeto = OPTIONS3_MENU;
        else                changeto = OPTIONS2_MENU;

        break;
    case OPTIONS2_MENU:
        if(move==MENU_LEFT) changeto = OPTIONS_MENU;
        else                changeto = OPTIONS3_MENU;

        break;
    case OPTIONS3_MENU:
        if(move==MENU_LEFT) changeto = OPTIONS2_MENU;
        else                changeto = OPTIONS_MENU;

        break;
    
    default:
        break;
    }
    changeToMenu(menus[changeto]);
}

int menuController::sidesOptionsMenu(int move){
    //OPTIONS3_MENU OPTIONS_MENU OPTIONS2_MENU
    auto changeto = 0;
    switch (currentMenu.id)
    {
    case OPTIONS_MENU:
        if(move==MENU_LEFT) changeto = OPTIONS3_MENU;
        else                changeto = OPTIONS2_MENU;

        break;
    case OPTIONS2_MENU:
        if(move==MENU_LEFT) changeto = OPTIONS_MENU;
        else                changeto = OPTIONS3_MENU;

        break;
    case OPTIONS3_MENU:
        if(move==MENU_LEFT) changeto = OPTIONS2_MENU;
        else                changeto = OPTIONS_MENU;

        break;
    
    default:
        break;
    }
    return changeto;
}

void menuController::changeToMenu(menu menuc){
    //Save current values
    menus[getCurrentMenuID()].optionValue = currentMenu.optionValue;

    //Change
    currentMenu = menuc;
    id = currentMenu.id;
    numberOptions = currentMenu.totalOptions;
    optionSelected = currentMenu.optionSelected;
    optionHovered = 0;
    sideHover     = 0;
    escapeValid = false;
    spaceValid  = false;
}

void menuController::changeToMenu(int menuid){
    //Change
    menu menuc = menus[menuid];
    changeToMenu(menuc);
}

int menuController::getNumberOptions(){
    return numberOptions;
}

int menuController::getCurrentOption(){
    return optionSelected;
}

int menuController::getOptionHovered(){
    return optionHovered;
}

int menuController::getCurrentMenuID(){
    return id;
}

std::vector<const char*> menuController::getCurrentNames(){
    return currentMenu.menuNames;
}

int menuController::getType(int num){
    int res = 0;
    if((int)currentMenu.optionType.size()>num) res = currentMenu.optionType[num];
    return res;
}

float menuController::getValue(int num){
    float res = 0;
    if((int)currentMenu.optionValue.size()>num) res = currentMenu.optionValue[num];
    return res;
}

float menuController::getMaxValue(int num){
    float res = 0;
    if((int)currentMenu.optionValue.size()>num) res = currentMenu.optionValueMax[num];
    return res;
}


float menuController::getMinValue(int num){
    float res = 0;
    if((int)currentMenu.optionValue.size()>num) res = currentMenu.optionValueMin[num];
    return res;
}


float* menuController::getValuePointer(int num){
    float* res = nullptr;
    if((int)currentMenu.optionValue.size()>num) res = &currentMenu.optionValue[num];
    return res;
}

float menuController::getGapValue(int num){
    float res = 0;
    if((int)currentMenu.optionValue.size()>num) res = currentMenu.optionGap[num];
    return res;
}

menuConfig menuController::getMenuConfig(){
    return mc;
}

const char* menuController::getFirstName(int menuid){
    return menus[menuid].menuNames[0];
}

int menuController::run(int menuId) {
    uiController->createImages();
    updateConfig();

    if(menuId==START_MENU) setIngame(false);
    else                   setIngame(true);
    
    escapeValid = false;
    spaceValid  = false;

    // Open menu
    changeToMenu(menuId);

    // Enable mouse motion
    graphicEngine->captureMouse(false);

    int state{0};
    c_clock::time_point oldTime {c_clock::now()};
    float time{0};
    //Action and option
    std::pair<int, int> actionOption;

    while(state == 0 && graphicEngine->runEngine()) {
        // Calculate delta time
        auto now        = c_clock::now();
        float deltaTime = std::min(std::chrono::duration_cast<std::chrono::microseconds>(now - oldTime).count() / 1000000.0f, 0.1f);
        oldTime         = now;

        int auxOpt  {optionSelected};
        int auxMenu {id};

        
        if (time >= UPDATE_MENU_TIME) {
            // Control keys
            if(!escapeValid && !sys_input.keyPress(ESCAPE))                                 escapeValid = true;
            if(!spaceValid  && !(sys_input.keyPress(SPACE) | sys_input.keyPress(ENTER)))    spaceValid  = true;

            //Get action done in menu
            actionOption = sys_input.updateMenu(numberOptions, auxOpt);
            //Play sound
            menuSounds(actionOption, auxOpt);


            //Check action in menu
            state += update(actionOption.first, actionOption.second, ingame);

            if(actionOption.first != 0 || actionOption.second != auxOpt)
                time -= UPDATE_MENU_TIME;
        }
        else {
            time += deltaTime;
        }
            
        //Draw menu
        UiEngine* uiEngine = graphicEngine->getUiEngine();
        uiEngine->ImGui_prerender();
        sys_render.drawMenu(*graphicEngine, ingame);
        uiEngine->ImGui_postrender();
    }    

    //Draw loading screen when game starts
    if(id==START_MENU && state == 1){
        soundEngine->stopMenuSound("event:/Música/Musica Menu Principal");
        UiEngine* uiEngine = graphicEngine->getUiEngine();

        //Preload
        uiEngine->ImGui_prerender();
        sys_render.drawLoadingScreen(*graphicEngine);
        uiEngine->ImGui_postrender();

        //Load
        uiEngine->ImGui_prerender();
        sys_render.drawLoadingScreen(*graphicEngine);
        uiEngine->ImGui_postrender();
    }

    // Play credits
    if(id==CREDITS_MENU) {
        playCinematic(CREDITS_CINEMATIC);
        state = 0;
    }

    return state;
}

void menuController::setIngame(bool b_p){
    ingame = b_p;
    if(ingame) mainMenu = PAUSE_MENU;
    else       mainMenu = START_MENU;
}

void menuController::setOptionSelected(int o_p){
    if(o_p > 0 && o_p <= currentMenu.totalOptions)
        optionSelected = o_p;
}

void menuController::setOptionHovered(int o_p){
    if((optionHovered != o_p)){
        if(o_p!=0){
            soundEngine->playMenuSound(true, MENU_MOVE);
        }
    }
    if(o_p > -1 && o_p <= currentMenu.totalOptions)
        optionHovered = o_p;
}

void menuController::setValue(int o_p, float nvalue){
    if(o_p > 0 && o_p <= currentMenu.totalOptions &&
    nvalue <= currentMenu.optionValueMax[o_p] && nvalue >= currentMenu.optionValueMin[o_p]){
        currentMenu.optionValue[o_p] = nvalue;
        updateMenuConfigOption(nvalue,o_p+1);
    }
}

void menuController:: setSoundEngine(SoundEngine* sE){
    soundEngine = sE;
    cineManager.initSoundEngine(sE);
}

void menuController:: menuSounds(actionOption actOpt, int auxOp){
    if(actOpt.second != auxOp)
        soundEngine->playMenuSound(true, MENU_MOVE);
    else if((actOpt.first == MENU_LEFT || actOpt.first == MENU_RIGHT) && getType(actOpt.second-1) == SLIDER_OPTION)
        soundEngine->playMenuSound(true, MENU_MOVE);
    else if((actOpt.first == MENU_LEFT || actOpt.first == MENU_RIGHT) && actOpt.second == 1
             && (id==OPTIONS_MENU || id==OPTIONS2_MENU || id==OPTIONS3_MENU))
        soundEngine->playMenuSound(true, MENU_MOVE);
    if(actOpt.first == MENU_ENTER)
        soundEngine->playMenuSound(true, MENU_ACCEPT);
    if(actOpt.first == MENU_BACK)
        soundEngine->playMenuSound(true, MENU_REFUSE);
}
void menuController:: setSideHover(int sideH){
    if(sideHover!=sideH){
        if(sideH!=0){
            hoverSound(sideH);        
        }
    }
    sideHover=sideH;
}


void menuController:: hoverSound(int op){
    soundEngine->playMenuSound(true, MENU_MOVE);
}

void menuController:: clickSound(){
    soundEngine->playMenuSound(true, MENU_ACCEPT);

}

void menuController::updateLastGameStats(const GameStatistics& s) noexcept {
    lastGameStats = s;
}

GameStatistics menuController::getLastGameStats() noexcept {
    return lastGameStats;
}


void menuController::playCinematic(size_t index) noexcept {
    if(     index == 0) cineManager.play(CINEMATIC_VIDEO_LOGO,      CINEMATIC_SPEED_LOGO,       CINEMATIC_SOUND_LOGO);
    else if(index == 1) cineManager.play(CINEMATIC_VIDEO_CREDITS,   CINEMATIC_SPEED_CREDITS,    CINEMATIC_SOUND_CREDITS);
    else if(index == 2) cineManager.play(CINEMATIC_VIDEO_INTRO,     CINEMATIC_SPEED_INTRO,      CINEMATIC_SOUND_INTRO);
    else if(index == 3) cineManager.play(CINEMATIC_VIDEO_END,       CINEMATIC_SPEED_END,        CINEMATIC_SOUND_END);
}