//
//  SoundController.cpp
//  Pivot
//
//  Created by Gordon Tenev on 3/26/23.
//

#include "SoundController.h"

/** The initial fade-in on scene activation */
#define INITIAL_FADE  8.0
/** The crossfade duration */
#define CROSS_FADE    0.5

bool SoundController::init(std::shared_ptr<cugl::AssetManager> assets){
    cugl::AudioEngine::get()->getMusicQueue()->setOverlap(CROSS_FADE);
    _sounds = std::unordered_map<std::string, std::shared_ptr<GameSound>>();
    _assets = assets;
    
    //3 audio nodes (main music, portal music, ending music, menu music)
    //_mixer = std::make_shared<cugl::audio::AudioMixer>();
    _mixer = _mixer->alloc(4, 2, 44100);
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
std::shared_ptr<GameSound> SoundController::createSound(std::string name){
    std::shared_ptr<cugl::Sound> source = _assets->get<cugl::Sound>(name);
    
    bool streaming = false;
    
    //cast the source into AudioSample (all of our sounds should be this, but just in case i've added some an edge case if we use AudioWaveform for some unknown reason) (which shouldn't be streamed)
    if (cugl::AudioSample* sample = dynamic_cast<cugl::AudioSample*>(source.get()))
    {
        streaming = sample->isStreamed();
    }

    std::shared_ptr<GameSound> sound = std::make_shared<GameSound>(name, source, 0.0f, streaming);;
    _sounds[name] = sound;
    
    return sound;
}


/**
 * attaches existent sound object to mixer. mixer channel is determined by:
 *  _m: main level music
 *  _p: portal menu music
 *  _e: level ending music _
 * @param name the name of the sound in the json.
 */
void SoundController::attachSound(std::string name){
    std::shared_ptr<GameSound> sound = _sounds[name];
    
    sound->attachSound(_mixer);
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

void SoundController::stopSound(std::string name){
    _sounds[name]->stop();
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
    
    std::shared_ptr<GameSound> sound = _sounds[name];
    
    /*if(_sounds[name]->isPlaying()){
        stopSound(name);
    }*/
    
    if(sound->isStreaming()){
        streamNode(sound->getNode(), volume, loop);
    } else {
        cugl::AudioEngine::get()->play(name,sound->getSource(),loop);
    }
    
    
    /*if(_sounds[name]->isPlaying()){
        _sounds[name]->stop();
    }
    _sounds[name]->play(loop);*/
}

std::string stringifyVector(std::vector<std::string> v){
    std::string s;
    for (const auto &piece : v) s += piece;
    return s;
}

void SoundController::streamSounds(std::vector<std::string> names, float volume, bool loop){
    for(std::string name : names){
        if(_sounds.find(name) == _sounds.end()){
            createSound(name);
        }
        
        _mixer->setName(stringifyVector(names));
        
        if(_sounds[name]->isStreaming()){
            _sounds[name]->attachSound(_mixer);
            streamNode(_mixer, volume, loop);
        }
    }
}

void SoundController::streamNode(std::shared_ptr<cugl::audio::AudioNode> node, float volume, bool loop){
    std::shared_ptr<cugl::AudioQueue> queue = cugl::AudioEngine::get()->getMusicQueue();
    //if the queue is inactive, OR the current song playing isn't the same as the one that we want to play...play the song.
    
    if(queue->getState() == cugl::AudioEngine::State::INACTIVE || queue->current() != node->getName()){
        queue->setLoop(loop);
        queue->clear(CROSS_FADE);
        queue->enqueue(node, loop, volume, CROSS_FADE);

    }
}

