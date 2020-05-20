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
#include <OgreConfigFile.h>
#include <OgreException.h>
#include <OgreTechnique.h>
#include <OgreTextureManager.h>

// Local Project
#include "OgreApplication.hpp"

/*
 * CSO = Crazy Sentences Online
 */
namespace CSO {

bool OgreApplication::initialiseRTShaderSystem() {
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  if (Ogre::RTShader::ShaderGenerator::initialize()) {
    mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();

    // Create and register the material manager listener if it doesn't exist
    // yet.
    if (!mMaterialMgrListener) {
      mMaterialMgrListener = new SGTechniqueResolverListener(mShaderGenerator);
      Ogre::MaterialManager::getSingleton().addListener(mMaterialMgrListener);
    }

    return true;
  }
#endif
  return false;
}

void OgreApplication::setRTSSWriteShadersToDisk(bool write) {
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  if (!write) {
    mShaderGenerator->setShaderCachePath("");
    return;
  }

  // Set shader cache path.
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
  auto subdir = "";
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
  auto subdir = "org.ogre3d.RTShaderCache/";
#else
  auto subdir = "RTShaderCache";
#endif
  boost::filesystem::path programPath = boost::dll::program_location();
  boost::filesystem::path path = programPath.parent_path() / subdir;
  if (!boost::filesystem::is_directory(path)) {
    boost::filesystem::create_directory(path);
  }
  mShaderGenerator->setShaderCachePath(path.string());
#endif
}

void OgreApplication::destroyRTShaderSystem() {
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  // mShaderGenerator->removeAllShaderBasedTechniques();
  // mShaderGenerator->flushShaderCache();

  // Restore default scheme.
  Ogre::MaterialManager::getSingleton().setActiveScheme(
      Ogre::MaterialManager::DEFAULT_SCHEME_NAME);

  // Unregister the material manager listener.
  if (mMaterialMgrListener != NULL) {
    Ogre::MaterialManager::getSingleton().removeListener(mMaterialMgrListener);
    delete mMaterialMgrListener;
    mMaterialMgrListener = NULL;
  }

  // Destroy RTShader system.
  if (mShaderGenerator != NULL) {
    Ogre::RTShader::ShaderGenerator::destroy();
    mShaderGenerator = NULL;
  }
#endif
}

void OgreApplication::setup() {
  locateResources();
  initialiseRTShaderSystem();
  loadResources();
}

void OgreApplication::locateResources() {
  boost::filesystem::path programPath = boost::dll::program_location();
  boost::filesystem::path resourceConfigPath =
      programPath.parent_path() / "resources.cfg";
  std::cout << "SceneMenu::setupResources resourceConfigPath="
            << resourceConfigPath << std::endl;

  Ogre::ConfigFile cf;
  cf.load(resourceConfigPath.string());

  Ogre::String secName, typeName;
  Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();

  // Ogre::ConfigFile::SettingsBySection;

  while (secIt.hasMoreElements()) {
    secName = secIt.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = secIt.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator setIt;

    for (setIt = settings->begin(); setIt != settings->end(); ++setIt) {
      typeName = setIt->first;
      // make paths relative to the executable
      boost::filesystem::path resourceDirPath =
          programPath.parent_path() / setIt->second;
      std::cout << "SceneMenu::setupResources archName=" << resourceDirPath
                << std::endl;
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
          resourceDirPath.string(), typeName, secName);
    }
  }
}

void OgreApplication::loadResources() {
  try {
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
  } catch (std::exception e) {
    std::cout << e.what() << std::endl;
    // Some material or may be even an unrelated scripts crashed the engine.
    // Load the model plain simple, no materials. Only skeleton and mesh.
  }
}

void OgreApplication::addSceneManagerToRTShader(
    Ogre::SceneManager *sceneManager) {
  mShaderGenerator->addSceneManager(sceneManager);
}

} // namespace CSO
