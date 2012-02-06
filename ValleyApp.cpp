//|||||||||||||||||||||||||||||||||||||||||||||||

#include "ValleyApp.hpp"

#include <OgreLight.h>
#include <OgreWindowEventUtilities.h>
#include <OgreStringConverter.h>
#include <OgreManualObject.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

ValleyApp::ValleyApp()
{
    m_pOgreHeadNode = 0;
    m_pOgreHeadEntity = 0;
    mTerrainsImported = false;

    mGenerator.createTerrain("heightmap2.bmp");
}

ValleyApp::~ValleyApp()
{
    destroyScene();
    delete OgreFramework::getSingletonPtr();
}

void ValleyApp::destroyScene(void)
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Scene cleaned up!");
    OGRE_DELETE mTerrainGroup;
    OGRE_DELETE mTerrainGlobals;
}

void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
    img.load("heightmap2.bmp", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
}

void ValleyApp::defineTerrain(long x, long y)
{
    Ogre::String filename = mTerrainGroup->generateFilename(x, y);
    if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
    {
        mTerrainGroup->defineTerrain(x, y);
    }
    else
    {
        Ogre::Image img;
        getTerrainImage(x % 2 != 0, y % 2 != 0, img);
        mTerrainGroup->defineTerrain(x, y, &img);
        mTerrainsImported = true;
    }
}

void ValleyApp::configureTerrainDefaults(Ogre::Light* light)
{
    // Configure global
    mTerrainGlobals->setMaxPixelError(8);
    // testing composite map
    mTerrainGlobals->setCompositeMapDistance(30000);
 
    // Important to set these so that the terrain knows what to use for derived (non-realtime) data
    mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
    mTerrainGlobals->setCompositeMapAmbient(OgreFramework::getSingletonPtr()->m_pSceneMgr->getAmbientLight());
    mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
 
    // Configure default import settings for if we use imported image
    Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
    defaultimp.terrainSize = 513;
    defaultimp.worldSize = 12000.0f;
    defaultimp.inputScale = 600*6;
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;
    // textures
    defaultimp.layerList.resize(3);
    defaultimp.layerList[0].worldSize = 100;
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
    defaultimp.layerList[1].worldSize = 30;
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
    defaultimp.layerList[2].worldSize = 200;
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}

void ValleyApp::initBlendMaps(Ogre::Terrain* terrain)
{
    Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
    Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
    Ogre::Real minHeight0 = 70;
    Ogre::Real fadeDist0 = 40;
    Ogre::Real minHeight1 = 70;
    Ogre::Real fadeDist1 = 15;
    float* pBlend0 = blendMap0->getBlendPointer();
    float* pBlend1 = blendMap1->getBlendPointer();
    for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
    {
        for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
        {
            Ogre::Real tx, ty;
 
            blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
            Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
            Ogre::Real val = (height - minHeight0) / fadeDist0;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend0++ = val;
 
            val = (height - minHeight1) / fadeDist1;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend1++ = val;
        }
    }
    blendMap0->dirty();
    blendMap1->dirty();
    blendMap0->update();
    blendMap1->update();
}

