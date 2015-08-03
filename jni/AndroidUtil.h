#ifndef __AndroidUtil_H__
#define __AndroidUtil_H__

#include "OIS.h"
#include <android_native_app_glue.h>
#include <android/log.h>
#include <EGL/egl.h>
#include "OgrePlatform.h"
#include "OgreLogManager.h"
#include "Android/OgreAndroidEGLWindow.h"

#include "OgreGLES2Plugin.h"

#include "Game.h"
#include "OgreUtil.h"

#   include "OgreRTShaderSystem.h"

#ifdef OGRE_STATIC_LIB
#   include "StaticPluginLoader.h"
#endif

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "OgreNative", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "OgreNative", __VA_ARGS__))

/*=============================================================================
  | Android input handling
  =============================================================================*/
class AndroidMultiTouch : public OIS::MultiTouch
{
public:
    AndroidMultiTouch():OIS::MultiTouch("Ogre", false, 0, 0){}

    /** @copydoc Object::setBuffered */
    virtual void setBuffered(bool buffered){}

    /** @copydoc Object::capture */
    virtual void capture(){}

    /** @copydoc Object::queryInterface */
    virtual OIS::Interface* queryInterface(OIS::Interface::IType type) {return 0;}

    /** @copydoc Object::_initialize */
    virtual void _initialize(){}

    OIS::MultiTouchState &getMultiTouchState(int i){
        while(i >= mStates.size()){
            Ogre::RenderWindow* pRenderWnd = static_cast<Ogre::RenderWindow*>(Ogre::Root::getSingleton().getRenderTarget("OgreWindow"));
            if(pRenderWnd)
            {
                OIS::MultiTouchState state;
                state.width = pRenderWnd->getWidth();
                state.height = pRenderWnd->getHeight();
                mStates.push_back(state);
            }
        }
        return mStates[i];
    }
};

class AndroidKeyboard : public OIS::Keyboard
{
public:
    AndroidKeyboard():OIS::Keyboard("Ogre", false, 1, 0){}

    /** @copydoc Object::setBuffered */
    virtual void setBuffered(bool buffered){}

    /** @copydoc Object::capture */
    virtual void capture(){}

    /** @copydoc Object::queryInterface */
    virtual OIS::Interface* queryInterface(OIS::Interface::IType type) {return 0;}

    /** @copydoc Object::_initialize */
    virtual void _initialize(){}

    virtual bool isKeyDown( OIS::KeyCode key ) const{
        return false;
    }

    virtual const std::string& getAsString( OIS::KeyCode kc ){
        static std::string defstr = "";
        return defstr;
    }

    virtual void copyKeyStates( char keys[256] ) const{

    }
};

/*=============================================================================
  | Android input injection
  =============================================================================*/
class AndroidInputInjector
{
private:
    OgreUtil* mOgreUtil;
    AndroidMultiTouch* mTouch;
    AndroidKeyboard* mKeyboard;

public:

    AndroidInputInjector(OgreUtil* ogreUtil, AndroidMultiTouch* touch, AndroidKeyboard* keyboard)
        : mOgreUtil(ogreUtil), mTouch(touch), mKeyboard(keyboard) {}

    void injectKeyEvent(int action, int32_t keyCode)
        {
            if(keyCode == AKEYCODE_BACK)
            {
                OIS::KeyEvent evt(mKeyboard, OIS::KC_ESCAPE, 0);
                if(action == 0)
                {
                    // mOgreUtil->keyPressed(evt);
                }
                else
                {
                    // mOgreUtil->keyReleased(evt);
                }
            }
        }

    void injectTouchEvent(int action, float x, float y, int pointerId = 0)
        {
            OIS::MultiTouchState &state = mTouch->getMultiTouchState(pointerId);

            switch(action)
            {
            case 0:
                state.touchType = OIS::MT_Pressed;
                break;
            case 1:
                state.touchType = OIS::MT_Released;
                break;
            case 2:
                state.touchType = OIS::MT_Moved;
                break;
            case 3:
                state.touchType = OIS::MT_Cancelled;
                break;
            default:
                state.touchType = OIS::MT_None;
            }

            if(state.touchType != OIS::MT_None)
            {
                int last = state.X.abs;
                state.X.abs =  (int)x;
                state.X.rel = state.X.abs - last;

                last = state.Y.abs;
                state.Y.abs = (int)y;
                state.Y.rel = state.Y.abs - last;

                state.Z.abs = 0;
                state.Z.rel = 0;

                OIS::MultiTouchEvent evt(mTouch, state);

                switch(state.touchType)
                {
                case OIS::MT_Pressed:
                    // mOgreUtil->touchPressed(evt);
                    break;
                case OIS::MT_Released:
                    // mOgreUtil->touchReleased(evt);
                    break;
                case OIS::MT_Moved:
                    // mOgreUtil->touchMoved(evt);
                    break;
                case OIS::MT_Cancelled:
                    // mOgreUtil->touchCancelled(evt);
                    break;
                default:
                    break;
                }
            }
        }
};

/*=============================================================================
  | Ogre Android bridge
  =============================================================================*/
class OgreAndroidBridge
{
public:
    static void init(struct android_app* state, Game* game)
        {
            state->onAppCmd = &OgreAndroidBridge::handleCmd;
            state->onInputEvent = &OgreAndroidBridge::handleInput;

            mGame = game;

            mRoot = new Ogre::Root();

            mLog = new Ogre::LogManager();
            mLog->createLog("/sdcard/ogre-native.log");

            mGLESPlugin = new Ogre::GLES2Plugin();
            mRoot->installPlugin(mGLESPlugin);

#ifdef OGRE_STATIC_LIB
            mStaticPluginLoader = new Ogre::StaticPluginLoader();
            mStaticPluginLoader->load();
#endif
            
            mRoot->setRenderSystem(mRoot->getAvailableRenderers().at(0));
            mRoot->initialise(false);
        }


