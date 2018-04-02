#include "virusscanner.h"

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <sys\stat.h>

struct exe {
    unsigned int ExeHead,
    PartPag,
    PageCnt,
    ReloCnt,
    HdrSize,
    MinMem,
    MaxMem,
    ReloSS,
    ExeSP,
    ChkSum,
    ExeIP,
    ReloCS,
    TablOff,
    Overlay;
};

struct com
{
    unsigned char jmp;
    unsigned int rel;
    unsigned char r[25];
};

union beg
{
    struct exe e;
    struct com c;
};
#endif

namespace defender_engine
{

bool VirusScanner::scan(const QString& filename)
{
#ifdef _WIN32
    unsigned char sign[]={0xFB, 0x3D, 0x00, 0x4B, 0x74, 0x51, 0x80, 0xFC, 0x11, 0x74};
    int f;
    long p = 0;
    union beg h;
    unsigned char buf[10];

    f = open(filename.toStdString().c_str(), O_RDWR, S_IREAD|S_IWRITE);
    if (f == -1)
        return false;
    read(f, &h, sizeof(union beg));

    if (h.e.ExeHead==0x5A4D)
        p = ((long)h.e.HdrSize+(long)h.e.ReloCS)*16+(long)h.e.ExeIP;

    if (h.c.jmp==0xE9)
        p = (long)h.c.rel + 3;

    if (!p)
        return false;

    if (lseek(f, p+0xA7, SEEK_SET) == -1L)
        return false;

    read(f, buf, 10 );
    close(f);

    for (int i = 0; i < 10; ++i)
    {
        if (buf[i] != sign[i])
            return false;
    }
    return true;
#else
    return false;
#endif
}


}
