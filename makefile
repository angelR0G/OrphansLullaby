#######################################################################################
#############################    INSTRUCTIONS    ######################################
#######################################################################################
##	For linux release
##		make release RELEASE=1 -jX
##	For windows debug
##		make win WINDOWS=1 -jX
##	For windows release
##		make releasewin WINDOWS=1 RELEASE=1 -jX
##
##	X->Number of compilation threads

#######################################################################################
################################    MACROS    #########################################
#######################################################################################
define C_TO_O_FILE
	$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst $(SRC_DIR)%,$(OBJ_DIR)%,$(1))))
endef

define CPP_COMPILE
$(OBJ_DIR)/$(1).o : $(SRC_DIR)/$(1).cpp $(SRC_DIR)/$(2) $(SRC_DIR)/$(3)
	$(CC) -c $(SRC_DIR)/$(1).cpp -o $(OBJ_DIR)/$(1).o $(CPPFLAGS) $(INC_LIBS)
endef

define C_COMPILE
$(OBJ_DIR)/$(1).o : $(SRC_DIR)/$(1).c $(SRC_DIR)/$(2) $(SRC_DIR)/$(3)
	$(C) -c $(SRC_DIR)/$(1).c -o $(OBJ_DIR)/$(1).o $(CFLAGS) $(INC_LIBS)
endef

# Makefile FMOD

ifndef CPU
    # $(error Specify CPU=[x86|x86_64|arm|arm64])
	CPU := x86_64
endif
ifndef CONFIG
    # $(error Specify CONFIG=[Debug|Release])
	CONFIG := Debug
endif

ifeq (${CPU}, arm)
    FLAGS += -marm -march=armv7-a -mfpu=neon -mfloat-abi=hard
else ifeq (${CPU}, arm64)
    FLAGS += -m64 -target aarch64-linux-gnu -march=armv8-a
else ifeq (${CPU}, x86)
    FLAGS += -m32
else
    override CPU = x86_64
    FLAGS += -m64
endif

ifeq (${CONFIG}, Debug)
    FLAGS += -g
    SUFFIX = L
else
    override CONFIG = Release
#FLAGS += -O2
    SUFFIX =
endif


#######################################################################################
###############################    CONSTANTS    #######################################
#######################################################################################

UNAME			:= $(shell uname)
GAME 			:= Orphans_Lullaby
CPPFLAGS		:= -std=c++20 -Wextra -Wpedantic
CFLAGS			:= -std=c++20
CC				:= g++
C				:= gcc
SRC_DIR			:= src
OBJ_DIR			:= obj
LIB_DIR			:= libs
REL_STRUCT		:= release release/libs
REL_DIR			:= release
MEDIA_DIR		:= media
current_dir = $(shell pwd)


ifeq ($(UNAME),Linux)
	OS := linux
else
	OS := windows
endif



ifdef WINDOWS
	CORE_LIB   	:= libs/fmod${SUFFIX}.dll
	STUDIO_LIB 	:= libs/fmodstudio${SUFFIX}.dll
	ASSIMP_LIB	:= libs/libassimp-5.dll
	CPPFLAGS		+= -DWINDOWS
	LIBS			:= $(LIB_DIR)/$(OS)/libreactphysics3d.a $(LIB_DIR)/$(OS)/libimgui.a $(LIB_DIR)/$(OS)/libglfw3.a -L ./libs/ -Wl,-Bdynamic -lfmod -lfmodstudio -Wl,-Bstatic ${ASSIMP_LIB} -static-libgcc -static-libstdc++ -lopengl32 -static
	LINK_FLAGS		:= -mwindows 
# To generate release version compile with 'RELEASE=1'
	ifdef RELEASE
		CPPFLAGS	+= -O3
	else
		CPPFLAGS	+= -g
	endif
else
	CORE_LIB   	:= libs/libfmod${SUFFIX}.so.13
	STUDIO_LIB 	:= libs/libfmodstudio${SUFFIX}.so.13
	ASSIMP_LIB 	:= libs/libassimp.so.5

	LIBS		:= $(LIB_DIR)/$(OS)/libreactphysics3d.a $(LIB_DIR)/linux/libimgui.a $(LIB_DIR)/linux/libglfw3.a -lGL -lXxf86vm -lXext -lX11 -lXrandr -lXi -ldl -lXcursor -Wl,-rpath=\$$ORIGIN/$(dir ${CORE_LIB}),-rpath=\$$ORIGIN/$(dir ${STUDIO_LIB}),-rpath=\$$ORIGIN/$(dir ${ASSIMP_LIB}) ${CORE_LIB} ${STUDIO_LIB} ${ASSIMP_LIB}
