#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "prerequisites.h"
#ifdef TSUKI_OS_WIN
    #define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/gl.h>

namespace Tsuki {
    class ShaderProgram
    {
        struct shader {
            int id;
            const char *file;
            GLenum type;
        };

    public:
        ShaderProgram();
        ~ShaderProgram();

        int addShaderFromFile(const char *filename, GLenum type);

        bool link();
        bool bind();
        void unbind();

        int getUniformLocation(const char *name);
        int getAttributeLocation(const char *name);

        GLuint getId();
        bool isBound();
        void release();
    protected:
        void printShaderLog(GLuint sId);

        GLuint mId;
        bool mIsLinked;
        bool mIsBound;
    };
}

#endif // SHADERPROGRAM_H
