#include "engine.hpp"

// OpenGL headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/trigonometric.hpp>
#include <glm/ext.hpp>
#include <stdexcept>

#include "sceneTree/sceneNode.hpp"
#include "sceneTree/sceneCamera.hpp"
#include "sceneTree/sceneLight.hpp"
#include "sceneTree/sceneMesh.hpp"
#include "sceneTree/scenePartEmitter.hpp"

#include "resource/resourceManager.hpp"
#include "resource/meshresource.hpp"
#include "resource/programresource.hpp"
#include "resource/shaderresource.hpp"
#include "resource/materialresource.hpp"

#include "particles/particleEmitter.hpp"

#include "inputController.hpp"
#include "uiEngine.hpp"
#include "SpatialTree.hpp"
#include "../uiController.hpp"

// Macros
#define WINDOW_INIT_WIDTH   1920
#define WINDOW_INIT_HEIGHT  1080
#define WINDOW_NAME         "Orphan's Lullaby"

#define SHADERS_NUM_LIGHTS              6
#define SHADER_POINT_LIGHT_ID           1
#define SHADER_DIRECCTIONAL_LIGHT_ID    2
#define SHADER_SPOTLIGHT_ID             3

#define SUN_DEPTH_MAP_SIZE_ULTRA    4096
#define DEPTH_CUBEMAP_SIZE_ULTRA    1024
#define DEPTH_MAP_SIZE_ULTRA        1024
#define NUM_SHADOWS_ULTRA           SHADERS_NUM_LIGHTS
#define SUN_DEPTH_MAP_SIZE_HIGH     4096
#define DEPTH_CUBEMAP_SIZE_HIGH     512
#define DEPTH_MAP_SIZE_HIGH         512
#define NUM_SHADOWS_HIGH            4
#define SUN_DEPTH_MAP_SIZE_LOW      2048
#define DEPTH_CUBEMAP_SIZE_LOW      512
#define DEPTH_MAP_SIZE_LOW          512
#define NUM_SHADOWS_LOW             2
#define PI 3.1415926

using namespace GE;

const std::string shadersPath = "media/shaders/";
const std::vector<std::string> shadersFile = {
    "vertex.glsl",              //0 ->Regular vertex shader
    "fragmentPBR.glsl",         //1 ->Regular fragment shader
    "vertexSkyBox.glsl",        //2 ->vertex shader for rendering the skybox
    "fragmentSkyBox.glsl",      //3 ->fragment shader for rendering the skybox
    "vertexParticles.glsl",     //4 ->vertex shader for particles
    "fragmentParticles.glsl",   //5 ->fragment shader for particles
    "vertexShadow.glsl",        //6 ->vertex shader shadows
    "fragmentShadow.glsl",      //7 ->fragment shader shadows
    "vertexPointShadow.glsl",   //8 ->vertex shader for point lights shadows
    "fragmentPointShadow.glsl", //9 ->fragment shader for point lights shadows
    "geometryPointShadow.glsl", //10->geometry shader for point lights shadows
    "vertexDebug.glsl",         //11->vertex shader for debug (render debug lines)
    "fragmentDebug.glsl",       //12->fragment shader for debug (render debug lines)
    "vertexVideo.glsl",         //13->vertex shader for video
    "fragmentVideo.glsl",       //14->fragment shader for video
    "vertexBlur.glsl",          //15->vertex shader for blur calculations
    "fragmentBlur.glsl",        //16->fragment shader for blur calculations
    "vertexBloom.glsl",         //17->vertex shader for bloom
    "fragmentBloom.glsl",       //18->fragment shader for bloom
    "new_downsample.vs",        //19->
    "new_downsample.fs",        //20->
    "new_upsample.vs",          //21->
    "new_upsample.fs",          //22->
};

#define SHADER(file) shadersPath + file

//Cubemap for the skybox
static const float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};

//Path for skybox textures
static const std::string skyPath = "media/maps/skybox/";
static const std::vector<std::string> skyBoxFaces{
    skyPath + "right.jpg",
    skyPath + "left.jpg",
    skyPath + "top.jpg",
    skyPath + "bottom.jpg",
    skyPath + "front.jpg",
    skyPath + "back.jpg"
};

void GraphicEngine::framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);

    // Update input controller window size
    engine->inputCont->setScreenSize(width, height);

    // Update UI controller window size
    UiController::Instance()->setWindowSize(width, height);
}

void GraphicEngine::close_window_callback(GLFWwindow* window) {
    GraphicEngine::Instance()->engineRunning = false;
}

GraphicEngine* GraphicEngine::Instance(){
    if(engine.get() == 0){
        engine = std::unique_ptr<GraphicEngine>(new GraphicEngine());
    }
    return engine.get();
}

GraphicEngine::GraphicEngine() {
    sceneRoot = std::unique_ptr<SceneNode>(new SceneNode());
    SceneNode* sceneRootNode {sceneRoot.get()};
    sceneRootNode->sEntity = std::unique_ptr<SceneEntity>(new SceneEntity());
    sceneRootNode->sEntity->setTreeNode(sceneRootNode);

    renderTreeRoot = std::unique_ptr<SpatialTree>(new SpatialTree());

    // Get pointer to input controller
    inputCont = InputController::Instance();

    // Get pointer to resource manager
    resourceMan = ResourceManager::Instance();

    initGraphicEngine();

    initShaders();

    initUi();

    initSkyBox();

    // Init depth map sizes
    sunDepthMapSize     = SUN_DEPTH_MAP_SIZE_HIGH;
    depthCubeMapSize    = DEPTH_CUBEMAP_SIZE_HIGH;
    depthMapSize        = DEPTH_MAP_SIZE_HIGH;
    initDepthMapsBuffers();

    initFrustrumCulling();

    initVideoPlayer();

    // Update default lightning
    updateLightning(0, 0, 0, 0);
}

GraphicEngine::~GraphicEngine() {}

void GraphicEngine::initGraphicEngine() {
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    //Create window
    window = glfwCreateWindow(WINDOW_INIT_WIDTH, WINDOW_INIT_HEIGHT, WINDOW_NAME, NULL, NULL);
    if(window == NULL){
        glfwTerminate();
        throw std::runtime_error("Error on window Inizialization");
    }
    glfwMakeContextCurrent(window);

    // Get window size
    Size2D windowSize {getWindowSize()};

    //Initialize GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        throw std::runtime_error("Error on glad Inizialization");
    }

    // Create viewport
    glViewport(0, 0, windowSize.width, windowSize.height);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    //Activate backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Activate depth test
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    // Input config
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Bind callback functions
    glfwSetFramebufferSizeCallback( window, framebuffer_size_callback);
    glfwSetKeyCallback(             window, InputController::process_keyboard);
    glfwSetCursorPosCallback(       window, InputController::process_mouse);
    glfwSetMouseButtonCallback(     window, InputController::process_mouse_buttons);
    glfwSetScrollCallback(          window, InputController::process_mouse_scroll);
    glfwSetWindowCloseCallback(     window, close_window_callback);

    // Update input controller window size
    inputCont->setScreenSize(windowSize.width, windowSize.height);
}

void GraphicEngine::initFrustrumCulling(){
    cameraFrustrum.resize(6);
}

void GraphicEngine::initUi() {
    uiEngine = std::unique_ptr<UiEngine>(new UiEngine(window));
}

void GraphicEngine::initDepthMapsBuffers() {
    // Create buffers
    depthMapBuffers.resize(     SHADERS_NUM_LIGHTS + 1);
    depthMapTextures.resize(    SHADERS_NUM_LIGHTS + 1);
    depthCubeMapBuffers.resize( SHADERS_NUM_LIGHTS);
    depthCubeMapTextures.resize(SHADERS_NUM_LIGHTS);
    const float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    for(size_t i{0}; i <= SHADERS_NUM_LIGHTS; ++i) {
        // Create buffer
        glGenFramebuffers(1, &depthMapBuffers[i]);

        // Create texture
        glGenTextures(1, &depthMapTextures[i]);
        glBindTexture(GL_TEXTURE_2D, depthMapTextures[i]);
        if(i==0)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, sunDepthMapSize, sunDepthMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depthMapSize, depthMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        // Bind texture with buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapBuffers[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTextures[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Create depth cubemap
        if(i < SHADERS_NUM_LIGHTS){
            // Create buffer
            glGenFramebuffers(1, &depthCubeMapBuffers[i]);
            glGenTextures(1, &depthCubeMapTextures[i]);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMapTextures[i]);
            
            // Generate a texture foreach cube face
            for(unsigned int face{}; face < 6; ++face){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT, depthCubeMapSize, depthCubeMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            }

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            // Attach depth texture as depth buffer
            glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapBuffers[i]);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMapTextures[i], 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
}

