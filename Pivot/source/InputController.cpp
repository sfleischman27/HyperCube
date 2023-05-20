//
//  InputController.cpp
//  Platformer
//
//  Created by JD on 2/27/23.
//

#include "InputController.h"

using namespace cugl;

#pragma mark Input Constants

/** The key to use for reseting the game */
#define RESET_KEY KeyCode::R
/** The key for toggling the debug display */
#define DEBUG_KEY KeyCode::D
/** The key for exitting the game */
#define EXIT_KEY  KeyCode::ESCAPE
/** The key for firing a bullet */
#define FIRE_KEY KeyCode::SPACE
/** The key for jumping up */
#define JUMP_KEY KeyCode::ARROW_UP
/** The key for dropping a glowstick */
#define GLOWSTICK_KEY KeyCode::R
/** The key for increasing the cut */
#define INCREASE_CUT_KEY KeyCode::E
/** The key for decreasing the cut */
#define DECREASE_CUT_KEY KeyCode::Q


/** How close we need to be for a multi touch */
#define NEAR_TOUCH      100

/** This defines the joystick "deadzone" (how far we must move) */
#define JSTICK_DEADZONE  15
/** This defines the joystick radial size (for reseting the anchor) */
#define JSTICK_RADIUS    25
/** How far to display the virtual joystick above the finger */
#define JSTICK_OFFSET    80
/** This defines the joystick "deadzone" (how far we must move) */
#define CUT_JSTICK_DEADZONE  1
/** This defines the joystick radial size (for reseting the anchor) */
#define CUT_JSTICK_RADIUS    25
/** How far to display the virtual joystick above the finger */
#define CUT_JSTICK_OFFSET    80
/** How far we must swipe up for a jump gesture */
#define SWIPE_LENGTH    50
/** How fast a double click must be in milliseconds */
#define DOUBLE_CLICK    400

// The screen is divided into four zones: Left, Bottom, Right and Main/
// These are all shown in the diagram below.
//
//   |---------------|
//   |   |       |   |
//   | L |   M   | R |
//   |   |       |   |
//   |---------------|
//
// The meaning of any touch depends on the zone it begins in.

/** The portion of the screen used for the left zone */
#define LEFT_ZONE       0.35f
/** the portion of the top left side of the screen NOT used for movement. Can be thought of as the portion of the top left side of the screen that controls the cut*/
#define LEFT_CUT_ZONE 0.75
/** The portion of the screen used for the right zone */
#define RIGHT_ZONE      0.35f

#pragma mark -
#pragma mark Input Controller
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
InputController::InputController() :
_active(false),
_resetPressed(false),
_debugPressed(false),
_exitPressed(false),
_firePressed(false),
_jumpPressed(false),
_keyJump(false),
_keyGlowstick(false),
_keyIncreaseCut(false),
_keyDecreaseCut(false),
_keyFire(false),
_keyReset(false),
_keyDebug(false),
_keyExit(false),
_keyLeft(false),
_keyRight(false),
_keyRunLeft(false),
_keyRunRight(false),
_horizontal(0.0f),
_moveNorm(0.00),
_joystick(false),
_hasJumped(false),
_keyKeepIncreasingCut(false),
_keyKeepDecreasingCut(false),
_keepChangingCut(false) {
}

/**
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void InputController::dispose() {
    if (_active) {
#ifndef CU_TOUCH_SCREEN
        Input::deactivate<Keyboard>();
        Mouse* mouse = Input::get<Mouse>();
        mouse->removePressListener(_inputKey);
        mouse->removeReleaseListener(_inputKey);
#else
        Touchscreen* touch = Input::get<Touchscreen>();
        touch->removeBeginListener(_inputKey);
        touch->removeEndListener(_inputKey);
#endif
        _active = false;
    }
}

/**
 * Initializes the input control for the given bounds
 *
 * The bounds are the bounds of the scene graph.  This is necessary because
 * the bounds of the scene graph do not match the bounds of the display.
 * This allows the input device to do the proper conversion for us.
 *
 * @param bounds    the scene graph bounds
 *
 * @return true if the controller was initialized successfully
 */
