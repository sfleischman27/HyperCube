//
//  InputController.h
//  Platformer
//
//  Contains functions that provide input information to be used by other controllers
//
//  Created by Sarah Fleischman on 2/19/23.
//

#ifndef InputController_h
#define InputController_h
#include <cugl/cugl.h>
#include <unordered_set>

/**
 * For now, have functions for key presses to control player movement and plane rotation
 *
 * In the future this will detect touches, and dictate the angle of rotation for a plane among other things.
 */

class InputController {
public:
    float cutFactor = 0;
    int settings_invertRotate = 1;
    bool isRotating = false;
    bool settings_isUsingJoystick = false;
    
private:
    /** Whether or not this input is active */
    bool _active;
    // KEYBOARD EMULATION
    /** Whether the jump key is down */
    bool  _keyJump;
    /** Whether the glowstick key is down */
    bool  _keyGlowstick;
    /** Whether the fire key is down */
    bool  _keyFire;
    /** Whether the reset key is down */
    bool  _keyReset;
    /** Whether the debug key is down */
    bool  _keyDebug;
    /** Whether the exit key is down */
    bool  _keyExit;
    /** Whether the left arrow key is down */
    bool  _keyLeft;
    /** Whether the right arrow key is down */
    bool  _keyRight;
    /** Whether the left arrow key is down */
    bool  _keyRunLeft;
    /** Whether the right arrow key is down */
    bool  _keyRunRight;
    /** Whether the increase cut key is pressed */
    bool  _keyIncreaseCut;
    /** Whether the decrease cut key is pressed */
    bool  _keyDecreaseCut;
    /** Whether the increase cut key is down */
    bool _keyKeepIncreasingCut;
    /** Whether the decrease cut key is down */
    bool _keyKeepDecreasingCut;
    // TOUCH SUPPORT
    /** The initial touch location for the current gesture, IN SCREEN COORDINATES */
    cugl::Vec2 _dtouch;
    
    cugl::Vec2 originalRightButtonPos;
    cugl::Vec2 originalLeftButtonPos;
  
protected:
    // INPUT RESULTS
    /** Whether the reset action was chosen. */
    bool _resetPressed;
    /** Whether the debug toggle was chosen. */
    bool _debugPressed;
    /** Whether the exit action was chosen. */
    bool _exitPressed;
    /** Whether the fire action was chosen. */
    bool _firePressed;
    /** Whether the jump action was chosen. */
    bool _jumpPressed;
    /** Whether the glowstick action was chosen. */
    bool _glowstickPressed;
    /** How much did we move horizontally? */
    float _horizontal;
    /** Whether the increase cut action was chosen. */
    bool _increaseCutPressed;
    /** Whether the decrease cut action was chosen. */
    bool _decreaseCutPressed;
    /** Whether the increase/decrease buttton is down. */
    bool _keepChangingCut;
    /** How much did we move norm? */
    float _moveNorm;
    /** Are we registering an object selection? */
    bool _select;
    
    std::shared_ptr<cugl::scene2::Button> _buttonJump;
    std::shared_ptr<cugl::scene2::Button> _buttonLeft;
    std::shared_ptr<cugl::scene2::Button> _buttonRight;
    std::shared_ptr<cugl::scene2::Button> _buttonGlowstick;
    
    std::shared_ptr<cugl::scene2::SceneNode> _runLeft;
    std::shared_ptr<cugl::scene2::SceneNode> _runRight;


#pragma mark Internal Touch Management
    // The screen is divided into four zones: Left, Bottom, Right and Main/
    // These are all shown in the diagram below.
    //
    //   |---------------|
    //   |   |       |   |
    //   | L |   M   | R |
    //   |   |       |   |
    //   -----------------
    //
    // The meaning of any touch depends on the zone it begins in.

    /** Information representing a single "touch" (possibly multi-finger) */
    struct TouchInstance {
        /** The anchor touch position (on start) */
        cugl::Vec2 position;
        /** The current touch time */
        cugl::Timestamp timestamp;
        /** The touch id(s) for future reference */
        std::unordered_set<Uint64> touchids;
    };

    /** Enumeration identifying a zone for the current touch */
    enum class Zone {
        /** The touch was not inside the screen bounds */
        UNDEFINED,
        /** The touch was in the left zone (as shown above) */
        LEFT,
        /** The touch was in the right zone (as shown above) */
        RIGHT,
        /** The touch was in the main zone (as shown above) */
        MAIN
    };