void GraphicEngine::initVideoPlayer() {
    // Create video texture
    videoFrame = std::unique_ptr<TextureResource>(new TextureResource());

    // Quad where video will be rendered
    const float videoQuad[] = {
    // Position             // UV
    -1.0f,  -1.0f,  0.0f,   0.0f, 1.0f,
    -1.0f,  1.0f,   0.0f,   0.0f, 0.0f,
    1.0f,   -1.0f,  0.0f,   1.0f, 1.0f,
    1.0f,   1.0f,   0.0f,   1.0f, 0.0f
    }; 

    // Generate array buffer
    unsigned int videoVBO;
    glGenVertexArrays(1, &videoVAO);
    glGenBuffers(1, &videoVBO);
    glBindVertexArray(videoVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, videoVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(videoQuad), videoQuad, GL_STREAM_DRAW);
    
    glEnableVertexAttribArray(0);
    // Vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // Vertex UV
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)12);
    glBindVertexArray(0);
}

bool GraphicEngine::runEngine() {
    return engineRunning;
}

void GraphicEngine::captureMouse(bool capture) {
    if(capture)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Reset cursor position
    inputCont->setCursorPosition(window, 0.5f, 0.5f);
}

bool GraphicEngine::getCaptureMouse(){
    bool    cursorCaptured  = false;
    int     currentMode        = glfwGetInputMode(window, GLFW_CURSOR);
    
    if(currentMode == GLFW_CURSOR_DISABLED){
        cursorCaptured = true;
    }
    
    return cursorCaptured;
}

void GraphicEngine::preRender() {
    uiEngine->ImGui_prerender();
}

void GraphicEngine::postRender() {
    uiEngine->ImGui_postrender();
}

void GraphicEngine::render(){
    if(activeCamera == nullptr) return;

    ShaderResource* vShader     = resourceMan->getShader(SHADER(shadersFile[0]),  GL_VERTEX_SHADER);
    ShaderResource* vShaderPart = resourceMan->getShader(SHADER(shadersFile[4]),  GL_VERTEX_SHADER);
    ShaderResource* vShaderDebug= resourceMan->getShader(SHADER(shadersFile[11]), GL_VERTEX_SHADER);
    
    generateDepthMap();
    
    // Activate program
    glUseProgram(resourceMan->getProgram(PROGRAM_DEFAULT)->getProgramId());

    sendDepthMapToShader();

    // Get projection and view matrix
    glm::mat4 view = activeCamera->getViewMatrix();
    glm::mat4 proj = activeCamera->getProjectionMatrix();

    // Get camera position
    glm::vec3 camPos = activeCamera->getTranslation();

    // Send matrices to vertex shader
    vShader->setUniformMat4(uView, view);
    vShader->setUniformMat4(uProj, proj);
    vShader->setUniformVec3(uCamPos, camPos);

    // Send these uniforms to other shaders that need them
    glUseProgram(resourceMan->getProgram(PROGRAM_PARTICLES)->getProgramId());
    vShaderPart->setUniformMat4(uViewPart, view);
    vShaderPart->setUniformMat4(uProjPart, proj);
    glUseProgram(resourceMan->getProgram(PROGRAM_DEBUG_LINES)->getProgramId());
    vShaderDebug->setUniformMat4(uDebugView, view);
    vShaderDebug->setUniformMat4(uDebugProj, proj);
    glUseProgram(resourceMan->getProgram(PROGRAM_DEFAULT)->getProgramId());

    //Calculate camera frustrum
    calculateCameraFrustrun();

    // Draw scene
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderSceneEntities(vShader);
    
    //Render skybox at last
    drawSkyBox();
    renderBloom();
    
}

void GraphicEngine::renderBloom(){
    ShaderResource* vShaderBloom= resourceMan->getShader(SHADER(shadersFile[17]), GL_VERTEX_SHADER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    renderBloomTexture(colorBuffers[1], bloomFilterRadius);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    unsigned int bloomFinalProgram = resourceMan->getProgram(PROGRAM_BLOOM)->getProgramId();
    glUseProgram(bloomFinalProgram);
    glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    glUniform1i(glGetUniformLocation(bloomFinalProgram, "scene"), 0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);

    // PH_BLOOM
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(bloomFinalProgram, "bloomBlur"), 1);
    glBindTexture(GL_TEXTURE_2D, mMipChain[0].texture);

    vShaderBloom->setUniformFloat(glGetUniformLocation(resourceMan->getProgram(PROGRAM_BLOOM)->getProgramId(), "exposure"), 10.0f);
    renderQuad();
    glActiveTexture(GL_TEXTURE0);
}

void GraphicEngine::renderSceneEntities(ShaderResource* shader, bool useFrustum) const noexcept {
    // Render entities from render tree
    renderTreeRoot->render(shader, cameraFrustrum, useFrustum);

    // Render other entities from render vector
    for(auto e : renderEntities) {
        e->draw(e->getTransformMatrix(), shader);
    }
}

