/*
 * @name Crazy Sentences Online
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief A game for ESL students to learn and have fun!
 */

#ifndef CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_SCENE_MENU_H
#define CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_SCENE_MENU_H

// config
#include "../core/config.hpp"

// c++17
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include "boost/dll/runtime_symbol_info.hpp"
#include "boost/filesystem.hpp"

/* Ogre3D 1.12.2
 * License: MIT
 */
#include <OgreBone.h>
#include <OgreCamera.h>
#include <OgreConfigFile.h>
#include <OgreEntity.h>
#include <OgreException.h>
#include <OgreFrameListener.h>
#include <OgreMaterialManager.h>
#include <OgreRenderWindow.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreSkeleton.h>
#include <OgreSkeletonInstance.h>
#include <OgreTechnique.h>
#include <OgreTextureManager.h>
#include <OgreViewport.h>
#include <RTShaderSystem/OgreRTShaderSystem.h>

/*
 * CSO = Crazy Sentences Online
 */
namespace CSO {

class ShaderGeneratorTechniqueResolverListener
    : public Ogre::MaterialManager::Listener {
public:
  ShaderGeneratorTechniqueResolverListener(
      Ogre::RTShader::ShaderGenerator *pShaderGenerator);

  virtual Ogre::Technique *
  handleSchemeNotFound(unsigned short schemeIndex,
                       const Ogre::String &schemeName,
                       Ogre::Material *originalMaterial,
                       unsigned short lodIndex, const Ogre::Renderable *rend);

protected:
  Ogre::RTShader::ShaderGenerator
      *mShaderGenerator; // The shader generator instance.
};

class SceneMenu {
private:
public:
  SceneMenu();
  ~SceneMenu();

  bool initialize(std::shared_ptr<Ogre::Root>,
                  std::shared_ptr<Ogre::RenderWindow>);
  void finalizeRTShaderSystem();
  bool initializeRTShaderSystem(Ogre::SceneManager *sceneMgr);
  void setupResources();
  void defaultCamera();
  void originCamera();
  void centerMeshCamera();
  void rotateCamera(int x, int y);
  void zoomInCamera(Ogre::Real wheelDelta);
  void moveCamera(int x, int y);
  void slideCamera(int x, int z);

private:
  std::shared_ptr<Ogre::Root> ogreRoot;
  // The shader generator instance.
  Ogre::RTShader::ShaderGenerator *mShaderGenerator;
  // Shader generator material manager listener.
  ShaderGeneratorTechniqueResolverListener *mMaterialMgrListener;
  Ogre::Entity *mMeshEntity;
  Ogre::Camera *mCamera;
  Ogre::SceneNode *mCameraNode;
  Ogre::SceneNode *mCameraTargetNode;
  Ogre::SceneNode *mNode;
};

} // namespace CSO

#endif
// end CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_SCENE_MENU_H
