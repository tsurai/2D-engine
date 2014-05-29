#include "graph.h"
#include <GL/gl.h>
#include <QDebug>
#include "fontengine/fontengine.h"

using Tsuki::FontEngine;

Graph::Graph(const char *desc, unsigned short nChannel, int offsetx, int offsety, int width, int height, int scalex, int scaley, int cachesize) {
    this->desc = desc;
    this->nChannel = nChannel;
    this->x = offsetx;
    this->y = offsety+1;
    this->width = width;
    this->height = height;
    this->scalex = scalex;
    this->scaley = scaley;
    this->cachesize = cachesize;
    this->maxvalue = 1;
    for(unsigned short i = 0; i < nChannel; ++i)
        this->data.push_back(std::list<unsigned long>());

    font = FontEngine::instance()->newFont("./data/fonts/OpenSans-Regular.ttf", 12);
}

void Graph::update(unsigned short channel, unsigned long value) {
    if(data[channel].size() >= width) {
        /* get new max value - kinda inefficient*/
        if(data[channel].front() >= maxvalue) {
            maxvalue = 0;
            std::list<unsigned long>::iterator it;
            for(it = ++data[channel].begin(); it != data[channel].end(); it++)
                if(*it > maxvalue)
                    maxvalue = *it;
        }
        data[channel].pop_front();
    }

    data[channel].push_back(value);

    if(maxvalue <= value)
        maxvalue = value;
}

void Graph::draw() {
    glDisable(GL_TEXTURE_2D);

    /* draw bounding box */
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2i(x, y);
        glVertex2i(x, y+height);
        glVertex2i(x+width, y+height);
        glVertex2i(x+width, y);
        glVertex2i(x, y);
    glEnd();

    /* guide line */
    glEnable(GL_LINE_STIPPLE);
    glLineStipple (3, 0xAAAA);
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINE_STRIP);
        glVertex2i(x, y+height/2);
        glVertex2i(x+width, y+height/2);
    glEnd();

    glDisable(GL_LINE_STIPPLE);

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for(unsigned short i = 0; i < nChannel; ++i) {
        int posx = this->x;
        double scale = (double)height/(double)(maxvalue);
        std::list<unsigned long>::iterator it;
        for(it = data[i].begin(); it != data[i].end(); it++) {
            glVertex2i(posx, y+height-*it*scale);
            posx++;
        }
    }
    glEnd();

    /* legend */
    FontEngine::instance()->wprint(x+width+5, y, font, 12, FontEngine::OriginTopLeft, L"%ld ns", maxvalue);
    FontEngine::instance()->wprint(x+width+5, y+height/2, font, 12, FontEngine::OriginBaseLeft, L"%ld ns", (maxvalue)/2);
}
