#pragma once

#include <cstdint>
#include <vector>
#include <stdio.h>

#define START_MENU      0
#define OPTIONS_MENU    1
#define OPTIONS2_MENU   2
#define OPTIONS3_MENU   3
#define CREDITS_MENU    4
#define PAUSE_MENU      5
#define CONTROLS_MENU   6
#define DEFEAT_MENU     7
#define WIN_MENU        8

#define NORMAL_OPTION   0
#define SLIDER_OPTION   1
#define CHECKBOX_OPTION 2

struct menu {
    menu();
    menu(int p_id,int p_no, std::vector<const char*> p_mn);
    menu(int p_id,int p_no, std::vector<const char*> p_mn, std::vector<int> p_ot);
    menu(int p_id,int p_no, std::vector<const char*> p_mn, std::vector<int> p_ot, std::vector<float> p_ov);
    menu(int p_id, int p_no, std::vector<const char*> p_mn, std::vector<int> p_ot, std::vector<float> p_ov
        , std::vector<float> p_ovmx, std::vector<float> p_ovmn);
    menu(int p_id, int p_no, std::vector<const char*> p_mn, std::vector<int> p_ot, std::vector<float> p_ov
        , std::vector<float> p_ovmx, std::vector<float> p_ovmn, std::vector<float> p_ovg);
    ~menu();

    void update(int action, int option);

    int id;

    int menuAction;
    int optionSelected;

    int totalOptions;
    std::vector<const char*> menuNames;
    std::vector<int> optionType;
    std::vector<float> optionValue;

    std::vector<float> optionValueMin;
    std::vector<float> optionValueMax;
    std::vector<float> optionGap;
};