#ifndef VIRUSSCANNER_H
#define VIRUSSCANNER_H

#include <QString>

namespace defender_engine
{

class VirusScanner
{
public:
    VirusScanner() = default;

    static bool scan(const QString& filename);
};

}

#endif // VIRUSSCANNER_H
