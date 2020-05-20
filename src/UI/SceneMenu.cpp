/*
 * @name Crazy Sentences Online
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief A game for ESL students to learn and have fun!
 */

// c++17
#include <fstream>
#include <iostream>

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
  std::cout << "SceneMenu::initialize" << std::endl;

  Ogre::SceneManager *m_sceneManager = ogreRoot_->getSceneManager("menu");

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

} // namespace CSO
