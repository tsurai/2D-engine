#include "tilemap.h"
#include "engine.h"
#include "glwindow.h"
#include <GL/glfw.h>
#include "imageformat.h"
#include "shaderprogram.h"
#include "debug.h"

namespace Tsuki {
    Tilemap::Tilemap() {

        const char *imagepath = "data/tile2.bmp";

        // Data read from the header of the BMP file
        unsigned char header[54]; // Each BMP file begins by a 54-bytes header
        unsigned int dataPos;     // Position in the file where the actual data begins
        unsigned int width, height;
        unsigned int imageSize;   // = width*height*3
        // Actual RGB data
        unsigned char * data;

        // Open the file
        FILE * file = fopen(imagepath,"rb");
        if (!file) {
            printf("Image could not be opened\n");
            return;
        }

        if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
            printf("Not a correct BMP file\n");
            return;
        }

        if ( header[0]!='B' || header[1]!='M' ){
            printf("Not a correct BMP file\n");
            return;
        }

        // Read ints from the byte array
        dataPos    = *(int*)&(header[0x0A]);
        imageSize  = *(int*)&(header[0x22]);
        width      = *(int*)&(header[0x12]);
        height     = *(int*)&(header[0x16]);

        // Some BMP files are misformatted, guess missing information
        if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
        if (dataPos==0)      dataPos=54; // The BMP header is done that

        // Create a buffer
        data = new unsigned char [imageSize];

        // Read the actual data from the file into the buffer
        fread(data,1,imageSize,file);

        //Everything is in memory now, the file can be closed
        fclose(file);

        // Create one OpenGL texture
        glGenTextures(1, &texId);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, texId);

        // Give the image to OpenGL
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        //Bitmap *bmp = new Bitmap();
        //bmp->load("./data/tile2.bmp");
        //texId = bmp->createTexture();

        pShaderProg = new ShaderProgram();
        pShaderProg->addShaderFromFile("./data/shader/tile.vert", GL_VERTEX_SHADER);
        pShaderProg->addShaderFromFile("./data/shader/tile.frag", GL_FRAGMENT_SHADER);
        pShaderProg->link();

        timeLoc = pShaderProg->getUniformLocation("uTime");
        posLoc = pShaderProg->getAttributeLocation("aPosition");
        texLoc = pShaderProg->getAttributeLocation("aTexCoord");
    }

    Tilemap::~Tilemap() {
        delete pShaderProg;
        free(tiles);
    }

    void Tilemap::loadMap() {
        this->width = ceil((float)Engine::instance()->getWindow()->cWidth()/64.0);
        this->height = ceil((float)Engine::instance()->getWindow()->cHeight()/64.0);

        this->tiles = (tile_t *)malloc(sizeof(tile_t)*width*height);

        int i = 0;
        for(int y = 0; y < this->height; y++) {
            for(int x = 0; x < this->width; x++) {
                tiles[y*width+x].aniIndex = -1;
                tiles[y*width+x].texture[0] = ++i%3;
            }
        }
    }

    void Tilemap::draw(float time) {
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

        glBindTexture(GL_TEXTURE_2D, texId);

        this->tilesize_w = 64;
        this->tilesize_h = 64;

        unsigned short *indices = (unsigned short *)malloc(sizeof(unsigned short)*(height*width*6));
        unsigned short *vertices = (unsigned short *)malloc(sizeof(unsigned short)*(height*width*8));
        float *texVertices = (float *)malloc(sizeof(float)*(height*width*8));

        int m = 0;
        int n = 0;
        int indOffset = 0;
        for(int y = 0; y < this->height; y++) {
            for(int x = 0; x < this->width; x++) {
                vertices[n] = x*tilesize_w;
                texVertices[n++] = 0.0f;
                vertices[n] = y*tilesize_h;
                texVertices[n++] = 0.0f;
                vertices[n] = x*tilesize_w;
                texVertices[n++] = 0.0f;
                vertices[n] = y*tilesize_h+tilesize_h;
                texVertices[n++] = 1.0f;
                vertices[n] = x*tilesize_w+tilesize_w;
                texVertices[n++] = 1.0f;
                vertices[n] = y*tilesize_h+tilesize_h;
                texVertices[n++] = 1.0f;
                vertices[n] = x*tilesize_w+tilesize_w;
                texVertices[n++] = 1.0f;
                vertices[n] = y*tilesize_h;
                texVertices[n++] = 0.0f;

                indices[m++] = indOffset;
                indices[m++] = indOffset+1;
                indices[m++] = indOffset+2;
                indices[m++] = indOffset;
                indices[m++] = indOffset+2;
                indices[m++] = indOffset+3;
                indOffset += 4;

            }
        }

        pShaderProg->bind();
        glUniform1f(timeLoc, time);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexAttribPointer(posLoc, 2, GL_SHORT, GL_FALSE, 0, vertices);
        glEnableVertexAttribArray(posLoc);

        glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 0, texVertices);
        glEnableVertexAttribArray(texLoc);

        glDrawElements(GL_TRIANGLES, width*height*6, GL_UNSIGNED_SHORT, indices);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        pShaderProg->unbind();

        free(vertices);
        free(texVertices);
    }

    void Tilemap::update() {
       /* for(int y = 0; y < this->height; y++) {
            for(int x = 0; x < this->width; x++) {
                tiles[y*width+x].aniIndex = aniIndex++%sizeof(texture));
            }
        }*/
    }
}