    /** The bounds of the entire game screen (in touch coordinates) */
    cugl::Rect _tbounds;
    /** The bounds of the entire game screen (in scene coordinates) */
    cugl::Rect _sbounds;
    /** The bounds of the left touch zone */
    cugl::Rect _lzone;
    /** The bounds of the right touch zone */
    cugl::Rect _rzone;

    // Each zone can have only one touch
    /** The current touch location for the left zone */
    TouchInstance _ltouch;
    /** The current touch location for the right zone */
    TouchInstance _rtouch;
    /** The current touch location for the bottom zone */
    TouchInstance _mtouch;
    
    /** Whether the virtual joystick is active */
    bool _joystick;
    /** The position of the virtual joystick */
    cugl::Vec2 _joycenter;
    
    float _buttonCenterPoint;
    /** The position of the virtual joystick that controls the cut */
    cugl::Vec2 _cutjoycenter;
    /** Whether or not we have processed a jump for this swipe yet */
    bool _hasJumped;
    /** The timestamp for a double tap on the right */
    cugl::Timestamp _rtime;
    /** The timestamp for a double tap in the middle */
    cugl::Timestamp _mtime;

    /**
     * Defines the zone boundaries, so we can quickly categorize touches.
     */
    void createZones();
  
    /**
     * Populates the initial values of the TouchInstances
     */
    void clearTouchInstance(TouchInstance& touchInstance);

    /**
     * Returns the correct zone for the given position.
     *
     * See the comments above for a description of how zones work.
     *
     * @param  pos  a position in screen coordinates
     *
     * @return the correct zone for the given position.
     */
    Zone getZone(const cugl::Vec2 pos) const;
    
    /**
     * Returns the scene location of a touch
     *
     * Touch coordinates are inverted, with y origin in the top-left
     * corner. This method corrects for this and scales the screen
     * coordinates down on to the scene graph size.
     *
     * @return the scene location of a touch
     */
    cugl::Vec2 touch2Screen(const cugl::Vec2 pos) const;

    /**
     * Processes movement for the floating joystick.
     *
     * This will register movement as left or right (or neither).  It
     * will also move the joystick anchor if the touch position moves
     * too far.
     *
     * @param  pos  the current joystick position
     */
    void processJoystick(const cugl::Vec2 pos);
    
    void resetButtonPositions(){
        _buttonRight->setPosition(originalRightButtonPos);
        _buttonLeft->setPosition(originalLeftButtonPos);
    }
    
    /**
     * Processes movement for the floating joystick that controls the cut.
     *
     * This will register movement as left or right (or neither).  It
     * will also move the joystick anchor if the touch position moves
     * too far.
     *
     * @param  pos  the current joystick position
     */
    void processCutJoystick(const cugl::Vec2 pos, TouchInstance loc);
    
    /**
     * Returns a nonzero value if this is a quick left or right swipe
     *
     * The function returns -1 if it is left swipe and 1 if it is a right swipe.
     *
     * @param  start    the start position of the candidate swipe
     * @param  stop     the end position of the candidate swipe
     * @param  current  the current timestamp of the gesture
     *
     * @return a nonzero value if this is a quick left or right swipe
     */
    int processSwipe(const cugl::Vec2 start, const cugl::Vec2 stop, cugl::Timestamp current);
  
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new input controller.
     *
     * This constructor does NOT do any initialzation.  It simply allocates the
     * object. This makes it safe to use this class without a pointer.
     */
    InputController(); // Don't initialize.  Allow stack based
    
    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~InputController() { dispose(); }
    
    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    void dispose();
    
    /**
     * Initializes the input control for the given bounds
     *
     * The bounds are the bounds of the scene graph.  This is necessary because
     * the bounds of the scene graph do not match the bounds of the display.
     * This allows the input device to do the proper conversion for us.
     *
     * @param bounds   the scene graph bounds
     * @param left     the button that moves the player left
     * @param right    the button that moves the player right
     *
     * @return true if the controller was initialized successfully
     */
    bool init(const cugl::Rect bounds, std::shared_ptr<cugl::scene2::Button> jump, std::shared_ptr<cugl::scene2::Button> left, std::shared_ptr<cugl::scene2::SceneNode> leftRun, std::shared_ptr<cugl::scene2::Button> right, std::shared_ptr<cugl::scene2::SceneNode> rightRun, std::shared_ptr<cugl::scene2::Button> glowstick);
    
#pragma mark
#pragma mark Input Detection
    /**
     * Returns true if the input handler is currently active
     *
     * @return true if the input handler is currently active
     */
    bool isActive( ) const { return _active; }

