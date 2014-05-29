#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <vector>
#include "fontengine/font.h"

class Graph
{
public:
    typedef struct {
        unsigned short r;
        unsigned short g;
        unsigned short b;
    } color_t;

    Graph(const char *desc, unsigned short nChannel, int offsetx, int offsety, int width, int height, int scalex, int scaley, int cachesize);
    void update(unsigned short channel, unsigned long value);
    void draw();
private:
    const char *desc;
    unsigned short nChannel;
    int x;
    int y;
    int width;
    int height;
    int scalex;
    int scaley;
    unsigned int cachesize;
    unsigned long maxvalue;
    color_t color;
    std::vector<std::list<unsigned long> > data;
    Tsuki::Font *font;
};

#endif // GRAPH_H
