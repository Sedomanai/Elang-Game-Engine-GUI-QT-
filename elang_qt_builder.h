#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(_ELANGQT_DLL)
#  define _ELANGQT_EXPORT Q_DECL_EXPORT
# else
#  define _ELANGQT_EXPORT Q_DECL_IMPORT
# endif
#else
# define _ELANGQT_EXPORT
#endif
