//
//  App.h
//  Platformer
//
//  This is the root class for the game. The file main.cpp accesses this class
//  to run the application. We will have most of the game logic in other classes
//
//  This class will change between different game modes (main, level select, etc)
//
//  Created by Sarah Fleischman on 2/19/23.
//
#ifndef App_h
#define App_h
#include <cugl/cugl.h>
#include "SoundController.h"
#include "DataController.h"
#include "GameplayController.h"
#include "RenderPipeline.h"
#include "MainMenu.h"
#include "LevelSelect.h"
#include "EndMenu.h"
#include "QuitMenu.h"

// Demo mode
#include "PFLoadingScene.h"

/**
 * This class represents the application root for the platform demo.
 */
class PivotApp : public cugl::Application {
protected:
    enum State {
        /** The loading scene */
        LOAD,
        /** The main menu scene */
        MAIN,
        /** The level select scene */
        LEVEL,
        /** The end of level scene */
        END,
        /** The quit level scene */
        QUIT,
        /** The scene to play the game */
        GAME
    };
    
    /** The global sprite batch for drawing (only want one of these) */
    std::shared_ptr<cugl::SpriteBatch> _batch;
    /** The global asset manager */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    // TODO: change this to be our loading and game scenes
    // TODO: add more scenes as they are created
    // Player modes
    /** The controller for the demo loading screen */
    LoadingScene _demoloading;
    /** The  controller for all sound functions */
    //SoundController _sound;
    /** The primary controller for the game world */
    GameplayController _gameplay;
    /** The main menu view */
    MainMenu _mainMenu;
    /** The level select view */
    LevelSelect _levelSelect;
    /** The end of level view */
    EndMenu _endMenu;
    /** The quit level view */
    QuitMenu _quitMenu;
    
    /** The controller for the loading screen */
    //LoadingScene _loading;
    
    /** The current active scene */
    State _scene;
    
public:
#pragma mark Constructors
    /**
     * Creates, but does not initialized a new application.
     *
     * This constructor is called by main.cpp.  You will notice that, like
     * most of the classes in CUGL, we do not do any initialization in the
     * constructor.  That is the purpose of the init() method.  Separation
     * of initialization from the constructor allows main.cpp to perform
     * advanced configuration of the application before it starts.
     */
    PivotApp() : cugl::Application(){ _scene = State::LOAD; }
    
    /**
     * Disposes of this application, releasing all resources.
     *
     * This destructor is called by main.cpp when the application quits.
     * It simply calls the dispose() method in Application.  There is nothing
     * special to do here.
     */
    ~PivotApp() { }
    
    
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
    virtual void onStartup() override;
    
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
    virtual void onShutdown() override;
    
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
    virtual void onSuspend() override;
    
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
    virtual void onResume()  override;
    
    
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
    virtual void update(float timestep) override;
    
    /**
     * The method called to draw the application to the screen.
     *
     * This is your core loop and should be replaced with your custom implementation.
     * This method should OpenGL and related drawing calls.
     *
     * When overriding this method, you do not need to call the parent method
     * at all. The default implmentation does nothing.
     */
    virtual void draw() override;
    
#pragma mark Menu Updates
private:
    /**
     * Inidividualized update method for the loading scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the loading scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateLoadingScene(float timestep);
    
    /**
     * Inidividualized update method for the gameplay scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the loading scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateGameScene(float timestep);
    
    /**
     * Inidividualized update method for the main menu scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the loading scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateMainScene(float timestep);
    
    /**
     * Inidividualized update method for the level select scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the loading scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateLevelScene(float timestep);
    
    /**
     * Inidividualized update method for the end of level menu scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the loading scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateEndScene(float timestep);
    
    /**
     * Inidividualized update method for the end of level menu scene.
     *
     * This method keeps the primary {@link #update} from being a mess of switch
     * statements. It also handles the transition logic from the loading scene.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void updateQuitScene(float timestep);
    
};

#endif /* App_h */






