//
//  Loading.h
//  Pivot
//
//  Created by Sarah Fleischman on 5/16/23.
//

#ifndef Loading_h
#define Loading_h
#include <cugl/cugl.h>

class Loading : public cugl::Scene2 {
public:
    enum Status {
        /** The assets are not yet loaded */
        NONE,
        /** The assets have been loaded */
        LOADED
    };
    
protected:
    /** The asset manager for loading */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    /** The progress bar */
    std::shared_ptr<cugl::scene2::ProgressBar> _bar;
    /** The progress */
    float _progress;
    /** The loading status */
    Status _status;
    
    /**
     * Returns the active screen size of this scene.
     *
     * This method is for graceful handling of different aspect
     * ratios
     */
    cugl::Size computeActiveSize() const;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new loading mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    Loading() : Scene2(), _progress(0.0f) {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~Loading() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /**
     * Initializes the controller contents, making it ready for loading
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
#pragma mark -
#pragma mark Progress Monitoring
    /**
     * The method called to update the game mode.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);
    
    /**
     * Returns the loading status.
     *
     * This will return NONE if the loading is not done.
     *
     * @return the loading scene status.
     */
    Status getStatus() const { return _status; }
    
};
#endif /* Loading_h */