bool InputController::init(const cugl::Rect bounds, std::shared_ptr<cugl::scene2::Button> jump, std::shared_ptr<cugl::scene2::Button> left, std::shared_ptr<cugl::scene2::SceneNode> leftRun, std::shared_ptr<cugl::scene2::Button> right, std::shared_ptr<cugl::scene2::SceneNode> rightRun, std::shared_ptr<cugl::scene2::Button> glowstick) {
    bool success = true;
    _sbounds = bounds;
    _tbounds = Application::get()->getDisplayBounds();
    
    createZones();
    clearTouchInstance(_ltouch);
    clearTouchInstance(_rtouch);
    clearTouchInstance(_mtouch);
    
#ifndef CU_TOUCH_SCREEN
    success = Input::activate<Keyboard>();
    success = success && Input::activate<Mouse>();
    Mouse* mouse = Input::get<Mouse>();
    
    _inputKey = mouse->acquireKey();
    
    mouse->setPointerAwareness(cugl::Mouse::PointerAwareness::ALWAYS);
    mouse->addPressListener(_inputKey, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
      this->mousePressBeganCB(event, clicks, focus);
    });
    mouse->addReleaseListener(_inputKey, [=](const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
      this->mouseReleasedCB(event, clicks, focus);
    });
#else
    _buttonJump = jump;
    _buttonLeft = left;
    _buttonRight = right;
    _buttonGlowstick = glowstick;
    
    _runRight = leftRun;
    _runLeft = rightRun;
    
    Touchscreen* touch = Input::get<Touchscreen>();
    
    _inputKey = touch->acquireKey();
    
    touch->addBeginListener(_inputKey,[=](const TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);
    });
    touch->addEndListener(_inputKey,[=](const TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);
    });
    touch->addMotionListener(_inputKey,[=](const TouchEvent& event, const Vec2& previous, bool focus) {
        this->touchesMovedCB(event, previous, focus);
    });
    
    _buttonJump->addListener([this](const std::string& name, bool down) {
        if (down) {
            _keyJump = true;
        }
    });
    
    _buttonLeft->addListener([this](const std::string& name, bool down) {
        if (down) {
            _keyLeft = true;
            _keyRight = false;
        }
    });
    
    originalLeftButtonPos = _buttonLeft->getPosition();
    
    _buttonRight->addListener([this](const std::string& name, bool down) {
        if (down) {
            _keyLeft = false;
            _keyRight = true;
        }
    });
    
    originalRightButtonPos = _buttonRight->getPosition();

    _buttonCenterPoint = ((_buttonRight->getPosition().x + _buttonLeft->getPosition().x)/2) + _buttonLeft->getBoundingBox().size.width;
    
    _buttonGlowstick->addListener([this](const std::string& name, bool down) {
        if (down) {
            _keyGlowstick = true;
            CULog("drop glowstick");
        }
    });
    
    if(settings_isUsingJoystick){
        _buttonRight->setVisible(false);
        _buttonLeft->setVisible(false);
        _runRight->setVisible(false);
        _runLeft->setVisible(false);
    }
    
