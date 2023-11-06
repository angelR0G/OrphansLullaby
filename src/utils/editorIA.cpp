#include "editorIA.hpp"

#include "waypoint.hpp"
#include "../engine/entityFactory.hpp"
#include "../engine/graphic/engine.hpp"

#include <IMGUI/imgui.h>
#include <IMGUI/imgui_impl_glfw.h>
#include <IMGUI/imgui_impl_opengl3.h>

#define EDITOR_WINDOW_POS_X (windowWidth)-690
#define EDITOR_WINDOW_POS_Y 40

#define FONT_SCALE_AI_EDITOR_TITLE      (windowWidth/1920)*0.25
#define FONT_SCALE_AI_EDITOR_CONTENT    (windowWidth/1920)*0.725
#define FONT_SCALE_AI_EDITOR_SELECTABLE (windowWidth/1920)*0.3
#define FONT_SCALE_AI_EDITOR_HELP_TEXT  (windowWidth/1920)*0.15

EditorIA::EditorIA(){
    engine3d = GraphicEngine::Instance();

    // Save initial window size
    auto windowSize {engine3d->getWindowSize()};
    windowWidth     = windowSize.width;
    windowHeight    = windowSize.height;
}

void EditorIA::helpMarker(const char* desc, float scale){
    ImGui::TextDisabled("[?]");
    if(ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)){
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize()*50.0f);
        ImGui::TextUnformatted(desc);
        ImGui::SetWindowFontScale(scale);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