void GraphicEngine::generateDepthMap() {
    // Do not do anything if shadows are disabled
    if(!castShadows) return;

    // Activate shadows shader to render depth map texture
    glUseProgram(resourceMan->getProgram(PROGRAM_SHADOWS)->getProgramId());

    // Change culling faces
    glCullFace(GL_FRONT);
    
    // Calculate depth map for each light and save it on a texture
    for(size_t i{0}; i < depthMapBuffers.size() && i <= renderLights.size() && static_cast<int>(i) <= shadowsGenerated; ++i) {
        bool isPointLight{false};    

        // First iteration, sun light, then other scene lights
        if(i != 0){
            GE::SceneLight* light = renderLights[i-1];
            if(light->getType() == 1)   isPointLight = true;
            if(light->getCastShadows()) calculateLightFrustum(light);
        }
        
        if(isPointLight){
            glUseProgram(resourceMan->getProgram(PROGRAM_POINT_SHADOWS)->getProgramId());
            glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapBuffers[i-1]);
        }else{
            glUseProgram(resourceMan->getProgram(PROGRAM_SHADOWS)->getProgramId());
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapBuffers[i]);
        }
        
        // Render scene if light matrices setup correctly
        if(setupShadowMatrices(i)) {
            glClear(GL_DEPTH_BUFFER_BIT);
            if(isPointLight) {
                // Point light shadow calculation
                ShaderResource* vShaderPointShad    = resourceMan->getShader(SHADER(shadersFile[8]), GL_VERTEX_SHADER);
                renderSceneEntities(vShaderPointShad);
            }
            else {
                // Directional/spot light shadow calculation
                ShaderResource* vShaderShad         = resourceMan->getShader(SHADER(shadersFile[6]), GL_VERTEX_SHADER);
                if(i == 0)  renderSceneEntities(vShaderShad, false);
                else        renderSceneEntities(vShaderShad);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Return viewport to original size
    Size2D windowSize {getWindowSize()};
    glViewport(0, 0, windowSize.width, windowSize.height);

    // Return culling faces to original value
    glCullFace(GL_BACK);
}

bool GraphicEngine::setupShadowMatrices(size_t index) {
    bool matricesReady {true};
    ShaderResource* vShader     = resourceMan->getShader(SHADER(shadersFile[0]), GL_VERTEX_SHADER);
    ShaderResource* vShaderShad = resourceMan->getShader(SHADER(shadersFile[6]), GL_VERTEX_SHADER);

    if(index == 0) {
        glViewport(0, 0, sunDepthMapSize, sunDepthMapSize);
        // Send sun light info
        glm::mat4 lightProjection {glm::ortho(-400.0, 400.0, -400.0, 400.0, 10.0, 3000.0)};
        glm::mat4 lightView {glm::lookAt(glm::vec3{sunLight.direction.x * 2000, sunLight.direction.y * 2000, sunLight.direction.z * 2000} + sunLight.referencePos,
                                        sunLight.referencePos,
                                        {0.0, 1.0, 0.0})};
        glm::mat4 lightTransform {lightProjection * lightView};

        glUseProgram(resourceMan->getProgram(PROGRAM_DEFAULT)->getProgramId());
        vShader->setUniformMat4(uLightTrans[index], lightTransform);
        glUseProgram(resourceMan->getProgram(PROGRAM_SHADOWS)->getProgramId());
        vShaderShad->setUniformMat4(uShadowLM, lightTransform);
    }
    else {
        // Adapt index to renderLights range
        index--;

        // Check if there is more lights data to be sent
        if(index >= renderLights.size()) return false;

        // Get light source
        GE::SceneLight* light = renderLights[index];

        // If does not cast shadows, stop
        if(!light->getCastShadows()) return false;

        // Depending on the type of light, different data is required in the shaders
        if(light->getType() == 1){
            //Point light
            ShaderResource* fShaderPShad = resourceMan->getShader(SHADER(shadersFile[9]), GL_FRAGMENT_SHADER);
            ShaderResource* gShaderPShad = resourceMan->getShader(SHADER(shadersFile[10]), GL_GEOMETRY_SHADER);

            glViewport(0, 0, depthCubeMapSize, depthCubeMapSize);
            float farPlane          = light->getRadius();
            glm::mat4 shadowProj    = glm::perspective(glm::radians(90.0f), 1.f, 1.0f, farPlane);
            glm::vec3 lightPos      = light->getWorldLocation();

            // Save light transform matrices for projecting points in shaders
            std::vector<glm::mat4> shadowTransforms {};
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
            shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
            
            // Send data to shaders
            glUseProgram(resourceMan->getProgram(PROGRAM_POINT_SHADOWS)->getProgramId());
            for(unsigned int i{}; i < 6; ++i)
                gShaderPShad->setUniformMat4(uPointLightTrans[i], shadowTransforms[i]);

            fShaderPShad->setUniformVec3(uPointLightPos, lightPos);
            fShaderPShad->setUniformFloat(uPointLightFarPlane, farPlane);

        }else{
            //Spot light
            glViewport(0, 0, depthMapSize, depthMapSize);
            // Send spot light info
            float fov {glm::radians(light->getInnerAngle() + light->getOuterAngle())};
            glm::vec3 up{0, 1, 0};
            glm::vec3 lightDir = light->getDirection();
            glm::vec3 lightPos = light->getTranslation();

            // Create a projection matrix from light data
            glm::mat4 lightProjection {glm::perspective(fov, 1.f, 1.f, light->getRadius())};

            // Modify up vector in specific case to prevent errors
            if(std::abs(lightDir.y) >= 0.999) up = {0.01, 1, 0};

            // Create a view matrix from lights point of view
            glm::mat4 lightView {glm::lookAt(lightPos, lightPos + lightDir, up)};

            // Combine projection and view matrix
            glm::mat4 lightTransform {lightProjection * lightView};

            // Send data to shaders
            glUseProgram(resourceMan->getProgram(PROGRAM_DEFAULT)->getProgramId());
            vShader->setUniformMat4(uLightTrans[index+1], lightTransform);

            glUseProgram(resourceMan->getProgram(PROGRAM_SHADOWS)->getProgramId());
            vShaderShad->setUniformMat4(uShadowLM, lightTransform);
        }
    }

    return true;
}

void GraphicEngine::sendDepthMapToShader() {
    ShaderResource* fShader     = resourceMan->getShader(SHADER(shadersFile[1]), GL_FRAGMENT_SHADER);
    glUseProgram(resourceMan->getProgram(PROGRAM_DEFAULT)->getProgramId());

    //Send number of lights
    fShader->setUniformInt(uNumLights, renderLights.size());
    fShader->setUniformInt(uNumShadowsGenerated, shadowsGenerated);

    //Send sun light depth map
    fShader->setUniformInt(uDepthMaps[0], 5);
    fShader->setUniformInt(uArePointLights[0], 0);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, depthMapTextures[0]);

    //Send the rest of lights
    size_t it {0};
    while(it < renderLights.size() && static_cast<int>(it) < shadowsGenerated) {
        GE::SceneLight* light = renderLights[it];
        if(light->getType() != 1){
            //Spot light
            int texturePadding {6};
            fShader->setUniformInt(uDepthMaps[it+1],         it+texturePadding);
            fShader->setUniformInt(uArePointLights[it+1],    0);
            glActiveTexture(GL_TEXTURE0 + it + texturePadding);
            glBindTexture(GL_TEXTURE_2D, depthMapTextures[it+1]);
        }
        sendCubeMap(fShader, it);

        ++it;
    }

    // Send remaining cubemaps
    while(it < SHADERS_NUM_LIGHTS) {
        sendCubeMap(fShader, it);

        ++it;
    }
    glActiveTexture(GL_TEXTURE0);
}

void GraphicEngine::sendCubeMap(ShaderResource* fShader, size_t i) {
    fShader->setUniformInt(uDepthCubeMaps[i], 15 + i);
    glActiveTexture(GL_TEXTURE15 + i);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMapTextures[i]);
}

void GraphicEngine::calculateCameraFrustrun(){
    std::pair<float, float> nearFar = activeCamera->getNearFarPlanes();
    float fov                       = glm::radians(activeCamera->getCamAngle());
    float aspect                    = activeCamera->getWindowAspect();

    glm::vec3 camRotation   = activeCamera->getWorldRotation();
    glm::vec3 camPostion    = activeCamera->getWorldLocation();
    camRotation.x *= -1;
    camRotation.y *= -1;
    camRotation.y -= PI/2;

    glm::mat3 rotationy = {cos(camRotation.y), 0, -sin(camRotation.y), 0, 1, 0, sin(camRotation.y), 0, cos(camRotation.y)};
    glm::mat3 rotationx = {cos(camRotation.x), sin(camRotation.x), 0, -sin(camRotation.x), cos(camRotation.x), 0, 0, 0, 1};

    //Calculate camera vectors
    glm::vec3 cameraFront = glm::vec3(1.0f, 0.0f, 0.0f) * rotationx * rotationy;
    cameraFront = glm::normalize(cameraFront);
    glm::vec3 cameraUp    = activeCamera->getUpVector();
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

	const float halfVSide = nearFar.second * tanf(fov * .5f);
	const float halfHSide = halfVSide * aspect;
	const glm::vec3 frontMultFar = nearFar.second * cameraFront;

    //Calculate all frustrum planes
	cameraFrustrum[FRUSTRUM_NEAR_FACE  ] = { camPostion + nearFar.first * cameraFront, cameraFront };                      // Near face
	cameraFrustrum[FRUSTRUM_FAR_FACE   ] = { camPostion + frontMultFar, -cameraFront };                                    // Far face
	cameraFrustrum[FRUSTRUM_RIGHT_FACE ] = { camPostion, glm::cross(frontMultFar - cameraRight * halfHSide, cameraUp) };   // Right face
	cameraFrustrum[FRUSTRUM_LEFT_FACE  ] = { camPostion, glm::cross(cameraUp, frontMultFar + cameraRight * halfHSide) };   // Left face
	cameraFrustrum[FRUSTRUM_TOP_FACE   ] = { camPostion, glm::cross(cameraRight, frontMultFar - cameraUp * halfVSide) };   // Top face
	cameraFrustrum[FRUSTRUM_BOTTOM_FACE] = { camPostion, glm::cross(frontMultFar + cameraUp * halfVSide, cameraRight) };   // Bottom face
}

void GraphicEngine::calculateLightFrustum(const GE::SceneLight* light){
    // Check if point light
    if(light->getType() == LIGHT_TYPE_SPOT) {
        std::pair<float, float> nearFar = {1, light->getRadius()};
        float fov        = glm::radians(light->getInnerAngle() + light->getOuterAngle());
        float aspect     = 1.0;

        glm::vec3 camPosition = light->getWorldLocation();
        glm::vec3 cameraFront = light->getDirection();
        glm::vec3 cameraUp    = {0, 1, 0};
        if(std::abs(cameraFront.y) >= 0.999) cameraUp.x = 0.01;
        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

        const float halfVSide = nearFar.second * tanf(fov * .5f);
        const float halfHSide = halfVSide * aspect;
        const glm::vec3 frontMultFar = nearFar.second * cameraFront;

        //Calculate all frustrum planes
        cameraFrustrum[FRUSTRUM_NEAR_FACE   ] = { camPosition + nearFar.first * cameraFront, cameraFront                    };  // Near face
        cameraFrustrum[FRUSTRUM_FAR_FACE    ] = { camPosition + frontMultFar, -cameraFront                                  };  // Far face
        cameraFrustrum[FRUSTRUM_RIGHT_FACE  ] = { camPosition, glm::cross(frontMultFar - cameraRight * halfHSide, cameraUp) };  // Right face
        cameraFrustrum[FRUSTRUM_LEFT_FACE   ] = { camPosition, glm::cross(cameraUp, frontMultFar + cameraRight * halfHSide) };  // Left face
        cameraFrustrum[FRUSTRUM_TOP_FACE    ] = { camPosition, glm::cross(cameraRight, frontMultFar - cameraUp * halfVSide) };  // Top face
        cameraFrustrum[FRUSTRUM_BOTTOM_FACE ] = { camPosition, glm::cross(frontMultFar + cameraUp * halfVSide, cameraRight) };  // Bottom face
    }
    else if(light->getType() == LIGHT_TYPE_OMNI) {
        glm::vec3 lightPos  {light->getWorldLocation()};
        float radius        {light->getRadius()};
        cameraFrustrum[FRUSTRUM_NEAR_FACE   ] = { {lightPos.x, lightPos.y, lightPos.z - radius}, {0, 0, 1}  };  // Near face
        cameraFrustrum[FRUSTRUM_FAR_FACE    ] = { {lightPos.x, lightPos.y, lightPos.z + radius}, {0, 0, -1} };  // Far face
        cameraFrustrum[FRUSTRUM_RIGHT_FACE  ] = { {lightPos.x + radius, lightPos.y, lightPos.z}, {-1, 0, 0} };  // Right face
        cameraFrustrum[FRUSTRUM_LEFT_FACE   ] = { {lightPos.x - radius, lightPos.y, lightPos.z}, {1, 0, 0}  };  // Left face
        cameraFrustrum[FRUSTRUM_TOP_FACE    ] = { {lightPos.x, lightPos.y + radius, lightPos.z}, {0, -1, 0} };  // Top face
        cameraFrustrum[FRUSTRUM_BOTTOM_FACE ] = { {lightPos.x, lightPos.y - radius, lightPos.z}, {0, 1, 0}  };  // Bottom face
    }
}

