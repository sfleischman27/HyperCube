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
    
    /** vector that stores the nodes inside the audiomixer */
    std::vector<std::shared_ptr<GameSound>> _mixerwrapper;
    
    /** panner for the portal hum */
    std::shared_ptr<cugl::audio::AudioPanner> _panner;
    
    /** master volume, range 0 - 100 */
    float _masterVolume = 0.5f;
    
    /** toggle volume, either 0 or 1 */
    float _volumeToggle = 1.0f;

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
     * returns the GameSound stored under a certain name
     * @param name the name of the sound in the json
     */
    std::shared_ptr<GameSound> getSound(std::string name);
    
    /**
     * attaches existent sound object to mixer. mixer channel is determined by:
     *  _m: main level music
     *  _p: portal menu music
     *  _e: level ending music _
     * @param name the name of the sound in the json.
     */
    void attachSound(std::string name);
    
    /**
     * returns AudioNode at mixer slot
     * @param slot the slot the node is in
     */
    std::shared_ptr<GameSound> getMixerSlot(int slot);

    
#pragma mark master sound controls
    /**
     * sets master volume
     * @param volume the volume (0 - 1.0)
     */
    void setMasterVolume(float volume);
    
    /**
     * returns the master volume (range 0 - 1.0)
     */
    float getMasterVolume();
    
    /**
     * enables sound
     * @param sound whether to enable/disable sound
     */
    void enableMusic(bool sound);
    
    /**
     * if sound is muted
     */
    bool musicEnabled();
    
    /**
     * sets the gain of all nodes to master volume
     */
    void setAllNodeGains();
#pragma mark sound playback
    
    /**
     * pre-emptively sets the volume of a sound, regardless if it is playing or not
     *  @param name the name of the sound in the json
     *  @param volume the volume to switch the sound to
    */
    void setVolume(std::string name, float volume);
    
    void stopSound(std::string name);
    
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
    
    /**
     * Streams multiple sounds that has been queued into the sound hashmap. Puts them all on the mixer and plays at the same time
     *  @param names the names of the sounds in the json, as a vector
     *  @param loop should the sound be looping
     *  @param volumes the volume of each sound in the order described in the vector, from 0.0-1.0
     */
    
    void streamSounds(std::vector<std::string> names, std::vector<float> volumes, bool loop);

    /**
     * Sets the volume of a certain amount of streamed audio tracks
     * @param names the names of the sounds in the json, as a vector
     * @param volume the volume to set the sound to, from 0.0-1.0
     */
    void setTrackVolumes(std::vector<std::string> names, float volume);
    
    /**
     * Sets the volume of a mixer slot
     * @param slot the slot of the song in the mixer m = 0, p = 1, e = 2, else = 3
     * @param volume the volume to set the sound to, from 0.0-1.0
     */
    void setTrackVolume(int slot, float volume);

    /**
     * Streams a AudioNode. With crossfade and all that.
     *  @param name the name of the sound in the json
     *  @param loop should the sound be looping
     *  @param volume the volume of the sound, from 0.0-1.0
     */
    void streamNode(std::shared_ptr<cugl::audio::AudioNode> node, float volume, bool loop);
    
    /**
     * Sets pan of spinner
     * @param angle the angle of the spinner, in RADIANS
     * @param lowest the lowest value a panned channel can be.
     */
    void setSpinnerPan(float angle, float lowest);
    
    /**
     * fades in selected audio
     */
    void fadeIn(std::string name, float duration);
    
    /**
     * fades out selected audio
     */
    void fadeOut(std::string name, float duration);
    
    /**
     * Streams current mixer contents
     */
    void streamMixer();
};

#endif /* SoundController_h */
