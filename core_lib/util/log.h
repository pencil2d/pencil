#ifndef LOG_H
#define LOG_H

#if QT_VERSION >= QT_VERSION_CHECK( 5, 2, 0 )
	#include<QLoggingCategory>
    #define ENABLE_DEBUG_LOG( Logger, OnOff ) Logger.setEnabled( QtDebugMsg, OnOff )
#endif

#if QT_VERSION < QT_VERSION_CHECK( 5, 2, 0 )
    #define QLoggingCategory QString
    #define qCDebug( C ) qDebug()
    #define qCWarning( C ) qWarning()
    #define ENABLE_DEBUG_LOG( Logger, OnOff )
#endif

#endif // LOG_H