void GraphicEngine::initShaders(){
    // Load vertex and fragment shader
    ShaderResource*  vertexShader   = resourceMan->getShader(SHADER(shadersFile[0]), GL_VERTEX_SHADER);
    ShaderResource*  fragmentShader = resourceMan->getShader(SHADER(shadersFile[1]), GL_FRAGMENT_SHADER);

    // Create a program with both shaders
    ProgramResource* program = resourceMan->getProgram(PROGRAM_DEFAULT);
    program->initProgram(vertexShader->getShaderId(), fragmentShader->getShaderId());

    // Get all uniforms location
    // Basic uniforms for rendering scene
    uModel          = glGetUniformLocation(program->getProgramId(), "model");
    uModelInv       = glGetUniformLocation(program->getProgramId(), "modelInverse");
    uView           = glGetUniformLocation(program->getProgramId(), "view");
    uProj           = glGetUniformLocation(program->getProgramId(), "projection");
    uCamPos         = glGetUniformLocation(program->getProgramId(), "camPosition");
    uAmbientLight   = glGetUniformLocation(program->getProgramId(), "ambientLight");

    // Sun light
    uSunLight.uColor        = glGetUniformLocation(program->getProgramId(), "sunLight.color");
    uSunLight.uDirection    = glGetUniformLocation(program->getProgramId(), "sunLight.direction");
    uSunLight.uIntensity    = glGetUniformLocation(program->getProgramId(), "sunLight.intensity");
    uSunLight.uCastShadows  = glGetUniformLocation(program->getProgramId(), "sunLight.castShadows");

    // Other scene lights
    uLights.resize(SHADERS_NUM_LIGHTS);
    for(size_t i=0; i<SHADERS_NUM_LIGHTS; ++i) {
        std::string sStruct = "lights[" + std::to_string(i) + "].";
        uLights[i].uPosition            = glGetUniformLocation(program->getProgramId(), (sStruct + "position"           ).c_str());
        uLights[i].uColor               = glGetUniformLocation(program->getProgramId(), (sStruct + "color"              ).c_str());
        uLights[i].uIntensity           = glGetUniformLocation(program->getProgramId(), (sStruct + "intensity"          ).c_str());
        uLights[i].uType                = glGetUniformLocation(program->getProgramId(), (sStruct + "type"               ).c_str());
        uLights[i].uAttenuationDistance = glGetUniformLocation(program->getProgramId(), (sStruct + "attenuationDistance").c_str());
        uLights[i].uDirection           = glGetUniformLocation(program->getProgramId(), (sStruct + "direction"          ).c_str());
        uLights[i].uInnerAngle          = glGetUniformLocation(program->getProgramId(), (sStruct + "innerAngle"         ).c_str());
        uLights[i].uOuterAngle          = glGetUniformLocation(program->getProgramId(), (sStruct + "outerAngle"         ).c_str());
        uLights[i].uCastShadows         = glGetUniformLocation(program->getProgramId(), (sStruct + "castShadows"        ).c_str());
    }
    uNumLights              = glGetUniformLocation(program->getProgramId(), "numLights");
    uNumShadowsGenerated    = glGetUniformLocation(program->getProgramId(), "shadowsGenerated");

    // Shadows uniforms
    uCastShadow     = glGetUniformLocation(program->getProgramId(), "shadowOptions");

    uLightTrans.resize      (SHADERS_NUM_LIGHTS + 1);
    uDepthMaps.resize       (SHADERS_NUM_LIGHTS + 1);
    uDepthCubeMaps.resize   (SHADERS_NUM_LIGHTS + 1);
    uArePointLights.resize  (SHADERS_NUM_LIGHTS + 1);

    for(size_t i=0; i<SHADERS_NUM_LIGHTS + 1; ++i) {
        std::string sStruct1    = "depthMaps["      + std::to_string(i) + "]";
        std::string sStruct2    = "depthCubeMaps["  + std::to_string(i) + "]";
        std::string sStruct3    = "lightTransform[" + std::to_string(i) + "]";
        std::string sIsPoint    = "isPointLight["   + std::to_string(i) + "]";
        uDepthMaps      [i]     = glGetUniformLocation(program->getProgramId(), sStruct1.c_str());
        uDepthCubeMaps  [i]     = glGetUniformLocation(program->getProgramId(), sStruct2.c_str());
        uLightTrans     [i]     = glGetUniformLocation(program->getProgramId(), sStruct3.c_str());
        uArePointLights [i]     = glGetUniformLocation(program->getProgramId(), sIsPoint.c_str());

    }

    // Initialize other shaders
    initShadersSkyBox();
    initParticlesShaders();
    initShadowsShaders();
    initDebugShaders();
    initVideoShaders();
    initBloomShaders();
}

void GraphicEngine::initShadersSkyBox(){
    // Load vertex and fragment shader for skybox
    ShaderResource*  vertexShaderSky   = resourceMan->getShader(SHADER(shadersFile[2]), GL_VERTEX_SHADER);
    ShaderResource*  fragmentShaderSky = resourceMan->getShader(SHADER(shadersFile[3]), GL_FRAGMENT_SHADER);

    // Create a program with both shaders
    ProgramResource* programSkyBox = resourceMan->getProgram(PROGRAM_SKYBOX);
    programSkyBox->initProgram(vertexShaderSky->getShaderId(), fragmentShaderSky->getShaderId());
    uViewSky          = glGetUniformLocation(programSkyBox->getProgramId(), "view");
    uProjSky          = glGetUniformLocation(programSkyBox->getProgramId(), "projection");

    uSkyboxTexture    = glGetUniformLocation(programSkyBox->getProgramId(), "skybox");
}

void GraphicEngine::initVideoShaders(){
    // Load vertex and fragment shader for skybox
    ShaderResource*  vertexShader   = resourceMan->getShader(SHADER(shadersFile[13]), GL_VERTEX_SHADER);
    ShaderResource*  fragmentShader = resourceMan->getShader(SHADER(shadersFile[14]), GL_FRAGMENT_SHADER);

    // Create a program with both shaders
    ProgramResource* programSkyBox = resourceMan->getProgram(PROGRAM_VIDEO_PLAYER);
    programSkyBox->initProgram(vertexShader->getShaderId(), fragmentShader->getShaderId());
}

void GraphicEngine::initParticlesShaders() {
    // Load vertex and fragment shader for particles
    ShaderResource*  vertShader = resourceMan->getShader(SHADER(shadersFile[4]), GL_VERTEX_SHADER);
    ShaderResource*  fragShader = resourceMan->getShader(SHADER(shadersFile[5]), GL_FRAGMENT_SHADER);

    // Create a program with both shaders
    ProgramResource* program    = resourceMan->getProgram(PROGRAM_PARTICLES);
    program->initProgram(vertShader->getShaderId(), fragShader->getShaderId());

    // Get all uniforms location
    uViewPart       = glGetUniformLocation(program->getProgramId(), "view");
    uProjPart       = glGetUniformLocation(program->getProgramId(), "projection");
    uAmbientPart    = glGetUniformLocation(program->getProgramId(), "ambientLight");
}

