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
    AudioEngine::start();
    _assets->loadDirectoryAsync(jsonPath, nullptr);

    // set clear color for entire canvas
    setClearColor(Color4(255, 255, 255, 255));
    
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
    // TODO: dispose of other modes here (ex: level select) when they are implemented
    _demoloading.dispose();
    _gameplay.dispose();
    _mainMenu.dispose();
    _levelSelect.dispose();
    _endMenu.dispose();
    _quitMenu.dispose();
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
    }
}

void PivotApp::updateLoadingScene(float timestep){
    if (_demoloading.isActive()) {
        _demoloading.update(timestep);
    } else {
        _demoloading.dispose(); // Permanently disables the input listeners in this mode
        _mainMenu.init(_assets);
        _levelSelect.init(_assets);
        _endMenu.init(_assets);
        _quitMenu.init(_assets);
        _gameplay.init(_assets, getDisplaySize());
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
            _scene = State::END;
            break;
    }
}

void PivotApp::updateMainScene(float timestep){
    switch (_mainMenu.getChoice()) {
        case MainMenu::Choice::NONE:
            _mainMenu.update(timestep);
            break;
        case MainMenu::Choice::START:
            _mainMenu.setActive(false);
            _levelSelect.setActive(true);
            _scene = State::LEVEL;
            break;
        case MainMenu::Choice::RESUME:
            _mainMenu.setActive(false);
            _levelSelect.setActive(true);
            _levelSelect.updateLevel(5); //TODO: get from save file!
            _scene = State::LEVEL;
            break;
    }
}

void PivotApp::updateLevelScene(float timestep){
    switch (_levelSelect.getChoice()) {
        case LevelSelect::Choice::NONE:
            _levelSelect.update(timestep);
            break;
        case LevelSelect::Choice::level1:
            _levelSelect.setActive(false);
            _gameplay.load("levelTest");
            _gameplay.setActive(true);
            _scene = State::GAME;
            break;
        case LevelSelect::level2:
            _levelSelect.setActive(false);
            _gameplay.load("First_Level_0000");
            _gameplay.setActive(true);
            _scene = State::GAME;
            break;
        case LevelSelect::level3:
            _levelSelect.setActive(false);
            _gameplay.load("debug_0000");
            _gameplay.setActive(true);
            _scene = State::GAME;
            break;
        case LevelSelect::level4:
            _levelSelect.update(timestep);
            break;
        case LevelSelect::level5:
            _levelSelect.update(timestep);
            break;
        case LevelSelect::level6:
            _levelSelect.update(timestep);
            break;
        case LevelSelect::level7:
            _levelSelect.update(timestep);
            break;
        case LevelSelect::level8:
            _levelSelect.update(timestep);
            break;
        case LevelSelect::level9:
            _levelSelect.update(timestep);
            break;
        case LevelSelect::level10:
            _levelSelect.update(timestep);
            break;
        case LevelSelect::level11:
            _levelSelect.update(timestep);
            break;
        case LevelSelect::level12:
            _levelSelect.update(timestep);
            break;
        case LevelSelect::level13:
            _levelSelect.update(timestep);
            break;
        case LevelSelect::level14:
            _levelSelect.update(timestep);
            break;
        case LevelSelect::level15:
            _levelSelect.update(timestep);
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
            _levelSelect.setActive(true);
            _scene = State::LEVEL;
            break;
    }
    // TODO: add actual next level logic
    // TODO: add the ability to show the 3D map
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
    }
}
