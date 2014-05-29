#ifndef TILEMAP_H
#define TILEMAP_H

#include "prerequisites.h"
#include <stdint.h>

namespace Tsuki {
    typedef struct {
        int texture[];
        uint8_t aniIndex;
        int flags;
        double trans;
    } tile_t;

    class Tilemap {
    public:
        Tilemap();
        ~Tilemap();
        void loadMap();
        void draw(float time);
        void update();
    private:
        int timeLoc;
        int posLoc;
        int texLoc;
        ShaderProgram *pShaderProg;
        unsigned int texId;
        tile_t *tiles;
        int width, height;
        int tilesize_w, tilesize_h;
    };
}

#endif // TILEMAP_H
