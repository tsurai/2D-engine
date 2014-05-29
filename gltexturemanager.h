#ifndef GLTEXTUREMANAGER_H
#define GLTEXTUREMANAGER_H

#include <QObject>
//#include "resourcemanager.h"

class GLTextureManager : public QObject/*: public ResourceManager */
{
    Q_OBJECT
public:
    explicit GLTextureManager();
    unsigned int load(QString filename);
signals:
    
public slots:
    
};

#endif // GLTEXTUREMANAGER_H
