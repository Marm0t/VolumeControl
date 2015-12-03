#include "KeyListener.h"
#include "logging.h"
using namespace std;

HANDLE KeyListener::newthread()
{
    return CreateThread(NULL, 0, &Runnable::run_thread, this, 0, NULL);
}


void KeyListener::addKey(UINT iKey, KeyCallback_t iCallback)
// adding [alt+shift+iKey] combination
{
    if (isRunning())
    {
        INFO("Cannot add new key when key listener is running!");
        return;
    }
    if (_keyHandlers.insert(pair<UINT, KeyCallback_t>(iKey, iCallback)).second == false)
    {
        INFO("The Key " << iKey << " is already added. You need to remove it first");
        return;
    }
}


void KeyListener::removeKey(UINT iKey)
{
    if (isRunning())
    {
        INFO("Cannot remove key when key listener is running!");
        return;
    }
    _keyHandlers.erase(iKey);
}


void KeyListener::start()
{
    if (_threadID == 0) //we can start only one thread per KeyListener ;)
    {
        _threadID = this->newthread();
        if (!_threadID)
        {
            ERR("ERROR: cannot create thread: " << GetLastError());
        }
    }
    else
    {
        INFO("Key Listener is already running!");
    }
    DBG("Key Listener started");
}


void KeyListener::stop()
{
    if (_threadID != 0)
    {
        if (!TerminateThread(_threadID, 0))
        {
            ERR("ERROR: Cannot terminate KeiListener thread: " << GetLastError());
        }
        else
        {
            DBG("Key Listener stopped ");
            UnregisterHotKey(NULL, 1);
            _threadID = 0;
        }
    }
}


DWORD KeyListener::run()
{
    BOOL res = true;
    for (map<UINT, KeyCallback_t>::iterator aIter = _keyHandlers.begin(); aIter != _keyHandlers.end(); ++aIter)
    {
        res = RegisterHotKey(NULL, 1, MOD_ALT | MOD_SHIFT , aIter->first); // register hotkeys from the map
    }

    if (!res)
    {
        ERR("Error while trying to register keys");
        return 1;
    }
    MSG msg = {0, 0, 0, 0, 0, 0, 0};

    while ( (GetMessage(&msg, NULL, 0, 0) != 0) )
    {
        if (msg.message == WM_HOTKEY)
        {
            LPARAM key = (msg.lParam >> 16) & 0xFFFF;
            LPARAM ctrlKey = msg.lParam & 0xFFFF;
            //cout << "WM_HOTKEY received. key pressed: 0x" <<hex<<uppercase<<key << ", control key: 0x" << hex<<uppercase<<ctrlKey << endl;
            try
            {
                _keyHandlers.at(key)();
            }
            catch (const std::exception& e)
            {
                ERR("Unknown hotkeys received. Implementation error. Key: 0x" <<hex<<key << ", control key: 0x" << hex<<ctrlKey);
                ERR("Exception details: " << e.what());
            }
        }
    }

    // in fact since we're killing this thread with TerminateThread()
    // this line is never called
    // but we also unregister hotkeys in KeyListener.stop() method so this line
    // is just redundant.. but it's okay ;)
    UnregisterHotKey(NULL, 1);
    return 0;
}
