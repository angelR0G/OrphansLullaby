/*
INCLUDES
*/
#include "sound.hpp"
#include "../components/sound.hpp"
#include "../Fmod/soundFactory.hpp"
#include "../Fmod/MasksPositions.hpp"

/*
DEFINES
*/
#define END_ROUND_LOW       0
#define END_ROUND_HIGH      19
#define KILL_LOW            20
#define KILL_HIGH           27
#define NO_POINTS           28
#define NEW_ZONE_LOW        29
#define NEW_ZONE_HIGH       32
#define BUY_WEAPON_LOW      33
#define BUY_WEAPON_HIGH     35
#define NO_AMMO_LOW         36
#define NO_AMMO_HIGH        38
#define EASTER_EGG_SOUND    230625

/*
Destructor
*/
SoundSystem::~SoundSystem(){
}

/*
Initialize the SoundEngine
*/
void SoundSystem::initialize(SoundEngine* sE){
    soundEngine = sE;
}

/*
Sound system update
*/
void SoundSystem::update(EntityMan& EM) {

    //Iterate throught entyties and get their sound and basic components
    using CList = MetaP::Typelist<SoundComponent>;
    using TList = MetaP::Typelist<>;
    int instanceID = -1;
    EM.foreach<CList, TList>([&](Entity& e){
        SoundComponent* soundCmp     = &EM.getComponent<SoundComponent>(e);
        BasicComponent* basicCmp     = &EM.getComponent<BasicComponent>(e);

        //If the entity is the player we update the listener position
        if(e.template hasTag<PlayerTag>()){
            CameraComponent* cameraCmp = &EM.getComponent<CameraComponent>(e);
            soundEngine->update3DListener(Transform{std::vector<float>{basicCmp->x, basicCmp->y, basicCmp->z, (float)basicCmp->orientation, 0, (float)basicCmp->verticalOrientation, 0, 0, 0}});
        }

        //We iterate throught the events vector to check the mask of every sound
        for(size_t i{}; i < soundCmp->idEvent.size(); ++i){

            //This is a normal sound, it starts and releases when played, its not a dialogue and doesnt need params.
            if( ((soundCmp->maskSounds[i] & SOUND_PLAY) == SOUND_PLAY) && ((soundCmp->maskSounds[i] & SOUND_NOT_RELEASE) != SOUND_NOT_RELEASE) &&  ((soundCmp->maskSounds[i] & SOUND_DIALOGUE) != SOUND_DIALOGUE) && ((soundCmp->maskSounds[i] & SOUND_UPDATE_PARAM) != SOUND_UPDATE_PARAM)){
                SoundFactory* factory = SoundFactory::Instance();
                uint16_t idInstance = factory->createInstanceFromEvent(soundCmp->idEvent[i],soundCmp->maskSounds[i]&SOUND_3D, Transform(std::vector<float>{basicCmp->x, basicCmp->y, basicCmp->z, (float)basicCmp->orientation, 0, (float)basicCmp->verticalOrientation}));
                soundEngine->playSound(idInstance);
                soundCmp->maskSounds[i] &= ~SOUND_PLAY;
            }
            //This is a normal sound that starts and releases but need to update a parameter before releasing
            else if(((soundCmp->maskSounds[i] & SOUND_NOT_RELEASE) != SOUND_NOT_RELEASE) && ((soundCmp->maskSounds[i] & SOUND_UPDATE_PARAM) == SOUND_UPDATE_PARAM) && ((soundCmp->maskSounds[i] & SOUND_PLAY) == SOUND_PLAY)){
                SoundFactory* factory = SoundFactory::Instance();
                uint16_t idInstance = factory->createInstanceFromEvent(soundCmp->idEvent[i],soundCmp->maskSounds[i]&SOUND_3D, Transform(std::vector<float>{basicCmp->x, basicCmp->y, basicCmp->z, (float)basicCmp->orientation, 0, (float)basicCmp->verticalOrientation}));
                std::unordered_map<std::string, float>::iterator it = soundCmp->paramValues.begin();
                soundEngine->startSound(idInstance);
                while(it != soundCmp->paramValues.end()){
                    soundEngine->setParameter(it->first, it->second, idInstance, soundCmp->idEvent[i]);
                    it++;
                }
                soundEngine->releaseSound(idInstance);
                soundCmp->maskSounds[i] &= ~SOUND_PLAY;
                soundCmp->maskSounds[i] &= ~SOUND_UPDATE_PARAM;
            }

            //These kind of sounds are created at the same time than the entity, so when the mask is set on play, and its not playing, we start the sound
            else if(soundCmp->maskSounds[i] & SOUND_NOT_RELEASE){
                if(((soundCmp->maskSounds[i] & SOUND_PLAY) == SOUND_PLAY) && (soundCmp->maskSounds[i] & SOUND_PLAYING) != SOUND_PLAYING){
                    soundEngine->startSound(soundEngine->searchInstance(soundCmp->idInstance, soundCmp->idEvent[i]));
                    soundCmp->maskSounds[i] |= SOUND_PLAYING;
                    soundCmp->maskSounds[i] &= ~SOUND_PLAY;                  
                }

                //Also we check if we need to update eny parameter
                if((soundCmp->maskSounds[i] & SOUND_UPDATE_PARAM) == SOUND_UPDATE_PARAM){
                    std::unordered_map<std::string, float>::iterator it = soundCmp->paramValues.begin();
                    while(it != soundCmp->paramValues.end()){
                        instanceID = soundEngine->searchInstance(soundCmp->idInstance, soundCmp->idEvent[i]);
                        if(instanceID != -1){
                            soundEngine->setParameter(it->first, it->second, instanceID, soundCmp->idEvent[i]);
                        }
                        it++;
                    }
                    soundCmp->maskSounds[i] &= ~SOUND_UPDATE_PARAM;
                }
            }

            //Dialogues have a different behaviour because, we want them to dont overleap and also give preference to questlines
            else if(((soundCmp->maskSounds[i] & SOUND_DIALOGUE) == SOUND_DIALOGUE) && ((soundCmp->maskSounds[i] & SOUND_PLAY) == SOUND_PLAY)){
                //We retrieve the instanceID from the data storage, there is only 1 dialogue instance that plays everything
                instanceID = soundEngine->searchInstance(soundCmp->idInstance, soundCmp->idEvent[i]);
                if(instanceID != -1){
                    //Source is for radio o player, and playedLine is the key that is assigned to the dialogue
                    uint16_t source{}, playedLine{};
                    //First we check if we can find both playerDialogue and radioDialogue
                    if((soundCmp->paramValues.find("playerDialogue") != soundCmp->paramValues.end()) && (soundCmp->paramValues.find("radioDialogue") != soundCmp->paramValues.end())){
                        //If the player is going to say something and the radio is not we check if the radio is already saying something
                        if((soundCmp->paramValues.find("playerDialogue")->second == PLAYER_DIALOGUE_PLAYING) && (soundCmp->paramValues.find("radioDialogue")->second == RADIO_DIALOGUE_STOPPED)){
                            //Check of the radio is speaking, if not we check what line to say
                            if(!soundEngine->getDialogueState()){
                                source = 0;
                                //TipoVoz = 1 means we play a buy sound with the dialogue TipoVoz = 2 has a bit of delay and TipoVoz = 3 doesnt have any delay
                                if(soundCmp->paramValues.find("line") != soundCmp->paramValues.end()){
                                    //This gets a random sentence for when player unlocks a new zone
                                    if(soundCmp->paramValues.find("line")->second == NEW_ZONE_SOUND){
                                        soundEngine->setParameter("TipoVoz", 1, instanceID, soundCmp->idEvent[i]);
                                        playedLine = NEW_ZONE_LOW + rand() % (NEW_ZONE_HIGH - NEW_ZONE_LOW + 1);
                                    }
                                    //Sound for when the player ends a round
                                    if(soundCmp->paramValues.find("line")->second == END_ROUND_SOUND){
                                        soundEngine->setParameter("TipoVoz", 2, instanceID, soundCmp->idEvent[i]);
                                        playedLine = END_ROUND_LOW + rand() % (END_ROUND_HIGH - END_ROUND_LOW + 1);
                                    }
                                    //Sound that is played sometimes when player kills enemies
                                    if(soundCmp->paramValues.find("line")->second == KILL_SOUND){
                                        soundEngine->setParameter("TipoVoz", 3, instanceID, soundCmp->idEvent[i]);
                                        playedLine = KILL_LOW + rand() % (KILL_HIGH - KILL_LOW + 1);
                                        //Easter egg that is an special laught
                                        if(playedLine == KILL_HIGH){
                                            if(1 != rand()%EASTER_EGG_SOUND)
                                                playedLine = KILL_LOW + rand() % ((KILL_HIGH-1) - KILL_LOW + 1);   
                                        }
                                    }
                                    //Sound when the player has no points
                                    if(soundCmp->paramValues.find("line")->second == NO_POINTS_SOUND){
                                        soundEngine->setParameter("TipoVoz", 3, instanceID, soundCmp->idEvent[i]);
                                        playedLine = NO_POINTS; 
                                    }
                                    //Sound when the player buys a weapon
                                    if(soundCmp->paramValues.find("line")->second == BUY_WEAPON_SOUND){
                                        soundEngine->setParameter("TipoVoz", 1, instanceID, soundCmp->idEvent[i]);
                                        playedLine = BUY_WEAPON_LOW + rand() % (BUY_WEAPON_HIGH - BUY_WEAPON_LOW + 1);
                                    }
                                    //Sound when the player tries to reload and doesnt have ammo
                                    if(soundCmp->paramValues.find("line")->second == NO_AMMO_SOUND){
                                        soundEngine->setParameter("TipoVoz", 3, instanceID, soundCmp->idEvent[i]);
                                        playedLine = NO_AMMO_LOW + rand() % (NO_AMMO_HIGH - NO_AMMO_LOW + 1); 
                                    }
                                }
                                //Then set the variable to dont play any sound to dont repeat another time the same line, remove the play from the mask
                                //and play the dialogue
                                soundCmp->paramValues.insert_or_assign("playerDialogue", PLAYER_DIALOGUE_STOPPED);
                                soundCmp->maskSounds[i] &= ~SOUND_PLAY;
                                soundEngine->playDialogue(instanceID, source, playedLine); 
                            }
                            //If there is a radio sound playing, remove the play command and change the variable so the sound will be ignored
                            else{
                                soundCmp->paramValues.insert_or_assign("playerDialogue", PLAYER_DIALOGUE_STOPPED);
                                soundCmp->maskSounds[i] &= ~SOUND_PLAY;
                            }
                        }
                        //Check if the voice its from the radio
                        else if(soundCmp->paramValues.find("radioDialogue")->second == RADIO_DIALOGUE_PLAYING){
                            /*
                            Check if there is a dialogue playing right now, if not, play the new dialogue, on the other hand just lets the radio 
                            dialogue to play and doesnt remove the play mask, so once the current dialogue stops, it will be followed by the other
                            one
                            */
                            if(!soundEngine->getDialogueState()){
                                source = 1;
                                if(soundCmp->paramValues.find("line") != soundCmp->paramValues.end()){
                                    playedLine = soundCmp->paramValues.find("line")->second;
                                }
                                soundCmp->paramValues.insert_or_assign("radioDialogue", RADIO_DIALOGUE_STOPPED);
                                soundCmp->maskSounds[i] &= ~SOUND_PLAY;
                                //TipoVoz = 0 means that wil have a radio activation sound and static
                                soundEngine->setParameter("TipoVoz", 0, instanceID, soundCmp->idEvent[i]);
                                soundEngine->playDialogue(instanceID, source, playedLine); 

                            }
                        }                        
                    }
                }                  
            }
            //Stops a sound that its currently playing, used for non released soundsand changes their masks
            if((soundCmp->maskSounds[i] & SOUND_STOP) == SOUND_STOP){
                soundCmp->maskSounds[i] &= ~SOUND_PLAYING;
                soundCmp->maskSounds[i] &= ~SOUND_STOP;
                soundEngine->stopSound(instanceID = soundEngine->searchInstance(soundCmp->idInstance, soundCmp->idEvent[i]));
            }
            //Updates the position of any sound, mostly used for non release ones
            if((soundCmp->maskSounds[i] & SOUND_UPDATE_POSITION) == SOUND_UPDATE_POSITION){
                soundCmp->maskSounds[i] &= ~SOUND_UPDATE_POSITION;
                soundEngine->update3DInstance(Transform{std::vector<float>{basicCmp->x, basicCmp->y, basicCmp->z, (float)basicCmp->orientation, 0, (float)basicCmp->verticalOrientation, 0, 0, 0}}, soundEngine->searchInstance(soundCmp->idInstance, soundCmp->idEvent[i]));
            }
            //Special mask that makes that a sound has his position updated always, on every update
            if((soundCmp->maskSounds[i] & SOUND_ALWAYS_UPDATE) == SOUND_ALWAYS_UPDATE){
                int instance = soundEngine->searchInstance(soundCmp->idInstance, soundCmp->idEvent[i]);
                if(instance != -1){
                    soundEngine->update3DInstance(Transform{std::vector<float>{basicCmp->x, basicCmp->y, basicCmp->z, (float)basicCmp->orientation, 0, (float)basicCmp->verticalOrientation, 0, 0, 0}}, instance);
                }
            }
            //Releases a sound
            if(soundCmp->maskSounds[i] & SOUND_RELEASE){
                instanceID = soundEngine->searchInstance(soundCmp->idInstance, soundCmp->idEvent[i]);
                if(instanceID != -1){
                    soundEngine->releaseSound(instanceID);
                }
            }
        }
    });
    //Adjust the volume, because it improves the dialogues if there is one of them playing.
    soundEngine->enhanceDialogues();
    soundEngine->update();
}