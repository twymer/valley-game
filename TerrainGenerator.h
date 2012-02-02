#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include "OgreFramework.hpp"

class TerrainGenerator {
    public:
        TerrainGenerator();
        ~TerrainGenerator();

        void createTerrain(Ogre::String);
        Ogre::Vector2** getLine();

};
#endif