# To generate release version compile with 'RELEASE=1'
	ifdef RELEASE
		CPPFLAGS	+= -O3
	else
		CPPFLAGS	+= -g
		LINK_FLAGS  += -fsanitize=address,undefined
	endif
endif

INC_LIBS		:= -I$(SRC_DIR) -I$(LIB_DIR) -I$(LIB_DIR)$(os)/fmod/inc/ -I$(LIB_DIR)$(os)/fmod/core/inc/


SRC_SUBDIRS		:= $(shell find $(SRC_DIR) -type d)
OBJ_SUBDIRS		:= $(patsubst $(SRC_DIR)%,$(OBJ_DIR)%,$(SRC_SUBDIRS))

SRC_CPP_FILES	:= $(shell find $(SRC_DIR) -type f -iname *.cpp)
SRC_C_FILES		:= $(shell find $(SRC_DIR) -type f -iname *.c)
OBJ_CPP_FILES	:= $(foreach F,$(SRC_CPP_FILES),$(call C_TO_O_FILE,$(F)))
OBJ_C_FILES		:= $(foreach F,$(SRC_C_FILES),$(call C_TO_O_FILE,$(F)))

OBJ_CLEAN_FILES	:= $(shell find $(OBJ_DIR) -type f -iname *.o)
DLL_CLEAN_FILES	:= $(shell find *.dll)

# Link every .o file to get the game 
$(GAME) : $(OBJ_SUBDIRS) $(OBJ_C_FILES) $(OBJ_CPP_FILES)
	$(CC) -o $(GAME) $(patsubst $(SRC)%,$(OBJ)%,$(OBJ_C_FILES) $(OBJ_CPP_FILES)) $(LIBS) $(LINK_FLAGS)

# CPP files to compile
$(eval $(call CPP_COMPILE,main))

# Systems
$(eval $(call CPP_COMPILE,systems/basic,systems/basic.hpp,components/basic.hpp))
$(eval $(call CPP_COMPILE,systems/input,systems/input.hpp,components/input.hpp))
$(eval $(call CPP_COMPILE,systems/camera,systems/camera.hpp,components/camera.hpp))
$(eval $(call CPP_COMPILE,systems/movement,systems/movement.hpp,components/movement.hpp))
$(eval $(call CPP_COMPILE,systems/render,systems/render.hpp,components/render.hpp))
$(eval $(call CPP_COMPILE,systems/sound,systems/sound.hpp,components/sound.hpp))
$(eval $(call CPP_COMPILE,systems/ai,systems/ai.hpp,components/ai.hpp))
$(eval $(call CPP_COMPILE,systems/weapon,systems/weapon.hpp,components/weapon.hpp))
$(eval $(call CPP_COMPILE,systems/collision,systems/collision.hpp,components/collision.hpp))
$(eval $(call CPP_COMPILE,systems/navigation,systems/navigation.hpp,components/navigation.hpp))
$(eval $(call CPP_COMPILE,systems/health,systems/health.hpp,components/health.hpp))
$(eval $(call CPP_COMPILE,systems/trigger,systems/trigger.hpp,components/trigger.hpp))
$(eval $(call CPP_COMPILE,systems/attack,systems/attack.hpp,components/attack.hpp))
$(eval $(call CPP_COMPILE,systems/animation,systems/animation.hpp,components/animation.hpp))
$(eval $(call CPP_COMPILE,components/attackstats.hpp))

