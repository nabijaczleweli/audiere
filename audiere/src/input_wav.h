#ifndef INPUT_WAV_H
#define INPUT_WAV_H


#include "audiere.h"
#include "types.h"
#include "utility.h"


namespace audiere {

  class WAVInputStream : public UnseekableSource {
  public:
    WAVInputStream();

    bool initialize(File* file);

    void getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);
    int read(int frame_count, void* samples);
    void reset();

  private:
    bool FindFormatChunk();
    bool FindDataChunk();
    bool SkipBytes(int size);

  private:
    RefPtr<File> m_file;

    // from format chunk
    int m_channel_count;
    int m_sample_rate;
    SampleFormat m_sample_format;

    // from data chunk
    int m_data_chunk_location; // bytes
    int m_data_chunk_length;   // in frames

    int m_frames_left_in_chunk;
  };

}


#endif