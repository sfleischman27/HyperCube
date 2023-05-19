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
    
    std::shared_ptr<cugl::audio::AudioNode> _node;
    
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
        _node = _source->createNode();
        _node->setName(_name);
        /*
        if (cugl::AudioSample* sample = dynamic_cast<cugl::AudioSample*>(source.get()))
        {
            _player = std::make_shared<cugl::audio::AudioPlayer>();
            _player->init(*sample);
        } else {
            CUAssertLog(false, "Audio is not a sample!!!");
        }*/
    }
    
    /** attaches sound to a mixer. returns the slot number that it fits into.
     * @param mixer the mixer you're attaching to
     */
    int attachSound(std::shared_ptr<cugl::audio::AudioMixer> mixer){
        //cugl::AudioEngine::get()->getMusicQueue()->enqueue(sound->getSource());
        int slot = findMixerSlot(_name);

        if(_streaming){
            mixer->attach(slot, _node);
        }
        
        return slot;
    }
    
    /** returns the name of the Sound */
    std::string getName(){
        return _name;
    }
    
    std::shared_ptr<cugl::Sound> getSource(){
        return _source;
    }
    
    std::shared_ptr<cugl::audio::AudioNode> getNode(){
        return _node;
    }
    
    float getVolume(){
        return _volume;
    }
    
    void setVolume(float volume){
        _node->setGain(volume);
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
            cugl::AudioEngine::get()->getMusicQueue()->play(_node, loop, _volume);
        } else {
            cugl::AudioEngine::get()->play(_name, _node, loop, _volume);
        }
    }
    
    void stop(){
        if(_streaming){
            float crossfade = 0.5;
            cugl::AudioEngine::get()->getMusicQueue()->advance(0, crossfade);
        } else {
            cugl::AudioEngine::get()->clear(_name);
        }
    }

    bool isPlaying(){
        return cugl::AudioEngine::get()->isActive(_name);
    }

    /** returns which slot should be used in the mixer given the string's name. mixer channel is determined by:
     *  _m: main level music
     *  _p: portal menu music
     *  _e: level ending music _
     * @param name the name of the sound in the json.
     */
    static int findMixerSlot(std::string name){
        int slot = 0;
        switch(name.back()) {
            case 'm': //main music
                slot = 0;
            break;
            case 'p':
                slot = 1;
            break;
            case 'e':
                slot = 2;
            default:
                slot = 3;
        }
        return slot;
    };
    
    
};

#endif /* Sound_h */
