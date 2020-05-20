/*
 * @name Crazy Sentences Online
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief A game for ESL students to learn and have fun!
 */

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include "boost/dll/runtime_symbol_info.hpp"
#include "boost/filesystem.hpp"

/* Ogre3D 1.12.2
 * License: MIT
 */
#include <OgreException.h>
#include <OgreTechnique.h>
#include <OgreTextureManager.h>

// Local Project
#include "RTShader.hpp"

/*
 * CSO = Crazy Sentences Online
 */
namespace CSO {

RTShader::RTShader(){

};
RTShader::~RTShader(){

};

bool RTShader::initialize(Ogre::SceneManager *sceneMgr) {
#if CSO_OGRE_REAL_TIME_SHADER_ENABLE
  if (!Ogre::RTShader::ShaderGenerator::initialize()) {

    return false;
  }

  Ogre::RTShader::ShaderGenerator *m_shaderGenerator =
      Ogre::RTShader::ShaderGenerator::getSingletonPtr();
  m_shaderGenerator->addSceneManager(sceneMgr);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID &&                                  \
    OGRE_PLATFORM != OGRE_PLATFORM_NACL &&                                     \
    OGRE_PLATFORM != OGRE_PLATFORM_WINRT
  // Setup core libraries and shader cache path.
  /*Ogre::StringVector groupVector =
  Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
  Ogre::StringVector::iterator itGroup = groupVector.begin();
  Ogre::StringVector::iterator itGroupEnd = groupVector.end();
  Ogre::String shaderCoreLibsPath;
  Ogre::String shaderCachePath;

  for (; itGroup != itGroupEnd; ++itGroup)
  {
      Ogre::ResourceGroupManager::LocationList resLocationsList =
  Ogre::ResourceGroupManager::
                                                  getSingleton().getResourceLocationList(*itGroup);
      Ogre::ResourceGroupManager::LocationList::iterator it =
  resLocationsList.begin(); Ogre::ResourceGroupManager::LocationList::iterator
  itEnd = resLocationsList.end(); bool coreLibsFound = false;

      // Try to find the location of the core shader lib functions and use it
      // as shader cache path as well - this will reduce the number of
  generated files
      // when running from different directories.
      for (; it != itEnd; ++it)
      {
          if ((*it)->archive->getName().find("RTShaderLib") !=
  Ogre::String::npos)
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
      return false;*/

  /*Ogre::StringVector groupVector =
      Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
  Ogre::StringVector::iterator itGroup = groupVector.begin();
  Ogre::StringVector::iterator enGroup = groupVector.end();

  bool coreLibsFound = false;
  while (itGroup != enGroup && !coreLibsFound) {
    Ogre::ResourceGroupManager::LocationList resLocationsList =
        Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(
            *itGroup);
    Ogre::ResourceGroupManager::LocationList::iterator itor =
        resLocationsList.begin();
    Ogre::ResourceGroupManager::LocationList::iterator end =
        resLocationsList.end();

    // Try to find the location of the core shader lib functions and use it
    // as shader cache path as well - this will reduce the number of generated
    // files when running from different directories.
    while (itor != end && !coreLibsFound) {
      if (itor->archive->getName().find("RTShaderLib") != Ogre::String::npos)
        coreLibsFound = true;
      ++itor;
    }

    ++itGroup;
  }

  if (!coreLibsFound) {
    std::cout << "RTShader::initialize RTShaderLib resources not found!" << std::endl;
    Ogre::RTShader::ShaderGenerator::destroy();
    return false;
  }*/

  boost::filesystem::path programPath = boost::dll::program_location();
  boost::filesystem::path rtShaderCachePath =
      programPath.parent_path() / "RTShaderCache/";
  std::cout << "RTShader::initialize rtShaderCachePath=" << rtShaderCachePath
            << std::endl;

  if (boost::filesystem::create_directory(rtShaderCachePath)) {
    std::cout << "Directory Created: " << rtShaderCachePath << std::endl;
  }
  m_shaderGenerator->setShaderCachePath(rtShaderCachePath.string());

#ifdef _RTSS_WRITE_SHADERS_TO_DISK
  // Set shader cache path.
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
  shaderCachePath = Ogre::macCachePath();
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
  shaderCachePath = Ogre::macCachePath() + "/org.ogre3d.RTShaderCache";
#endif
  try {
    m_shaderGenerator->setShaderCachePath(shaderCachePath);
  } catch (Ogre::Exception &e) {
    e;
    m_shaderGenerator->setShaderCachePath("");
  }
#endif
#endif
  // Create and register the material manager listener if it doesn't exist
  // yet.
  if (mMaterialMgrListener == NULL) {
    mMaterialMgrListener =
        new ShaderGeneratorTechniqueResolverListener(m_shaderGenerator);
    Ogre::MaterialManager::getSingleton().addListener(mMaterialMgrListener);
  }

  Ogre::MaterialManager::getSingleton().setActiveScheme(
      Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

#endif
  return true;
}

/*-----------------------------------------------------------------------------
| Finalize the RT Shader system.
-----------------------------------------------------------------------------*/
void RTShader::finalize() {
  // Restore default scheme.
  Ogre::MaterialManager::getSingleton().setActiveScheme(
      Ogre::MaterialManager::DEFAULT_SCHEME_NAME);

  // Unregister the material manager listener.
  if (mMaterialMgrListener != NULL) {
    Ogre::MaterialManager::getSingleton().removeListener(mMaterialMgrListener);
    delete mMaterialMgrListener;
    mMaterialMgrListener = NULL;
  }

  // Finalize RTShader system.
  if (mShaderGenerator != NULL) {
    Ogre::RTShader::ShaderGenerator::destroy();
    mShaderGenerator = NULL;
  }
}

ShaderGeneratorTechniqueResolverListener::
    ShaderGeneratorTechniqueResolverListener(
        Ogre::RTShader::ShaderGenerator *pShaderGenerator) {
  mShaderGenerator = pShaderGenerator;
}

/** This is the hook point where shader based technique will be created.
It will be called whenever the material manager won't find appropriate technique
that satisfy the target scheme name. If the scheme name is out target RT Shader
System scheme name we will try to create shader generated technique for it.
*/
Ogre::Technique *ShaderGeneratorTechniqueResolverListener::handleSchemeNotFound(
    unsigned short schemeIndex, const Ogre::String &schemeName,
    Ogre::Material *originalMaterial, unsigned short lodIndex,
    const Ogre::Renderable *rend) {
  std::cout << "ShaderGeneratorTechniqueResolverListener::handleSchemeNotFound"
            << ", schemeName: " << schemeName.c_str()
            << ", Material: " << originalMaterial->getName().c_str()
            << std::endl;
  Ogre::Technique *generatedTech = NULL;

  // Case this is the default shader generator scheme.
  if (schemeName == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME) {
    bool techniqueCreated;

    std::cout << "TP1" << std::endl;

    // Create shader generated technique for this material.
    techniqueCreated = mShaderGenerator->createShaderBasedTechnique(
        *originalMaterial, Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
        schemeName);
    // Case technique registration succeeded.
    if (techniqueCreated) {
      // Force creating the shaders for the generated technique.
      mShaderGenerator->validateMaterial(schemeName,
                                         originalMaterial->getName());
      // Grab the generated technique.
      for (Ogre::Technique *curTech : Ogre::Material::Techniques()) {
          std::cout << "ShaderGeneratorTechniqueResolverListener curTech=" << curTech->getName() << std::endl;
          if (curTech->getSchemeName() == schemeName) {
            generatedTech = curTech;
            break;
          }
      }
    }
  }

  return generatedTech;
}

bool RTShader::isListen(){
    if(mShaderGenerator){
       return true;
    }
    return false;
}

} // namespace CSO