void GraphicEngine::initShadowsShaders() {
    // Load vertex and fragment shader for shadows
    ShaderResource*  vertexShader   = resourceMan->getShader(SHADER(shadersFile[6]), GL_VERTEX_SHADER);
    ShaderResource*  fragmentShader = resourceMan->getShader(SHADER(shadersFile[7]), GL_FRAGMENT_SHADER);

    // Create a program with both shaders
    ProgramResource* program        = resourceMan->getProgram(PROGRAM_SHADOWS);
    program->initProgram({vertexShader->getShaderId(), fragmentShader->getShaderId()});

    uShadowLM           = glGetUniformLocation(program->getProgramId(), "lightMatrix");

    // Load vertex, fragment and geometry shader for point shadows
    ShaderResource*  vertexShaderPS     = resourceMan->getShader(SHADER(shadersFile[8]), GL_VERTEX_SHADER);
    ShaderResource*  fragmentShaderPS   = resourceMan->getShader(SHADER(shadersFile[9]), GL_FRAGMENT_SHADER);
    ShaderResource*  geometryShaderPS   = resourceMan->getShader(SHADER(shadersFile[10]), GL_GEOMETRY_SHADER);

    // Create a program with three shaders for point light shadows
    ProgramResource* programPointShadow = resourceMan->getProgram(PROGRAM_POINT_SHADOWS);
    programPointShadow->initProgram({vertexShaderPS->getShaderId(), fragmentShaderPS->getShaderId(), geometryShaderPS->getShaderId()});

    uPointLightPos      = glGetUniformLocation(programPointShadow->getProgramId(), "lightPos");
    uPointLightFarPlane = glGetUniformLocation(programPointShadow->getProgramId(), "farPlane");

    uPointLightTrans.resize(6);
    for(unsigned int i{}; i < 6; ++i){
        std::string sShadowMatrices = "shadowMatrices[" + std::to_string(i) + "]";
        uPointLightTrans[i] = (glGetUniformLocation(programPointShadow->getProgramId(), sShadowMatrices.c_str()));
    }
}


void GraphicEngine::initDebugShaders() {
    // Load vertex and fragment shader for debug
    ShaderResource*  vertexShader   = resourceMan->getShader(SHADER(shadersFile[11]), GL_VERTEX_SHADER);
    ShaderResource*  fragmentShader = resourceMan->getShader(SHADER(shadersFile[12]), GL_FRAGMENT_SHADER);

    // Create a program for debuging lines
    ProgramResource* program = resourceMan->getProgram(PROGRAM_DEBUG_LINES);
    program->initProgram({vertexShader->getShaderId(), fragmentShader->getShaderId()});

    uDebugView      = glGetUniformLocation(program->getProgramId(), "view");
    uDebugProj      = glGetUniformLocation(program->getProgramId(), "projection");
}

void GraphicEngine::initBloomShaders(){

    // Initialize bloom frame buffer object
    const unsigned int numBloomMips = 3;//6->default
    bool bloomFBO_State = initBloomFBO(getWindowSize(), numBloomMips);
    if (!bloomFBO_State) {
        assert("Cannot init bloom shaders");
    }

    // Load vertex and fragment shader for blur
    ShaderResource*  vertexShader   = resourceMan->getShader(SHADER(shadersFile[15]), GL_VERTEX_SHADER);
    ShaderResource*  fragmentShader = resourceMan->getShader(SHADER(shadersFile[16]), GL_FRAGMENT_SHADER);

    // Create a program
    ProgramResource* program = resourceMan->getProgram(PROGRAM_BLUR);
    program->initProgram({vertexShader->getShaderId(), fragmentShader->getShaderId()});
    
    // Load vertex and fragment shader for bloom
    vertexShader   = resourceMan->getShader(SHADER(shadersFile[17]), GL_VERTEX_SHADER);
    fragmentShader = resourceMan->getShader(SHADER(shadersFile[18]), GL_FRAGMENT_SHADER);

    // Create a program
    program = resourceMan->getProgram(PROGRAM_BLOOM);
    program->initProgram({vertexShader->getShaderId(), fragmentShader->getShaderId()});

    // Create bloom downsample
    vertexShader   = resourceMan->getShader(SHADER(shadersFile[19]), GL_VERTEX_SHADER);
    fragmentShader = resourceMan->getShader(SHADER(shadersFile[20]), GL_FRAGMENT_SHADER);

    program = resourceMan->getProgram(PROGRAM_DOWNSAMPLE);
    program->initProgram({vertexShader->getShaderId(), fragmentShader->getShaderId()});
    unsigned int programID = resourceMan->getProgram(PROGRAM_DOWNSAMPLE)->getProgramId();
    glUseProgram(programID);
    fragmentShader->setUniformInt(glGetUniformLocation(programID, "srcTexture"), 0);
    // Create bloom upsample
    vertexShader   = resourceMan->getShader(SHADER(shadersFile[21]), GL_VERTEX_SHADER);
    fragmentShader = resourceMan->getShader(SHADER(shadersFile[22]), GL_FRAGMENT_SHADER);

    program = resourceMan->getProgram(PROGRAM_UPSAMPLE);
    program->initProgram({vertexShader->getShaderId(), fragmentShader->getShaderId()});
    programID = resourceMan->getProgram(PROGRAM_UPSAMPLE)->getProgramId();
    glUseProgram(programID);
    fragmentShader->setUniformInt(glGetUniformLocation(programID, "srcTexture"), 0);
    glUseProgram(0);


    initBloomBuffers();
    initBloomBlur();
}

void GraphicEngine::renderDownSamples(unsigned int srcTexture)
{
    Size2D windowSize = getWindowSize();

	ShaderResource*  vertexShader   = resourceMan->getShader(SHADER(shadersFile[19]), GL_VERTEX_SHADER);
    ShaderResource*  fragmentShader = resourceMan->getShader(SHADER(shadersFile[20]), GL_FRAGMENT_SHADER);
    unsigned int programID = resourceMan->getProgram(PROGRAM_DOWNSAMPLE)->getProgramId();
	glUseProgram(programID);
    
	fragmentShader->setUniformVec2(glGetUniformLocation(programID, "srcResolution"), glm::vec2(windowSize.width, windowSize.height));
	if (mKarisAverageOnDownsample) {
        
		fragmentShader->setUniformInt(glGetUniformLocation(programID, "mipLevel"), 0);
	}

	// Bind srcTexture (HDR color buffer) as initial texture input
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexture);
    glDisable(GL_BLEND);
	// Progressively downsample through the mip chain
	for (int i = 0; i < (int)mMipChain.size(); i++)
	{
		const bloomMip& mip = mMipChain[i];
		glViewport(0, 0, mip.size.x, mip.size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		                       GL_TEXTURE_2D, mip.texture, 0);

		// Render screen-filled quad of resolution of current mip
		renderQuad();

		// Set current mip resolution as srcResolution for next iteration
		fragmentShader->setUniformVec2(glGetUniformLocation(programID, "srcResolution"), mip.size);
		// Set current mip as texture input for next iteration
		glBindTexture(GL_TEXTURE_2D, mip.texture);
		// Disable Karis average for consequent downsamples
		if (i == 0) { fragmentShader->setUniformInt(glGetUniformLocation(programID, "mipLevel"), 1); }
	}
    glEnable(GL_BLEND);
	glUseProgram(0);
}

void GraphicEngine::renderUpSamples(float filterRadius)
{
    ShaderResource*  vertexShader   = resourceMan->getShader(SHADER(shadersFile[21]), GL_VERTEX_SHADER);
    ShaderResource*  fragmentShader = resourceMan->getShader(SHADER(shadersFile[22]), GL_FRAGMENT_SHADER);
    unsigned int programID = resourceMan->getProgram(PROGRAM_DOWNSAMPLE)->getProgramId();
	glUseProgram(programID);
    
	fragmentShader->setUniformFloat(glGetUniformLocation(programID, "filterRadius"), filterRadius);

	// Enable additive blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	for (int i = (int)mMipChain.size() - 1; i > 0; i--)
	{
		const bloomMip& mip = mMipChain[i];
		const bloomMip& nextMip = mMipChain[i-1];

		// Bind viewport and texture from where to read
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mip.texture);

		// Set framebuffer render target (we write to this texture)
		glViewport(0, 0, nextMip.size.x, nextMip.size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		                       GL_TEXTURE_2D, nextMip.texture, 0);

		// Render screen-filled quad of resolution of current mip
		renderQuad();
	}

	// Disable additive blending
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	glUseProgram(0);
}

void GraphicEngine::renderBloomTexture(unsigned int srcTexture, float filterRadius)
{   
    Size2D screenSize = getWindowSize();
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	renderDownSamples(srcTexture);
	renderUpSamples(filterRadius);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Restore viewport
	glViewport(0, 0, screenSize.width, screenSize.height);
}

