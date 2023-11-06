#include "menu.hpp"

menu::menu(int p_id, int p_no, std::vector<const char*> p_mn) {
    menuAction = 0;
    optionSelected = 1;

    id           = p_id;
    totalOptions = p_no;
    menuNames    = p_mn;
    for(size_t i = 0; i<p_mn.size();i++){
        optionType.push_back(0);
    }
}

menu::menu(int p_id, int p_no, std::vector<const char*> p_mn, std::vector<int> p_ot) {
    menuAction = 0;
    optionSelected = 1;

    id           = p_id;
    totalOptions = p_no;
    menuNames    = p_mn;
    optionType   = p_ot;

    for(size_t i = 0; i<p_mn.size();i++){
        float defaultValue = 0;
        if(optionType[i]==SLIDER_OPTION)        defaultValue = 100;
        else if(optionType[i]==CHECKBOX_OPTION) defaultValue = 1;

        optionValue.push_back(defaultValue);
        optionValueMax.push_back(defaultValue);
        optionValueMin.push_back(0);

        if(optionValue[i]<=1)
            optionGap.push_back(1);
        else
            optionGap.push_back(5);
    }
}

menu::menu(int p_id, int p_no, std::vector<const char*> p_mn, std::vector<int> p_ot, std::vector<float> p_ov) {
    menuAction = 0;
    optionSelected = 1;

    id           = p_id;
    totalOptions = p_no;
    menuNames    = p_mn;
    optionType   = p_ot;
    optionValue  = p_ov;

    for(size_t i = 0; i<p_mn.size();i++){
        optionValueMax.push_back(p_ov[i]);
        optionValueMin.push_back(0);

        optionGap.push_back(optionValueMax[i]/100);
    }
}

menu::menu(int p_id, int p_no, std::vector<const char*> p_mn, std::vector<int> p_ot, std::vector<float> p_ov
        , std::vector<float> p_ovmx, std::vector<float> p_ovmn) {
    menuAction = 0;
    optionSelected = 1;

    id           = p_id;
    totalOptions = p_no;
    menuNames    = p_mn;
    optionType   = p_ot;
    optionValue  = p_ov;
    optionValueMax  = p_ovmx;
    optionValueMin  = p_ovmn;

    for(size_t i = 0; i<p_mn.size();i++){
        optionGap.push_back(optionValueMax[i]/100);
    }
}

//id, totaloptions, names, optiontypes, optionvalues, optionmaxvalues, optionminvalues, optiongaps
menu::menu(int p_id, int p_no, std::vector<const char*> p_mn, std::vector<int> p_ot, std::vector<float> p_ov
        , std::vector<float> p_ovmx, std::vector<float> p_ovmn, std::vector<float> p_ovg) {
    menuAction = 0;
    optionSelected = 1;

    id           = p_id;
    totalOptions = p_no;
    menuNames    = p_mn;
    optionType   = p_ot;
    optionValue  = p_ov;
    optionValueMax  = p_ovmx;
    optionValueMin  = p_ovmn;
    optionGap    = p_ovg;
}

menu::menu() {
}

menu::~menu() {
    menuNames.clear();
    optionType.clear();
    optionValue.clear();

    optionValueMin.clear();
    optionValueMax.clear();
    optionGap.clear();
}

void menu::update(int action, int option){
    
}