#endif
    _active = success;
    return success;
}


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
void InputController::update(float dt) {
#ifndef CU_TOUCH_SCREEN
    // DESKTOP CONTROLS
    Keyboard* keys = Input::get<Keyboard>();

    // Map "keyboard" events to the current frame boundary
    _keyReset  = keys->keyPressed(RESET_KEY);
    _keyDebug  = keys->keyPressed(DEBUG_KEY);
    _keyExit   = keys->keyPressed(EXIT_KEY);
    _keyFire   = keys->keyPressed(FIRE_KEY);
    _keyJump   = keys->keyPressed(JUMP_KEY);
    _keyGlowstick = keys->keyPressed(GLOWSTICK_KEY);
    _keyIncreaseCut = keys->keyPressed(INCREASE_CUT_KEY);
    _keyDecreaseCut = keys->keyPressed(DECREASE_CUT_KEY);

    _keyLeft = keys->keyDown(KeyCode::ARROW_LEFT);
    _keyRight = keys->keyDown(KeyCode::ARROW_RIGHT);
    _keyRunLeft = keys->keyDown(KeyCode::A);
    _keyRunRight = keys->keyDown(KeyCode::D);
    _keyKeepIncreasingCut = keys->keyDown(INCREASE_CUT_KEY);
    _keyKeepDecreasingCut = keys->keyDown(DECREASE_CUT_KEY);
#endif

    _resetPressed = _keyReset;
    _debugPressed = _keyDebug;
    _exitPressed  = _keyExit;
    _firePressed  = _keyFire;
    _jumpPressed  = _keyJump;
    _glowstickPressed = _keyGlowstick;
    _increaseCutPressed = _keyIncreaseCut;
    _decreaseCutPressed = _keyDecreaseCut;
    _keepChangingCut = _keyKeepIncreasingCut || _keyKeepDecreasingCut;

    // Directional controls
    _horizontal = 0.0f;
    if (_keyRight) {
        _horizontal += 1.0f;
    }
    if (_keyLeft) {
        _horizontal -= 1.0f;
    }
    if (_keyRunRight) {
        _horizontal *= 1.5f;
    }
    if (_keyRunLeft) {
        _horizontal *= 1.5f;
    }
    
    _moveNorm = 0.00;
    if (_keyKeepIncreasingCut) {
        _moveNorm += 0.01;
    }
    if (_keyKeepDecreasingCut) {
        _moveNorm -= 0.01;
    }

// If it does not support keyboard, we must reset "virtual" keyboard
#ifdef CU_TOUCH_SCREEN
    _keyExit = false;
    _keyReset = false;
    _keyDebug = false;
    _keyJump  = false;
    _keyGlowstick = false;
    _keyIncreaseCut = false;
    _keyDecreaseCut = false;
    _keyFire  = false;
#endif
}

/**
 * Clears any buffered inputs so that we may start fresh.
 */
void InputController::clear() {
    _resetPressed = false;
    _debugPressed = false;
    _exitPressed  = false;
    _jumpPressed = false;
    _glowstickPressed = false;
    _firePressed = false;
    _increaseCutPressed = false;
    _decreaseCutPressed = false;
    _keepChangingCut = false;
    _dtouch = Vec2::ZERO;
    _select = false;
}

#pragma mark -
#pragma mark Touch Controls

/**
 * Defines the zone boundaries, so we can quickly categorize touches.
 */
void InputController::createZones() {
    _lzone = _tbounds;
    _lzone.size.width *= LEFT_ZONE;
    _rzone = _tbounds;
    _rzone.size.width *= RIGHT_ZONE;
    _rzone.origin.x = _tbounds.origin.x+_tbounds.size.width-_rzone.size.width;
}

/**
 * Populates the initial values of the input TouchInstance
 */
void InputController::clearTouchInstance(TouchInstance& touchInstance) {
    touchInstance.touchids.clear();
    touchInstance.position = Vec2::ZERO;
}


/**
 * Returns the correct zone for the given position.
 *
 * See the comments above for a description of how zones work.
 *
 * @param  pos  a position in screen coordinates
 *
 * @return the correct zone for the given position.
 */
InputController::Zone InputController::getZone(const Vec2 pos) const {
//    CULog("%f", _tbounds.getMaxY() * 0.75f);
//    CULog("%f", pos.y);
    if (_lzone.contains(pos)) {
        return Zone::LEFT;
    } else if (_rzone.contains(pos)) {
        return Zone::RIGHT;
    } else if (_tbounds.contains(pos)) {
        //&& pos.y < _tbounds.getMaxY() * 0.75f
        return Zone::MAIN;
    }
    return Zone::UNDEFINED;
}

/**
 * Returns the scene location of a touch
 *
 * Touch coordinates are inverted, with y origin in the top-left
 * corner. This method corrects for this and scales the screen
 * coordinates down on to the scene graph size.
 *
 * @return the scene location of a touch
 */
Vec2 InputController::touch2Screen(const Vec2 pos) const {
    float px = pos.x/_tbounds.size.width -_tbounds.origin.x;
    float py = pos.y/_tbounds.size.height-_tbounds.origin.y;
    Vec2 result;
    result.x = px*_sbounds.size.width +_sbounds.origin.x;
    result.y = (1-py)*_sbounds.size.height+_sbounds.origin.y;
    return result;
}