    static void shutdown()
        {
            if (mOgreUtil)
            {
                mOgreUtil->shutdown();
                OGRE_DELETE mOgreUtil;
                mOgreUtil = NULL;
            }

            OGRE_DELETE mRoot;
            mRoot = NULL;
            mRenderWnd = NULL;

            delete mTouch;
            mTouch = NULL;

            delete mKeyboard;
            mKeyboard = NULL;

            delete mInputInjector;
            mInputInjector = NULL;

#ifdef OGRE_STATIC_LIB
            mStaticPluginLoader->unload();
            delete mStaticPluginLoader;
            mStaticPluginLoader = NULL;
#endif
        }

    static int32_t handleInput(struct android_app* app, AInputEvent* event)
        {
            if (mInputInjector)
            {
                if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
                {
                    int action = (int)(AMOTION_EVENT_ACTION_MASK & AMotionEvent_getAction(event));

                    if(action == 0)
                        mInputInjector->injectTouchEvent(2, AMotionEvent_getRawX(event, 0), AMotionEvent_getRawY(event, 0) );

                    mInputInjector->injectTouchEvent(action, AMotionEvent_getRawX(event, 0), AMotionEvent_getRawY(event, 0) );
                }
                else
                {
                    mInputInjector->injectKeyEvent(AKeyEvent_getAction(event), AKeyEvent_getKeyCode(event));
                }

                return 1;
            }
            return 0;
        }

    
    static void handleCmd(struct android_app* app, int32_t cmd)
        {
            switch (cmd)
            {
            case APP_CMD_SAVE_STATE:
                break;
            case APP_CMD_INIT_WINDOW:
                if (app->window && mRoot)
                {
                    AConfiguration* config = AConfiguration_new();
                    AConfiguration_fromAssetManager(config, app->activity->assetManager);

                    if (!mRenderWnd)
                    {
                        Ogre::NameValuePairList opt;
                        opt["externalWindowHandle"] = Ogre::StringConverter::toString(reinterpret_cast<size_t>(app->window));
                        opt["androidConfig"] = Ogre::StringConverter::toString(reinterpret_cast<size_t>(config));
                        // Optionally preserve the GL context,
                        // prevents reloading all resources, this is
                        // false by default.
                        opt["preserveContext"] = "true";

                        // Add this opt to force minimum 32 bit color.
                        opt["minColourBufferSize"] = Ogre::StringConverter::toString(32);
                        
                        mRenderWnd = Ogre::Root::getSingleton().createRenderWindow("OgreWindow", 0, 0, false, &opt);

                        if(!mTouch)
                            mTouch = new AndroidMultiTouch();

                        if(!mKeyboard)
                            mKeyboard = new AndroidKeyboard();

                        if (!mOgreUtil)
                        {
                            mOgreUtil = new OgreUtil();
                            mOgreUtil->initAppForAndroid(mRenderWnd, app, mTouch, mKeyboard);

                            mGame->initialize(mRoot, mRenderWnd);
                            mGame->createScene();

                            mOgreUtil->initRTSS();

                            Ogre::Root::getSingleton().getRenderSystem()->_initRenderTargets();
                            Ogre::Root::getSingleton().clearEventTimes();
                            
                            mInputInjector = new AndroidInputInjector(mOgreUtil, mTouch, mKeyboard);
                        }
                    }
                    else
                    {
                        static_cast<Ogre::AndroidEGLWindow*>(mRenderWnd)->_createInternalResources(app->window, config);
                    }

                    AConfiguration_delete(config);
                }
                break;
            case APP_CMD_TERM_WINDOW:
                if(mRoot && mRenderWnd)
                    static_cast<Ogre::AndroidEGLWindow*>(mRenderWnd)->_destroyInternalResources();
                break;
            case APP_CMD_GAINED_FOCUS:
                break;
            case APP_CMD_LOST_FOCUS:
                break;
            case APP_CMD_CONFIG_CHANGED:
                break;
            }
        }
    
    static void go(struct android_app* state)
        {
            int ident, events;
            struct android_poll_source* source;

            while (true)
            {
                while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0)
                {
                    if (source != NULL)
                        source->process(state, source);

                    if (state->destroyRequested != 0)
                        return;
                }

                if (mRenderWnd != NULL && mRenderWnd->isActive())
                {
                    mLog->logMessage("Render one frame.");
                    mRenderWnd->windowMovedOrResized();
                    mRoot->renderOneFrame();
                }
            }
        }

    static Ogre::RenderWindow* getRenderWindow()
        {
            return mRenderWnd;
        }


private:
    static OgreUtil* mOgreUtil;
    static AndroidInputInjector* mInputInjector;
    static AndroidMultiTouch* mTouch;
    static AndroidKeyboard* mKeyboard;
    static Ogre::RenderWindow* mRenderWnd;
    static Ogre::Root* mRoot;
    static Ogre::LogManager* mLog;
    static bool mInit;

    static Ogre::GLES2Plugin* mGLESPlugin;

    static Game* mGame;

#ifdef OGRE_STATIC_LIB
    static Ogre::StaticPluginLoader* mStaticPluginLoader;
#endif
};

#endif
