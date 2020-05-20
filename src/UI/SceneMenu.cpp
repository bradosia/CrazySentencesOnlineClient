/*
 * @name Crazy Sentences Online
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief A game for ESL students to learn and have fun!
 */

// Local Project
#include "SceneMenu.hpp"

/*
 * CSO = Crazy Sentences Online
 */
namespace CSO {

SceneMenu::SceneMenu() {}
SceneMenu::~SceneMenu() {}

bool SceneMenu::initialize(std::shared_ptr<Ogre::Root> ogreRoot_,
                           Ogre::RenderWindow *ogreWindow_) {
  setupResources();

  try {
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
  } catch (std::exception e) {
    std::cout << e.what() << std::endl;
    // Some material or may be even an unrelated scripts crashed the engine.
    // Load the model plain simple, no materials. Only skeleton and mesh.
  }

  // chooseSceneManager
  Ogre::SceneManager *m_sceneManager =
      ogreRoot_->createSceneManager("DefaultSceneManager", "menu");

  // initialize real time shader system
  if (!initializeRTShaderSystem(m_sceneManager)) {
    return false;
  }

  // createCamera()
  mCamera = m_sceneManager->createCamera("PlayerCam");

  mCameraNode =
      m_sceneManager->getRootSceneNode()->createChildSceneNode("Camera Node 0");
  // mCameraNode = mCameraNode->createChildSceneNode("Camera Node");
  mCameraNode->attachObject(mCamera);
  mCameraNode->pitch(Ogre::Degree(-45.0f));
  // mCameraNode->setPosition(Ogre::Vector3(0, 0, 0));

  mCameraTargetNode = m_sceneManager->getRootSceneNode()->createChildSceneNode(
      "Camera Target Node 0");
  mCameraTargetNode->setPosition(Ogre::Vector3(0, 0, 0));

  mCamera->setPosition(Ogre::Vector3(45, 20, 20));
  mCamera->lookAt(Ogre::Vector3(0, 0, 0));
  mCamera->setNearClipDistance(5);

  // create viewports
  Ogre::Viewport *vp = ogreWindow_->addViewport(mCamera);
  vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
  // When a viewport is already created
  vp->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

  mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) /
                          Ogre::Real(vp->getActualHeight()));

  ogreRoot_->getRenderSystem()->_initRenderTargets();
  ogreRoot_->clearEventTimes();

  // textures
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

  // createResourceListener();
  // loadResources();

  // createScene()
  m_sceneManager->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));

  mMeshEntity = m_sceneManager->createEntity("01_pmx.mesh");
  mNode = m_sceneManager->getRootSceneNode()->createChildSceneNode(
      Ogre::Vector3(0, 0, 0));
  mNode->attachObject(mMeshEntity);

  // mMeshEntity->setDisplaySkeleton(true);
  Ogre::SkeletonInstance *skeleton = mMeshEntity->getSkeleton();
  for (auto bone : skeleton->getBones()) {
    std::cout << "bone #" << bone->getHandle() << ": " << bone->getName()
              << std::endl;
  }

  Ogre::Entity *ogEntity;
  Ogre::Entity *ogEntity2;
  try {
      /* createEntity() will load the resource into memory
       * if it does not exist it will throw an exception
       * check the resource exists before creating the entity
       */
    ogEntity = m_sceneManager->createEntity("ogrehead.mesh");
    Ogre::SceneNode *ogNode =
        m_sceneManager->getRootSceneNode()->createChildSceneNode(
            Ogre::Vector3(22, 14, 0));
    ogNode->attachObject(ogEntity);
    ogNode->setScale(0.2, 0.2, 0.2);

    ogEntity2 = m_sceneManager->createEntity("ogrehead.mesh");
    Ogre::SceneNode *ogNode2 =
        m_sceneManager->getRootSceneNode()->createChildSceneNode(
            Ogre::Vector3(-8, 15, 0));
    ogNode2->attachObject(ogEntity2);
    ogNode2->setScale(0.2, 0.2, 0.2);
  } catch (Ogre::Exception e) {
    std::cout << "ground load exception: " << e.what() << std::endl;
  }

  //! [pointlight]
  Ogre::Light *pointLight = m_sceneManager->createLight("PointLight");
  pointLight->setType(Ogre::Light::LT_POINT);
  //! [pointlight]

  //! [pointlightcolor]
  pointLight->setDiffuseColour(0.3, 0.3, 0.3);
  pointLight->setSpecularColour(0.3, 0.3, 0.3);
  //! [pointlightcolor]

  //! [pointlightpos]
  Ogre::SceneNode *pointLightNode =
      m_sceneManager->getRootSceneNode()->createChildSceneNode();
  pointLightNode->attachObject(pointLight);
  pointLightNode->setPosition(Ogre::Vector3(0, 25, 25));
  //! [pointlightpos]
  return true;
}