/**
 * Processes movement for the floating joystick.
 *
 * This will register movement as left or right (or neither).  It
 * will also move the joystick anchor if the touch position moves
 * too far.
 *
 * @param  pos  the current joystick position
 */
void InputController::processJoystick(const cugl::Vec2 pos) {
    //Vec2 diff =  _ltouch.position-pos;
    
    if(_buttonRight->isDown() && _ltouch.position.x + 200 < pos.x){
//        CULog("running right");
        _keyRunRight = true;
        _runRight->setVisible(false);
        _runLeft->setVisible(true);
    }else{
        _keyRunRight = false;
        _runLeft->setVisible(false);
    }
    if(_buttonLeft->isDown() && _ltouch.position.x - 200 > pos.x){
//        CULog("running left");
        _keyRunLeft= true;
        _runLeft->setVisible(false);
        _runRight->setVisible(true);
    }else{
        _keyRunLeft = false;
        _runRight->setVisible(false);
    }
    
    if(_ltouch.position.x > pos.x + 40){
        _buttonLeft->setDown(true);
        _buttonRight->setDown(false);
        //CULog("right to left");
    }
    if(_ltouch.position.x < pos.x - 40){
        _buttonRight->setDown(true);
        _buttonLeft->setDown(false);
        //CULog("left to right");
    }
    
    // Reset the anchor if we drifted too far
//    if (diff.lengthSquared() > JSTICK_RADIUS*JSTICK_RADIUS) {
//        diff.normalize();
//        diff *= (JSTICK_RADIUS+JSTICK_DEADZONE)/2;
//        _ltouch.position = pos+diff;
//    }
    
//    _ltouch.position.y = pos.y;
//    _joycenter = touch2Screen(_ltouch.position);
//    _joycenter.y += JSTICK_OFFSET;
//
//    if (std::fabsf(diff.x) > JSTICK_DEADZONE) {
//        _joystick = true;
//        if (diff.x > 0) {
//            _keyLeft = true;
//            _keyRight = false;
//        } else {
//            _keyLeft = false;
//            _keyRight = true;
//        }
//    } else {
//        _joystick = false;
//        _keyLeft = false;
//        _keyRight = false;
//    }
}

/**
 * Processes movement for the floating joystick.
 *
 * This will register movement as left or right (or neither).  It
 * will also move the joystick anchor if the touch position moves
 * too far.
 *
 * @param  pos  the current joystick position
 */
void InputController::processCutJoystick(const cugl::Vec2 pos, TouchInstance loc) {
    Vec2 diff =  loc.position-pos;

    // Reset the anchor if we drifted too far
    if (diff.lengthSquared() > CUT_JSTICK_RADIUS*CUT_JSTICK_RADIUS) {
        diff.normalize();
        diff *= (CUT_JSTICK_RADIUS+CUT_JSTICK_DEADZONE)/2;
        loc.position = pos+diff;
    }
    _mtouch.position.y = pos.y;
    _cutjoycenter = touch2Screen(_mtouch.position);
    _cutjoycenter.y += CUT_JSTICK_OFFSET;
    
    if (std::fabsf(diff.x) > CUT_JSTICK_DEADZONE) {
        _joystick = true;
        if (diff.x > 0) {
            _keyIncreaseCut = true;
            _keyDecreaseCut = false;
        } else {
            _keyIncreaseCut = false;
            _keyDecreaseCut = true;
        }
    } else {
        _joystick = false;
        _keyLeft = false;
        _keyRight = false;
    }
}

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
int InputController::processSwipe(const Vec2 start, const Vec2 stop, Timestamp current) {
    // Look for swipes up that are "long enough"
    float xdiff = (stop.x-start.x);
    cutFactor = xdiff;
    float thresh = SWIPE_LENGTH;
    if (xdiff > thresh) {
        return 1;
    } else if (xdiff < thresh * -1) {
        return -1;
    }
    return 0;
}



#pragma mark -
#pragma mark Touch and Mouse Callbacks

