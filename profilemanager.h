#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include "singleton.h"
#include <vector>
#include <QDebug>
#include <time.h>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

#define MAX_CATEGORIES 16
#define MAX_FRAMES 600
#define PROFILING_ENABLED 1

#if PROFILING_ENABLED
#ifdef Q_OS_WIN
#define PROFILE_TIMER(cat, expr) {\
    static LONGLONG before_##cat; \
    static LONGLONG after_##cat; \
    QueryPerformanceCounter((LARGE_INTEGER *)&before_##cat); \
    expr; \
    QueryPerformanceCounter((LARGE_INTEGER *)&after_##cat); \
    ProfileManager::instance()->profile(cat, ((double)(after_##cat - before_##cat) / (double)ProfileManager::instance()->frequency)*1000000000); }
#endif
#ifdef Q_OS_LINUX
#define PROFILE_TIMER(cat, expr) {\
    static timespec ts_before_##cat; \
    static timespec ts_after_##cat; \
    clock_gettime(CLOCK_MONOTONIC, &ts_before_##cat); \
    expr; \
    clock_gettime(CLOCK_MONOTONIC, &ts_after_##cat); \
    ProfileManager::instance()->profile(cat, ts_after_##cat.tv_nsec - ts_before_##cat.tv_nsec); }
#endif
#else
#define PROFILE_TIMER(cat, expr) expr;
#endif

struct _ProfileData {
    unsigned long totalTime;
    unsigned long totalCalls;
    unsigned long minTime;
    unsigned long maxTime;
};

struct ProfileData {
    const char *desc;
    unsigned long totalTimeMs;
    unsigned long totalCalls;
    unsigned long minTime;
    unsigned long maxTime;
    _ProfileData perFrame[MAX_FRAMES];
};

class ProfileManager : public Singleton<ProfileManager>
{
public:
    ProfileManager();

    static ProfileManager *instance();

    void newFrame();
    void profile(int category, unsigned long time);
    int registerCategory(const char *description);
    ProfileData *getProfileData(int category);
    unsigned long getLastFrameTime(int category);

#ifdef Q_OS_WIN
    LONGLONG frequency;
#endif
private:
    std::vector<ProfileData *> data;
    short curFrame;
};

#endif // PROFILEMANAGER_H
