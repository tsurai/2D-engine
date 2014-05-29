#ifndef GLWIDGET_H
#define GLWIDGET_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <QGLWidget>
#include <QTimer>
#include <QTime>
#include "graph.h"
#include "fontengine/fontengine.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    GLWidget();
    ~GLWidget();

    READ_ONLY(int, cWidth)
    READ_ONLY(int, cHeight)
    READ_ONLY(int, cXoffset)
    READ_ONLY(int, cYoffset)

    void drawGL();
    bool setVsync(bool enable);
    bool setForcedAspectRatio(int w=0, int h=0);
    void setMouseLock(bool enable);
    void forceAspectRatio(bool isForced);
    void changeResolution(int width, int height);
private:
    Tsuki::Font *font;

    int fps;
    int frames;
    QTimer fpsTimer;
    bool isAspectRatioForced;
    bool isVsyncEnabled;
    bool isMouseLocked;
    int ratioW, ratioH;
    //needed when switching back from fullscreen mode
    int windowedWidth, windowedHeight;

    Graph *graph;
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    //void paintGL();

    void keyPressEvent(QKeyEvent *);
    void changeEvent(QEvent *);
    void closeEvent(QCloseEvent *);
signals:
    void closed();
public slots:
    void setFps();
    
};

#endif // GLWIDGET_H
