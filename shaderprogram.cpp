#include "shaderprogram.h"
#include "debug.h"

namespace Tsuki {
    ShaderProgram::ShaderProgram() : mId(0), mIsLinked(false), mIsBound(false) {
        mId = glCreateProgram();
    }

    ShaderProgram::~ShaderProgram() {
        release();
    }

    int ShaderProgram::addShaderFromFile(const char *filename, GLenum type) {
        FILE *f = fopen(filename, "rb");
        if(f == NULL) {
            //warning("Shader file %s doesn't exit", filename);
            return 0;
        }
        fseek(f, 0, SEEK_END);
        unsigned int size = ftell(f);
        rewind(f);

        char *src = (char *)malloc(sizeof(char)*(size+1));

        unsigned int ret = 0;
        if((ret = fread(src, 1, size, f)) != size) {
            warning("couldn't read whole shader file %d %d", size, ret);
            return 0;
        }
        fclose(f);

        src[size] = '\0';

        GLuint shaderId = 0;
        shaderId = glCreateShader(type);
        glShaderSource(shaderId, 1, (const GLchar **)&src, NULL);
        glCompileShader(shaderId);

        int successful = false;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &successful);
        if(!successful) {
            warning("Unable to compile shader %d", filename);
            printShaderLog(shaderId);
            glDeleteShader(shaderId);
            return 0;
        }

        GLenum error = glGetError();
        glAttachShader(mId, shaderId);
        if(error != GL_NO_ERROR) {
            warning("Error linking shader! %s", gluErrorString(error));
            glDeleteShader(shaderId);
            return 0;
        }

        free(src);

        return shaderId;
    }

    bool ShaderProgram::link() {
        if(mIsLinked)
            return false;

        glLinkProgram(mId);

        //Check for errors
        int successful = false;
        glGetProgramiv(mId, GL_LINK_STATUS, &successful);
        if(!successful) {
            //warning("Failed to link program %d", mId);
            return false;
        }

        mIsLinked = true;
        return true;
    }

    bool ShaderProgram::bind() {
        if(mIsBound)
            return true;

        glUseProgram(mId);

        GLenum error = glGetError();
        if(error != GL_NO_ERROR) {
            //warning("Error binding shader! %s", gluErrorString(error));
            glUseProgram(0);
            return false;
        }
        mIsBound = true;
        return true;
    }

    void ShaderProgram::unbind() {
        glUseProgram(0);
        mIsBound = false;
    }

    int ShaderProgram::getUniformLocation(const char *name) {
        if(name == NULL)
            return -1;

        if(!mIsLinked) {
            warning("Can't get Uniform Location. Program is not yet linked");
            return -1;
        }

        return glGetUniformLocation(mId, name);
    }

    int ShaderProgram::getAttributeLocation(const char *name) {
        if(name == NULL)
            return -1;

        if(!mIsLinked) {
            warning("Can't get Uniform Location. Program is not yet linked");
            return -1;
        }

        return glGetAttribLocation(mId, name);
    }

    GLuint ShaderProgram::getId() {
        return mId;
    }

    bool ShaderProgram::isBound() {
        return mIsBound;
    }

    void ShaderProgram::release() {
        unbind();
        glDeleteProgram(mId);
    }

    void ShaderProgram::printShaderLog(GLuint sId) {
        if(glIsShader(sId)) {
            int strLength, maxLength = 0;

            glGetShaderiv(sId, GL_INFO_LOG_LENGTH, &maxLength);
            char* infoLog = new char[maxLength];
            glGetShaderInfoLog(sId, maxLength, &strLength, infoLog);

            if(strLength > 0) {
                infoLog[strLength-1] = '\0';
                // TODO: causes _debug to crash
                warning("InfoLog: %s", infoLog);
            }
            delete[] infoLog;
        } else {
            warning("Name %d is not a shader", sId);
        }
    }
}
