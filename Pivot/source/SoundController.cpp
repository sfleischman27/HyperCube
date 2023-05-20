//
//  SoundController.cpp
//  Pivot
//
//  Created by Gordon Tenev on 3/26/23.
//

#include "SoundController.h"

/** The initial fade-in on scene activation */
#define INITIAL_FADE  8.0
/** The default fade-in */
#define DEFAULT_FADE  0.15
/** The crossfade duration */
#define CROSS_FADE    0.25

bool SoundController::init(std::shared_ptr<cugl::AssetManager> assets){
    cugl::AudioEngine::get()->getMusicQueue()->setOverlap(CROSS_FADE);
    _sounds = std::unordered_map<std::string, std::shared_ptr<GameSound>>();
    _assets = assets;
    
    //4 audio nodes (main music, portal music, ending music, menu music)
    //_mixer = std::make_shared<cugl::audio::AudioMixer>();
    _mixer = _mixer->alloc(4, 2, 48000);
    
    _panner = std::make_shared<cugl::audio::AudioPanner>();
    _panner->init(2, 48000);
    
    _mixer->attach(1, _panner);
    _mixerwrapper = std::vector<std::shared_ptr<GameSound>>(_mixer->getWidth());
    return true;
}

/** removes the sound hashmap */
void SoundController::dispose(){
    _sounds.clear(); //TODO: make sure there are no leaks here!
}

/**
 * returns the GameSound stored under a certain name
 * @param name the name of the sound in the json
 */
std::shared_ptr<GameSound> SoundController::getSound(std::string name){
    return _sounds[name];
}

/**
 * sets master volume
 * @param volume the volume (0 - 100)
 */
void SoundController::setMasterVolume(float volume){
    _masterVolume = volume * 0.01;
    setAllNodeGains();
}

/**
 * returns the master volume (range 0 - 100)
 */
float SoundController::getMasterVolume(){
    return _masterVolume * 100.0;
}

void SoundController::enableMusic(bool sound){
    _volumeToggle = sound ? 1.0 : 0.0;
    setAllNodeGains();
}

bool SoundController::musicEnabled(){
    return _volumeToggle != 0.0;
}

void SoundController::setAllNodeGains(){
    for (auto const& it : _sounds){
        std::shared_ptr<GameSound> sound = it.second;
        if(sound->isStreaming()){ //portal is not counted as music
            sound->getNode()->setGain(sound->getVolume() * _masterVolume * _volumeToggle);
        } else {
            sound->getNode()->setGain(sound->getVolume() * _masterVolume);
        }
        //CULog("gain %f, %i", sound->getNode()->getGain(), sound->getNode()->getGain());
    }
    if(getMixerSlot(1) != nullptr){
        getMixerSlot(1)->getNode()->setGain(getMixerSlot(1)->getVolume() * _masterVolume);
    }
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
    _mixerwrapper[getSound(name)->attachSound(_mixer, _panner)] = getSound(name);
}

/**
 * returns AudioNode at mixer slot
 * @param slot the slot the node is in
 */
std::shared_ptr<GameSound> SoundController::getMixerSlot(int slot){
    return _mixerwrapper[slot];
}

/**
 * pre-emptively sets the volume of a sound, regardless if it is playing or not
 *  @param name the name of the sound in the json
 *  @param volume the volume to switch the sound to
*/
void SoundController::setVolume(std::string name, float volume){
    getSound(name)->setVolume(volume);
    if(getSound(name)->isStreaming()){
        getSound(name)->getNode()->setGain(getSound(name)->getVolume() * _masterVolume * _volumeToggle);
    } else {
        getSound(name)->getNode()->setGain(getSound(name)->getVolume() * _masterVolume);
    }

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
    getSound(name)->stop();
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
    
    std::shared_ptr<GameSound> sound = getSound(name);
    
    /*if(getSound(name)->isPlaying()){
        stopSound(name);
    }*/
    sound->getNode()->reset();
    if(sound->isStreaming()){
        streamNode(sound->getNode(), volume, loop);
    } else {
        cugl::AudioEngine::get()->play(name,sound->getNode(), loop, volume * _masterVolume, true);
    }
}

std::string stringifyVector(std::vector<std::string> v){
    std::string s;
    for (const auto &piece : v) s += piece;
    return s;
}

void SoundController::streamSounds(std::vector<std::string> names, std::vector<float> volumes, bool loop){
    for(int i = 0; i < names.size(); i++){
        std::string name = names[i];
        
        if(_sounds.find(name) == _sounds.end()){
            createSound(name);
        }
        
        _mixer->setName(stringifyVector(names));
        
        getSound(name)->setVolume(volumes[i]);
        
        if(getSound(name)->isStreaming()){
            attachSound(name);
        }
    }
    streamNode(_mixer, 1.0, loop);     //mixer streams at 1.0 -- acts as a scale for the tracks inside the mixer
}

