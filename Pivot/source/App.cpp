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

    getDisplaySize();

    std::string jsonPath = "json/assets.json";
	
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
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());
    
    // TODO: make our own loading screen
    // Create a "loading" screen
    _demoloading.init(_assets);
    
    // Queue up the other assets
    _assets->loadDirectoryAsync(jsonPath, nullptr);
    
    //init audio engine singleton and sound controller
    AudioEngine::start();
    _sound = std::make_shared<SoundController>();
    _sound->init(_assets);

    // set clear color for entire canvas
    setClearColor(Color4(255, 255, 255, 255));
    
    Application::setVSync(false);
    Application::setFPS(80);
    
    Application::onStartup(); // YOU MUST END with call to parent
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
    // save the level data (after loading is done)
    if (_scene != LOAD){
        _gameplay.save();
    }
    
    // TODO: dispose of other modes here (ex: level select) when they are implemented
    _demoloading.dispose();
    _gameplay.dispose();
    _mainMenu.dispose();
    _levelSelect.dispose();
    _endMenu.dispose();
    _quitMenu.dispose();
    _settings.dispose();
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
    // pause the music
    AudioEngine::get()->pause();
    // save the level data (after loading is done)
    if (_scene != LOAD){
        _gameplay.save();
    }
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
    // TODO: figure out how this works
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
    switch (_scene) {
        case LOAD:
            updateLoadingScene(timestep);
            break;
        case MAIN:
            updateMainScene(timestep);
            break;
        case LEVEL:
            updateLevelScene(timestep);
            break;
        case END:
            updateEndScene(timestep);
            break;
        case QUIT:
            updateQuitScene(timestep);
            break;
        case GAME:
            updateGameScene(timestep);
            break;
        case SETTINGS:
            updateSettingsScene(timestep);
            break;
    }
    
    //level sound cues
    if(_levelSelect._playMusic){
        enqueueOnce("menu", 0.5, true);
        _levelSelect._playMusic = false;
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
    switch (_scene) {
        case LOAD:
            _demoloading.render(_batch);
            break;
        case MAIN:
            _mainMenu.render(_batch);
            break;
        case LEVEL:
            _levelSelect.render(_batch);
            break;
        case END:
            _endMenu.render(_batch);
            break;
        case QUIT:
            _quitMenu.render(_batch);
            break;
        case GAME:
            _gameplay.render(_batch);
            break;
        case SETTINGS:
            _settings.render(_batch);
            break;
    }
}

void PivotApp::updateLoadingScene(float timestep){
    if (_demoloading.isActive()) {
        _demoloading.update(timestep);
    } else {
        _demoloading.dispose(); // Permanently disables the input listeners in this mode
        _mainMenu.init(_assets);
        if(_testing){
            _levelSelect.initMax(_assets);
        }else{
            _levelSelect.init(_assets);
        }
        _endMenu.init(_assets);
        _quitMenu.init(_assets);
        _gameplay.init(_assets, getDisplaySize(), _sound);
        if(_testing){_gameplay.setMaxLevel(_levelSelect.getMaxLevel());} // save the max level if not default
        _settings.init(_assets, _gameplay.getDataController());
        updateSettings(); // update game to reflect settings
        _mainMenu.setActive(true);
        _scene = State::MAIN;
    }
}

void PivotApp::updateGameScene(float timestep){
    switch (_gameplay.getState()) {
        case GameplayController::State::NONE:
            _gameplay.update(timestep);
            break;
        case GameplayController::State::QUIT:
            _gameplay.setActive(false);
            _quitMenu.setActive(true);
            _scene = State::QUIT;
            break;
        case GameplayController::State::END:
            _gameplay.setActive(false);
            _endMenu.setActive(true);
            // unlock next level (if not yet unlocked)
            _levelSelect.unlockNextLevel();
            _gameplay.setMaxLevel(_levelSelect.getMaxLevel());
            // save
            _gameplay.save();
            // go to end if last in pack
            if (_levelSelect.isLastInPack()){
                _scene = State::END;
                _sound->playSound("end", 0.5, false);
            } else {
                _gameplay.setActive(true);
                _gameplay.load(_levelSelect.getNextLevelString());
            }
            break;
    }
}

