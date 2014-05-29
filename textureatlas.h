#ifndef TEXATLAS_H
#define TEXATLAS_H

#include "prerequisites.h"

// TODO: export as seperate font atlas tool

/* Implimentation of the Skyline back left algorithm */
namespace Tsuki {
    typedef struct {
        int x;
        int y;
        int width;
    } skyline_node_t;

    typedef struct {
        int x, y, width, height;
    } rect_t;

    class TextureAtlas {
    public:
        TextureAtlas(const size_t width, const size_t height, size_t depth);
        ~TextureAtlas();

        unsigned int glid;

        // TODO: too much public functions
        rect_t getRect(int rectWidth, int rectHeight);
        void setRect(int x, int y, int rectWidth, int rectHeight, const unsigned char *data, int stride);
        void unsetRect(int x, int y, int rectWidth, int rectHeight);
        void upload();
        void reset();
        void createDistanceMap();
        int getWidth() { return width; }
        int getHeight() { return height; }
        int getDepth() { return depth; }
        int getNodeCount() { return skyline.size(); }
    private:
        std::vector<skyline_node_t *> skyline;
        int width;
        int height;
        int depth;
        unsigned long used;
        unsigned char *data;

        int fits(int rectWidth, int rectHeight, int nodeIndex);
        void merge();
    };
}

#endif // TEXATLAS_H
