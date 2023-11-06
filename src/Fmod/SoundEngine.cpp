/*
Include section
*/
#include "SoundEngine.hpp"
#include <cmath>
#include <algorithm>
using namespace std;

/*
ERRCHECK to see if we have any problem using FMOD
*/
void ERRCHECK_FMOD (FMOD_RESULT result, const char * file, int line)
{
	if(result != FMOD_OK)
	{
        printf("%s, %s", result, FMOD_ErrorString(result));
		exit(-1);
	}
}
#define ERRCHECK(_result) ERRCHECK_FMOD(_result, __FILE__, __LINE__)

/*
Constructor receives the size of the sound system and if we need the extradriver data
*/
SoundEngine::SoundEngine(int size, int extraDriver) {

    /*
    Create system and coreSystem
    */
    system = nullptr;
    ERRCHECK( FMOD::Studio::System::create(&system) );
    ERRCHECK( system->getCoreSystem(&coreSystem) );
    ERRCHECK( coreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0) );

    /*
    Programer sound context variables initialization
    */
    programmerSoundContext.sys = system;
    programmerSoundContext.coreSys = coreSystem;
    programmerSoundContext.dialogueString = "";
    
    /*
    Initialization of the sound system, we use the LiveUpdate to test the sound with FMOD
    */
    if(extraDriver==0){
        extraDriverData = nullptr;
        ERRCHECK( system->initialize(size, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_NORMAL, &extraDriverData) );
    }
    else{
        ERRCHECK( system->initialize(size, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_NORMAL, 0) );
    }

    /*
    To avoid console spamming
    */
    ERRCHECK(FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_NONE, FMOD_DEBUG_MODE_TTY));
}

/*
Destructor
*/
SoundEngine::~SoundEngine(){
    /*
    Release the sound system
    */
    ERRCHECK( system->release() );
    /*
    Empty the data storage
    */
    banks.clear();
    eventDescriptions.clear();
    eventInstances.clear();
}
/*
Update the sound engine
*/
void SoundEngine::update(){
    ERRCHECK( system->update() );
}
/*
When the player dies or resets the game we clean up the engine.
*/
void SoundEngine::release(){

    //Reset the dialogue state variables
    isDialogueEnhanced  = false;
    isDialoguePlaying   = false;

    //Flush the commants and the sample loading that was about to execute
    system->flushCommands();
    system->flushSampleLoading();

    //Unload the banks that arent essential
    vector<string> banksToErase;
    unordered_map<string, FMOD::Studio::Bank*>::iterator it = banks.begin();
    while (it != banks.end()) {
        if(it->first != "media/banks/Master.bank" && it->first != "media/banks/Master.strings.bank" && it->first != "media/banks/Armas.bank" && it->first != "media/banks/Jugador.bank" &&
        it->first != "media/banks/SonidosMaquinasExpendedoras.bank" && it->first != "media/banks/Zombies.bank" && it->first != "media/banks/VocesPersonaje_Esp.bank" && it->first != "media/banks/Musica.bank" && it->first != "media/banks/Menus.bank"
        && it->first != "media/banks/MediosNoLineales.bank"){
            banksToErase.emplace_back(it->first);
        }
        it++;
    }
    for (unsigned int i = 0; i<banksToErase.size(); i++){
        unloadBank(banksToErase[i]);
    }

    //Stop All the sounds of the general bus
    FMOD::Studio::Bus* bus  =   nullptr;
    system->getBus("bus:/", &bus);
    bus->stopAllEvents(FMOD_STUDIO_STOP_IMMEDIATE);

    //Release all instances of every descriptor
    for(auto ev : eventDescriptions){
        ev.second->releaseAllInstances();
    }

    //Clear the data storage
    eventDescriptions.clear();
    eventInstances.clear();

    //Update the system
    system->update();
}

/*
This method receives the bank path and loads it in the SoundEngine.
*/
void SoundEngine::loadBank(string bankName){

    //Load the bank in
    FMOD::Studio::Bank* newBank = nullptr;
    ERRCHECK( system->loadBankFile(bankName.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &newBank) );

    //Insert it in our storage
    banks.insert(pair<string, FMOD::Studio::Bank*>(bankName, newBank));

}

