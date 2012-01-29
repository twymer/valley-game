//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef OGRE_DEMO_HPP
#define OGRE_DEMO_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "OgreFramework.hpp"
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

class DemoApp : public OIS::KeyListener
{
    public:
        DemoApp();
        ~DemoApp();

        void startDemo();

        bool keyPressed(const OIS::KeyEvent &keyEventRef);
        bool keyReleased(const OIS::KeyEvent &keyEventRef);

    private:
        void setupDemoScene();
        void runDemo();
        void defineTerrain(long int, long int);
        void initBlendMaps(Ogre::Terrain*);
        void configureTerrainDefaults(Ogre::Light*);
        void destroyScene(void);

        Ogre::TerrainGlobalOptions* mTerrainGlobals;
        Ogre::TerrainGroup* mTerrainGroup;
        bool mTerrainsImported;

        Ogre::SceneNode* m_pOgreHeadNode;
        Ogre::Entity* m_pOgreHeadEntity;

        bool m_bShutdown;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif 

//|||||||||||||||||||||||||||||||||||||||||||||||
