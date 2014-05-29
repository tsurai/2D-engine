#include "profilemanager.h"
#include <algorithm>
#include <climits>

ProfileManager::ProfileManager() {
#ifdef Q_OS_WIN
    QueryPerformanceFrequency((LARGE_INTEGER *)&frequency);
#endif
    curFrame = 0;
}

/* override */
template<> ProfileManager* Singleton<ProfileManager>::m_instance = 0;
ProfileManager *ProfileManager::instance() {
    assert(m_instance);
    return m_instance;
}

void ProfileManager::newFrame() {
    (curFrame < MAX_FRAMES-1) ? ++curFrame : curFrame = 0;
    for(unsigned int i = 0; i < data.size(); ++i) {
        data[i]->perFrame[curFrame].totalCalls = 0;
        data[i]->perFrame[curFrame].totalTime = 0;
        data[i]->perFrame[curFrame].minTime = LONG_MAX;
        data[i]->perFrame[curFrame].maxTime = 0;
    }
}

void ProfileManager::profile(int category, unsigned long time) {
    data[category]->totalTimeMs += time / 1000000;
    ++data[category]->totalCalls;
    ++data[category]->perFrame[curFrame].totalCalls;
    data[category]->perFrame[curFrame].totalTime += time;
    data[category]->perFrame[curFrame].minTime = std::min(data[category]->perFrame[curFrame].minTime, time);
    data[category]->perFrame[curFrame].maxTime = std::max(data[category]->perFrame[curFrame].maxTime, time);
    data[category]->minTime = std::min(data[category]->perFrame[curFrame].minTime, data[category]->minTime);
    data[category]->maxTime = std::max(data[category]->perFrame[curFrame].maxTime, data[category]->maxTime);
}

int ProfileManager::registerCategory(const char *description) {
    if(data.size() == MAX_CATEGORIES)
        return -1;

    ProfileData *newdata = new ProfileData();
    newdata->desc = description;
    newdata->totalCalls = 0;
    newdata->totalTimeMs = 0;
    newdata->minTime = LONG_MAX;
    newdata->maxTime = 0;
    data.push_back(newdata);

    return data.size();
}

ProfileData *ProfileManager::getProfileData(int category) {
    return data[category];
}

unsigned long ProfileManager::getLastFrameTime(int category) {
    if(data.size() < 2)
        return 0;

    return data[category]->perFrame[(curFrame > 0) ? curFrame-1 : data.size()-1].totalTime;
}
