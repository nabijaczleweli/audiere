#include "mixer.hpp"


////////////////////////////////////////////////////////////////////////////////

Mixer::Mixer()
{
  memset(m_last_sample, 0, 4);
}

////////////////////////////////////////////////////////////////////////////////

Mixer::~Mixer()
{
}

////////////////////////////////////////////////////////////////////////////////

void
Mixer::Read(void* buffer, int sample_count)
{
  char* out = (char*)buffer;
  while (sample_count--) {
    memcpy(out, m_last_sample, 4);
    out += 4;
  }
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////