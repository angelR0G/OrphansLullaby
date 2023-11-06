#pragma once

#include <memory>
#include <vector>
#include <glad/glad.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/mat4x4.hpp>

#include "frustrumPlane.hpp"

#include "particles/emitterTypes.hpp"

// Forward declaration
struct GLFWwindow;
struct ResourceManager;
struct MaterialResource;
struct ShaderResource;
struct TextureResource;
struct InputController;
struct UiEngine;

namespace GE {
    struct SceneCamera;
    struct SceneLight;
    struct SceneMesh;
    struct SceneParticleEmitter;
    struct SceneEntity;
    struct SceneNode;
    struct SpatialTree;
}

struct GraphicEngine {
    friend GE::SceneCamera;
    friend GE::SceneLight;
    friend GE::SceneNode;

    struct Size2D {
        int width,
            height;
    };
    
    struct SunLight {
        glm::vec3   color {1.0, 1.0, 1.0};
        glm::vec3   direction {0.5, 1.0, 0.5};
        glm::vec3   referencePos {0, 0, 0};
        float       intensity {0.1};
    };

    static GraphicEngine* Instance();

    ~GraphicEngine();

    void render();
    void preRender();
    void postRender();
    
    bool runEngine();
    void captureMouse(bool);
    bool getCaptureMouse();
    
    void setCursorPosition(float, float);

    [[nodiscard]] Size2D getWindowSize() const noexcept;

    [[nodiscard]] UiEngine* getUiEngine() const noexcept;
    

    // Scene entities methods
    GE::SceneLight*             addSceneLight(
                                    GE::SceneEntity* parent = nullptr);
    GE::SceneCamera*            addSceneCamera(
                                    GE::SceneEntity* parent = nullptr);
    GE::SceneMesh*              addSceneMesh(
                                    GE::SceneEntity* parent = nullptr);
    GE::SceneEntity*            addDefaultSceneEntity(
                                    GE::SceneEntity* parent = nullptr);
    GE::SceneParticleEmitter*   addParticleEmitter(
                                    ParticleEmitterType type,
                                    GE::SceneEntity* parent = nullptr);
    void loadSceneMesh(std::string);
    void loadMaterial(std::string);
    void loadAnimation(std::string, std::string);

    void                removeSceneLight(         GE::SceneLight* light,        bool destroyChildren = false);
    void                removeSceneCamera(        GE::SceneCamera* camera,      bool destroyChildren = false);
    void                removeSceneMesh(          GE::SceneMesh*  mesh,         bool destroyChildren = false);
    void                removeParticleEmitter(    GE::SceneParticleEmitter* e,  bool destroyChildren = false);
    void                removeSceneDefaultEntity( GE::SceneEntity* entity,      bool destroyChildren = false);
    
    MaterialResource*   getMaterial(std::string);

    void deleteScene();
    void freeAllResources();
    
    void setAmbientLightning(glm::vec3, float) noexcept;
    void setSunlight(glm::vec3, glm::vec3, float) noexcept;
    void setBoneTransform(GLint, glm::mat4);
    void updateLightning(float, float, float, float) noexcept;

    void setFov(float) noexcept;
    void setRenderDistance(float) noexcept;
    void enableShadows(bool) noexcept;
    void setSmoothShadows(bool) noexcept;
    
	void setShadowQuality(size_t);
    void setParticlesQuality(bool, float) noexcept;

    void draw3DLine(float, float, float, float, float, float) noexcept;
    void updateSpatialTree() noexcept;

    void playVideo(const std::string&, float fps = 30.f) noexcept;
    bool updateVideoFrame(float) noexcept;
    void stopVideo() noexcept;
    
    private:
        GraphicEngine();
        inline static std::unique_ptr<GraphicEngine> engine{nullptr};

        GLFWwindow*         window;
        InputController*    inputCont;
        bool                engineRunning{true};

        // ui manager
        std::unique_ptr<UiEngine> uiEngine{nullptr};

        // Resource manager pointer
        ResourceManager* resourceMan{nullptr};

        // Scene elements
        std::unique_ptr<GE::SceneNode>  sceneRoot {nullptr};
        std::unique_ptr<GE::SpatialTree>renderTreeRoot {nullptr};
        std::vector<GE::SceneEntity*>   renderEntities {};
        
        GE::SceneCamera*                activeCamera{nullptr};
        std::vector<GE::SceneCamera*>   sceneCameras{};
        std::vector<GE::SceneLight*>    sceneLights{};
        // Lights that are currently rendering scene
        std::vector<GE::SceneLight*>    renderLights{};

