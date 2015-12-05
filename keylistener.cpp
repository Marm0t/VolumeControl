#include "KeyListener.h"
#include "logging.h"
using namespace std;

bool KeyListener::CheckHotKey(UINT iKey, UINT iModifiers)
{
    bool aRes = RegisterHotKey(NULL, 0xFA, iModifiers , iKey);
    if (aRes) UnregisterHotKey(NULL, 0xFA);
    return aRes;
}


HANDLE KeyListener::newthread()
{
    return CreateThread(NULL, 0, &Runnable::run_thread, this, 0, NULL);
}


void KeyListener::addKey(UINT iKey, KeyCallback_t iCallback)
// adding [alt+shift+iKey] combination
{
    addKey(iKey, (MOD_ALT | MOD_SHIFT), iCallback );
}


void KeyListener::addKey(UINT iKey, UINT iModifiers, KeyCallback_t iCallback)
{
    if (isRunning())
    {
        INFO("Cannot add new key when key listener is running!");
        return;
    }
    if (_keyHandlers.insert(pair< pair<UINT, UINT>, KeyCallback_t>( {iKey, iModifiers&0xFF}, iCallback)).second == false)
    {
        INFO("The Key " << iKey << " is already added. You need to remove it first");
        return;
    }
}


void KeyListener::removeKey(UINT iKey)
{
    removeKey(iKey, (MOD_ALT | MOD_SHIFT));
}

void KeyListener::removeKey(UINT iKey, UINT iModifiers)
{
    if (isRunning())
    {
        INFO("Cannot remove key when key listener is running!");
        return;
    }
    _keyHandlers.erase({iKey, iModifiers });
}

void KeyListener::clear()
{
    _keyHandlers.clear();
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
    for (map<pair<UINT,UINT>, KeyCallback_t>::iterator aIter = _keyHandlers.begin(); aIter != _keyHandlers.end(); ++aIter)
    {
        res = RegisterHotKey(NULL, 1, aIter->first.second , aIter->first.first); // register hotkeys from the map
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
            LPARAM ctrlKey = msg.lParam & 0xFFFF;
            LPARAM key = (msg.lParam >> 16) & 0xFFFF;
            DBG("ctrlKey: " << ctrlKey << " key: " << key);

            //cout << "WM_HOTKEY received. key pressed: 0x" <<hex<<uppercase<<key << ", control key: 0x" << hex<<uppercase<<ctrlKey << endl;
            try
            {
                _keyHandlers.at({key, ctrlKey})();
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