// strg_vector: vector of strings containing all selectable options
// select_op:   index of selectable
// scale:       font scale for options of selectable
// text:        text appearing at right of the selectable
size_t EditorIA::selectable(std::vector<std::string> strg_vector, size_t select_opt, float scale, const char* text){
    if(ImGui::BeginCombo(text, strg_vector[select_opt].c_str())){
        ImGui::SetWindowFontScale(scale);
        for (size_t n = 0; n < strg_vector.size(); n++){
            const bool is_selected = (select_opt == n);
            if (ImGui::Selectable(strg_vector[n].c_str(), is_selected))
                select_opt = n;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    return select_opt;
}

void EditorIA::checkForEnemiesAlive(EntityMan &EM, std::vector<IAParameters>* enemies_alive, int filter){
    using CList = MetaP::Typelist<AIComponent>;
    using TList = MetaP::Typelist<EnemyNormalTag, EnemyExplosiveTag, EnemyThrowerTag, EnemyRugbyTag>;

    EM.foreachOR<CList, TList>([&](Entity& e){
        AIComponent*            ai       = &EM.getComponent<AIComponent>(e);
        AttackStatsComponent*   atckstat = &EM.getComponent<AttackStatsComponent>(e);
        HealthComponent*        hp       = &EM.getComponent<HealthComponent>(e);

        std::string enemyTg = "Enemy Normal";
        if(e.template hasTag<EnemyExplosiveTag>()){
            enemyTg = "Enemy Explosive";
        }
        else if(e.template hasTag<EnemyThrowerTag>()){
            enemyTg = "Enemy Thrower";
        }
        else if(e.template hasTag<EnemyRugbyTag>()){
            enemyTg = "Enemy Rugby";
        }

        size_t ent_id = e.getId();

        // check if entity is already inside entities_alive vector
        bool alreadyIn {false};
        size_t i;

        if(enemies_alive->size()>0){
            for(i=0; i<enemies_alive->size() && !alreadyIn; i++){
                if(enemies_alive->at(i).entity_id == ent_id){
                    alreadyIn = true;
                }
            }
        }
        
        //0 -> All | 1 -> Normal | 2 -> Explosive | 3 -> Thrower | 4-> Rugby
        if(filter==0 || (filter==1 && e.template hasTag<EnemyNormalTag>())      || 
                        (filter==2 && e.template hasTag<EnemyExplosiveTag>())   || 
                        (filter==3 && e.template hasTag<EnemyThrowerTag>())     ||
                        (filter==4 && e.template hasTag<EnemyRugbyTag>())       )
        {
            NavigationComponent* navcmp = &EM.getComponent<NavigationComponent>(e);
            std::string pathids = getNavPathIds(navcmp);
            if(!alreadyIn){
                IAParameters iaparams {ent_id, ai->behaviour, &ai->arrivalRadius, true, enemyTg, true, &atckstat->basic_dmg, &atckstat->basic_cd, &atckstat->special_dmg, &atckstat->special_cd, &hp->infection, pathids};
                enemies_alive->push_back(iaparams);
            }
            else{
                IAParameters iaparams {ent_id, ai->behaviour, &ai->arrivalRadius, true, enemyTg, enemies_alive->at(i-1).modified, &atckstat->basic_dmg, &atckstat->basic_cd, &atckstat->special_dmg, &atckstat->special_cd, &hp->infection, pathids};
                enemies_alive->at(i-1) = iaparams;
            }
        }
    });
}

std::string EditorIA::getNavPathIds(NavigationComponent* navcmp){
    std::string ids {"Navigation path: \n"};

    Waypoint_t * wp;

    // search all waypoints and get their ids in one string
    for(size_t i=0; i<navcmp->path.size(); i++){
        wp = navcmp->path.at(i);

        ids += std::to_string(wp->id) + ": " +  std::to_string(wp->x) + " | " +
                                                std::to_string(wp->y) + " | " + 
                                                std::to_string(wp->z);

        if(i<navcmp->path.size()-1)
            ids += "\n";
    }

    if(ids=="Navigation path: \n"){
        ids = "Navigation path is empty";
    }

    return ids;
}

void EditorIA::checkModifiedValue(EntityMan& EM, bool* modified, Entity& enemy){
    if(&enemy!=nullptr){
        if(*modified==false){
            if(!enemy.hasTag<EnemyExecuteBT>())
                EM.addTag<EnemyExecuteBT>(enemy);       // Add executeBT tag to make ai execute behaviour tree
        }
        else{
            if(enemy.hasTag<EnemyExecuteBT>())
                EM.removeTag<EnemyExecuteBT>(enemy);    // Remove executeBT to make ai don't execute behaviour tree
        }
    }
}

void EditorIA::showAttackStats(IAParameters* enemy){
    if( enemy->enenmy_type == "Enemy Normal"    || 
        enemy->enenmy_type == "Enemy Thrower"   || 
        enemy->enenmy_type == "Enemy Rugby"     ){
            ImGui::SliderFloat(" Basic Attack DMG",   enemy->basic_dmg,   0.1f, 100.0f);
            ImGui::SliderFloat(" Basic Attack CD",    enemy->basic_cd,    0.1f, 10.0f);
    }

    if( enemy->enenmy_type == "Enemy Explosive" || 
        enemy->enenmy_type == "Enemy Thrower"   || 
        enemy->enenmy_type == "Enemy Rugby"     ){
            ImGui::SliderFloat(" Special Attack DMG", enemy->special_dmg, 0.1f, 100.0f);
            ImGui::SliderFloat(" Special Attack CD",  enemy->special_cd,  0.1f, 50.0f);
    }

    ImGui::SliderFloat(" Infection DMG", enemy->infection, 0.0f, 3.0f);
}

void EditorIA::updateEditorIA(EntityMan& EM, int max_enemies, std::vector<IAParameters>* enemies_alive, bool showEditor){
    if(showEditor){
        EntityFactory* factory = EntityFactory::Instance();

        // Search for enemies alive using filter option
        checkForEnemiesAlive(EM, enemies_alive, filter_enemy);

        ImGuiWindowFlags window_flags = 0;
        bool * open_ptr = nullptr;
        static float f = 0.0f;
        float   width   { (float)engine3d->getWindowSize().width/3 }, 
                height  { (float)(engine3d->getWindowSize().height*10)/15 };

        // Create Window for ai editor
        ImGui::Begin("Editor in-game IA", open_ptr, window_flags);
        ImGui::SetWindowSize(ImVec2(width, height));
        ImGui::SetWindowPos(ImVec2(EDITOR_WINDOW_POS_X, EDITOR_WINDOW_POS_Y));
        ImGui::SetWindowFontScale(FONT_SCALE_AI_EDITOR_TITLE);

        std::string enemiesAliveText = std::to_string(enemies_alive->size()) + " enemies alive:";

        filter_enemy = selectable(enemy_types, filter_enemy, FONT_SCALE_AI_EDITOR_CONTENT, " Enemy filter");

        ImGui::Text(enemiesAliveText.c_str());

        float spacing {30.0};
        float ent_window_w{width-spacing}, ent_window_h{325};
        for(unsigned int it=0; it<enemies_alive->size(); it++){
            // Get enemy entity with his id
            Entity& enemy = *EM.getEntityById(enemies_alive->at(it).entity_id);

            std::string id          = "Entity ID: " + std::to_string(enemies_alive->at(it).entity_id);

            ImVec4 typeEnemyColor {0.733f, 0.0f, 1.0f, 1.0f};

            ImGui::BeginChild(id.c_str(), ImVec2(ent_window_w, ent_window_h), true, window_flags);
            ImGui::SetWindowFontScale(FONT_SCALE_AI_EDITOR_CONTENT);
                // Color text for type of enemy
                ImGui::TextColored(typeEnemyColor, enemies_alive->at(it).enenmy_type.c_str());

                // Checkbox for Modify enemy
                bool lastModified = enemies_alive->at(it).modified;
                if(ImGui::Checkbox("Execute Behaviour Tree", &enemies_alive->at(it).modified)){
                    checkModifiedValue(EM, &enemies_alive->at(it).modified, enemy);
                }

                ImGui::SameLine();
                helpMarker("Don't check it to change among steerings behaviours\n", FONT_SCALE_AI_EDITOR_HELP_TEXT);

                // Text for enemy entity id
                ImGui::Text(id.c_str());

                // Selectable for steering behaviour
                size_t sb_select = selectable(sb_types, (int)enemies_alive->at(it).steering_bv, FONT_SCALE_AI_EDITOR_SELECTABLE, " Str Behaviour");
                if(&enemy!=nullptr && enemy.hasComponent<AIComponent>()){
                    AIComponent* aicmp = &EM.getComponent<AIComponent>(enemy);
                    aicmp->behaviour = static_cast<SB>(sb_select);
                }

                //helpMarker("Ayudita\n", FONT_SCALE_AI_EDITOR_SELECTABLE);

                // Slider float for arrival radius
                ImGui::SliderFloat(" Arrival Radius", enemies_alive->at(it).arrival_radius, 0.0f, 20.0f);

                // Show all attack stats of that enemy
                showAttackStats(&enemies_alive->at(it));

                // Show help and navigation path of the enemy
                helpMarker("Waypoint structure\nWaypoint id: X | Y | Z\n", FONT_SCALE_AI_EDITOR_HELP_TEXT);
                ImGui::SameLine();
                ImGui::Text(enemies_alive->at(it).pathIDs.c_str());

                ImGui::SetWindowPos(ImVec2(ent_window_w, ent_window_h));
                spacing += ent_window_h+10;
            ImGui::EndChild();
        }

        ImGui::End();

        for(unsigned int i=0; i<enemies_alive->size(); i++){
            if(enemies_alive->at(i).alive){
                enemies_alive->at(i).alive = false;
            }
            else{
                enemies_alive->erase(enemies_alive->begin()+i);
            }
        }
    }
}