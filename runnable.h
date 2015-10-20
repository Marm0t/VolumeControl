#ifndef RUNNABLE
#define RUNNABLE

#include <Windows.h>
class Runnable
{
public:
    virtual ~Runnable(){}

protected:
    static DWORD WINAPI run_thread(LPVOID args)
    {
        Runnable *prunnable = static_cast<Runnable*>(args);
        return prunnable->run();
    }
    virtual DWORD run() = 0; //derived class must implement this!
};

#endif // RUNNABLE

