#ifndef UTIL_H
#define UTIL_H

#include <cstddef>
#include <functional>

// for QObject
template<typename T>
void SafeDelete( T*& p )
{
    p->deleteLater();
    p = NULL;
}

class ScopeGuard
{
public:
    ScopeGuard( std::function< void() > onScopeExit )
    {
        m_onScopeExit = onScopeExit;
    }

    ~ScopeGuard()
    {
        m_onScopeExit();
    }

private:
    std::function< void() > m_onScopeExit;
};

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)

#define OnScopeExit( callback ) ScopeGuard SCOPEGUARD_LINENAME( myScopeGuard, __LINE__ ) ( [&] { callback; } );

#endif // UTIL_H
