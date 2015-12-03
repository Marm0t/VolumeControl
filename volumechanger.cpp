#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <Windows.h>
#include "logging.h"
#include "volumechanger.h"

/*
* Details of winapi methods used: https://msdn.microsoft.com/en-us/library/aa964574.aspx
* Big thanks to this guy: http://blogs.msdn.com/b/larryosterman/archive/2007/03/06/how-do-i-change-the-master-volume-in-windows-vista.aspx
*/


VolumeChanger::VolumeChanger()
    :_initialized(false),_endpointVolume(NULL)
{
    this->init();
}


VolumeChanger::~VolumeChanger()
{
    this->unInit();
}


void VolumeChanger::init()
{
    if (!_initialized)
    {
        IMMDeviceEnumerator *deviceEnumerator = NULL;
        IMMDevice *defaultDevice = NULL;

        CoInitialize(NULL);
        //TODO: add error handling
        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
        if (hr != S_OK)
        {
            ERR("CoCreateInstance error: " << hr);
            return;
        }
        hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
        if (hr != S_OK)
        {
            ERR("Cannot GetDefaultAudioEndpoint: " << hr);
            return;
        }
        deviceEnumerator->Release();
        deviceEnumerator = NULL;
        hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&_endpointVolume);
        if (hr != S_OK)
        {
            ERR("Cannot activate default device: " << hr);
            return;
        }
        defaultDevice->Release();
        defaultDevice = NULL;
        _initialized = true;
    }
}

void VolumeChanger::unInit()
{
    _endpointVolume->Release();
    CoUninitialize();
    _initialized = false;
}


// Public methods

VolumeChanger& VolumeChanger::Instance()
{
    static VolumeChanger singleton;
    return singleton;
}


bool VolumeChanger::setVolume(double iVol)
{
    if (!_initialized) return false;

    HRESULT hr = _endpointVolume->SetMasterVolumeLevelScalar(static_cast<float>(iVol), NULL);
    if (hr != 0)
    {
        //TODO: throw exceptions?
        ERR( "["<<__func__<<"]: Error "<< hr );
        return false;
    }
    return true;
}


double VolumeChanger::getVolume() const
{
    if (!_initialized){ return -1;}
    float fVol;
    HRESULT hr = _endpointVolume->GetMasterVolumeLevelScalar(&fVol);
    if (hr != 0)
    {
        //TODO: throw exceptions?
        ERR(hr);
        return -2;
    }
    return static_cast<double>(fVol);
}


bool VolumeChanger::setMute (bool iMute)
{
   HRESULT hr = _endpointVolume->SetMute(iMute, NULL);
   if (hr != 0)
   {
       ERR(hr);
       return false;
   }
   return true;
}


bool VolumeChanger::isMute() const
{
    BOOL mute = true;
    HRESULT hr = _endpointVolume->GetMute(&mute);
    if (hr != 0){ERR(hr);}
    return (mute)?true:false; //f*cking vs warnings!!!
}
