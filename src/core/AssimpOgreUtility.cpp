/*
 * @name Assimp Ogre
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief Utilities for assimp-ogre interaction
 */
// C++
#include <iostream>

/* Ogre3D 1.12.2
 * License: MIT
 */
#include <OgreMesh.h>
#include <OgreMeshManager.h>
#include <OgreRoot.h>
#include <OgreSubMesh.h>
#include <OgreSkeletonManager.h>
#include <OgreMaterialManager.h>

// Local Project
#include "AssimpOgreUtility.hpp"

/* AssimpOgre
 * These utilities are meant to be used after the aiScene has been created
 */
namespace AssimpOgre {

bool import(Ogre::Root *rootPtr, aiScene *scenePtr) {
  /* The import steps are as follows:
   * - import meshes (uv, materials, vertexes, bones, faces)
   * - import bone-node hierarchy
   * - import animations
   */
  Ogre::MeshManager *meshManagerPtr = Ogre::MeshManager::getSingletonPtr();
  Ogre::SharedPtr<Ogre::Mesh> meshPtr =
      meshManagerPtr->createManual("myMeshName", "General");
  // For each Assimp mesh in the Assimp scene create an Ogre submesh
  for (unsigned int meshId = 0; meshId < scenePtr->mNumMeshes; meshId++) {
    aiMesh *assMesh = scenePtr->mMeshes[meshId];
    aiString &meshName = assMesh->mName;
    Ogre::SubMesh *ogSubMesh = meshPtr->createSubMesh(meshName.C_Str());
    // Create a VertexData
    Ogre::VertexData *ogVertexData = new Ogre::VertexData();
    ogSubMesh->useSharedVertices = false;
    ogSubMesh->vertexData = ogVertexData;
    // Provide the number of vertices
    ogVertexData->vertexCount = assMesh->mNumVertices;
    // Then we can create our VertexDeclaration
    Ogre::VertexDeclaration *ogVertexDecl = ogVertexData->vertexDeclaration;
    ogVertexDecl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
    size_t offset = 0;
    ogVertexDecl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
    ogVertexDecl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
    ogVertexDecl->addElement(0, offset, Ogre::VET_FLOAT2,
                             Ogre::VES_TEXTURE_COORDINATES);
    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
    ogVertexDecl->addElement(0, offset, Ogre::VET_FLOAT2,
                             Ogre::VES_TEXTURE_COORDINATES, 1);
  }

  /* Material importing
   */
  Ogre::MaterialManager* ogMaterialManager = Ogre::MaterialManager::getSingletonPtr();
  /* Ogre has a proper skeleton data structure
   * while Assimp has a awkward Bone-Node structure
   */
  Ogre::SkeletonManager* ogSkeletonManager = Ogre::SkeletonManager::getSingletonPtr();



  return true;
}

bool import(std::shared_ptr<Ogre::Root> rootPtr,
            std::shared_ptr<aiScene> scenePtr) {
  return import(rootPtr.get(), scenePtr.get());
}

} // namespace AssimpOgre
