#ifndef __Game_h_
#define __Game_h_

#include "OgreCamera.h"
#include "OgreEntity.h"
#include "OgreManualObject.h"
#include "OgreRoot.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreViewport.h"

#include "OgreTechnique.h"
#include "OgreRTShaderSystem.h"


class Game
{
public:
    Game()
        : mCamera(NULL)
        , mRenderWindow(NULL)
        , mRoot(NULL)
        , mSceneMgr(NULL)
        , mViewport(NULL)
        {
            
        }
    
    void initialize(Ogre::Root* root, Ogre::RenderWindow* renderWindow)
        {
            mRenderWindow = renderWindow;
            mRoot = root;
        }
    
    void createScene()
        {
            if (!mRoot || !mRenderWindow)
                return; //TODO: Throw error.
                
            mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);


            rtshaderinit(mSceneMgr);

            // mCameraMan->setStyle(CS_ORBIT);
            // Camera* cam = mCameraMan->getCamera();
            mCamera = mSceneMgr->createCamera("MyCam");

            Ogre::Viewport* mViewport = mRenderWindow->addViewport(mCamera);
            mViewport->setBackgroundColour(Ogre::ColourValue(0,0,1));

            Ogre::SceneNode* rootNode = mSceneMgr->getRootSceneNode();

            mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));
            Ogre::Light* l = mSceneMgr->createLight("MainLight");
            l->setPosition((Ogre::Real)20.0, (Ogre::Real)80.0, (Ogre::Real)50.0);

            mCamera->setPosition(0, 5, -30);
            // mCamera->setPosition(0, 0, -80);
            mCamera->lookAt(0, 0, 0);
            mCamera->setNearClipDistance(2); // default 100
            mCamera->setFarClipDistance(10000); // default 10000
            mCamera->setAspectRatio(
                Ogre::Real(mViewport->getActualWidth()
                           / Ogre::Real(mViewport->getActualHeight())));
            // mCamera->setPolygonMode(Ogre::PM_WIREFRAME);


            // // White triangle for debugging.
            // Ogre::ManualObject* manObj = new Ogre::ManualObject("triangle");
            // manObj->setDynamic(false);
            // manObj->begin("BaseWhiteNoLighting",
            // // manObj->begin("White",
            //               Ogre::RenderOperation::OT_TRIANGLE_LIST);
            // {
            //     manObj->position(0, 1, 0);
            //     manObj->position(0, 0, 1);
            //     manObj->position(1, 0, 0);

            //     manObj->position(2, 0, 0);
            //     manObj->position(0, 0, 2);
            //     manObj->position(0, 2, 0);

            //     manObj->position(3, 0, 0);
            //     manObj->position(0, 3, 0);
            //     manObj->position(0, 0, 3);

            //     manObj->position(0, 4, 0);
            //     manObj->position(4, 0, 0);
            //     manObj->position(0, 0, 4);

            //     manObj->position(0, 0, 5);
            //     manObj->position(5, 0, 0);
            //     manObj->position(0, 5, 0);

            //     manObj->position(0, 0, 6);
            //     manObj->position(0, 6, 0);
            //     manObj->position(6, 0, 0);

            //     manObj->position(10, 10, 0);
            //     manObj->position(0, 10, 0);
            //     manObj->position(0, 0, 0);
            // }
            // manObj->end();
            // Ogre::SceneNode* manObjNode = rootNode->createChildSceneNode();
            // // manObjNode->setPosition(0, 0, 0);
            // // manObjNode->scale(Ogre::Vector3(5));
            // manObjNode->attachObject(manObj);

            // Ogre::Entity* sphere = mSceneMgr->createEntity("sphere", "sphere.mesh");
            Ogre::Entity* sphere = mSceneMgr->createEntity("ogre", "ogrehead.mesh");
            sphere->setMaterialName("White");
            Ogre::SceneNode* sphereNode = rootNode->createChildSceneNode();
            sphereNode->setPosition(0, 0, 0);
            // sphereNode->setPosition(0, 0, 5);
            // sphereNode->scale(0.5, 0.5, 0.5); 
            sphereNode->scale(0.1, 0.1, 0.1); 
            // sphereNode->scale(2, 2, 2); // Make OGRE head approx 1 meter
            sphereNode->attachObject(sphere);

        }


    void rtshaderinit(Ogre::SceneManager* sm)
        {
            // Initialize shader generator.
            // Must be before resource loading in order to allow parsing extended material attributes.
            bool success = initialiseRTShaderSystem(sm);
            if (!success)
            {
                OGRE_EXCEPT(Ogre::Exception::ERR_FILE_NOT_FOUND,
                            "Shader Generator Initialization failed - Core shader libs path not found",
                            "SampleBrowser::createDummyScene");
            }
            if(mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_FIXED_FUNCTION) == false)
            {
                // creates shaders for base material BaseWhite using the RTSS
                Ogre::MaterialPtr baseWhite = Ogre::MaterialManager::getSingleton().getByName("BaseWhite", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
                mShaderGenerator->createShaderBasedTechnique(
                    "BaseWhite",
                    Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
                    Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
                mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
                                                   "BaseWhite");
                if(baseWhite->getNumTechniques() > 1)
                {
                    baseWhite->getTechnique(0)->getPass(0)->setVertexProgram(
                        baseWhite->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
                    baseWhite->getTechnique(0)->getPass(0)->setFragmentProgram(
                        baseWhite->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
                }

                // creates shaders for base material BaseWhiteNoLighting using the RTSS
                mShaderGenerator->createShaderBasedTechnique(
                    "BaseWhiteNoLighting",
                    Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
                    Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
                mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
                                                   "BaseWhiteNoLighting");
                Ogre::MaterialPtr baseWhiteNoLighting = Ogre::MaterialManager::getSingleton().getByName("BaseWhiteNoLighting", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
                if(baseWhiteNoLighting->getNumTechniques() > 1)
                {
                    baseWhiteNoLighting->getTechnique(0)->getPass(0)->setVertexProgram(
                        baseWhiteNoLighting->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
                    baseWhiteNoLighting->getTechnique(0)->getPass(0)->setFragmentProgram(
                        baseWhiteNoLighting->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
                }
            }
        }

    
    /*-----------------------------------------------------------------------------
      | Initialize the RT Shader system.
      -----------------------------------------------------------------------------*/
    bool initialiseRTShaderSystem(Ogre::SceneManager* sceneMgr)
        {
            if (Ogre::RTShader::ShaderGenerator::initialize())
            {
                mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();

                mShaderGenerator->addSceneManager(sceneMgr);

                // Setup core libraries and shader cache path.
                Ogre::StringVector groupVector = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
                Ogre::StringVector::iterator itGroup = groupVector.begin();
                Ogre::StringVector::iterator itGroupEnd = groupVector.end();
                Ogre::String shaderCoreLibsPath;
                Ogre::String shaderCachePath;

                for (; itGroup != itGroupEnd; ++itGroup)
                {
                    Ogre::ResourceGroupManager::LocationList resLocationsList = Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(*itGroup);
                    Ogre::ResourceGroupManager::LocationList::iterator it = resLocationsList.begin();
                    Ogre::ResourceGroupManager::LocationList::iterator itEnd = resLocationsList.end();
                    bool coreLibsFound = false;

                    // Try to find the location of the core shader lib functions and use it
                    // as shader cache path as well - this will reduce the number of generated files
                    // when running from different directories.
                    for (; it != itEnd; ++it)
                    {
                        if ((*it)->archive->getName().find("RTShaderLib") != Ogre::String::npos)
                        {
                            shaderCoreLibsPath = (*it)->archive->getName() + "/cache/";
                            shaderCachePath = shaderCoreLibsPath;
                            coreLibsFound = true;
                            break;
                        }
                    }
                    // Core libs path found in the current group.
                    if (coreLibsFound)
                        break;
                }

                // Core shader libs not found -> shader generating will fail.
                if (shaderCoreLibsPath.empty())
                    return false;
            }
            
            return true;
        }

private:
    Ogre::Camera* mCamera;
    Ogre::RenderWindow* mRenderWindow;
    Ogre::Root* mRoot;
    Ogre::SceneManager* mSceneMgr;
    Ogre::Viewport* mViewport;
    Ogre::RTShader::ShaderGenerator* mShaderGenerator;
};

#endif // __Game_h_