        struct LightUniforms {
            GLint   uPosition {-1},
                    uColor {-1},
                    uIntensity {-1},
                    uType {-1},
                    uAttenuationDistance {-1},
                    uDirection {-1},
                    uInnerAngle {-1},
                    uOuterAngle {-1},
                    uCastShadows {-1};
        };
        struct SunLightUniforms {
            GLint   uColor {-1},
                    uDirection {-1},
                    uIntensity {-1},
                    uCastShadows {-1};
        };

        // Scene ambient and sun lightning
        glm::vec3   ambientColor {1.0, 1.0, 1.0};
        float       ambientIntensity {0.1};
        SunLight    sunLight;

        // Uniform locations
        GLint   uModel{-1},
                uModelInv{-1}, 
                uView{-1}, 
                uProj{-1},
                uCamPos{-1},
                uAmbientLight{-1};
        std::vector<LightUniforms> uLights;
        SunLightUniforms uSunLight;

        // Uniform locations for particles shader
        GLint   uModelPart{-1},
                uViewPart{-1},
                uProjPart{-1},
                uAmbientPart{-1};
        
        // Shadows variables
        bool    castShadows     {true},
                smoothShadows   {true};
        int     shadowsGenerated{3};
        unsigned int	sunDepthMapSize	{},
						depthCubeMapSize{},
						depthMapSize 	{}; 
        std::vector<unsigned int> depthMapBuffers       {};
        std::vector<unsigned int> depthMapTextures      {};
        std::vector<unsigned int> depthCubeMapBuffers   {};
        std::vector<unsigned int> depthCubeMapTextures  {};
        GLint               uShadowLM           {-1},
                            uCastShadow         {-1},
                            uPointLightPos      {-1},
                            uPointLightFarPlane {-1},
                            uNumLights          {-1},
                            uNumShadowsGenerated{-1};
        std::vector<GLint>  uLightTrans         {},
                            uPointLightTrans    {},
                            uDepthMaps          {},
                            uDepthCubeMaps      {},
                            uArePointLights     {};

        // Debug variables
        GLint   uDebugView  {-1},
                uDebugProj  {-1};
        unsigned int    debugVAO {},
                        debugVBO {};

        //Uniform locations for skybox
        GLint   uViewSky        {-1},
                uProjSky        {-1},
                uSkyboxTexture  {-1};

        std::vector<FrustrumPlane> cameraFrustrum{};

        // Video variables
        std::unique_ptr<TextureResource> videoFrame{};
        unsigned int videoVAO {};
        std::string videoPath {};
        float       videoTime {};
        float       videoFPS {30.f};

        // Private methods
        void initGraphicEngine();
        void initShaders();
        void initShadersSkyBox();
        void initParticlesShaders();
        void initShadowsShaders();
        void initDebugShaders();
        void initVideoShaders();
        void initUi();
        void initVideoPlayer();
        void initDepthMapsBuffers();
        void initFrustrumCulling();
		void freeDepthMapsBuffers();
        void freeDebugBuffers();

        void renderSceneEntities(ShaderResource* shader, bool useFrustum = true) const noexcept;
        void generateDepthMap();
        void sendDepthMapToShader();
        bool setupShadowMatrices(size_t);
        void sendCubeMap(ShaderResource*, size_t);

        void calculateCameraFrustrun();
        void calculateLightFrustum(const GE::SceneLight* light);

        GE::SceneNode*  addSceneNode(GE::SceneNode*);
        void            deleteSceneEntity(GE::SceneEntity*, bool);
        void            deleteLight(GE::SceneLight*);
        void            deleteCamera(GE::SceneCamera*);
        static void     framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void     close_window_callback(GLFWwindow* window);
        void            sendAmbientLightToShader() noexcept;
        void            sendSunLightToShader() noexcept;
        void            sendLightDataToShader(float, float, float) noexcept;
        std::vector<GE::SceneLight*> getClosestLights(float, float, float, float) const noexcept;
        void            drawVideoFrame() noexcept;
        std::string     nextFramePath(size_t, size_t) noexcept;

        ///////////////////////
        //Skybox
        //
        unsigned int skyboxVAO{}, skyboxVBO{}, skyboxTexture{};
        //Methods
        void initSkyBox();
        void drawSkyBox();
        void freeSkyBox();
};