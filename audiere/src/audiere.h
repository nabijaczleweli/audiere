/**
 * Audiere Sound System
 * Version 1.2.0
 * (c) 2002 Chad Austin
 *
 * This API uses principles explained at
 * http://aegisknight.org/cppinterface.html
 *
 * This code licensed under the terms of the LGPL.  See license.txt.
 */

#ifndef AUDIERE_H
#define AUDIERE_H


#ifndef __cplusplus
  #error Audiere requires C++
#endif


// DLLs in Windows should use the standard calling convention
#if defined(WIN32) || defined(_WIN32)
  #define ADR_CALL __stdcall
#else
  #define ADR_CALL
#endif


#define ADR_FUNCTION(ret, name) extern "C" ret ADR_CALL name


namespace audiere {

  /**
   * A helper class for DLL-compatible interfaces.  Derive your cross-DLL
   * interfaces from this class.
   *
   * When deriving from this class, do not declare a virtual destructor
   * on your interface.
   */
  class DLLInterface {
  private:
    /**
     * Destroy the object, freeing all associated memory.  This is
     * the same as a destructor.
     */
    virtual void destroy() = 0;

  public:
    /**
     * "delete image" should actually call image->destroy(), thus putting the
     * burden of calling the destructor and freeing the memory on the image
     * object, and thus on Corona's side of the DLL boundary.
     */
    void operator delete(void* p) {
      if (p) {
        DLLInterface* i = static_cast<DLLInterface*>(p);
        i->destroy();
      }
    }
  };


  /**
   * A helper class for DLL-compatible interface implementations.  Derive
   * your implementations from DLLImplementation<YourInterface>.
   */
  template<class Interface>
  class DLLImplementation : public Interface {
  public:
    /**
     * So the implementation can put its destruction logic in the destructor,
     * as natural C++ code does.
     */
    virtual ~DLLImplementation() { }

    /**
     * Call the destructor in a Win32 ABI-compatible way.
     */
    virtual void destroy() {
      delete this;
    }

    /**
     * So destroy()'s "delete this" doesn't go into an infinite loop,
     * calling the interface's operator delete, which calls destroy()...
     */
    void operator delete(void* p) {
      ::operator delete(p);
    }
  };


  /**
   * Represents a random-access file, usually stored on a disk.  Files
   * are always binary: that is, they do no end-of-line
   * transformations.  File objects are roughly analogous to ANSI C
   * FILE* objects.
   */
  class File : public DLLInterface {
  public:
    /**
     * The different ways you can seek within a file.
     */
    enum SeekMode {
      BEGIN,
      CURRENT,
      END,
    };

    /**
     * Read size bytes from the file, storing them in buffer.
     *
     * @param buffer  buffer to read into
     * @param size    number of bytes to read
     *
     * @return  number of bytes successfully read
     */
    virtual int read(void* buffer, int size) = 0;

    /**
     * Jump to a new position in the file, using the specified seek
     * mode.  Remember: if mode is END, the position must be negative,
     * to seek backwards from the end of the file into its contents.
     * If the seek fails, the current position is undefined.
     *
     * @param position  position relative to the mode
     * @param mode      where to seek from in the file
     *
     * @return  true on success, false otherwise
     */
    virtual bool seek(int position, SeekMode mode) = 0;

    /**
     * Get current position within the file.
     *
     * @return  current position
     */
    virtual int tell() = 0;
  };


  /// Storage formats for sample data.
  enum SampleFormat {
    SF_U8,     ///< unsigned 8-bit integer [0,255]
    SF_S16_LE, ///< little-endian, signed 16-bit integer [-32768,32767]
  };


  /**
   * Source of raw PCM samples.  Sample sources have an intrinsic format
   * (@see SampleFormat), sample rate, and number of channels.  They can
   * be read from or reset.
   *
   * Some sample sources are seekable.  Seekable sources have two additional
   * properties: length and position.  Length is read-only. 
   */
  class SampleSource : public DLLInterface {
  public:
    /**
     * Retrieve the number of channels, sample rate, and sample format of
     * the sample source.
     */
    virtual void getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format) = 0;

    /**
     * Read sample_count samples into buffer.  buffer must be at least
     * |GetSampleSize(format) * sample_count| bytes long.
     *
     * @param sample_count  number of samples to read
     * @param buffer        buffer to store samples in
     *
     * @return  number of samples actually read
     */
    virtual int read(int sample_count, void* buffer) = 0;