bool SceneMenu::initializeRTShaderSystem(Ogre::SceneManager *sceneMgr) {
#if CSO_OGRE_REAL_TIME_SHADER_ENABLE
  if (!Ogre::RTShader::ShaderGenerator::initialize())
    return false;

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

  Ogre::StringVector groupVector =
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
    Ogre::RTShader::ShaderGenerator::destroy();
    return false;
  }

  std::string path = "RTShaderCache/";
  boost::filesystem::path dir(path);
  if (boost::filesystem::create_directory(dir)) {
    std::cout << "Directory Created: " << dir << std::endl;
  }

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
void SceneMenu::finalizeRTShaderSystem() {
#if CSO_OGRE_REAL_TIME_SHADER_ENABLE
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
#endif
}

bool SceneMenu::setupResources() {
  boost::filesystem::path programPath = boost::dll::program_location();
  boost::filesystem::path resourceConfigPath =
      programPath.parent_path() / "resources.cfg";
  std::cout << "SceneMenu::setupResources resourceConfigPath="
            << resourceConfigPath << std::endl;

  Ogre::ConfigFile cf;
  cf.load(resourceConfigPath.string());

  Ogre::String secName, typeName;
  Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();

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
  return true;
}

//-----------------------------------------------------------------------------
// defaultCamera()
// Description:
//	Defaults camera position to 0, centerY, 0, with the default angle,
//	making it look to the model as a whole
// Notes:
//	1) If no model is loaded, it redirects to originCamera()
//-----------------------------------------------------------------------------
void SceneMenu::defaultCamera() {
  if (mMeshEntity) {
    Ogre::Vector3 vCenter =
        Ogre::Vector3(0.0f,
                      (mMeshEntity->getBoundingBox().getMaximum().y +
                       mMeshEntity->getBoundingBox().getMinimum().y) *
                          0.5f,
                      0.0f);
    /*Ogre::Vector3 vCenter = (mMeshEntity->getBoundingBox().getMaximum() +
                             mMeshEntity->getBoundingBox().getMinimum()) *
    0.5f; mCamera->setPosition( vCenter + Ogre::Vector3( 0, 1.5f, 1.5f) *
    mMeshEntity->getBoundingRadius() ); mCamera->lookAt( vCenter );*/
    mCamera->setPosition(Ogre::Vector3::UNIT_Z *
                         mMeshEntity->getBoundingRadius() * 1.8f);
    mCameraNode->setOrientation(Ogre::Quaternion::IDENTITY);
    mCameraNode->pitch(Ogre::Degree(-45.0f));
    mCameraNode->getParent()->setPosition(vCenter);
    mCameraNode->setPosition(Ogre::Vector3::ZERO);
  } else
    originCamera();
}

//-----------------------------------------------------------------------------
// originCamera()
// Description:
//	Defaults camera position to 0, 0, 0, with the default angle
//-----------------------------------------------------------------------------
void SceneMenu::originCamera() {
  float factor = 10.0f;
  if (mMeshEntity)
    factor = mMeshEntity->getBoundingRadius() * 1.8f;

  mCamera->setPosition(Ogre::Vector3::UNIT_Z * factor);
  mCameraNode->setOrientation(Ogre::Quaternion::IDENTITY);
  mCameraNode->pitch(Ogre::Degree(-45.0f));
  mCameraNode->getParent()->setPosition(Ogre::Vector3::ZERO);
  mCameraNode->setPosition(Ogre::Vector3::ZERO);
}