void InputController::mousePressBeganCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    Vec2 pos = event.position;
    if (!_select) {
        _dtouch = pos;
    }
    _select = true;
}
void InputController::mouseReleasedCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    _select = false;
}
/**
 * Callback for the beginning of a touch event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputController::touchBeganCB(const TouchEvent& event, bool focus) {
    //CULog("Touch began %lld", event.touch);
    // Update the touch location for later gestures
    Vec2 pos = event.position;
    if (!_select) {
        _dtouch = pos;
    }
    _select = true;
    Zone zone = getZone(pos);
    switch (zone) {
        case Zone::LEFT:
            // Only process if no touch in zone
            if (_ltouch.touchids.empty()) {
                // Left is the floating joystick
                _ltouch.position = event.position;
                _ltouch.timestamp.mark();
                _ltouch.touchids.insert(event.touch);
                
//                if(_ltouch.position.y < _lzone.size.height * LEFT_CUT_ZONE){
//                    isRotating = true;
//                } else{
                
                //set the y pos of the movement buttons to be the y pos of our touch location
                if(settings_isUsingJoystick){
                    CULog("%f", _buttonLeft->getPosition().x);
                    CULog("%f",_ltouch.position.x);
                    _buttonRight->setVisible(true);
                    _buttonLeft->setVisible(true);
                    _buttonRight->setPosition(touch2Screen(_ltouch.position).x + 20, touch2Screen(pos).y - _lzone.size.height * 0.05f);
                    _buttonLeft->setPosition(touch2Screen(_ltouch.position).x - 120, touch2Screen(pos).y - _lzone.size.height * 0.05f);
                } else {
                    _buttonRight->setVisible(true);
                    _buttonLeft->setVisible(true);
                    if(_ltouch.position.x > _buttonRight->getPositionX() + 400 && _ltouch.position.y > _tbounds.getMaxY() * 0.75f){
//                        CULog("running right");
                        //CULog("%f",_ltouch.position.x);
                        _keyRight = true;
                        _keyRunRight = true;
                        _runLeft->setVisible(true);
                        //_runRight->setVisible(false);
                        //_runLeft->setVisible(true);
                    }
                    if(_ltouch.position.x < _buttonLeft->getPositionX() + 100 && _ltouch.position.y > _tbounds.getMaxY() * 0.75f){
                        CULog("running left");
                        _keyRunLeft= true;
                        _keyLeft = true;
                        _runRight->setVisible(true);
                        //_runLeft->setVisible(false);
                        //_runRight->setVisible(true);
                    }
                }
                
                //set the correct movement button to be down based on which side of the left zone we touched
                
                _joystick = true;
                _joycenter = touch2Screen(event.position);
                _joycenter.y += JSTICK_OFFSET;
                //}
            }
            break;
        case Zone::RIGHT:
            // Only process if no touch in zone
            if (_rtouch.touchids.empty()) {
                // Right is jump AND fire controls
//                _keyFire = (event.timestamp.ellapsedMillis(_rtime) <= DOUBLE_CLICK);
                _rtouch.position = event.position;
                _rtouch.timestamp.mark();
                _rtouch.touchids.insert(event.touch);
                if(!_keyJump || _ltouch.position.y < _tbounds.getMaxY() * 0.75f){
                    //isRotating = true;
                }
//                _hasJumped = false;
            }
            break;
        case Zone::MAIN:
            if (_mtouch.touchids.empty()) {
                _mtouch.position = event.position;
                _mtouch.timestamp.mark();
                _mtouch.touchids.insert(event.touch);
                //isRotating = true;

//                _joystick = true;
//                _cutjoycenter = touch2Screen(event.position);
//                _cutjoycenter.y += JSTICK_OFFSET;
            }
            break;
            // Only check for double tap in Main if nothing else down
//            if (_ltouch.touchids.empty() && _rtouch.touchids.empty() && _mtouch.touchids.empty()) {
//                _keyDebug = (event.timestamp.ellapsedMillis(_mtime) <= DOUBLE_CLICK);
//            }
//
//            // Keep count of touches in Main zone if next to each other.
//            if (_mtouch.touchids.empty()) {
//                _mtouch.position = event.position;
//                _mtouch.touchids.insert(event.touch);
//            } else {
//                Vec2 offset = event.position-_mtouch.position;
//                if (offset.lengthSquared() < NEAR_TOUCH*NEAR_TOUCH) {
//                    _mtouch.touchids.insert(event.touch);
//                }
//            }
//            break;
        default:
            CUAssertLog(false, "Touch is out of bounds");
            break;
    }
}

 
/**
 * Callback for the end of a touch event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputController::touchEndedCB(const TouchEvent& event, bool focus) {
    // Reset all keys that might have been set
    Vec2 pos = event.position;
    _select = false;
    Zone zone = getZone(pos);
    if (_ltouch.touchids.find(event.touch) != _ltouch.touchids.end()) {
        _ltouch.touchids.clear();
        _keyLeft = false;
        _keyRight = false;
        _keyRunLeft = false;
        _keyRunRight = false;
        _keyIncreaseCut = false;
        _keyDecreaseCut = false;
        _joystick = false;
        isRotating = false;
        if(settings_isUsingJoystick){
            _buttonRight->setVisible(false);
            _buttonLeft->setVisible(false);
        }
        _runRight->setVisible(false);
        _runLeft->setVisible(false);
    } else if (_rtouch.touchids.find(event.touch) != _rtouch.touchids.end()) {
        _hasJumped = false;
        isRotating = false;
        _rtime = event.timestamp;
        _rtouch.touchids.clear();
    } else if (_mtouch.touchids.find(event.touch) != _mtouch.touchids.end()) {
        //_mtouch.touchids.erase(event.touch);
        _mtouch.touchids.clear();
        _keyLeft = false;
        _keyRight = false;
        _keyIncreaseCut = false;
        _keyDecreaseCut = false;
        _joystick = false;
        isRotating = false;
    } else if (zone == Zone::MAIN) {
        if (_mtouch.touchids.find(event.touch) != _mtouch.touchids.end()) {
            _mtouch.touchids.erase(event.touch);
        }
        _mtime = event.timestamp;
    }
}


/**
 * Callback for a touch moved event.
 *
 * @param event The associated event
 * @param previous The previous position of the touch
 * @param focus    Whether the listener currently has focus
 */
