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

protected:
    void init();
    void unInit();

private:
    VolumeChanger();
    ~VolumeChanger();
    VolumeChanger(const VolumeChanger&);
    VolumeChanger& operator=(const VolumeChanger&);

    bool _initialized;
    IAudioEndpointVolume * _endpointVolume; // = NULL

};

#endif // VOLUMECHANGER_H
