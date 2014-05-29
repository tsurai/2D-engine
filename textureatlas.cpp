#include "textureatlas.h"
#include "debug.h"
#include <GL/gl.h>

// TODO: decide for one error syntax. its with file and line here REMOVE before release
namespace Tsuki {
    TextureAtlas::TextureAtlas(const size_t width, const size_t height, size_t depth) {
        if((depth != 1) && (depth != 3) && (depth != 4))
            fatal("[%s:%d] Invalid pixel depth (%d)", __FILE__, __LINE__, depth);

        glid = 0;
        this->width = width;
        this->height = height;
        this->depth = depth;

        /* init skyline algorithm
         * 1,1,widthprint-2 for an 1px border in the atlas */
        used = 0;
        skyline.clear();
        skyline_node_t *node = (skyline_node_t *)malloc(sizeof(skyline_node_t));
        node->x = 1;
        node->y = 1;
        node->width = width-2;
        skyline.push_back(node);

        data = (unsigned char *)calloc(width*height*depth, sizeof(unsigned char));

        if(!data)
            fatal("[%s:%d] Not enough free memory to allocate %d bytes", __FILE__, __LINE__, width*height*depth);
    }

    TextureAtlas::~TextureAtlas() {
        while(!skyline.empty()) {
            free(skyline.back());
            skyline.pop_back();
        }
        skyline.clear();

        free(data);

        if(!glid)
            glDeleteTextures(1, &glid);
    }

    rect_t TextureAtlas::getRect(int rectWidth, int rectHeight) {
        int y, bestHeight, bestWidth, bestIndex;
        skyline_node_t *node, *prev;
        rect_t region = { 0, 0, rectWidth, rectHeight };

        bestWidth = 99999;
        bestHeight = 99999;
        bestIndex = -1;

        for(unsigned int i = 0; i < skyline.size(); ++i) {
            y = fits(rectWidth, rectHeight, i);
            if(y >= 0) {
                node = skyline[i];
                if(((y + rectHeight) < bestHeight) || (((y + rectHeight) == bestHeight) && (node->width < bestWidth))) {
                    bestHeight = y + rectHeight;
                    bestIndex = i;
                    bestWidth = node->width;
                    region.x = node->x;
                    region.y = y;
                }
            }
        }

        if(bestIndex == -1) {
            region.x = -1;
            region.y = -1;
            region.width = 0;
            region.height = 0;
            return region;
        }

        node = (skyline_node_t *)malloc(sizeof(skyline_node_t));
        if(node == NULL)
            fatal("[%s:%d] Not enough memory to allocate node", __FILE__, __LINE__);

        node->x = region.x;
        node->y = region.y + rectHeight;
        node->width = rectWidth;

        skyline.insert(skyline.begin()+bestIndex, node);

        for(unsigned int i = bestIndex+1; i < skyline.size(); ++i) {
            node = skyline[i];
            prev = skyline[i-1];
            if(node->x < (prev->x + prev->width)) {
                int shrink = prev->x + prev->width - node->x;
                node->x += shrink;
                node->width -= shrink;
                if (node->width <= 0) {
                    free(skyline.at(i));
                    skyline.erase(skyline.begin()+i);
                    --i;
                } else
                    break;
            } else
                break;
        }

        merge();
        used += rectWidth * rectHeight;

        return region;
    }

    void TextureAtlas::setRect(int x, int y, int rectWidth, int rectHeight, const unsigned char *data, int stride) {
        ASSERT((x > 0) && (y > 0));
        ASSERT(x < (width-1));
        ASSERT(x+rectWidth <= (width-1));
        ASSERT(y < (height-1));
        ASSERT(y+rectHeight <= (height-1));

        for(int i = 0; i < rectHeight; ++i) {
            memcpy(this->data+((y+i)*this->width + x) * sizeof(char) * depth, data + (i*stride) * sizeof(char), rectWidth * sizeof(char) * depth);
        }
    }

    void TextureAtlas::unsetRect(int x, int y, int rectWidth, int rectHeight) {
        for(int i = 0; i < rectHeight; ++i) {
            memset(this->data+((y+i)*this->width + x) * sizeof(char) * depth, 0, rectWidth * sizeof(char) * depth);
        }
    }

    int TextureAtlas::fits(int rectWidth, int rectHeight, int nodeIndex) {
        skyline_node_t *node = skyline[nodeIndex];
        int x = node->x, y, widthLeft = rectWidth;
        int i = nodeIndex;

        if((x + rectWidth) > (this->width-1))
            return -1;

        y = node->y;

        while(widthLeft > 0) {
            node = skyline[i];
            if(node->y > y)
                y = node->y;
            if((y + rectHeight) > (this->height-1))
                return -1;

            widthLeft -= node->width;
            ++i;
        }
        return y;
    }

    void TextureAtlas::merge() {
        skyline_node_t *node, *next;

        for(size_t i = 0; i < skyline.size()-1; ++i) {
            node = skyline[i];
            next = skyline[i+1];
            if(node->y == next->y) {
                node->width += next->width;
                free(skyline.at(i+1));
                skyline.erase(skyline.begin()+i+1);
                --i;
            }
        }
    }

    // TODO: get texture depth working LOWPRIO
    void TextureAtlas::upload() {
        ASSERT(data);

        if(!glid)
            glGenTextures(1, &glid);

        glBindTexture(GL_TEXTURE_2D, glid);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        if(depth == 4) {
    #ifdef GL_UNSIGNED_INT_8_8_8_8_REV
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, data);
    #else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    #endif
        } else if(depth == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
    }

    void TextureAtlas::reset() {
        used = 0;
        skyline.clear();
        skyline_node_t *node = (skyline_node_t *)malloc(sizeof(skyline_node_t));
        node->x = 1;
        node->y = 1;
        node->width = width-2;
        skyline.push_back(node);

        memset(data, 0, width*height*depth);
    }

    void TextureAtlas::createDistanceMap() {

    }
}
