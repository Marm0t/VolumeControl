#ifndef LOGGING
#define LOGGING

#include <QDebug>
#include <QDateTime>

#ifndef __func__
#define __func__ __FUNCTION__
#endif

#define DBG( msg ) (qDebug() << "[DBG][" << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") << " " << __func__ << "]: " << msg)
#define INFO( msg )(qInfo() << "[INFO][" << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") << " " << __func__ << "]: " << msg)
#define ERR( msg ) (qInfo()  << "[ERR][" << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") << " " << __func__ << "]: " << msg)



#endif // LOGGING

