#ifndef RESSOURCEMANAGER_H
#define RESSOURCEMANAGER_H
#include <QHash>

template <class T> class ResourceManager {
public:
    ResourceManager() { }
    ~ResourceManager() { releaseAllResources(); }

    T *getResource(QString name) {
        T *resource = find(name);
        if(resource == NULL) {
            resource = load(name);
            if(resource != NULL)
                resources.insert(name, resource);
        }
        return resource;
    }

    void releaseResource(QString name) {
        T *resource = find(name);
        if(resource != NULL) {
            delete resource;
            resources.erase(resources.find(name));
        }
    }

    void releaseAllResources() {
        while(resources.begin() != resources.end()) {
            delete resources.begin().value();
            resources.erase(resources.begin());
        }
    }

private:
    QHash<QString, T> resources;

    // TODO: ressource manger is fucked up
    T *find(QString name) {
        T *resource = NULL;
        QHash<QString, T>::const_iterator i = resources.find(name);
        if(i == resources.end())
            resource = i.value();

        return resource;
    }

protected:
    virtual T *load(QString filename) = 0;
};

#endif // RESSOURCEMANAGER_H