void ValleyApp::startGame()
{
    new OgreFramework();
    if(!OgreFramework::getSingletonPtr()->initOgre("ValleyApp v1.0", this, 0))
        return;

    m_bShutdown = false;

    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Game initialized!");

    setupGameScene();
    runGame();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ValleyApp::setupGameScene()
{
    OgreFramework::getSingletonPtr()->m_pCamera->setPosition(Ogre::Vector3(1683, 10000, 2116));
    OgreFramework::getSingletonPtr()->m_pCamera->lookAt(Ogre::Vector3(0, 0, 0));
    OgreFramework::getSingletonPtr()->m_pCamera->setNearClipDistance(0.1);
    OgreFramework::getSingletonPtr()->m_pCamera->setFarClipDistance(50000);
 
    if (OgreFramework::getSingletonPtr()->m_pRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
    {
        OgreFramework::getSingletonPtr()->m_pCamera->setFarClipDistance(0);   // enable infinite far clip distance if we can
    }
 
// Play with startup Texture Filtering options
// Note: Pressing T on runtime will discarde those settings
//  Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
//  Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);

    //Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
    Ogre::Vector3 lightdir(0.2, -1.0, 0.2);
    lightdir.normalise();

    Ogre::Light* light = OgreFramework::getSingletonPtr()->m_pSceneMgr->createLight("tstLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightdir);
    light->setDiffuseColour(Ogre::ColourValue::White);
    light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

    OgreFramework::getSingletonPtr()->m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));

    mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();

    mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(OgreFramework::getSingletonPtr()->m_pSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
    mTerrainGroup->setFilenameConvention(Ogre::String("ValleyTerrain"), Ogre::String("dat"));
    mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);

    configureTerrainDefaults(light);

    for (long x = 0; x <= 0; ++x)
        for (long y = 0; y <= 0; ++y)
            defineTerrain(x, y);

    // sync load since we want everything in place when we start
    mTerrainGroup->loadAllTerrains(true);

    if (mTerrainsImported)
    {
        Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
        while(ti.hasMoreElements())
        {
            Ogre::Terrain* t = ti.getNext()->instance;
            initBlendMaps(t);
        }
    }

    mTerrainGroup->freeTemporaryResources();

    Ogre::ManualObject* manual = OgreFramework::getSingletonPtr()->m_pSceneMgr->createManualObject("manual");
    manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);

    Ogre::Vector2* line = mGenerator.getLine();
    //TODO: can't hardcode 6
    for(int i = 0; i < 6 * 6; i++) {
      //OgreFramework::getSingletonPtr()->m_pLog->logMessage(
              //Ogre::StringConverter::toString(line[i].x) +
              //Ogre::StringConverter::toString(line[i].y));
      manual->position(line[i].x, 1500, -line[i].y);
    }

    manual->end();

    Ogre::SceneNode* sn = OgreFramework::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
    sn->setScale(12000/512,1,12000/512);
    sn->translate(-12000/2,0, 12000/2);
    sn->attachObject(manual);


    //Cheap water..
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 1000);
    Ogre::MeshManager::getSingleton().createPlane("water",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            plane, 12000, 12000, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
    Ogre::Entity* entWater = OgreFramework::getSingletonPtr()->m_pSceneMgr->createEntity("WaterEntity", "water");
    OgreFramework::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entWater);
    entWater->setCastShadows(false);
    entWater->setMaterialName("Examples/WaterStream");


    //Test trees
    Ogre::Entity* entTree = OgreFramework::getSingletonPtr()->m_pSceneMgr->createEntity("Tree", "oakA.mesh");
    Ogre::SceneNode* treeNode = OgreFramework::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
    treeNode->setScale(10,10,10);
    treeNode->translate(0,1500,0);
    treeNode->attachObject(entTree);
    entTree->setMaterialName("oak");
}

void ValleyApp::runGame()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Start main loop...");

    int timeSinceLastFrame = 0;
    int startTime = 0;

    OgreFramework::getSingletonPtr()->m_pRenderWnd->resetStatistics();

    while(!m_bShutdown && !OgreFramework::getSingletonPtr()->isOgreToBeShutDown()) 
    {
        if(OgreFramework::getSingletonPtr()->m_pRenderWnd->isClosed())m_bShutdown = true;

        Ogre::WindowEventUtilities::messagePump();

        if(OgreFramework::getSingletonPtr()->m_pRenderWnd->isActive())
        {
            startTime = OgreFramework::getSingletonPtr()->m_pTimer->getMilliseconds();

            OgreFramework::getSingletonPtr()->m_pKeyboard->capture();
            OgreFramework::getSingletonPtr()->m_pMouse->capture();

            OgreFramework::getSingletonPtr()->updateOgre(timeSinceLastFrame);
            OgreFramework::getSingletonPtr()->m_pRoot->renderOneFrame();

            timeSinceLastFrame = OgreFramework::getSingletonPtr()->m_pTimer->getMilliseconds() - startTime;
        }
        else
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
        }
    }

    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Main loop quit");
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Shutdown OGRE...");
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool ValleyApp::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_F))
    {
        //do something
    }

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool ValleyApp::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||