void GraphicEngine::renderQuad() {
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


GE::SceneNode* GraphicEngine::addSceneNode(GE::SceneNode* parent) {
    SceneNode* node{nullptr};
    
    // Add a child node to the parent
    node = parent->addChild(std::unique_ptr<SceneNode>(new SceneNode()));

    return node;
}

GE::SceneLight* GraphicEngine::addSceneLight(GE::SceneEntity* parent) {
    SceneNode* node{nullptr};

    if(parent != nullptr)
        // Add a child to an existing node
        node = addSceneNode(parent->treeNode);
    else
        // Add new node to the scene
        node = addSceneNode(sceneRoot.get());

    // Create a light entity in the new node
    node->sEntity = std::unique_ptr<SceneLight>(new SceneLight());
    node->sEntity->setTreeNode(node);

    // Save a pointer in lights vector
    SceneLight* light_ptr {static_cast<SceneLight*>(node->sEntity.get())};
    sceneLights.emplace_back(light_ptr);

    return light_ptr;
}

GE::SceneCamera* GraphicEngine::addSceneCamera(GE::SceneEntity* parent) {
    SceneNode* node{nullptr};

    if(parent != nullptr)
        // Add a child to an existing node
        node = addSceneNode(parent->treeNode);
    else
        // Add new node to the scene
        node = addSceneNode(sceneRoot.get());

    // Create a camera entity in the new node
    node->sEntity = std::unique_ptr<SceneCamera>(new SceneCamera());
    node->sEntity->setTreeNode(node);

    // Set camera viewport size
    SceneCamera*    camera_ptr  {static_cast<SceneCamera*>(node->sEntity.get())};
    Size2D          size        {getWindowSize()};
    camera_ptr->setProjectionViewportSize(size.width, size.height);

    // Save a pointer in cameras vector
    sceneCameras.emplace_back(camera_ptr);

    return camera_ptr;
}

GE::SceneMesh* GraphicEngine::addSceneMesh(GE::SceneEntity* parent) {
    SceneNode* node{nullptr};

    if(parent != nullptr)
        // Add a child to an existing node
        node = addSceneNode(parent->treeNode);
    else
        // Add new node to the scene
        node = addSceneNode(sceneRoot.get());

    // Create a mesh entity in the new node
    node->sEntity = std::unique_ptr<SceneMesh>(new SceneMesh());
    node->sEntity->setTreeNode(node);

    // Add mesh to render entities
    renderEntities.push_back(node->sEntity.get());

    return static_cast<SceneMesh*>(node->sEntity.get());
}

GE::SceneParticleEmitter* GraphicEngine::addParticleEmitter(ParticleEmitterType type, GE::SceneEntity* parent) {
    SceneNode* node{nullptr};

    if(parent != nullptr)
        // Add a child to an existing node
        node = addSceneNode(parent->treeNode);
    else
        // Add new node to the scene
        node = addSceneNode(sceneRoot.get());

    // Create a particle emitter entity in the new node
    node->sEntity = std::unique_ptr<SceneParticleEmitter>(new SceneParticleEmitter(type));
    node->sEntity->setTreeNode(node);

    // Add emitter to render entities
    renderEntities.push_back(node->sEntity.get());

    return static_cast<SceneParticleEmitter*>(node->sEntity.get());
}


GE::SceneEntity* GraphicEngine::addDefaultSceneEntity(GE::SceneEntity* parent) {
    SceneNode* node{nullptr};

    if(parent != nullptr)
        // Add a child to an existing node
        node = addSceneNode(parent->treeNode);
    else
        // Add new node to the scene
        node = addSceneNode(sceneRoot.get());

    // Create a scene entity in the new node
    node->sEntity = std::unique_ptr<SceneEntity>(new SceneEntity());
    node->sEntity->setTreeNode(node);

    return node->sEntity.get();
}

void GraphicEngine::loadSceneMesh(std::string path) {
    resourceMan->getMesh(path);
}

void GraphicEngine::loadMaterial(std::string path) {
    resourceMan->getMaterial(path);
}

void GraphicEngine::loadAnimation(std::string path, std::string pathMesh){
    resourceMan->getAnimation(path, resourceMan->getMesh(pathMesh));
}

MaterialResource* GraphicEngine::getMaterial(std::string path) {
    return resourceMan->getMaterial(path);
}

void GraphicEngine::deleteSceneEntity(GE::SceneEntity* entity, bool destroyChildren) {
    entity->treeNode->deleteNode(destroyChildren);
}

void GraphicEngine::removeSceneLight(GE::SceneLight* light, bool destroyChildren) {
    deleteLight(light);

    deleteSceneEntity(light, destroyChildren);
}

void GraphicEngine::removeSceneCamera(GE::SceneCamera* camera, bool destroyChildren) {
    deleteCamera(camera);

    deleteSceneEntity(camera, destroyChildren);
}

void GraphicEngine::removeSceneMesh(GE::SceneMesh* mesh, bool destroyChildren) {
    if(mesh->spatialTreeNode != nullptr) {
        // Remove mesh from render tree
        renderTreeRoot->removeEntity(mesh);
    }
    else {
        // Seek mesh in renderEntities vector        
        for(auto it{renderEntities.begin()}; it < renderEntities.end(); ++it) {
            if(*it == mesh) {
                renderEntities.erase(it);
                break;
            }
        }
    }

    deleteSceneEntity(mesh, destroyChildren);
}

void GraphicEngine::removeParticleEmitter(GE::SceneParticleEmitter* emitter, bool destroyChildren) {
    if(emitter->spatialTreeNode != nullptr) {
        // Remove emitter from render tree
        renderTreeRoot->removeEntity(emitter);
    }
    else {
        // Seek emitter in renderEntities vector        
        for(auto it{renderEntities.begin()}; it < renderEntities.end(); ++it) {
            if(*it == emitter) {
                renderEntities.erase(it);
                break;
            }
        }
    }

    deleteSceneEntity(emitter, destroyChildren);
}

void GraphicEngine::removeSceneDefaultEntity(GE::SceneEntity* entity, bool destroyChildren) {
    deleteSceneEntity(entity, destroyChildren);
}

void GraphicEngine::deleteLight(GE::SceneLight* light) {
    bool searching{true};
    size_t numLights{sceneLights.size()};

    // Search light in scene lights vector
    for(size_t i {0}; searching && i < numLights; ++i) {
        if(light == sceneLights[i]) {
            // Remove light from vector
            if(i != numLights-1) {
                sceneLights[i] = sceneLights[numLights-1];
            }
            sceneLights.pop_back();
        }
    }
}

void GraphicEngine::deleteCamera(GE::SceneCamera* camera) {
    // If it is the active camera, deactivate it
    if(activeCamera == camera) activeCamera = nullptr;

    bool searching{true};
    size_t numCameras{sceneCameras.size()};
    // Search camera in scene cameras vector
    for(size_t i {0}; searching && i < numCameras; ++i) {
        if(camera == sceneCameras[i]) {
            // Remove camera from vector
            if(i != numCameras-1) {
                sceneCameras[i] = sceneCameras[numCameras-1];
            }
            sceneCameras.pop_back();
        }
    }
}

GraphicEngine::Size2D GraphicEngine::getWindowSize() const noexcept {
    Size2D size{0,0};

    glfwGetWindowSize(window, &size.width, &size.height);

    return size;
}

UiEngine* GraphicEngine::getUiEngine() const noexcept {
    return uiEngine.get();
} 

void GraphicEngine::setCursorPosition(float x, float y) {
    glfwSetCursorPos(window, x, y);
}

void GraphicEngine::setAmbientLightning(glm::vec3 color, float intensity) noexcept{
    // Update ambient lightning values
    ambientColor        = color;
    ambientIntensity    = intensity;

    // Send data to shaders
    sendAmbientLightToShader();
}

void GraphicEngine::setSunlight(glm::vec3 color, glm::vec3 direction, float intensity) noexcept {
    // Update sun light
    sunLight.color      = color;
    sunLight.direction  = direction;
    sunLight.intensity  = intensity;

    // Send data to shaders
    sendSunLightToShader();
}

void GraphicEngine::updateLightning(float x, float y, float z, float orientation) noexcept {
    // Update sunlight reference position
    sunLight.referencePos = {x, y, z};

    // Update lights that will be rendered
    renderLights = getClosestLights(x, y, z, orientation);
    
    // Send lightning data to shaders
    glUseProgram(resourceMan->getProgram(PROGRAM_DEFAULT)->getProgramId());
    sendAmbientLightToShader();
    sendSunLightToShader();
    sendLightDataToShader(x, y, z);
}

void GraphicEngine::sendAmbientLightToShader() noexcept {
    ShaderResource* fShader     = resourceMan->getShader(SHADER(shadersFile[1]), GL_FRAGMENT_SHADER);
    glm::vec4 ambientLight  = glm::vec4(ambientColor * ambientIntensity, 1.0);

    fShader->setUniformVec4(uAmbientLight, ambientLight);
}

void GraphicEngine::sendSunLightToShader() noexcept {
    ShaderResource* fShader     = resourceMan->getShader(SHADER(shadersFile[1]), GL_FRAGMENT_SHADER);

    fShader->setUniformVec3(uSunLight.uColor, sunLight.color);
    fShader->setUniformVec3(uSunLight.uDirection, sunLight.direction);
    fShader->setUniformFloat(uSunLight.uIntensity, sunLight.intensity);
    fShader->setUniformInt(uSunLight.uCastShadows, true);
}

void GraphicEngine::sendLightDataToShader(float refX, float refY, float refZ) noexcept {
    ShaderResource* fShader     = resourceMan->getShader(SHADER(shadersFile[1]), GL_FRAGMENT_SHADER);

    size_t lightsSent{0};
    for(auto light : renderLights) {
        // Send basic light data to shader
        int lightType {light->getType()};
        fShader->setUniformVec3(uLights[lightsSent].uPosition,      light->getTranslation());
        fShader->setUniformVec3(uLights[lightsSent].uColor,         light->getColor()); 
        fShader->setUniformFloat(uLights[lightsSent].uIntensity,    light->getIntensity());
        fShader->setUniformInt(uLights[lightsSent].uType,           lightType);
        fShader->setUniformInt(uLights[lightsSent].uCastShadows,    light->getCastShadows());

        if(lightType == SHADER_POINT_LIGHT_ID) {
            // Send specific data of point lights
            fShader->setUniformFloat(uLights[lightsSent].uAttenuationDistance, light->getRadius());
        }
        else if (lightType == SHADER_DIRECCTIONAL_LIGHT_ID) {
            // Send specific data of directional lights
            fShader->setUniformVec3(uLights[lightsSent].uDirection, light->getDirection());
        }
        else if(lightType == SHADER_SPOTLIGHT_ID) {
            // Send specific data of spotlights
            fShader->setUniformFloat(uLights[lightsSent].uAttenuationDistance,  light->getRadius());
            fShader->setUniformVec3(uLights[lightsSent].uDirection,             light->getDirection());

            float angle = glm::cos(glm::radians(light->getInnerAngle()/2));
            fShader->setUniformFloat(uLights[lightsSent].uInnerAngle, angle);

            angle = glm::cos(glm::radians(light->getOuterAngle()/2 + light->getInnerAngle()/2));
            fShader->setUniformFloat(uLights[lightsSent].uOuterAngle, angle);
        }

        // Update lights sent value
        ++lightsSent;
    }

    // Add empty data if needed
    while(lightsSent < SHADERS_NUM_LIGHTS) {
        int defaultType{0};
        fShader->setUniformInt(uLights[lightsSent].uType, defaultType);

        ++lightsSent;
    }

    // Send shadow options
    int shadowOptions{0};
    if(castShadows) {
        if(smoothShadows)   shadowOptions = 2;
        else                shadowOptions = 1;
    }
    fShader->setUniformInt(uCastShadow, shadowOptions);
}

std::vector<GE::SceneLight*> GraphicEngine::getClosestLights(float refX, float refY, float refZ, float refOrient) const noexcept{
    // Create a vector to save closest lights and initialize it
    glm::vec3 refPos {refX, refY, refZ};
    std::vector<std::pair<float,SceneLight*>> closestLights;

    auto isBehind = [](const glm::vec3& vector, float refOrientation) {
        float maxAngleDiff {glm::radians(90.f)};

        // Calculate orientation and compare with the reference orientation
        float orientation {std::atan2(vector.x, vector.z) + 3.1415f};

        if ((refOrientation >= orientation  && 
                (refOrientation - orientation < maxAngleDiff || (2*3.1415 - refOrientation) + orientation < maxAngleDiff)) || 
            (refOrientation < orientation   && 
                (orientation - refOrientation < maxAngleDiff || (2*3.1415 - orientation) + refOrientation < maxAngleDiff))  ) {
                return true;
        }
        return false;
    };

    // Search closest lights to reference position
    for(auto light : sceneLights) {
        // If light is disabled, continue
        if(!light->getActive()) continue;

        glm::vec3 lightPos      {light->getTranslation()};
        glm::vec3 distVector    {lightPos - refPos};

        // Check distance
        float distance {std::sqrt(distVector.x*distVector.x + distVector.y*distVector.y + distVector.z*distVector.z)};
        
        // If the light is behind and far, continue
        if(isBehind(distVector, refOrient) && distance > light->getRadius()) continue;

        if(closestLights.empty()) {
            // Vector empty, save light
            closestLights.emplace_back(distance, light);
        }
        else if(distance < closestLights.back().first) {
            // Closer than other lights, save it
            for(auto it{closestLights.begin()}; it < closestLights.end(); ++it) {
                if(distance < it->first) {
                    closestLights.insert(it, {distance, light});
                    break;
                }
            }

            // After saving it, check if there are too many lights saved
            if(closestLights.size() >= SHADERS_NUM_LIGHTS) closestLights.pop_back();
        }
        else if(closestLights.size() < SHADERS_NUM_LIGHTS) {
            // Its further than other lights, but there is enough space
            closestLights.emplace_back(distance, light);
        }
    }

    // Save closest lights
    std::vector<SceneLight*> l {};
    for(auto light : closestLights) l.emplace_back(light.second);

    return l;
}

void GraphicEngine::deleteScene() {
    sceneRoot->deleteNode(true);
    renderTreeRoot->clear();
    renderEntities.clear();
    activeCamera = nullptr;
    sceneLights.clear();
    renderLights.clear();
    sceneCameras.clear();
}

void GraphicEngine::freeAllResources() {
    deleteScene();
    freeDepthMapsBuffers();
    freeSkyBox();
    freeDebugBuffers();
    resourceMan->freeAllResources();
    GE::SceneMesh::resetSceneMesh();

    // Init some buffers and resources again
    initShaders();
    initDepthMapsBuffers();
    initSkyBox();
}

void GraphicEngine::setBoneTransform(GLint loc, glm::mat4 t){
    ShaderResource* vShader = resourceMan->getShader(SHADER(shadersFile[0]), GL_VERTEX_SHADER);
    glUseProgram(resourceMan->getProgram(PROGRAM_DEFAULT)->getProgramId());
    vShader->setUniformMat4(loc, t);
}

void GraphicEngine::freeDepthMapsBuffers() {
    for(size_t i{0}; i <= SHADERS_NUM_LIGHTS; ++i) {
        // Free texture
        glDeleteTextures(1, &depthMapTextures[i]);

        // Free buffer
        glDeleteFramebuffers(1, &depthMapBuffers[i]);

        // Free depth cubemap
        if(i < SHADERS_NUM_LIGHTS){
            // Free texture
            glDeleteTextures(1, &depthCubeMapTextures[i]);

            // Free buffer
            glDeleteFramebuffers(1, &depthCubeMapBuffers[i]);
        }
    }
}

void GraphicEngine::freeDebugBuffers() {
    glDeleteVertexArrays(1, &debugVAO);
    glDeleteBuffers(1, &debugVBO);

    debugVAO = debugVBO = 0;
}

// bloom
void GraphicEngine::initBloomBlur() {
    Size2D screen_size = getWindowSize();
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_size.width, screen_size.width, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        
    }
}