# Engines
$(eval $(call CPP_COMPILE,engine/inputController,engine/inputController.hpp))
$(eval $(call CPP_COMPILE,engine/camera,engine/camera.hpp))
$(eval $(call CPP_COMPILE,engine/entityFactory,engine/entityFactory.hpp,engine/entityFactory.tpp))
$(eval $(call CPP_COMPILE,engine/entitymanager,engine/entitymanager.hpp))
$(eval $(call CPP_COMPILE,engine/menu,engine/menu.hpp))
$(eval $(call CPP_COMPILE,engine/menuController,engine/menuController.hpp))
$(eval $(call CPP_COMPILE,engine/cinematicManager,engine/cinematicManager.hpp))
# Physics engine
$(eval $(call CPP_COMPILE,engine/physics/collisionObject,engine/physics/collisionObject.hpp))
$(eval $(call CPP_COMPILE,engine/physics/physicsEngine,engine/physics/physicsEngine.hpp))
#Graphic engine
$(eval $(call C_COMPILE,engine/graphic/glad))
$(eval $(call CPP_COMPILE,engine/graphic/engine,engine/graphic/engine.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/SpatialTree,engine/graphic/SpatialTree.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/uiEngine,engine/graphic/uiEngine.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/inputController,engine/graphic/inputController.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/stb_image))
$(eval $(call CPP_COMPILE,engine/graphic/particles/particleTypes,engine/graphic/particles/particleTypes.hpp,engine/graphic/particles/particle.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/particles/particleEmitter,engine/graphic/particles/particleEmitter.hpp,engine/graphic/particles/emitterTypes.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/sceneTree/sceneEntity,engine/graphic/sceneTree/sceneEntity.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/sceneTree/sceneCamera,engine/graphic/sceneTree/sceneCamera.hpp,engine/graphic/sceneTree/sceneEntity.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/sceneTree/sceneLight,engine/graphic/sceneTree/sceneLight.hpp,engine/graphic/sceneTree/sceneEntity.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/sceneTree/sceneMesh,engine/graphic/sceneTree/sceneMesh.hpp,engine/graphic/sceneTree/sceneEntity.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/sceneTree/sceneNode,engine/graphic/sceneTree/sceneNode.hpp,engine/graphic/sceneTree/sceneEntity.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/sceneTree/scenePartEmitter,engine/graphic/sceneTree/scenePartEmitter.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/sceneTree/transform,engine/graphic/sceneTree/transform.hpp,engine/graphic/sceneTree/sceneEntity.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/resource/resourceManager,engine/graphic/resource/resourceManager.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/resource/resource,engine/graphic/resource/resource.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/resource/meshresource,engine/graphic/resource/meshresource.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/resource/textureresource,engine/graphic/resource/textureresource.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/resource/materialresource,engine/graphic/resource/materialresource.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/resource/animationresource,engine/graphic/resource/animationresource.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/resource/shaderresource,engine/graphic/resource/shaderresource.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/resource/programresource,engine/graphic/resource/programresource.hpp))
$(eval $(call CPP_COMPILE,engine/physics/collisionCallback,engine/physics/collisionCallback.hpp))
$(eval $(call CPP_COMPILE,engine/uiController,engine/uiController.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/resource/animations/bone,engine/graphic/resource/animations/bone.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/utils/assimp_helper,engine/graphic/utils/assimp_helper.hpp))
$(eval $(call CPP_COMPILE,engine/graphic/frustrumPlane,engine/graphic/frustrumPlane.hpp))

# Utils
$(eval $(call CPP_COMPILE,utils/vectorMath,utils/vectorMath.hpp))
$(eval $(call CPP_COMPILE,utils/WaypointCreator,utils/WaypointCreator.hpp))
$(eval $(call CPP_COMPILE,utils/CollisionCreator,utils/CollisionCreator.hpp))
$(eval $(call CPP_COMPILE,utils/ObjectsCreator,utils/ObjectsCreator.hpp))
$(eval $(call CPP_COMPILE,utils/Profiler,utils/Profiler.hpp))
$(eval $(call CPP_COMPILE,utils/transform,utils/transform.hpp))
$(eval $(call CPP_COMPILE,utils/editorIA,utils/editorIA.hpp))
$(eval $(call CPP_COMPILE,utils/menuConfig,utils/menuConfig.hpp))
$(eval $(call CPP_COMPILE,utils/damageMark,utils/damageMark.hpp))

ifeq ($(UNAME),Linux)
#For linux
$(eval $(call CPP_COMPILE,Fmod/SoundEngine,Fmod/SoundEngine.hpp))
$(eval $(call CPP_COMPILE,Fmod/soundFactory,Fmod/soundFactory.hpp,Fmod/SoundEngine.hpp))
else
#For windows
$(eval $(call CPP_COMPILE,Fmod/SoundEngineWin,Fmod/SoundEngineWin.hpp))
$(eval $(call CPP_COMPILE,Fmod/soundFactory,Fmod/soundFactory.hpp,Fmod/SoundEngineWin.hpp))
endif

