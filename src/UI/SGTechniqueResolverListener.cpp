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
#include "SGTechniqueResolverListener.hpp"

/*
 * CSO = Crazy Sentences Online
 */
namespace CSO {

SGTechniqueResolverListener::SGTechniqueResolverListener(Ogre::RTShader::ShaderGenerator *pShaderGenerator)
{
    mShaderGenerator = pShaderGenerator;
}

/** This is the hook point where shader based technique will be created.
It will be called whenever the material manager won't find appropriate technique
that satisfy the target scheme name. If the scheme name is out target RT Shader
System scheme name we will try to create shader generated technique for it.
*/
Ogre::Technique *SGTechniqueResolverListener::handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String &schemeName, Ogre::Material *originalMaterial, unsigned short lodIndex, const Ogre::Renderable *rend) {
    if (!mShaderGenerator->hasRenderState(schemeName))
    {
        return NULL;
    }
    // Case this is the default shader generator scheme.

    // Create shader generated technique for this material.
    bool techniqueCreated = mShaderGenerator->createShaderBasedTechnique(
                *originalMaterial,
                Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
                schemeName);

    if (!techniqueCreated)
    {
        return NULL;
    }
    // Case technique registration succeeded.

    // Force creating the shaders for the generated technique.
    mShaderGenerator->validateMaterial(schemeName, originalMaterial->getName(), originalMaterial->getGroup());

    // Grab the generated technique.
    Ogre::Material::Techniques::const_iterator it;
    for(it = originalMaterial->getTechniques().begin(); it != originalMaterial->getTechniques().end(); ++it)
    {
        Ogre::Technique* curTech = *it;

        if (curTech->getSchemeName() == schemeName)
        {
            return curTech;
        }
    }

    return NULL;
}

bool SGTechniqueResolverListener::afterIlluminationPassesCreated(Ogre::Technique *tech)
{
    if(mShaderGenerator->hasRenderState(tech->getSchemeName()))
    {
        Ogre::Material* mat = tech->getParent();
        mShaderGenerator->validateMaterialIlluminationPasses(tech->getSchemeName(),
                                                             mat->getName(), mat->getGroup());
        return true;
    }
    return false;
}

bool SGTechniqueResolverListener::beforeIlluminationPassesCleared(Ogre::Technique *tech)
{
    if(mShaderGenerator->hasRenderState(tech->getSchemeName()))
    {
        Ogre::Material* mat = tech->getParent();
        mShaderGenerator->invalidateMaterialIlluminationPasses(tech->getSchemeName(),
                                                               mat->getName(), mat->getGroup());
        return true;
    }
    return false;
}

} // namespace CSO
