/*
Include section
*/
#include "soundFactory.hpp"

#include <vector>
#include <string>

/*
Vector with all paths from events
*/
const std::vector<std::string> eventPath = { 
    "event:/Efectos/Armas/Pistolas/Pistola basica/Disparo pistola basica", "event:/Voces/MaquinaBalasBienvenida", 
    "event:/Efectos/Zombie General/Dolor", "event:/Efectos/Zombie General/Ataque zombie", 
    "event:/Efectos/Armas/Pistolas/Pistola basica/Recarga pistola basica", "event:/Voces/MaquinaBalasDespedida",
    "event:/Efectos/Armas/Hitmarker", "event:/Efectos/Armas/Impacto bala zombie", 
    "event:/Efectos/Zombie Explosivo/Pasos", "event:/Efectos/Zombie Explosivo/Explosion",
    "event:/Efectos/Jugador/Dolor", "event:/Efectos/Jugador/Pasos Ceramica", 
    "event:/Efectos/Zombie General/Pasos Ceramica", "event:/Voces/MaquinaMejoras",
    "event:/Ambientes/Ambiente Centro Comercial", "event:/Voces/Dialogos",
    "event:/Efectos/Armas/Escopetas/Escopeta de combate/Disparo escopeta combate", "event:/Efectos/Armas/Escopetas/Escopeta de combate/Recarga escopeta de combate",
    "event:/Efectos/Armas/Pistolas/Revolver/Disparo revolver", "event:/Efectos/Armas/Pistolas/Revolver/Recarga revolver",
    "event:/Efectos/Armas/Pistolas/Pistola repetidora/Disparo pistola repetidora", "event:/Efectos/Armas/Rifles/M16/Disparo M16",
    "event:/Efectos/Armas/Rifles/M16/Recarga M16", "event:/Efectos/Armas/Escopetas/Escopeta de doble canyon/Disparo escopeta",
    "event:/Efectos/Armas/Escopetas/Escopeta de doble canyon/Recarga escopeta", "event:/Efectos/Armas/Lanza Misiles/Disparo lanza misiles",
    "event:/Efectos/Armas/Lanza Misiles/Recarga lanza misiles", "event:/Efectos/Armas/Lanza Misiles/Explosion lanza misiles",
    "event:/Efectos/Armas/Metralletas/Metralleta ligera/Disparo metralleta ligera", "event:/Efectos/Armas/Metralletas/Metralleta ligera/Recarga metralleta ligera",
    "event:/Efectos/Armas/Metralletas/Metralleta pesada/Disparo metralleta pesada", "event:/Efectos/Armas/Metralletas/Metralleta pesada/Recarga metralleta pesada",
    "event:/Efectos/Armas/Rifles/M1/Disparo M1", "event:/Efectos/Armas/Rifles/M1/Recarga M1",
    "event:/Voces/MaquinaInyeccionesBienvenida", "event:/Voces/MaquinaInyeccionesDespedida",
    "event:/Efectos/Generales/Explosion", "event:/Efectos/Generales/Compra",
    "event:/Efectos/Jugador/Vida Baja", "event:/Música/Musica Accion",
    "event:/Menús/PausaJuego", "event:/Efectos/Armas/Pistolas/Pistola laser/Disparo",
    "event:/Efectos/Armas/Pistolas/Pistola laser/Recarga", "event:/Efectos/Jugador/Recoger Objeto",
    "event:/Ambientes/Generales/Musica Maquina Inyecciones", "event:/Ambientes/Generales/Musica Maquina Municion",
    "event:/Ambientes/Generales/Panel Electrico", "event:/Efectos/Generales/BarrilSenyuelo",
    "event:/Efectos/Generales/Nueva Ronda", "event:/Efectos/Jugador/Respiracion Agonica",
    "event:/Efectos/M1K3/Vuelo", "event:/Efectos/Zombie Tanque/Pasos",
    "event:/Efectos/Zombie Tanque/Ataque Normal", "event:/Efectos/Zombie Tanque/Ataque Especial",
    "event:/Efectos/Zombie General/Grito Ataque", "event:/Efectos/Zombie Escupidor/Escupitajo",
    "event:/Ambientes/Generales/Televisiones", "event:/Ambientes/Generales/Zona navidad",
    "event:/Ambientes/Generales/Zona arcade", "event:/Ambientes/Generales/Turron easterEgg",
    "event:/Efectos/Armas/Cambio de Arma", "event:/Efectos/Armas/Disparo sin municion"
};

/*
If there is not an instance of the SoundFactory it creates a new one (Singleton)
*/
SoundFactory* SoundFactory::Instance(){

    if(pSoundFactory.get() ==  nullptr){
        pSoundFactory  =   std::unique_ptr<SoundFactory> (new SoundFactory() );
    }

    return pSoundFactory.get();

}

//Constructor private
SoundFactory::SoundFactory(){
}

//Destructor
SoundFactory::~SoundFactory(){
}

/*
Initializes the soundEngine for the factory
*/
void SoundFactory::initializeEngine(SoundEngine* sE){

    soundEngine = sE;

}

/*
Method to create any non-dialogue instance
*/
uint16_t SoundFactory::createInstanceFromEvent(uint16_t eventPosition, bool is3D, Transform coords){

    return soundEngine->prepareSound(eventPath[eventPosition], eventPosition, is3D, coords);

}

/*
Method to create a dialogue instance
*/
uint16_t SoundFactory::createDialogue(uint16_t eventPosition){

    return soundEngine->prepareDialogue(eventPath[eventPosition], eventPosition);

}
