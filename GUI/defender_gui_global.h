#ifndef DEFENDER_GUI_GLOBAL_H
#define DEFENDER_GUI_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(DEFENDER_GUI_LIBRARY)
#  define DEFENDER_GUI_EXPORT Q_DECL_EXPORT
#else
#  define DEFENDER_GUI_EXPORT Q_DECL_IMPORT
#endif

#endif // DEFENDER_GUI_GLOBAL_H