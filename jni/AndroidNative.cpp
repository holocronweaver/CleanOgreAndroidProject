#include "AndroidUtil.h"
#include "Game.h"

OgreUtil* OgreAndroidBridge::mOgreUtil = NULL;
AndroidInputInjector* OgreAndroidBridge::mInputInjector = NULL;
AndroidMultiTouch* OgreAndroidBridge::mTouch = NULL;
AndroidKeyboard* OgreAndroidBridge::mKeyboard = NULL;
Ogre::RenderWindow* OgreAndroidBridge::mRenderWnd = NULL;
Ogre::Root* OgreAndroidBridge::mRoot = NULL;
Ogre::LogManager* OgreAndroidBridge::mLog = NULL;
bool OgreAndroidBridge::mInit = false;

Ogre::GLES2Plugin* OgreAndroidBridge::mGLESPlugin = NULL;

Game* OgreAndroidBridge::mGame = NULL;

Ogre::StaticPluginLoader* OgreAndroidBridge::mStaticPluginLoader = NULL;

void android_main(struct android_app* state) {
    // Make sure glue isn't stripped.
    app_dummy();
    
    OgreAndroidBridge::init(state, new Game());
    OgreAndroidBridge::go(state);
}
