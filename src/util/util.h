#ifndef UTIL_H
#define UTIL_H

// for QObject
template<typename T>
void SafeDelete( T*& p )
{
    p->deleteLater();
    p = NULL;
}

#endif // UTIL_H
