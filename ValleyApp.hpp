//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef OGRE_VALLEY_HPP
#define OGRE_VALLEY_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "OgreFramework.hpp"
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

class ValleyApp : public OIS::KeyListener
{
    public:
        ValleyApp();
        ~ValleyApp();

        void startGame();

        bool keyPressed(const OIS::KeyEvent &keyEventRef);
        bool keyReleased(const OIS::KeyEvent &keyEventRef);

    private:
        void setupGameScene();
        void runGame();
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
