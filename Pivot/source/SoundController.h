//
//  SoundController.h
//  Platformer
//
//  Controls the sound/music by providing functions that play music for different game modes
//
//  Created by Gordi Tenev on 3/12/23.
//

#ifndef SoundController_h
#define SoundController_h
#include <cugl/cugl.h>
#include "GameSound.h"

/**
 *  Include functions that play music for different states
 *
 *  Include start music and stop music functions
 */

class SoundController {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** The audio mixer for vertical sound*/
    std::shared_ptr<cugl::audio::AudioMixer> _mixer;
    
    /** the hashmap that stores all the sounds that the soundcontroller plays*/
    std::unordered_map<std::string, std::shared_ptr<GameSound>> _sounds;

#pragma mark constructor
public:
    SoundController() { };
    
    ~SoundController(){ dispose(); };
    
    bool init(std::shared_ptr<cugl::AssetManager> _assets);
    
    /** removes the sound hashmap */
    void dispose();
    
#pragma mark sound creation
    
    /**
     * creates a new sound object and adds it to the sounds hashmap
     * @param name the name of the sound in the json
     * @param streaming if the sound is streaming or not (is it music or SFX?)
     */
    std::shared_ptr<GameSound> createSound(std::string name);
    
    /**
     * attaches existent sound object to mixer. mixer channel is determined by:
     *  _m: main level music
     *  _p: portal menu music
     *  _e: level ending music _
     * @param name the name of the sound in the json.
     */
    void attachSound(std::string name);
    
#pragma mark sound playback
    
    /**
     * pre-emptively sets the volume of a sound, regardless if it is playing or not
     *  @param name the name of the sound in the json
     *  @param volume the volume to switch the sound to
    */
    void setVolume(std::string name, float volume);
    
    /**
     * Plays sound without looping
     *  @param name the name of the sound in the json
     *  @param volume the volume of the sound, from 0.0-1.0
     */
    void playSound(std::string name, float volume);
    
    /**
     * Plays a sound that has been queued into the sound hashmap.
     *  @param name the name of the sound in the json
     *  @param loop should the sound be looping
     *  @param volume the volume of the sound, from 0.0-1.0
     */
    void playSound(std::string name, float volume, bool loop);
};

#endif /* SoundController_h */