/*
Finds the bank that its sent as parameter, unloads and erases it
*/
void SoundEngine::unloadBank(string bankName){

    ERRCHECK( banks.find(bankName)->second->unload() );
    banks.erase(bankName);

}

/*
Load the basic banks for the game, those banks are used on every level for our game
*/
void SoundEngine::loadBasicBanks(){

    loadBank("media/banks/Master.bank");
    loadBank("media/banks/Master.strings.bank");
    loadBank("media/banks/Armas.bank");
    loadBank("media/banks/Jugador.bank");
    loadBank("media/banks/SonidosMaquinasExpendedoras.bank");
    loadBank("media/banks/Zombies.bank");
    loadBank("media/banks/VocesPersonaje_Esp.bank");
    loadBank("media/banks/Musica.bank");
    loadBank("media/banks/Menus.bank");
    loadBank("media/banks/MediosNoLineales.bank");

}

/*
Creates a event descriptor using the path and an int as a descriptor
*/
void SoundEngine::createDescriptor(string eventPath, uint16_t descriptorID){

    FMOD::Studio::EventDescription* eventDescription = nullptr;
    ERRCHECK( system->getEvent(eventPath.c_str(), &eventDescription) ); 
    eventDescriptions.insert(std::pair<uint16_t, FMOD::Studio::EventDescription*>(descriptorID, eventDescription));

}

/*
Creates a 2D instance using the descriptorID
*/
uint16_t SoundEngine::createInstance(uint16_t descriptorID){

    //Search the descriptor in our data storage
    FMOD::Studio::EventDescription* eventDescription = getDescriptor(descriptorID);

    //Create the instance
    FMOD::Studio::EventInstance* eventInstance = nullptr;
    ERRCHECK( eventDescription->createInstance(&eventInstance) );

    //Look for the first avaliable slot in our data storage and returns it if its not the final one
    for(uint16_t i = 0; i<eventInstances.size(); i++){
        if(eventInstances[i]==nullptr){
            eventInstances[i]=eventInstance;
            return i;
        }
    }

    //If its the final one return the last position
    eventInstances.push_back(eventInstance);
    return eventInstances.size()-1;

}

/*
Creates a 3D instance using the descriptorID
*/
uint16_t SoundEngine::createInstance(uint16_t descriptorID, Transform coords){

    //Search the descriptor in our data storage
    FMOD::Studio::EventDescription* eventDescription = getDescriptor(descriptorID);

    //Create de instance
    FMOD::Studio::EventInstance* eventInstance = nullptr;
    ERRCHECK( eventDescription->createInstance(&eventInstance) );

    //Retrieve the 3D attributes and assign them to the instance
    double nrx = sin(coords.rx);
    double nrz = cos(coords.rx);
    FMOD_3D_ATTRIBUTES attributes {FMOD_VECTOR{coords.x, coords.y, coords.z}, FMOD_VECTOR{0, 0, 0}, FMOD_VECTOR{(float)nrx, 0, (float)nrz}, FMOD_VECTOR{0, 1, 0}};
    ERRCHECK(eventInstance->set3DAttributes(&attributes));
    
    //Look for the first avaliable slot in our data storage and returns it if its not the final one
    for(uint16_t i = 0; i<eventInstances.size(); i++){
        if(eventInstances[i]==nullptr){
            eventInstances[i]=eventInstance;
            return i;
        }
    }
    //If its the final one return the last position
    eventInstances.push_back(eventInstance);
    return eventInstances.size()-1;

}

/*
This method is the used to create the instances because it checks if there is a description created and if not, it crates it
then if the sound is 3D it uses the 3D instance creation method or 2D if not necessary. Returns the index where the instance
is stored.
*/
uint16_t SoundEngine::prepareSound(string eventPath, uint16_t descriptorID, bool is3D, Transform coords){

    //Checks if there is a descriptor created
    if(eventDescriptions.find(descriptorID)==eventDescriptions.end()){
        
        //If its not created he calls the method to do it
        createDescriptor(eventPath, descriptorID);

    }
    //Checks if the sound is 3D or 2D
    if(is3D){
        return createInstance(descriptorID, coords);
    }
    return createInstance(descriptorID);
    
}

/*
Releases a sound instance, needs the index of the instance.
*/
void SoundEngine::releaseSound(uint16_t instanceIndex){

    ERRCHECK( eventInstances[instanceIndex]->release() );

}