void PivotApp::updateMainScene(float timestep){
    switch (_mainMenu.getChoice()) {
        case MainMenu::Choice::NONE:
            _mainMenu.update(timestep);
            break;
        case MainMenu::Choice::START:
            _mainMenu.dispose();
            _levelSelect.setActive(true);
            _scene = State::LEVEL;
            break;
        case MainMenu::Choice::RESUME:
            _mainMenu.dispose();
            _levelSelect.setActive(true);
            // unlock levels specified in save file
            _levelSelect.updateLevel(_gameplay.getMaxLevel());
            _scene = State::LEVEL;
            break;
    }
}

void PivotApp::updateLevelScene(float timestep){
    switch (_levelSelect.getChoice()) {
        case LevelSelect::Choice::NONE:
            _levelSelect.update(timestep);
            break;
        case LevelSelect::Choice::NEXT:
            _levelSelect.nextPack();
            _levelSelect.update(timestep);
            break;
        case LevelSelect::Choice::PREV:
            _levelSelect.prevPack();
            _levelSelect.update(timestep);
            break;
        case LevelSelect::Choice::SETTINGS:
            _levelSelect.setActive(false);
            _settings.setActive(true);
            _scene = State::SETTINGS;
            break;
        default:
            _levelSelect.setActive(false);
            _gameplay.load(_levelSelect.getLevelString());
            _gameplay.setActive(true);
            _scene = State::GAME;            
            break;
    }
}

void PivotApp::updateEndScene(float timestep){
    switch(_endMenu.getChoice()) {
        case EndLevelMenu::Choice::NONE:
            _endMenu.update(timestep);
            break;
        case EndLevelMenu::Choice::MAP:
            _endMenu.update(timestep);
            break;
        case EndLevelMenu::Choice::REPLAY:
            _endMenu.setActive(false);
            _gameplay.setActive(true);
            _gameplay.reset();
            _scene = State::GAME;
            break;
        case EndLevelMenu::Choice::LEVEL:
            _endMenu.setActive(false);
            _levelSelect.setActive(true);
            _scene = State::LEVEL;
            break;
        case EndLevelMenu::Choice::NEXT:
            _endMenu.setActive(false);
            if(_levelSelect.isLast()){
                _levelSelect.setActive(true);
                _scene = State::LEVEL;
            } else {
                _gameplay.setActive(true);
                _gameplay.load(_levelSelect.getNextLevelString()); ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                _scene = State::GAME;
            }
            break;
    }
}

void PivotApp::updateQuitScene(float timestep){
    switch(_quitMenu.getChoice()) {
        case QuitMenu::Choice::NONE:
            _quitMenu.update(timestep);
            break;
        case QuitMenu::Choice::Y:
            _quitMenu.setActive(false);
            _gameplay.setActive(true);
            _scene = State::GAME;
            break;
        case QuitMenu::Choice::N:
            _quitMenu.setActive(false);
            _levelSelect.setActive(true);
            _scene = State::LEVEL;
            break;
        case QuitMenu::Choice::RESTART:
            _quitMenu.setActive(false);
            _gameplay.setActive(true);
            _gameplay.reset();
            _scene = State::GAME;
            break;
    }
}

void PivotApp::updateSettingsScene(float timestep){
    switch(_settings.getChoice()) {
        case SettingsMenu::Choice::NONE:
            _settings.update(timestep);
            updateSettings();
            break;
        case SettingsMenu::Choice::EXIT:
            // save
            _gameplay.save();
            // change settings
            updateSettings();
            // switch scenes
            _settings.setActive(false);
            _levelSelect.setActive(true);
            _scene = State::LEVEL;
            break;
        case SettingsMenu::OVERLAY:
            _settings.setOverlayActive(true);
            _settings.update(timestep);
            break;
        case SettingsMenu::OVEROFF:
            _settings.setOverlayActive(false);
            _settings.update(timestep);
            break;
    }
    
}

void PivotApp::updateSettings(){
    // if reset, update level select
    if (_gameplay.getMaxLevel() == 0){
        _levelSelect.resetMax();
    }
    // change between buttons and joystick
    _gameplay.updateJoystick(!_gameplay.getControls());
    // update volume
    _sound->setMasterVolume(_gameplay.getVolume());
    // turn music on or off
    
    // switch compass between 180 and 360
    _gameplay.updateCompass(_gameplay.getCompass());
    // switch rotate between clockwise and counter
    _gameplay.updateRotate(_gameplay.getRotate());
    // turn on or off the outline
    _gameplay.updateOutline(_gameplay.getOutline());
    // switch from right handed to left handed
    
}
