/*
 * @name Assimp Ogre
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief Utilities for assimp-ogre interaction
 */
// C++
#include <memory>

/* Ogre3D 1.12.2
 * License: MIT
 */
#include <OgreRoot.h>

/* assimp 5.0.1
 * License: BSD
 */
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h>      // Output data structure

#ifndef ASSIMP_OGRE_UTIL_H
#define ASSIMP_OGRE_UTIL_H

/* AssimpOgre
 * These utilities are meant to be used after the aiScene has been created
 */
namespace AssimpOgre {

/* Loads the assimp scene data into the ogre instance.
 * Ogre must have already been initialized and created.
 * This function does not take ownership of the pointer.
 * Passing the Ogre root instance is not very helpful since the Ogre resource managers are global.
 * @param scenePtr The Assimp scene source
 */
bool import(aiScene *scenePtr);

/* For shared pointers
 */
bool import(std::shared_ptr<aiScene> scenePtr);

} // namespace AssimpOgre

#endif // ASSIMP_OGRE_UTIL_H
