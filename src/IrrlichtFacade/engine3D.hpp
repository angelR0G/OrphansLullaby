#pragma once

#include <irrlicht/irrlicht.h>
#include <cstdint>
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>
#include <wchar.h>

#include <IrrIMGUI/IncludeIrrlicht.h>
#include <IrrIMGUI/IncludeIMGUI.h>
#include <IrrIMGUI/IrrIMGUI.h>
#include <IrrIMGUI/IrrIMGUIDebug.h>

#include "text.hpp"
#include "../engine/inputController.hpp"

struct engine3D{
    public:
        using DestructorFunc = void (*)(irr::IrrlichtDevice*);
        using irrDevice = std::unique_ptr<irr::IrrlichtDevice, DestructorFunc>;
        
        static engine3D* engineInstance(uint32_t const w, uint32_t const h);
        static engine3D* engineInstance();

        bool run() const;

        void beginScene(std::vector<int> colorBackground = {255, 100, 101, 140});
        void endScene();

        void beginDraw(float);
        void endDraw();
        void drawAll();
        void setEventReceiver(inputController* er);
        void draw3DLine(float x1, float y1, float z1, float x2, float y2, float z2, uint32_t r, uint32_t g, uint32_t b);

        irr::video::IVideoDriver* getVideoDriver();
        irr::scene::ISceneManager* getSceneManager();
        IrrIMGUI::IIMGUIHandle* getIMGUIhandle();
        irrDevice* getDevice();
        std::vector<uint32_t> getWindowSize();

        void hideCursor();
        void drawText(std::string text, float, float, std::vector<int>);
        void drawTextCenter(std::string, float, float, std::vector<int>);
        void drawTextIMGUI(std::string text, std::vector<float>, float, float);
        void drawTextCenterIMGUI();
        void setTextCenterIMGUI(std::string text, std::vector<float>, float, float, float, bool acitve=true);
        void activeText(bool);
        void createSkyBox();
        std::string formatText(std::string, std::vector<char> replaces, std::vector<std::string> substitutes);
        ~engine3D();

    protected:
        engine3D(uint32_t const w, uint32_t const h, irr::SIrrlichtCreationParameters, IrrIMGUI::CIMGUIEventReceiver);
        
    private:
        inline static std::unique_ptr<engine3D> pengine3D{nullptr};

        static void destroy(irr::IrrlichtDevice* p) {
            p->drop();
        }
        
        Text uiText;

        uint32_t const width_{}, height_{};
        // irrDevice device_ {
        // irr::createDevice( irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(width_, height_), 16,
        //     false, false, false, 0), destroy};
        irrDevice device_{nullptr, destroy};
        // irr::video::IVideoDriver*  const driver_ { device_ ? device_->getVideoDriver()    : nullptr};
        // irr::scene::ISceneManager* const smgr_   { device_ ? device_->getSceneManager()   : nullptr };
        // irr::gui::IGUIEnvironment* const guienv_ { device_ ? device_->getGUIEnvironment() : nullptr };
        irr::scene::ISceneNode* skybox;
        irr::video::IVideoDriver*  driver_; 
        irr::scene::ISceneManager* smgr_  ;
        irr::gui::IGUIEnvironment* guienv_; 

        //Imgui
        IrrIMGUI::IIMGUIHandle* pGUI;
        IrrIMGUI::CIMGUIEventReceiver EventReceiver;
        irr::SIrrlichtCreationParameters IrrlichtParams;
};