    /**
     * Reset the sample source.  This has the same effect as setPosition(0)
     * on a seekable source.  On an unseekable source, it resets all internal
     * state to the way it was when the source was first created.
     */
    virtual void reset() = 0;

    /**
     * @return  true if the stream is seekable, false otherwise
     */
    virtual bool isSeekable() = 0;

    /**
     * @return  number of samples in the stream, or 0 if the stream is not
     *          seekable
     */
    virtual int getLength() = 0;
    
    /**
     * Sets the current position within the sample source.  If the stream
     * is not seekable, this method does nothing.
     */
    virtual void setPosition(int position) = 0;

    /**
     * Returns the current position within the sample source.
     *
     * @return  current position
     */
    virtual int getPosition() = 0;
  };


  /**
   * A connection to an audio device.  Multiple output streams are
   * mixed by the audio device to produce the final waveform that the
   * user hears.
   *
   * Each output stream can be independently played and stopped.  They
   * also each have a volume from 0.0 (silence) to 1.0 (maximum volume).
   */
  class OutputStream : public DLLInterface {
  public:
    /**
     * Start playback of the output stream.  If the stream is already
     * playing, this does nothing.
     */
    virtual void play() = 0;

    /**
     * Stop playback of the output stream.  If the stream is already
     * stopped, this does nothing.
     */
    virtual void stop() = 0;

    /**
     * Reset the output stream's internal buffer, causing it to buffer new
     * samples from its sample source.  This should be called when the sample
     * source has been changed in some way and the internal buffer's cached
     * data made invalid.
     *
     * On some output streams, this operation can be moderately slow, as up to
     * several seconds of PCM buffer must be refilled.
     */
    virtual void reset() = 0;

    /**
     * @return  true if the output stream is playing, false otherwise
     */
    virtual bool isPlaying() = 0;

    /**
     * Sets the stream's volume.
     *
     * @param  volume  0.0 = silence, 1.0 = maximum volume
     */
    virtual void setVolume(float volume) = 0;

    /**
     * Gets the current volume.
     *
     * @return  current volume of the output stream
     */
    virtual float getVolume() = 0;
  };


  /**
   * AudioDevice represents a device on the system which is capable
   * of opening and mixing multiple output streams.  In Windows,
   * DirectSound is such a device.
   */
  class AudioDevice : public DLLInterface {
  public:
    /**
     * Tell the device to do any internal state updates.  Some devices
     * update on an internal thread.  If that is the case, this method
     * does nothing.
     */
    virtual void update() = 0;

    /**
     * Open an output stream with a given sample source.  If the sample
     * source ever runs out of data, the output stream automatically stops
     * itself.
     *
     * @note  Not all audio devices support streaming audio.  At this time,
     *        they all do, but in the future this may not be the case.
     *
     * @param  source  the source used to feed the output stream with samples
     *
     * @return  new output stream, or 0 if failure
     */
    virtual OutputStream* openStream(SampleSource* source) = 0;

    /**
     * @todo  support opening a single buffer where all PCM data is
     *        available immediately.  This facilitates efficient DS
     *        and OpenAL support, as long as working with AudioWorks.
     */
  };


  enum SoundMode {
    /// Stream the sound from its file.
    STREAM,

    /// Read the sound samples into a buffer in memory and read from that.
    BUFFER,
  };


  /**
   * Convenience object to handle playing, seeking, and repeating sounds.
   * The Sound interface extends OutputStream, so all of those methods are
   * available as well.
   *
   * @note  Sound objects implement reset() in a way unlike OutputStream.
   *        reset() will reset the input stream and then reset the output
   *        stream.
   */
  class Sound : public OutputStream {
  public:
    /**
     * Set the repeating state of the sound.
     */
    virtual void setRepeat(bool repeat) = 0;

    /**
     * Return the current repeating state.
     */
    virtual bool getRepeat() = 0;

    /**
     * @return  true if the sound is seekable, false otherwise
     */
    virtual bool isSeekable() = 0;

    /**
     * @return  number of samples in the sound, or 0 if the sound is not
     *          seekable
     */
    virtual int getLength() = 0;

    /**
     * Sets the current position within the sound.  If the sound
     * is not seekable, this method does nothing.
     */
    virtual void setPosition(int position) = 0;

    /**
     * Returns the current position within the sound.
     *
     * @return  current position
     */
    virtual int getPosition() = 0;
  };


  /// Unimplemented, so undocumented.
  class OutputStream3D : public OutputStream {
  public:
    virtual void setPosition(double x, double y, double z) = 0;
    // ... ?
  };


  /// Unimplemented, so undocumented.
  class AudioDevice3D : public AudioDevice {
    virtual OutputStream3D* openStream3D(SampleSource* source) = 0;
  };


  /// PRIVATE API - for internal use only
  namespace hidden {

    // these are extern "C" so we don't mangle the names

    ADR_FUNCTION(const char*, AdrGetVersion)();
    ADR_FUNCTION(AudioDevice*, AdrOpenDevice)(
      const char* name,
      const char* parameters);
    ADR_FUNCTION(AudioDevice3D*, AdrOpenDevice3D)(
      const char* name,
      const char* parameters);

    ADR_FUNCTION(SampleSource*, AdrOpenSampleSource)(const char* filename);
    ADR_FUNCTION(SampleSource*, AdrOpenSampleSourceFromFile)(File* file);
    ADR_FUNCTION(SampleSource*, AdrCreateTone)(double frequency);

    ADR_FUNCTION(Sound*, AdrOpenSound)(
      AudioDevice* device,
      SampleSource* source,
      SoundMode mode);
  }


  /* PUBLIC API */


  /**
   * Return the Audiere version string.
   *
   * @return  Audiere version information
   */
  inline const char* GetVersion() {
    return hidden::AdrGetVersion();
  }

  /**
   * Open a new audio device. If name or parameters are not specified,
   * defaults are used.
   *
   * Each platform has its own set of audio devices.  Every platform supports
   * the "null" audio device.
   *
   * @param  name  name of audio device that should be used
   * @param  parameters  comma delimited list of audio-device parameters;
   *                     for example, "buffer=100,rate=44100"
   *
   * @return  new audio device object if OpenDevice succeeds, and 0 in case
   *          of failure
   */
  inline AudioDevice* OpenDevice(
    const char* name = 0,
    const char* parameters = 0)
  {
    return hidden::AdrOpenDevice(name, parameters);
  }

  /// Unimplemented, so undocumented.  
  inline AudioDevice3D* OpenDevice3D(
    const char* name = 0,
    const char* parameters = 0)
  {
    return hidden::AdrOpenDevice3D(name, parameters);
  }

  /**
   * Create a streaming sample source from a sound file.  This factory simply
   * opens a default file from the system filesystem and calls
   * OpenSampleSource(File*).
   *
   * @see OpenSampleSource(File*)
   */
  inline SampleSource* OpenSampleSource(const char* filename) {
    return hidden::AdrOpenSampleSource(filename);
  }

  /**
   * Opens a sample source from the specified file object.  If the sound file
   * cannot be opened, this factory function returns 0.
   *
   * @note  Some sound files support seeking, while some don't.
   *
   * @param file  File object from which to open the decoder
   *
   * @return  new SampleSource if OpenSampleSource succeeds, 0 otherwise
   */
  inline SampleSource* OpenSampleSource(File* file) {
    return hidden::AdrOpenSampleSourceFromFile(file);
  }

  /**
   * Create a tone sample source with the specified frequency.
   *
   * @param  frequency  The frequency of the tone in Hz
   *
   * @return  tone sample source
   */
  inline SampleSource* CreateTone(double frequency) {
    return hidden::AdrCreateTone(frequency);
  }

  /**
   * Create a convenience sound object using the specified AudioDevice and
   * sample source.
   *
   * @param device  AudioDevice to open the sound on
   *
   * @param source  SampleSource used to generate samples for the sound
   *                object.  OpenSound takes ownership of source, even
   *                if it returns 0.  (in that case, OpenSound immediately
   *                deletes the SampleSource.)
   *
   * @param mode    There are two ways to open a sound.  One is STREAM,
   *                and the other is BUFFER.  STREAM streams the sound from
   *                the file, and BUFFER tries to read the entire sound into
   *                memory and play that.  These are just hints, so if
   *                STREAMing fails, it will then try BUFFER, and if BUFFER
   *                fails, it will try STREAM.
   *                
   * @return  A new sound object if OpenSound succeeds, and 0 otherwise
   */
  inline Sound* OpenSound(
    AudioDevice* device,
    SampleSource* source,
    SoundMode mode)
  {
    return hidden::AdrOpenSound(device, source, mode);
  }
}


#endif