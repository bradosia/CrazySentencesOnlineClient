/*
 * @name Crazy Sentences Online
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief A game for ESL students to learn and have fun!
 */

#ifndef CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_OGRE_APP_H
#define CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_OGRE_APP_H

// Config
#include "../core/config.hpp"

// C++
#include <iostream>
#include <memory>

/* Ogre3D 1.12.2
 * License: MIT
 */
#if CSO_OGRE_ENABLE
#include <OgreMaterialManager.h>
#include <OgreRoot.h>
#if CSO_OGRE_REAL_TIME_SHADER_ENABLE
#include <RTShaderSystem/OgreRTShaderSystem.h>
#endif
#endif

// Local Project
#if CSO_OGRE_REAL_TIME_SHADER_ENABLE
#include "SGTechniqueResolverListener.hpp"
#endif

/*
 * CSO = Crazy Sentences Online
 */
namespace CSO {

/**
    Base class responsible for setting up a common context for applications.
    Subclass to implement specific event callbacks.
    */
class OgreApplication {
public:
  /**
   * Initialize the RT Shader system.
   */
  bool initialiseRTShaderSystem();

  /**
   * make the RTSS write out the generated shaders for caching and debugging
   *
   * by default all shaders are generated to system memory.
   * Must be called before loadResources
   * @param write
   */
  void setRTSSWriteShadersToDisk(bool write);

  /**
   * Destroy the RT Shader system.
   */
  void destroyRTShaderSystem();

  /**
   * Sets up the context after configuration.
   */
  void setup();

  /**
   * Finds context-wide resource groups. I load paths from a config file
   * here, but you can choose your resource locations however you want.
   */
  void locateResources();

  /**
   * Loads context-wide resource groups. I chose here to simply initialise all
   * groups, but you can fully load specific ones if you wish.
   */
  void loadResources();

  void addSceneManagerToRTShader(Ogre::SceneManager *sceneManager);

protected:
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  Ogre::RTShader::ShaderGenerator
      *mShaderGenerator; // The Shader generator instance.
  SGTechniqueResolverListener
      *mMaterialMgrListener; // Shader generator material manager listener.
#endif                       // INCLUDE_RTSHADER_SYSTEM
};

} // namespace CSO

#endif
// CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_OGRE_APP_H
