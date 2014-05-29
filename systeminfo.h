#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <QSettings>
#include <QSysInfo>
#include <QProcess>

/* Done for Win & Linux - Not tested though
 * Mac OS still missing */

// TODO: add renderer from widget glGetString(GL_RENDERER)
class SystemInfo
{
public:
    SystemInfo();
    QString OSVersion;
    #ifdef Q_OS_LINUX
        QString OSVersionDetails;
    #endif
    int memorysize;
    QString cpuinfo;
    QString graphicscardinfo;
    QString language;
    QString endian;
private:
#ifdef Q_OS_LINUX
    QProcess process;
    QString execCommand(QString cmd, bool useArgs=false);
#endif
    void getOSInfo();
    void getCPUInfo();
    void getGraphicCardInfo();
    void getMemorySize();
    void getEndian() {
        switch(QSysInfo::Endian()) {
        case QSysInfo::BigEndian:
            endian = "Big Endian";
            break;
        case QSysInfo::LittleEndian:
            endian = "Little Endian";
            break;
        default:
            endian = "Unknown Endian";
            break;
        }
    }
};

#endif // SYSTEMINFO_H