void SoundController::setTrackVolumes(std::vector<std::string> names, float volume){
    for(std::string name : names){
        int slot = GameSound::findMixerSlot(name);
        setTrackVolume(slot, volume);
    }
}

/**
 * Sets the volume of a mixer slot
 * @param slot the slot of the song in the mixer m = 0, p = 1, e = 2, else = 3
 * @param volume the volume to set the sound to, from 0.0-1.0
 */
void SoundController::setTrackVolume(int slot, float volume){
    std::shared_ptr<cugl::audio::AudioNode> n = _mixerwrapper[slot]->getNode();
    if(n == nullptr){
        //CULogError("setTrackVolume node is null, name: %s, slot: %i", n->getName().c_str(), slot);
    }
    if(slot == 1){
        n->setGain(volume * _masterVolume);
    } else {
        n->setGain(volume * _volumeToggle * _masterVolume);
    }
}

void SoundController::streamNode(std::shared_ptr<cugl::audio::AudioNode> node, float volume, bool loop){
    std::shared_ptr<cugl::AudioQueue> queue = cugl::AudioEngine::get()->getMusicQueue();
    //if the queue is inactive, OR the current song playing isn't the same as the one that we want to play...play the song.
    
    if(queue->getState() == cugl::AudioEngine::State::INACTIVE || queue->current() != node->getName()){
        queue->setLoop(loop);
        queue->clear(CROSS_FADE);
        queue->enqueue(node, loop, volume * _volumeToggle * _masterVolume, CROSS_FADE);

    }
}

void SoundController::fadeIn(std::string name, float duration){    
    auto sound = getSound(name);
    
    setVolume(name, 1.0);
    
    if(sound->getNode()->isFadeIn()){
        sound->_keepFade = true;
    }
    
    if(sound->getNode()->isFadeOut()){
        sound->_keepFade = false;
        return;
    }
    
    if(std::find(_fadeIns.begin(), _fadeIns.end(), getSound(name)) == _fadeIns.end()){
        _fadeIns.push_back(sound);
    }
    
    //sound->setVolume(1.0);
    
    sound->_keepFade = true;
    
    sound->getNode()->mark();
    sound->getNode()->reset();
    sound->getNode()->unmark();
    sound->getNode()->setMarkOut(false);
    sound->getNode()->fadeIn(duration);
}

void SoundController::fadeOut(std::string name, float duration){
    auto sound = getSound(name);
    
    if(sound->getNode()->isFadeOut()){
        sound->_keepFade = true;
    }
    
    if(sound->getNode()->isFadeIn()){
        sound->_keepFade = false;
        return;
    }
    
    if(std::find(_fadeOuts.begin(), _fadeOuts.end(), getSound(name)) == _fadeOuts.end()){
        _fadeOuts.push_back(sound);
    }
    
    sound->_keepFade = true;
    
    sound->getNode()->setMarkOut(false);
    sound->getNode()->fadeOut(duration, true);
}

void SoundController::checkFades(){
    for (int i = _fadeIns.size()-1; i >= 0; i--){
        std::shared_ptr<GameSound> fader = _fadeIns[i];
        if(!fader->getNode()->isFadeIn()){
            _fadeIns.erase(_fadeIns.begin() + i);
            if(fader->_keepFade){
                fader->setVolume(1.0);
            } else {
                fader->_keepFade = true;
                fadeOut(fader->getName(), DEFAULT_FADE);
            }
        }
        if(_fadeIns.size() == 0 ){
            break;
        }
    }

    for (int i = _fadeOuts.size()-1; i >= 0; i--){
        std::shared_ptr<GameSound> fader = _fadeOuts[i];
        if(!fader->getNode()->isFadeOut()){
            _fadeOuts.erase(_fadeOuts.begin() + i);
            if(fader->_keepFade){
                fader->getNode()->setGain(0.0);
            } else {
                fader->_keepFade = true;
                fadeIn(fader->getName(), DEFAULT_FADE);
            }
        }
        if(_fadeOuts.size() == 0 ){
            break;
        }
    }
    
//    for (std::shared_ptr<GameSound> fader : _fadeOuts) {
//        if(!fader->getNode()->isFadeOut()){
//            fader->setVolume(0.0);
//            //_fadeOuts.erase(fader);
//        }
//        if(_fadeOuts.size() == 0 ){
//            break;
//        }
//    }
}

void SoundController::streamMixer(){
    streamNode(_mixer, 1.0, true);
}

void SoundController::setSpinnerPan(float angle, float lowest){
    //channel 0, 1 = left, right
    //angle 0 = forward, -pi left pi right
    
    float rad = cos(angle);
    //_panner->setPan(1,1,0);
    //_panner->setPan(0,0,1);
    float left = (rad + 1)/2 * (1-lowest) + lowest;
    float right = (-rad + 1)/2 * (1-lowest) + lowest;
    
    _panner->setPan(0,0,left);
    _panner->setPan(1,1,right);
    //CULog("pan L R: %f, %f ", left, right);
}
