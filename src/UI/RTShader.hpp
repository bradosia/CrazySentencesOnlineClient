/*
 * @name Crazy Sentences Online
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief A game for ESL students to learn and have fun!
 */

#ifndef CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_RT_SHADER_H
#define CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_RT_SHADER_H

// Config
#include "../core/config.hpp"

// C++
#include <iostream>
#include <memory>

/* Dear ImGui 1.75
 * License: MIT
 */
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl.h>

/* Ogre3D 1.12.2
 * License: MIT
 */
#if CSO_OGRE_ENABLE
#include <OgreRoot.h>
#include <OgreMaterialManager.h>
#if CSO_OGRE_REAL_TIME_SHADER_ENABLE
#include <RTShaderSystem/OgreRTShaderSystem.h>
#endif
#endif

/*
 * CSO = Crazy Sentences Online
 */
namespace CSO {

#if CSO_OGRE_REAL_TIME_SHADER_ENABLE
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
#endif

class RTShader {

public:
  RTShader();
  ~RTShader();
  void finalize();
  bool initialize(Ogre::SceneManager *sceneMgr);
  bool isListen();

private:
#if CSO_OGRE_REAL_TIME_SHADER_ENABLE
  // The shader generator instance.
  Ogre::RTShader::ShaderGenerator *mShaderGenerator;
  // Shader generator material manager listener.
  ShaderGeneratorTechniqueResolverListener *mMaterialMgrListener;
#endif
};

} // namespace CSO

#endif
// CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_RT_SHADER_H
