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
    void removeKey(UINT iKey); // removing hotkey from configuration

    void start(); // starts keys handler in separated thread
    void stop();  // stops keys handler thread

    bool isRunning() const {return _threadID?true:false;}

private:
    HANDLE newthread();
    DWORD  run();

    HANDLE _threadID;
    std::map<UINT, KeyCallback_t> _keyHandlers;

};

#endif // KEYLISTENER_H
