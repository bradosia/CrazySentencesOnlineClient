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

/*
 * CSO = Crazy Sentences Online
 */
namespace CSO {

/** This class demonstrates basic usage of the RTShader system.
    It sub class the material manager listener class and when a target scheme callback
    is invoked with the shader generator scheme it tries to create an equivalent shader
    based technique based on the default technique of the given material.
*/
class SGTechniqueResolverListener : public Ogre::MaterialManager::Listener {
public:
    explicit SGTechniqueResolverListener(Ogre::RTShader::ShaderGenerator* pShaderGenerator);

    /** This is the hook point where shader based technique will be created.
        It will be called whenever the material manager won't find appropriate technique
        that satisfy the target scheme name. If the scheme name is out target RT Shader System
        scheme name we will try to create shader generated technique for it.
    */
    Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex,
                                          const Ogre::String& schemeName,
                                          Ogre::Material* originalMaterial, unsigned short lodIndex,
                                          const Ogre::Renderable* rend);

    bool afterIlluminationPassesCreated(Ogre::Technique* tech);

    bool beforeIlluminationPassesCleared(Ogre::Technique* tech);

protected:
    Ogre::RTShader::ShaderGenerator* mShaderGenerator; // The shader generator instance.
};

} // namespace CSO

#endif
// CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_RT_SHADER_H