bool GraphicEngine::initBloomFBO(Size2D screen_size, unsigned int mipChainLength){
    glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	glm::vec2 mipSize((float)screen_size.width, (float)screen_size.height);
	glm::ivec2 mipIntSize((int)screen_size.width, (int)screen_size.height);
	// Safety check
	if (screen_size.width > (unsigned int)INT_MAX || screen_size.height > (unsigned int)INT_MAX) {
		//std::cerr << "Window size conversion overflow - cannot build bloom FBO!" << std::endl;
		assert("Window size conversion overflow - cannot build bloom FBO");
        return false;
	}

	for (GLuint i = 0; i < mipChainLength; i++)
	{
		bloomMip mip;

		mipSize *= 0.5f;
		mipIntSize /= 2;
		mip.size = mipSize;
		mip.intSize = mipIntSize;

		glGenTextures(1, &mip.texture);
		glBindTexture(GL_TEXTURE_2D, mip.texture);
		// we are downscaling an HDR color buffer, so we need a float texture format
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F,
		             (int)mipSize.x, (int)mipSize.y,
		             0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		
		mMipChain.emplace_back(mip);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	                       GL_TEXTURE_2D, mMipChain[0].texture, 0);

	// setup attachments
	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	// check completion status
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("gbuffer FBO error, status: 0x%x\n", status);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void GraphicEngine::initBloomBuffers(){


    Size2D screen_size = getWindowSize();
    
    // configure (floating point) framebuffers
    // ---------------------------------------
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    // create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_size.width, screen_size.height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    // create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_size.width, screen_size.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    glDrawBuffers(2, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void GraphicEngine::initSkyBox(){
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    skyboxTexture = TextureResource::loadCubeMap(skyBoxFaces);

}

void GraphicEngine::freeSkyBox() {
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteTextures(1, &skyboxTexture);

    skyboxVAO = skyboxVBO = skyboxTexture = 0;
}

void GraphicEngine::drawSkyBox(){
    // draw skybox as last
    glDepthFunc(GL_LEQUAL);

    glUseProgram(resourceMan->getProgram(PROGRAM_SKYBOX)->getProgramId());

    // Draw inside faces
    glCullFace(GL_FRONT);

    if(activeCamera != nullptr){ 
        ShaderResource* vShader = resourceMan->getShader(SHADER(shadersFile[2]), GL_VERTEX_SHADER);

        // Get projection and view matrix
        glm::mat4 view = glm::mat4(glm::mat3(activeCamera->getViewMatrix()));
        glm::mat4 proj = activeCamera->getProjectionMatrix();

        // Send matrices to vertex shader
        vShader->setUniformMat4(uViewSky, view);
        vShader->setUniformMat4(uProjSky, proj);

        ShaderResource* fShader = resourceMan->getShader(SHADER(shadersFile[3]), GL_FRAGMENT_SHADER);
        int skyUniform = 0;
        fShader->setUniformInt(uSkyboxTexture, skyUniform);

    }

    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    // Culling faces to original value
    glCullFace(GL_BACK);
}

void GraphicEngine::setFov(float angle) noexcept{
    if(activeCamera!= nullptr)
        activeCamera->setProjectionCameraAngle(angle);
}

void GraphicEngine::setRenderDistance(float distance) noexcept {
    GE::SceneCamera::setRenderDistance(std::min(std::max(distance, 0.3f), 1.0f));

    // Update all cameras
    for(auto cam : sceneCameras) cam->setUpdateProjection();
}

void GraphicEngine::enableShadows(bool s) noexcept {
    castShadows = s;
}

void GraphicEngine::setSmoothShadows(bool s) noexcept {
    smoothShadows = s;
}

void GraphicEngine::setShadowQuality(size_t i) {
    // Check i is a valid value
    if(i > 2) return;

    unsigned int prevSunDepthValue  {sunDepthMapSize};
    unsigned int prevDepthValue     {depthMapSize};
    unsigned int prevCubeDepthValue {depthCubeMapSize};
    if(i == 0) {            // LOW QUALITY
        depthCubeMapSize    = DEPTH_CUBEMAP_SIZE_LOW;
        sunDepthMapSize     = SUN_DEPTH_MAP_SIZE_LOW;
        depthMapSize        = DEPTH_MAP_SIZE_LOW;
        shadowsGenerated    = NUM_SHADOWS_LOW;
    }
    else if (i == 1) {      // MEDIUM QUALITY
        depthCubeMapSize    = DEPTH_CUBEMAP_SIZE_HIGH;
        sunDepthMapSize     = SUN_DEPTH_MAP_SIZE_HIGH;
        depthMapSize        = DEPTH_MAP_SIZE_HIGH;
        shadowsGenerated    = NUM_SHADOWS_HIGH;
    }
    else if (i == 2) {      // HIGH QUALITY
        depthCubeMapSize    = DEPTH_CUBEMAP_SIZE_ULTRA;
        sunDepthMapSize     = SUN_DEPTH_MAP_SIZE_ULTRA;
        depthMapSize        = DEPTH_MAP_SIZE_ULTRA;
        shadowsGenerated    = NUM_SHADOWS_ULTRA;
    }

    // Check if value has changed
    if(prevSunDepthValue != sunDepthMapSize || prevDepthValue != depthMapSize || prevCubeDepthValue != depthCubeMapSize) {
        // Free buffers and textures to init them with new size
        freeDepthMapsBuffers();
        initDepthMapsBuffers();
    }
}

void GraphicEngine::setParticlesQuality(bool interpolate, float amount) noexcept {
    ParticleEmitter::setInterpolation(interpolate);
    ParticleEmitter::setGenerationFactor(amount);
}

void GraphicEngine::draw3DLine(float x1, float y1, float z1, float x2, float y2, float z2) noexcept {
    // Activate debug program
    glUseProgram(resourceMan->getProgram(PROGRAM_DEBUG_LINES)->getProgramId());

    // Creates an array with the data of the line to be drawn
    float debugPoints[6] {
        x1, y1, z1,
        x2, y2, z2
    };
    
    // Check if vertex array object has been created
    if(debugVAO == 0) {
        // Create vertex array and buffer objects
        glGenVertexArrays(1, &debugVAO);
        glGenBuffers(1, &debugVBO);

        // Bind VAO and VBO and configure data
        glBindVertexArray(debugVAO);
        glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(debugPoints), debugPoints, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    // Bind VAO
    glBindVertexArray(debugVAO);
    glBindBuffer(GL_ARRAY_BUFFER, debugVBO);

    // Update buffer values
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(debugPoints), &debugPoints);

    // Draw 3D line
    glDrawArrays(GL_LINES, 0, 2);

    // Restore default values
    glBindVertexArray(0);
    glUseProgram(resourceMan->getProgram(PROGRAM_DEFAULT)->getProgramId());
}

void GraphicEngine::playVideo(const std::string& path, float fps) noexcept {
    videoPath = path;
    videoFrame->loadResource(path, "");
    videoFPS = 1.f/fps;
    videoTime = 0.0f;
}

void GraphicEngine::stopVideo() noexcept {
    videoPath.clear();
}

bool GraphicEngine::updateVideoFrame(float dt) noexcept {
    if(videoPath.empty() || !engine->runEngine()) return false;
    bool videoUpdated {true};

    // Current frame
    size_t currentFrame {static_cast<size_t>(videoTime/videoFPS)};

    // New frame
    videoTime += dt;
    size_t newFrame     {static_cast<size_t>(videoTime/videoFPS)};

    // If different, load new frame
    if(newFrame != currentFrame) {
        // Calculate path
        std::string newPath {nextFramePath(newFrame, 3)};

        // Load new frame
        videoUpdated = videoFrame->updateTextureFromFile(newPath);
    }

    // Draw video frame
    if(videoUpdated)    drawVideoFrame();
    // No frame found, video ends
    else                stopVideo();

    return videoUpdated;
}

std::string GraphicEngine::nextFramePath(size_t frame, size_t padding) noexcept {
    // Calculate path
    size_t pos          {videoPath.find_last_of(".")};
    std::string newPath {videoPath.substr(0, pos-padding)};

    // Get frame number as string
    std::string frameNumber {std::to_string(frame)};
    while(frameNumber.length() < padding) frameNumber.insert(frameNumber.begin(), '0');

    // Change path
    newPath.append(frameNumber);
    newPath.append(videoPath.substr(pos));

    return newPath;
}

void GraphicEngine::drawVideoFrame() noexcept {
    if(videoPath.empty()) return;

    // Disable face culling
    glDisable(GL_CULL_FACE);

    // Activate particles program and get vertex shader
    glUseProgram(resourceMan->getProgram(PROGRAM_VIDEO_PLAYER)->getProgramId());

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, videoFrame->getTexture().id);

    // Draw alive particles
    glBindVertexArray(videoVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
    glBindVertexArray(0);

    // Return to default culling
    glEnable(GL_CULL_FACE);

    glUseProgram(resourceMan->getProgram(PROGRAM_DEFAULT)->getProgramId());
}

void GraphicEngine::updateSpatialTree() noexcept {
    for(SceneEntity* e : renderEntities) {
        // Check if it is a SceneMesh
        SceneMesh* eMesh = dynamic_cast<SceneMesh*>(e);
        if(eMesh != nullptr) {
            AABBBoundingBox bbox = eMesh->getTransformedBoundingBox();

            renderTreeRoot->addEntity(bbox, e);
            continue;
        }

        // Check if it is a SceneParticleEmitter
        SceneParticleEmitter* ePart = dynamic_cast<SceneParticleEmitter*>(e);
        if(ePart != nullptr) {
            auto location {ePart->getWorldLocation()};
            AABBBoundingBox bbox{{location.x, location.y, location.z}, {10.f}};

            renderTreeRoot->addEntity(bbox, e);
        }
    }

    renderEntities.clear();
}