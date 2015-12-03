#ifndef VOLUMECHANGER_H
#define VOLUMECHANGER_H

struct IAudioEndpointVolume;

class VolumeChanger
{
public:
    static VolumeChanger& Instance();

    bool  setVolume(double iVol);
    double getVolume() const;

    bool setMute (bool iMute);
    bool isMute() const;

private:
    VolumeChanger();
    ~VolumeChanger();
    VolumeChanger(const VolumeChanger&);
    VolumeChanger& operator=(const VolumeChanger&);

    void init();
    void unInit();

    bool _initialized;
    IAudioEndpointVolume * _endpointVolume; // = NULL

};

#endif // VOLUMECHANGER_H
