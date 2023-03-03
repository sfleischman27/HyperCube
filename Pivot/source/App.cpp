//
//  App.cpp
//  Platformer
//
//  This is the root class for the game. The file main.cpp accesses this class
//  to run the application. We will have most of the game logic in other classes
//
//  This class will change between different game modes (main, level select, etc)
//
//  Created by Sarah Fleischman on 2/22/23.
//

#include "App.h"

using namespace cugl;


#pragma mark -
#pragma mark Application State

bool demo = false;

/**
 * The method called after OpenGL is initialized, but before running the application.
 *
 * This is the method in which all user-defined program intialization should
 * take place.  You should not create a new init() method.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to FOREGROUND,
 * causing the application to run.
 */
void PivotApp::onStartup() {

    bool isMac;
    // Detect OS
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    CULog("System: This is Windows");
    isMac = false;
#else
    CULog("System: This is Mac");
    isMac = true;
#endif

    std::string jsonPath = isMac ? "../json/assets.json" : "../../assets/jsonWindows/assets.json";
	
    if (demo){
        _assets = AssetManager::alloc();
        _batch  = SpriteBatch::alloc();
        
        // Start-up basic input
#ifdef CU_TOUCH_SCREEN
        Input::activate<Touchscreen>();
#else
        Input::activate<Mouse>();
#endif

        _assets->attach<Font>(FontLoader::alloc()->getHook());
        _assets->attach<Texture>(TextureLoader::alloc()->getHook());
        _assets->attach<Sound>(SoundLoader::alloc()->getHook());
        _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
        
        // Create a "loading" screen
        _loaded = false;
        _demoloading.init(_assets);
        
        // Queue up the other assets
        AudioEngine::start();
        _assets->loadDirectoryAsync(jsonPath, nullptr);
        
        Application::onStartup(); // YOU MUST END with call to parent
    }
    else{
        _assets = AssetManager::alloc();
        _batch  = SpriteBatch::alloc();
        
        // Start-up basic input
#ifdef CU_TOUCH_SCREEN
        Input::activate<Touchscreen>();
#else
        Input::activate<Mouse>();
#endif
        
        // TODO: remove the unnessisary loaders
        _assets->attach<Font>(FontLoader::alloc()->getHook());
        _assets->attach<Texture>(TextureLoader::alloc()->getHook());
        _assets->attach<Sound>(SoundLoader::alloc()->getHook());
        _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
        
        // TODO: make our own loading screen
        // Create a "loading" screen
        _loaded = false;
        _demoloading.init(_assets);
        
        // TODO: change assets to be our assets (if/when we get some)
        // Queue up the other assets
        AudioEngine::start();
        _assets->loadDirectoryAsync(jsonPath, nullptr);
        
        Application::onStartup(); // YOU MUST END with call to parent
    }
}

/**
 * The method called when the application is ready to quit.
 *
 * This is the method to dispose of all resources allocated by this
 * application.  As a rule of thumb, everything created in onStartup()
 * should be deleted here.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to NONE,
 * causing the application to be deleted.
 */
void PivotApp::onShutdown() {
    if (demo){
        _demoloading.dispose();
        _demogameplay.dispose();
        _assets = nullptr;
        _batch = nullptr;
        
        // Shutdown input
#ifdef CU_TOUCH_SCREEN
        Input::deactivate<Touchscreen>();
#else
        Input::deactivate<Mouse>();
#endif
        
        AudioEngine::stop();
        Application::onShutdown();  // YOU MUST END with call to parent
    }
    else{
        // TODO: dispose of other modes here (ex: level select) when they are implemented
        _demoloading.dispose();
        _gameplay.dispose();
        _assets = nullptr;
        _batch = nullptr;
        
        // Shutdown input
#ifdef CU_TOUCH_SCREEN
        Input::deactivate<Touchscreen>();
#else
        Input::deactivate<Mouse>();
#endif
        
        AudioEngine::stop();
        Application::onShutdown();  // YOU MUST END with call to parent
    }
}

/**
 * The method called when the application is suspended and put in the background.
 *
 * When this method is called, you should store any state that you do not
 * want to be lost.  There is no guarantee that an application will return
 * from the background; it may be terminated instead.
 *
 * If you are using audio, it is critical that you pause it on suspension.
 * Otherwise, the audio thread may persist while the application is in
 * the background.
 */
void PivotApp::onSuspend() {
    AudioEngine::get()->pause();
}

/**
 * The method called when the application resumes and put in the foreground.
 *
 * If you saved any state before going into the background, now is the time
 * to restore it. This guarantees that the application looks the same as
 * when it was suspended.
 *
 * If you are using audio, you should use this method to resume any audio
 * paused before app suspension.
 */
void PivotApp::onResume() {
    AudioEngine::get()->resume();
}


#pragma mark -
#pragma mark Application Loop

/**
 * The method called to update the application data.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should contain any code that is not an OpenGL call.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void PivotApp::update(float timestep) {
    if (demo){
        if (!_loaded && _demoloading.isActive()) {
            _demoloading.update(0.01f);
        } else if (!_loaded) {
            _demoloading.dispose(); // Disables the input listeners in this mode
            _demogameplay.init(_assets);
            _loaded = true;
        } else {
            _demogameplay.update(timestep);
        }
    }
    else{
        // TODO: add structure for multiple "modes" once they have been created
        if (!_loaded && _demoloading.isActive()) {
            _demoloading.update(0.01f);
        } else if (!_loaded) {
            _demoloading.dispose(); // Disables the input listeners in this mode
            _gameplay.init(_assets);
            _loaded = true;
        } else {
            _gameplay.update(timestep);
        }
    }
}

/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should OpenGL and related drawing calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void PivotApp::draw() {
    if (demo){
        if (!_loaded) {
            _demoloading.render(_batch);
        } else {
            _demogameplay.render(_batch);
        }
    }
    else{
        // TODO: add structure for multiple "modes" once they have been created
        if (!_loaded) {
            _demoloading.render(_batch);
        } else {
            _gameplay.render(_batch);
        }
    }
}