void InputController::touchesMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
    Vec2 pos = event.position;
    // Only check for swipes in the main zone if there is more than one finger.
    if (_ltouch.touchids.find(event.touch) != _ltouch.touchids.end()) {
        //if(_ltouch.position.y < _lzone.size.height * LEFT_CUT_ZONE){
//            int swipe = processSwipe(_ltouch.position, event.position, event.timestamp);
            //CULog("rotate");
        //} else{
        //have the y pos of the movement buttons follow the y pos of our finger
//        _buttonRight->setPosition(_buttonRight->getPosition().x, touch2Screen(pos).y - _lzone.size.height * 0.05f);
//        _buttonLeft->setPosition(_buttonLeft->getPosition().x, touch2Screen(pos).y - _lzone.size.height * 0.05f);
        //if(settings_isUsingJoystick){
        if(settings_isUsingJoystick){
            processJoystick(pos);
        } else if (_ltouch.position.y < _tbounds.getMaxY() * 0.75f) {
            processSwipe(_ltouch.position, event.position, event.timestamp);
            isRotating = true;
        } else {
            processJoystick(pos);
        }
        //}
            //CULog("joystick");
        //}
//        processCutJoystick(pos, _ltouch);
    } else if (_rtouch.touchids.find(event.touch) != _rtouch.touchids.end()) {
        //processCutJoystick(pos, _rtouch);
        if(_rtouch.position.y < _tbounds.getMaxY() * 0.75f){
            processSwipe(_rtouch.position, event.position, event.timestamp);
            isRotating = true;
        }
//        if (!_hasJumped) {
//            if ((_rtouch.position.y-pos.y) > SWIPE_LENGTH) {
//                _keyJump = true;
//                _hasJumped = true;
//            }
//        }
    } else if (_mtouch.touchids.find(event.touch) != _mtouch.touchids.end()) {
        processSwipe(_mtouch.position, event.position, event.timestamp);
        isRotating = true;
        //processCutJoystick(pos, _mtouch);
    } else if (_mtouch.touchids.size() > 1) {
        // We only process multifinger swipes in main
        //int swipe = processSwipe(_mtouch.position, event.position, event.timestamp);
//        if (swipe == 1) {
//            _keyReset = true;
//        } else if (swipe == -1) {
//            _keyExit = true;
//        }
    }
}