/*
Updates the coordinates of an instance
*/
void SoundEngine::update3DInstance(Transform coords, uint16_t instanceID){

    double nrx = sin(coords.rx);
    double nrz = cos(coords.rx);
    FMOD_3D_ATTRIBUTES attributes {FMOD_VECTOR{coords.x, coords.y, coords.z}, FMOD_VECTOR{0, 0, 0}, FMOD_VECTOR{(float)nrx, 0, (float)nrz}, FMOD_VECTOR{0, 1, 0}};
    ERRCHECK(eventInstances[instanceID]->set3DAttributes(&attributes));

}

/*
Given an instance index vector and a descriptor index, it uses the descriptor to find the instance that was created with that descriptor.
*/
int SoundEngine::searchInstance(std::vector<uint16_t> instances, uint16_t descriptorID){
    
    int instance = -1;

    //Checks if the descriptor is created
    FMOD::Studio::EventDescription* descriptionFound = nullptr;
    unordered_map<uint16_t, FMOD::Studio::EventDescription*>::iterator eventFound = eventDescriptions.find(descriptorID);
        if(eventFound!=eventDescriptions.end()){

            //If exists, iterates throught the instances vector to see the one that match
            for(unsigned int i=0; i<instances.size(); i++){
                ERRCHECK( eventInstances[instances[i]]->getDescription(&descriptionFound) );

                //If found it sets the return to return the index.
                if(eventFound->second == descriptionFound){
                    instance = instances[i];
                }
            }
        }
    return instance;
}

/*
Starts an releases an instance
*/
void SoundEngine::playSound(uint16_t instanceIndex){
    
    ERRCHECK( eventInstances[instanceIndex]->start() );
    ERRCHECK( eventInstances[instanceIndex]->release() );

}

/*
Starts a sound
*/
void SoundEngine::startSound(uint16_t instanceIndex){
    ERRCHECK( eventInstances[instanceIndex]->start() );
}

/*
Stops a sound
*/
void SoundEngine::stopSound(uint16_t instanceIndex){

    ERRCHECK( eventInstances[instanceIndex]->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT) );

}

/*
Prepares an special instance for playing all the dialogues only with one of this kind of instance.
*/
uint16_t SoundEngine::prepareDialogue(string evenPath, uint16_t descriptorID){

    //Creates the instance
    uint16_t dev = prepareSound(evenPath, descriptorID, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}});

    //Using the programmerSound struct to store the information
    ERRCHECK( eventInstances[dev]->setUserData(&programmerSoundContext) );

    //Sets a callback that uses the instance to use the information store to pick the line to play, release when it ends, and to
    //track if the dialogue is playing.
    ERRCHECK( eventInstances[dev]->setCallback(programmerSoundCallback, FMOD_STUDIO_EVENT_CALLBACK_CREATE_PROGRAMMER_SOUND | FMOD_STUDIO_EVENT_CALLBACK_DESTROY_PROGRAMMER_SOUND | FMOD_STUDIO_EVENT_CALLBACK_STARTING | FMOD_STUDIO_EVENT_CALLBACK_STOPPED) );
    return dev;
}

/*
Play a dialogue, needs the instance index, the source and the line
*/
void SoundEngine::playDialogue(uint16_t instanceIndex, uint16_t vectorIndex, uint16_t keyIndex){
    programmerSoundContext.dialogueString = dialogueVector[vectorIndex][keyIndex];
    ERRCHECK( eventInstances[instanceIndex]->start() );
}

/*
Iterates throught the unordered map to search for a descriptor and returns a pointer for that descriptor if found, if not, returns nullptr.
*/
FMOD::Studio::EventDescription* SoundEngine::getDescriptor(uint16_t descriptorID){

    unordered_map<uint16_t, FMOD::Studio::EventDescription*>::iterator eventFound = eventDescriptions.find(descriptorID);

    if(eventFound!=eventDescriptions.end()){

        return eventFound->second;
    }
    
    return nullptr;

}

