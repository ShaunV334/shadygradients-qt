#ifndef SHADERQT_GLOBAL_H
#define SHADERQT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SHADERQT_LIBRARY)
#define SHADERQT_EXPORT Q_DECL_EXPORT
#else
#define SHADERQT_EXPORT Q_DECL_IMPORT
#endif

#endif // SHADERQT_GLOBAL_H
