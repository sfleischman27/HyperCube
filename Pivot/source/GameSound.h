//
//  Sound.h
//  Platformer
//
//  A single sound instance.
//
//  Created by Gordi Tenev on 3/12/23.
//

#ifndef GameSound_h
#define GameSound_h
#include <cugl/cugl.h>

/**
 *  Include functions that play music for different states
 *
 *  Include start music and stop music functions
 */

class GameSound {
protected:
    /** name of sound */
    std::string _name;
    
    /** source of sound to play */
    std::shared_ptr<cugl::Sound> _source;
    
    /** volume of the sound, from 0.0-1.0 */
    float _volume;
    
    /** is the sound streamed? (is it music) */
    bool _streaming;
    
public:
    /** inits Sound with...
     * @param name the name of the sound file (to use as a key when playing as an SFX)
     * @param source the source of the sound in the assetmanager, handled outside of this class
     * @param streaming if the sound will be streamed (music)
     * @param volume the volume of the sound, from 0.0 to 1.0
     */
    GameSound(std::string name, std::shared_ptr<cugl::Sound> source, float volume, bool streaming){
        _name = name;
        _source = source;
        _volume = volume;
        _streaming = streaming;
    }
    
    /** returns the name of the Sound */
    std::string getName(){
        return _name;
    }
    
    std::shared_ptr<cugl::Sound> getSource(){
        return _source;
    }
    
    float getVolume(){
        return _volume;
    }
    
    void setVolume(float volume){
        _volume = volume;
    }
    
    /** returns if the Sound is streaming or not */
    bool isStreaming(){
        return _streaming;
    }
    /** plays the Sound without looping */
    void play(){
        play(false);
    }

    void play(bool loop){
        if(_streaming){
            cugl::AudioEngine::get()->getMusicQueue()->play(_source, loop, _volume);
        } else {
            cugl::AudioEngine::get()->play(_name, _source, loop, _volume);
        }
    }
    
    void stop(){
        if(_streaming){
            cugl::AudioEngine::get()->getMusicQueue()->clear();
        } else {
            cugl::AudioEngine::get()->clear(_name);
        }
    }

    bool isPlaying(){
        return cugl::AudioEngine::get()->isActive(_name);
    }

    
};

#endif /* Sound_h */