/*
Plays a sound without use the ECS, this is needed for the main menu, it receives a boolean to see if the sound must be released and the soundPath
*/
void SoundEngine:: playMenuSound(bool release, string soundPath){

    //Creates the descriptor
    FMOD::Studio::EventDescription* eventDescription = nullptr;
    ERRCHECK( system->getEvent(soundPath.c_str(), &eventDescription));

    //Creates the instance
    FMOD::Studio::EventInstance* eventInstance = nullptr;
    ERRCHECK( eventDescription->createInstance(&eventInstance) );

    //Plays and updates the system
    eventInstance->start();
    if(release)
        eventInstance->release();
    system->update();

}

/*
Stops and releases all the instances from a descriptor
*/
void SoundEngine:: stopMenuSound(string soundPath){
    FMOD::Studio::EventDescription* eventDescription = nullptr;
    ERRCHECK( system->getEvent(soundPath.c_str(), &eventDescription));
    eventDescription->releaseAllInstances();
}

/*
Updates the coordinates of the 3D listener (Usually the player)
*/
void SoundEngine::update3DListener(Transform coords){
    
    double nrx = sin(coords.rx);
    double nrz = cos(coords.rx);
    FMOD_3D_ATTRIBUTES attributes {FMOD_VECTOR{coords.x, coords.y, coords.z}, FMOD_VECTOR{0, 0, 0}, FMOD_VECTOR{(float)nrx, 0, (float)nrz}, FMOD_VECTOR{0, 1, 0}};
    ERRCHECK(system->setListenerAttributes(0, &attributes));

}

/*
Changes the parameter from an instance, it needs the parameter name, the chosen value, the instance index and the index for the description
*/
void SoundEngine::setParameter(string parameterName, uint8_t parameterValue, uint16_t instanceIndex, uint16_t descriptorID){
    
    //Iterator using the desciptor index
    unordered_map<uint16_t, FMOD::Studio::EventDescription*>::iterator eventFound = eventDescriptions.find(descriptorID);

    //If it exist, checks if that event has a parameter with the same name and changes it.
    if(eventFound!=eventDescriptions.end()){
        if(eventFound->second->getParameterDescriptionByName(parameterName.c_str(), &paramDesc)!= FMOD_ERR_EVENT_NOTFOUND){
            parameterID = paramDesc.id;
            ERRCHECK( eventInstances[instanceIndex]->setParameterByID(parameterID, parameterValue) );
        }
    }

}

/*
Retrieves the volume from the VCA
*/
float SoundEngine::getVCAVolume(string path){

    float dev;
    FMOD::Studio::VCA* vca = nullptr;
    ERRCHECK(system->getVCA(path.c_str(), &vca));
    vca->getVolume(&dev, NULL);
    return dev;

}

/*
Changes the volume of a VCA
*/
void SoundEngine::setVCAVolume(string path, float value){

    FMOD::Studio::VCA* vca = nullptr;
    ERRCHECK(system->getVCA(path.c_str(), &vca));
    vca->setVolume(value);

}

/*
Returns the actual dialogue state
*/
bool SoundEngine::getDialogueState(){

    return isDialoguePlaying;

}

/*
When a dialogue is playing the rest of the sound are lowered and then when it finish playing they return to the same levels.
*/
void SoundEngine::enhanceDialogues(){

    if(isDialoguePlaying && !isDialogueEnhanced){
        setVCAVolume(vcaRoutes[VCA_EFFECTS_ROUTE], getVCAVolume(vcaRoutes[VCA_EFFECTS_ROUTE])*0.6);
        setVCAVolume(vcaRoutes[VCA_MUSIC_ROUTE], getVCAVolume(vcaRoutes[VCA_MUSIC_ROUTE])*0.6);
        setVCAVolume(vcaRoutes[VCA_AMBIENT_ROUTE], getVCAVolume(vcaRoutes[VCA_AMBIENT_ROUTE])*0.6);
        isDialogueEnhanced = true;
    }
    else if (!isDialoguePlaying && isDialogueEnhanced){
        setVCAVolume(vcaRoutes[VCA_EFFECTS_ROUTE], getVCAVolume(vcaRoutes[VCA_EFFECTS_ROUTE])/0.6);
        setVCAVolume(vcaRoutes[VCA_MUSIC_ROUTE], getVCAVolume(vcaRoutes[VCA_MUSIC_ROUTE])/0.6);
        setVCAVolume(vcaRoutes[VCA_AMBIENT_ROUTE], getVCAVolume(vcaRoutes[VCA_AMBIENT_ROUTE])/0.6);
        isDialogueEnhanced = false;
    }

}