    /**
     * Processes the currently cached inputs.
     *
     * This method is used to to poll the current input state.  This will poll the
     * keyboad and accelerometer.
     *
     * This method also gathers the delta difference in the touches. Depending on
     * the OS, we may see multiple updates of the same touch in a single animation
     * frame, so we need to accumulate all of the data together.
     */
    void  update(float dt);

    /**
     * Clears any buffered inputs so that we may start fresh.
     */
    void clear();
    
#pragma mark -
#pragma mark Input Results
    /**
     * Returns the amount of vertical movement.
     *
     * -1 = down, 1 = up, 0 = still
     *
     * @return the amount of vertical movement.
     */
    bool didSelect() const { return _select; }

    /**
     * Returns the location (in world space) of the selection.
     *
     * The origin of the coordinate space is the top left corner of the
     * screen.  This will need to be transformed to world coordinates
     * (via the scene graph) to be useful
     *
     * @return the location (in world space) of the selection.
     */
    const cugl::Vec2& getSelection() const { return _dtouch; }
    /**
     * Returns the amount of norm changes.
     *
     * @return the amount of norm changes.
     */
    float getMoveNorm() const { return _moveNorm; }
    /**
     * Returns the amount of sideways movement.
     *
     * -1 = left, 1 = right, 0 = still
     *
     * @return the amount of sideways movement.
     */
    float getHorizontal() const { return _horizontal; }
    
    /**
     * Returns if the increase cut button was pressed.
     *
     * @return if the increase cut button was pressed.
     */
    float didIncreaseCut() const { return _increaseCutPressed; }
    
    /**
     * Returns if the decrease cut button was pressed.
     *
     * @return if the decrease cut button was pressed.
     */
    float didDecreaseCut() const { return _decreaseCutPressed; }
    /**
     * Returns if increase/decrease button was down.
     *
     * @return if the increase/decrease  button was down.
     */
    float didKeepChangingCut() const { return _keepChangingCut; }
    
    /**
     * Returns if the jump button was pressed.
     *
     * @return if the jump button was pressed.
     */
    float didJump() const { return _jumpPressed; }
    
    /**
     * Returns if running is happening.
     *
     * @return if running
     */
    float isRun() const { return _keyRunRight || _keyRunLeft; }
    
    /**
     * Returns if the glowstick button was pressed.
     *
     * @return if the glowstick button was pressed.
     */
    float didGlowstick() const { return _glowstickPressed; }

    /**
     * Returns true if the fire button was pressed.
     *
     * @return true if the fire button was pressed.
     */
    bool didFire() const { return _firePressed; }

    /**
     * Returns true if the reset button was pressed.
     *
     * @return true if the reset button was pressed.
     */
    bool didReset() const { return _resetPressed; }

    /**
     * Returns true if the player wants to go toggle the debug mode.
     *
     * @return true if the player wants to go toggle the debug mode.
     */
    bool didDebug() const { return _debugPressed; }

    /**
     * Returns true if the exit button was pressed.
     *
     * @return true if the exit button was pressed.
     */
    bool didExit() const { return _exitPressed; }
    
    /**
     * Returns true if the virtual joystick is in use (touch only)
     *
     * @return true if the virtual joystick is in use (touch only)
     */
    bool withJoystick() const { return _joystick; }

    /**
     * Returns the scene graph position of the virtual joystick
     *
     * @return the scene graph position of the virtual joystick
     */
    cugl::Vec2 getJoystick() const { return _joycenter; }
    
    /**
     * Returns the scene graph position of the virtual joystick that controls the cut
     *
     * @return the scene graph position of the virtual joystick
     */
    cugl::Vec2 getCutJoystick() const { return _cutjoycenter; }

#pragma mark -
#pragma mark Touch and Mouse Callbacks
    
    void mousePressBeganCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);
  
    /**
     * Callback for a mouse release event.
     *
     * @param t     The touch information
     * @param event The associated event
     */
    void mouseReleasedCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);
    /**
     * Callback for the beginning of a touch event
     *
     * @param event The associated event
     * @param focus    Whether the listener currently has focus
     *
     */
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);

    /**
     * Callback for the end of a touch event
     *
     * @param event The associated event
     * @param focus    Whether the listener currently has focus
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);
  
    /**
     * Callback for a mouse release event.
     *
     * @param event The associated event
     * @param previous The previous position of the touch
     * @param focus    Whether the listener currently has focus
     */
    void touchesMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus);
  
};

#endif /* InputController_h */
