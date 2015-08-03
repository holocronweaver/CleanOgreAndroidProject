#ifndef __OgreUtil_H__
#define __OgreUtil_H__

#include "Ogre.h"
#include "OgreFileSystemLayer.h"

#include <android_native_app_glue.h>
#include "Android/OgreAPKFileSystemArchive.h"
#include "Android/OgreAPKZipArchive.h"

#include "OgreRTShaderSystem.h"

/** This class demonstrates basic usage of the RTShader system.
    It sub class the material manager listener class and when a target scheme callback
    is invoked with the shader generator scheme it tries to create an equivalent shader
    based technique based on the default technique of the given material.
*/
class ShaderGeneratorTechniqueResolverListener : public Ogre::MaterialManager::Listener
{
public:

    ShaderGeneratorTechniqueResolverListener(Ogre::RTShader::ShaderGenerator* pShaderGenerator)
        {
            mShaderGenerator = pShaderGenerator;
        }

    /** This is the hook point where shader based technique will be created.
        It will be called whenever the material manager won't find appropriate technique
        that satisfy the target scheme name. If the scheme name is out target RT Shader System
        scheme name we will try to create shader generated technique for it.
    */
    Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex,
                                          const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex,
                                          const Ogre::Renderable* rend)
        {
            if (schemeName != Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
            {
                return NULL;
            }
            // Case this is the default shader generator scheme.

            // Create shader generated technique for this material.
            bool techniqueCreated = mShaderGenerator->createShaderBasedTechnique(
                originalMaterial->getName(),
                Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
                schemeName);

            if (!techniqueCreated)
            {
                return NULL;
            }
            // Case technique registration succeeded.

            // Force creating the shaders for the generated technique.
            mShaderGenerator->validateMaterial(schemeName, originalMaterial->getName());

            // Grab the generated technique.
            Ogre::Material::TechniqueIterator itTech = originalMaterial->getTechniqueIterator();

            while (itTech.hasMoreElements())
            {
                Ogre::Technique* curTech = itTech.getNext();

                if (curTech->getSchemeName() == schemeName)
                {
                    return curTech;
                }
            }

            return NULL;
        }

    virtual bool afterIlluminationPassesCreated(Ogre::Technique* tech)
        {
            if(tech->getSchemeName() == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
            {
                Ogre::Material* mat = tech->getParent();
                mShaderGenerator->validateMaterialIlluminationPasses(tech->getSchemeName(), mat->getName(), mat->getGroup());
                return true;
            }
            return false;
        }

    virtual bool beforeIlluminationPassesCleared(Ogre::Technique* tech)
        {
            if(tech->getSchemeName() == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
            {
                Ogre::Material* mat = tech->getParent();
                mShaderGenerator->invalidateMaterialIlluminationPasses(tech->getSchemeName(), mat->getName(), mat->getGroup());
                return true;
            }
            return false;
        }

protected:
    Ogre::RTShader::ShaderGenerator*        mShaderGenerator;                       // The shader generator instance.
};


class OgreUtil
    : public Ogre::FrameListener
    , public Ogre::WindowEventListener
    , public OIS::KeyListener
    , public OIS::MouseListener
    , public OIS::MultiTouchListener  // note: we will send events either to MouseListener or to MultiTouchListener, depending on platform
{
public:
    OgreUtil()
        {
            // RTSS
            mShaderGenerator     = NULL;
            mMaterialMgrListener = NULL;
        }

    ~OgreUtil()
        {
            destroyRTShaderSystem();
        }


    void initAppForAndroid(Ogre::RenderWindow *window, struct android_app* app, OIS::MultiTouch *mouse, OIS::Keyboard *keyboard)
        {
            mWindow = window;
            // mInputContext.mMultiTouch = mouse;
            // mInputContext.mKeyboard = keyboard;

            if (app != NULL)
            {
                mAssetMgr = app->activity->assetManager;
                Ogre::ArchiveManager::getSingleton().addArchiveFactory( new Ogre::APKFileSystemArchiveFactory(app->activity->assetManager) );
                Ogre::ArchiveManager::getSingleton().addArchiveFactory( new Ogre::APKZipArchiveFactory(app->activity->assetManager) );
            }

            mRoot = Ogre::Root::getSingletonPtr();

            setup();

            mWindow->removeAllViewports();                  // wipe viewports


        }

    void setup()
        {
            windowResized(mWindow); // Do an initial adjustment of mouse area.

            // Locate resources.
            Ogre::ConfigFile cf;
            cf.load(openAPKFile("resources.cfg"));
            
            Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
            Ogre::String sec, type, arch;

            // Go through all specified resource groups.
            while (seci.hasMoreElements())
            {
                sec = seci.peekNextKey();
                Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
                Ogre::ConfigFile::SettingsMultiMap::iterator i;

                // Go through all resource paths.
                for (i = settings->begin(); i != settings->end(); i++)
                {
                    type = i->first;
                    arch = i->second;
                    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch, type, sec);
                }
            }

            Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

            // // RTSS
            // if(mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_FIXED_FUNCTION) == false)
            // {
            //     Ogre::RTShader::ShaderGenerator::getSingletonPtr()->addSceneManager(mRoot->getSceneManager("DummyScene"));
            // }

            Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

            // Adds context as listener to process context-level
            // (above the sample level) events.
            mRoot->addFrameListener(this);
        }


    // void windowMovedOrResized()
    //     {
    //         mWindow->windowMovedOrResized();    // notify window
    //         // windowResized(mWindow);   // notify window event listeners
    //     }

    /*-----------------------------------------------------------------------------
      | Destroy the RT Shader system.
      -----------------------------------------------------------------------------*/
    void destroyRTShaderSystem()
        {
            // Restore default scheme.
            Ogre::MaterialManager::getSingleton().setActiveScheme(Ogre::MaterialManager::DEFAULT_SCHEME_NAME);

            // Unregister the material manager listener.
            if (mMaterialMgrListener != NULL)
            {
                Ogre::MaterialManager::getSingleton().removeListener(mMaterialMgrListener);
                delete mMaterialMgrListener;
                mMaterialMgrListener = NULL;
            }

            // Destroy RTShader system.
            if (mShaderGenerator != NULL)
            {
                Ogre::RTShader::ShaderGenerator::destroy();
                mShaderGenerator = NULL;
            }
        }


    Ogre::DataStreamPtr openAPKFile(const Ogre::String& fileName)
        {
            Ogre::DataStreamPtr stream;
            AAsset* asset = AAssetManager_open(mAssetMgr, fileName.c_str(), AASSET_MODE_BUFFER);
            if(asset)
            {
                off_t length = AAsset_getLength(asset);
                void* membuf = OGRE_MALLOC(length, Ogre::MEMCATEGORY_GENERAL);
                memcpy(membuf, AAsset_getBuffer(asset), length);
                AAsset_close(asset);
                
                stream = Ogre::DataStreamPtr(new Ogre::MemoryDataStream(membuf, length, true, true));
            }
            return stream;
        }

    void shutdown()
        {
            // Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
            // shutdownInput();
        }

    void windowResized(Ogre::RenderWindow* rw)
        {
            // if(mInputContext.mMouse)
            // {
            //     // as OIS work in windowing system units we need to convert pixels to them
            //     const OIS::MouseState& ms = mInputContext.mMouse->getMouseState();
            //     float scale = rw->getViewPointToPixelScale();
            //     ms.width = (int)(rw->getWidth() / scale);
            //     ms.height = (int)(rw->getHeight() / scale);
            // }
        }

    bool keyPressed  (const OIS::KeyEvent& arg) { return true; }
    bool keyReleased (const OIS::KeyEvent& arg) { return true; }

    bool mouseMoved    (const OIS::MouseEvent &arg) { return true; }
    bool mousePressed  (const OIS::MouseEvent &arg, OIS::MouseButtonID id) { return true; }
    bool mouseReleased (const OIS::MouseEvent &arg, OIS::MouseButtonID id) { return true; }

    bool touchMoved     (const OIS::MultiTouchEvent &arg) { return true; }
    bool touchPressed   (const OIS::MultiTouchEvent &arg) { return true; }
    bool touchReleased  (const OIS::MultiTouchEvent &arg) { return true; }
    bool touchCancelled (const OIS::MultiTouchEvent &arg) { return true; }

    
    void initRTSS()
        {
            mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
            
            // Create and register the material manager listener if it doesn't exist yet.
            if (mMaterialMgrListener == NULL) {
                mMaterialMgrListener = new ShaderGeneratorTechniqueResolverListener(mShaderGenerator);
                Ogre::MaterialManager::getSingleton().addListener(mMaterialMgrListener);
            }
        }


private:
    // RTSS
    Ogre::RTShader::ShaderGenerator*                        mShaderGenerator;                       // The Shader generator instance.
    ShaderGeneratorTechniqueResolverListener*       mMaterialMgrListener;           // Shader generator material manager listener.
    AAssetManager* mAssetMgr;       // Android asset manager to access files inside apk
    // InputContext mInputContext;     // all OIS devices are here

    Ogre::Root* mRoot;
    Ogre::FileSystemLayer* mFSLayer; // File system abstraction layer

    Ogre::RenderWindow* mWindow;    // render window
};

#endif