/*
Stops/Resume the bus received by the busPath parameter
*/
void SoundEngine:: changePauseState(string busPath, bool state){
    FMOD::Studio::Bus* bus  =   nullptr;
    system->getBus(busPath.c_str(), &bus);
    ERRCHECK(bus->setPaused(state));

    //If its paused changes a global variable to activa a low-pass & high-pass filter that affects everything but the menu sounds via a snapshot.
    if(state) setPauseState(0);
    else setPauseState(1);
    system->update();

}

/*
Stops all the sounds from a bus
*/
void SoundEngine:: stopAll(){

    FMOD::Studio::Bus* bus  =   nullptr;
    system->getBus("bus:/", &bus);
    ERRCHECK(bus->stopAllEvents(FMOD_STUDIO_STOP_IMMEDIATE));

}

/*
Changes the volume from a return buss that applies the reverb, and also changes a global parameter to change the actual ambient sound.
*/
void SoundEngine:: setZone(float reverbZone){

    zone = reverbZone;
    FMOD::Studio::Bus* bus  =   nullptr;
    system->getBus("bus:/Generales/Reverb", &bus);
    bus->setVolume(reverbZone);
    ERRCHECK(system->getParameterDescriptionByName("Localizacion", &paramDesc));
    if(trunc(reverbZone)==1)
        ERRCHECK(system->setParameterByID(paramDesc.id, 0));
    else
        ERRCHECK(system->setParameterByID(paramDesc.id, 1));

}

/*
Gets the actual zone
*/
float SoundEngine:: getZone(){

    return zone;

}

/*
Activates the low-pass and high-pass filter bia snapshot
*/
void SoundEngine:: setPauseState(uint8_t estado){
    ERRCHECK(system->getParameterDescriptionByName("Estadojuego", &paramDesc));
    ERRCHECK(system->setParameterByID(paramDesc.id, estado));
}

/*
For ERRCHECK
*/
#define CHECK_RESULT(op) \
    { \
        FMOD_RESULT res = (op); \
        if (res != FMOD_OK) \
        { \
            return res; \
        } \
    }

/*
Call back for programmer sound to use the data to know what line to play, also for release once it finishes and for change the isDialoguePlaying
variable
*/
FMOD_RESULT F_CALLBACK SoundEngine::programmerSoundCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE* event, void *parameters)
{
    FMOD::Studio::EventInstance* eventInstance = (FMOD::Studio::EventInstance*)event;

    if (type == FMOD_STUDIO_EVENT_CALLBACK_CREATE_PROGRAMMER_SOUND)
    {
        FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES* props = (FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES*)parameters;

        // Get our context from the event instance user data
        ProgrammerSoundContext* context = NULL;
        CHECK_RESULT( eventInstance->getUserData((void**)&context) );

        // Find the audio file in the audio table with the key
        FMOD_STUDIO_SOUND_INFO info;
        CHECK_RESULT( context->sys->getSoundInfo(context->dialogueString, &info) );

        FMOD::Sound* sound = NULL;
        CHECK_RESULT( context->coreSys->createSound(info.name_or_data, FMOD_LOOP_NORMAL | FMOD_CREATECOMPRESSEDSAMPLE | FMOD_NONBLOCKING | info.mode, &info.exinfo, &sound) );

        // Pass the sound to FMOD
        props->sound = (FMOD_SOUND*)sound;
        props->subsoundIndex = info.subsoundindex;
    }
    else if (type == FMOD_STUDIO_EVENT_CALLBACK_DESTROY_PROGRAMMER_SOUND)
    {
        FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES* props = (FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES*)parameters;

        // Obtain the sound
        FMOD::Sound* sound = (FMOD::Sound*)props->sound;

        // Release the sound
        CHECK_RESULT( sound->release() );
    }
    else if(type == FMOD_STUDIO_EVENT_CALLBACK_STARTING){
        isDialoguePlaying = true;
    }
    else if(type == FMOD_STUDIO_EVENT_CALLBACK_STOPPED){
        isDialoguePlaying = false;
    }

    return FMOD_OK;
}