$(eval $(call CPP_COMPILE,IrrlichtFacade/mesh,IrrlichtFacade/mesh.hpp))
$(eval $(call CPP_COMPILE,IrrlichtFacade/image,IrrlichtFacade/image.hpp))
$(eval $(call CPP_COMPILE,IrrlichtFacade/emitter,IrrlichtFacade/emitter.hpp))

# Managers
$(eval $(call CPP_COMPILE,manager/gamemanager,manager/gamemanager.hpp))
$(eval $(call CPP_COMPILE,manager/eventmanager,manager/eventmanager.hpp))
$(eval $(call CPP_COMPILE,manager/listener,manager/listener.hpp))
$(eval $(call CPP_COMPILE,manager/event,manager/event.hpp))
$(eval $(call CPP_COMPILE,manager/goals,manager/goals.hpp))
$(eval $(call CPP_COMPILE,manager/roundmanager,manager/roundmanager.hpp))
$(eval $(call CPP_COMPILE,utils/scheduler,utils/scheduler.hpp))
$(eval $(call CPP_COMPILE,utils/behaviourData,utils/behaviourData.hpp))

# Create a directory to save the compiled files
$(OBJ_SUBDIRS) :
	mkdir -p $(OBJ_SUBDIRS)


all :

# Clean operation, deletes all .o files and the final game file
clean :
	rm -r $(OBJ_CLEAN_FILES) $(GAME) $(REL_DIR) $(DLL_CLEAN_FILES)

#Create a directory with game assets, libs and final game file
release : $(GAME)
	mkdir -p $(REL_STRUCT)
	cp -r $(MEDIA_DIR) $(REL_DIR)
	cp $(LIB_DIR)/linux/fmod/core/lib/x86_64/libfmodL.so.13 $(REL_DIR)/libs/
	cp $(LIB_DIR)/linux/fmod/core/lib/x86_64/libfmodL.so.13.10 $(REL_DIR)/libs/
	cp $(LIB_DIR)/linux/fmod/lib/x86_64/libfmodstudioL.so.13 $(REL_DIR)/libs/
	cp $(LIB_DIR)/linux/fmod/lib/x86_64/libfmodstudioL.so.13.10 $(REL_DIR)/libs/
	cp $(LIB_DIR)/libassimp.so $(REL_DIR)/libs/
	cp $(LIB_DIR)/libassimp.so.5 $(REL_DIR)/libs/
	cp $(LIB_DIR)/libassimp.so.5.2.4 $(REL_DIR)/libs/
	cp $(GAME) $(REL_DIR)

releasewin : $(GAME)
	cp $(LIB_DIR)/fmod.dll $(current_dir)
	cp $(LIB_DIR)/fmodstudio.dll $(current_dir)
	cp $(LIB_DIR)/libassimp-5.dll $(current_dir)
	cp $(LIB_DIR)/libgcc_s_seh-1.dll $(current_dir)
	cp $(LIB_DIR)/libstdc++-6.dll $(current_dir)
	cp $(LIB_DIR)/libwinpthread-1.dll $(current_dir)
	mkdir -p $(REL_STRUCT)
	cp -r $(MEDIA_DIR) $(REL_DIR)
	cp $(LIB_DIR)/fmod.dll $(REL_DIR)
	cp $(LIB_DIR)/fmodstudio.dll $(REL_DIR)
	cp $(LIB_DIR)/libassimp-5.dll $(REL_DIR)
	cp $(LIB_DIR)/libgcc_s_seh-1.dll $(REL_DIR)
	cp $(LIB_DIR)/libstdc++-6.dll $(REL_DIR)
	cp $(LIB_DIR)/libwinpthread-1.dll $(REL_DIR)
	cp $(GAME) $(REL_DIR)

win : $(GAME)
	cp $(LIB_DIR)/fmod.dll $(current_dir)
	cp $(LIB_DIR)/fmodstudio.dll $(current_dir)
	cp $(LIB_DIR)/libassimp-5.dll $(current_dir)
	cp $(LIB_DIR)/libgcc_s_seh-1.dll $(current_dir)
	cp $(LIB_DIR)/libstdc++-6.dll $(current_dir)
	cp $(LIB_DIR)/libwinpthread-1.dll $(current_dir)
