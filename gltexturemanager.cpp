#include "gltexturemanager.h"
#include "engine.h"

GLTextureManager::GLTextureManager()
{
}

GLuint GLTextureManager::load(QString filename) {
    return Engine::instance()->getWindow()->bindTexture(filename, GL_TEXTURE_2D, GL_RGBA );
}
