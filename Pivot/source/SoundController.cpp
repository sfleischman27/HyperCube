//
//  SoundController.cpp
//  Pivot
//
//  Created by Gordon Tenev on 3/26/23.
//

#include "SoundController.h"

bool SoundController::init(std::shared_ptr<cugl::AssetManager> assets){
    _sounds = std::unordered_map<std::string, std::shared_ptr<GameSound>>();
    _assets = assets;
    return true;
}

/** removes the sound hashmap */
void SoundController::dispose(){
    _sounds.clear(); //TODO: make sure there are no leaks here!
}

/**
 * creates a new sound object and adds it to the sounds hashmap
 * @param name the name of the sound in the json
 * @param streaming if the sound is streaming or not (is it music or SFX?)
 */
void SoundController::createSound(std::string name){
    std::shared_ptr<cugl::Sound> source = _assets->get<cugl::Sound>(name);
    
    bool streaming = false;
    
    //cast the source into AudioSample (all of our sounds should be this, but just in case i've added some an edge case if we use AudioWaveform for some unknown reason) (which shouldn't be streamed)
    if (cugl::AudioSample* sample = dynamic_cast<cugl::AudioSample*>(source.get()))
    {
        streaming = sample->isStreamed();
    }

    std::shared_ptr<GameSound> sound = std::make_shared<GameSound>(name, source, 0.0f, streaming);;
    _sounds[name] = sound;

}

/**
 * pre-emptively sets the volume of a sound, regardless if it is playing or not
 *  @param name the name of the sound in the json
 *  @param volume the volume to switch the sound to
*/
void SoundController::setVolume(std::string name, float volume){
    _sounds[name]->setVolume(volume);
}

/**
 * Plays sound without looping
 *  @param name the name of the sound in the json
 *  @param volume the volume of the sound, from 0.0-1.0
 */
void SoundController::playSound(std::string name, float volume){
    playSound(name, volume, false);
}

/**
 * Plays a sound that has been queued into the sound hashmap under name.
 * If there is no such sound, create the sound object and add it to the hashmap before playing.
 *  @param name the name of the sound in the json
 *  @param loop should the sound be looping
 *  @param volume the volume of the sound, from 0.0-1.0
 */
void SoundController::playSound(std::string name, float volume, bool loop){
    //sound->play(loop);
    if(_sounds.find(name) == _sounds.end()){
        createSound(name);
    }
    setVolume(name, volume);
    
    
    if(_sounds[name]->isPlaying()){
        _sounds[name]->stop();
    }
    _sounds[name]->play(loop);
}
