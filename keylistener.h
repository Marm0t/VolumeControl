#ifndef KEYLISTENER_H
#define KEYLISTENER_H

#include <Windows.h>
#include <map>
#include "Runnable.h"

typedef void (*KeyCallback_t)();

class KeyListener: public Runnable
{
public:
    KeyListener():_threadID(0){}

    void addKey(UINT iKey, KeyCallback_t callback); // adding [alt+shift+iKey] combination
    void addKey(UINT iKey, UINT iModifiers, KeyCallback_t iCallback);

    void removeKey(UINT iKey); // removing hotkey from configuration
    void removeKey(UINT iKey, UINT iModifiers);

    void clear();


    void start(); // starts keys handler in separated thread
    void stop();  // stops keys handler thread

    bool isRunning() const {return _threadID?true:false;}

    static bool CheckHotKey(UINT iKey, UINT iModifiers = MOD_ALT | MOD_SHIFT ); // true - if key is not bound

private:
    HANDLE newthread();
    DWORD  run();

    HANDLE _threadID;
    std::map<std::pair<UINT, UINT>, KeyCallback_t> _keyHandlers; // pair<key, midifier>

};

#endif // KEYLISTENER_H
