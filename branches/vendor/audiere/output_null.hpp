#ifndef OUTPUT_NULL_HPP
#define OUTPUT_NULL_HPP


#include "output.hpp"


class NullOutputContext : public IOutputContext
{
public:
  bool Initialize(const char* parameters);

  void Update();

  IOutputStream* OpenStream(
    int channel_count,
    int sample_rate,
    int bits_per_sample,
    ADR_SAMPLE_SOURCE source,
    ADR_SAMPLE_RESET reset,
    void* opaque
  );
};


class NullOutputStream : public IOutputStream
{
public:
  NullOutputStream();

  void Play();
  void Stop();
  void Reset();
  bool IsPlaying();
  void SetVolume(int volume);
  int  GetVolume();
  void SetPan(int pan);
  int  GetPan();

private:
  int m_volume;
  int m_pan;
};


#endif