//-----------------------------------------------------------------------------
// centerMeshCamera()
// Description:
//	Like defaultCamera(), but pointing at the exact center of the mesh
// Notes:
//	1) Does nothing if the mesh isn't loaded
//-----------------------------------------------------------------------------
void SceneMenu::centerMeshCamera() {
  if (mMeshEntity) {
    const Ogre::Vector3 vCenter = (mMeshEntity->getBoundingBox().getMaximum() +
                                   mMeshEntity->getBoundingBox().getMinimum()) *
                                  0.5f;
    mCamera->setPosition(Ogre::Vector3::UNIT_Z *
                         mMeshEntity->getBoundingRadius() * 1.8f);
    mCameraNode->setOrientation(Ogre::Quaternion::IDENTITY);
    mCameraNode->pitch(Ogre::Degree(-45.0f));
    mCameraNode->getParent()->setPosition(vCenter);
    mCameraNode->setPosition(Ogre::Vector3::ZERO);
  }
}

//-----------------------------------------------------------------------------
// rotateCamera()
// Style same as Ogre::CameraMan::Orbit
//-----------------------------------------------------------------------------
void SceneMenu::rotateCamera(int x, int y) {
  Ogre::Real dist =
      (mCamera->getPosition() - mNode->_getDerivedPosition()).length();

  // mCamera->setPosition(mNode->_getDerivedPosition());

  // mCamera->yaw(Ogre::Degree(x * 0.25f));
  // mCamera->pitch(Ogre::Degree(y * 0.25f));

  // mCamera->moveRelative(Ogre::Vector3(0, 0, dist));

  mCameraNode->yaw(Ogre::Degree(-x * 0.4f), Ogre::Node::TS_PARENT);
  mCameraNode->pitch(Ogre::Degree(-y * 0.4f));
}

//-----------------------------------------------------------------------------
// zoomInCamera()
// Description:
//
// Input:
//	1) Z delta
//-----------------------------------------------------------------------------
void SceneMenu::zoomInCamera(Ogre::Real wheelDelta) {
  Ogre::Real dist =
      (mCamera->getPosition() - mCameraTargetNode->_getDerivedPosition())
          .length();
  mCamera->moveRelative(Ogre::Vector3(0, 0, -wheelDelta * 0.018f * dist));
}

//-----------------------------------------------------------------------------
// moveCamera()
// Description:
//	Moves the whole camera among the relative X and Y axis
// Input:
//	1) X delta, in pixels
//	2) Y delta, in pixels
//-----------------------------------------------------------------------------
void SceneMenu::moveCamera(int x, int y) {
  if (mMeshEntity) {
    Ogre::Vector3 vTrans(-x, y, 0);
    vTrans *= mMeshEntity->getBoundingRadius() * 0.005f;
    mCameraTargetNode->translate(vTrans, Ogre::Node::TS_LOCAL);
    mCameraNode->translate(vTrans, Ogre::Node::TS_LOCAL);
    mCamera->lookAt(mCameraTargetNode->getPosition());
  }
}

//-----------------------------------------------------------------------------
// slideCamera()
// Description:
//	Moves the whole camera among the relative X and Z axis
// Notes:
//	1) slideCamera( x, 0 ) == moveCamera( x, 0 )
// Input:
//	1) X delta, in pixels
//	2) Z delta, in pixels
//-----------------------------------------------------------------------------
void SceneMenu::slideCamera(int x, int z) {
  if (mMeshEntity) {
    Ogre::Vector3 vTrans(-x, 0, -z);
    vTrans *= mMeshEntity->getBoundingRadius() * 0.005f;
    mCameraNode->translate(vTrans, Ogre::Node::TS_LOCAL);
  }
}

#if CSO_OGRE_REAL_TIME_SHADER_ENABLE
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
      Ogre::Material::TechniqueIterator itTech =
          originalMaterial->getTechniqueIterator();
      while (itTech.hasMoreElements()) {
        Ogre::Technique *curTech = itTech.getNext();

        if (curTech->getSchemeName() == schemeName) {
          generatedTech = curTech;
          break;
        }
      }
    }
  }

  return generatedTech;
}
#endif

} // namespace CSO
