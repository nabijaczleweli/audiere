#ifndef AUDIERE_H
#define AUDIERE_H


/* extern C */
#ifdef __cplusplus
extern "C" {
#endif


/* calling convention */
#ifdef _WIN32
#  define ADR_CALL __stdcall
#else
#  define ADR_CALL
#endif

/* file callback types */
struct ADR_FileHandle;
typedef ADR_FileHandle* ADR_FILE;

typedef ADR_FILE (ADR_CALL *ADR_FILE_OPEN)(void* opaque, const char* filename);
typedef void (ADR_CALL *ADR_FILE_CLOSE)(ADR_FILE file);
typedef int  (ADR_CALL *ADR_FILE_READ)(ADR_FILE file, void* buffer, int size);
typedef int  (ADR_CALL *ADR_FILE_SEEK)(ADR_FILE file, int destination);
typedef int  (ADR_CALL *ADR_FILE_TELL)(ADR_FILE file);


/* constants */
#define ADR_VOLUME_MIN (0)
#define ADR_VOLUME_MAX (255)


/* audiere opaque types */
typedef struct ADR_CONTEXT_ATTRimp* ADR_CONTEXT_ATTR;
typedef struct ADR_CONTEXTimp*      ADR_CONTEXT;
typedef struct ADR_STREAMimp*       ADR_STREAM;


/* boolean */
typedef int ADR_BOOL;
#define ADR_TRUE  1
#define ADR_FALSE 0



/*
 * AdrGetVersion()
 *
 * Returns Audiere version string.
 *
 */
const char* ADR_CALL AdrGetVersion(void);


/*
 * AdrCreateContextAttr()
 *
 * Returns context attributes object with values set to defaults.
 *
 * output_device = ""
 * parameters    = ""
 * opaque        = 0
 * open          = default open
 * close         = default close
 * read          = default read
 * seek          = default seek
 * tell          = default tell
 *
 */
ADR_CONTEXT_ATTR ADR_CALL AdrCreateContextAttr(void);


/*
 * AdrDestroyContextAttr(attr)
 *
 * Destroys a context attributes object.
 *
 */
void ADR_CALL AdrDestroyContextAttr(
  ADR_CONTEXT_ATTR attr);


/*
 * output_device -- 
 *   string that represents the output device you want to use
 *   "" or "autodetect" will search for a good default device
 *   "null" is no sound
 *
 */
void ADR_CALL AdrContextAttrSetOutputDevice(
  ADR_CONTEXT_ATTR attr,
  const char* output_device);

/*
 * parameters --
 *   comma-delimited list of output device-specific parameters
 *   for example, "buffer=100,rate=44100"
 *
 */
void ADR_CALL AdrContextAttrSetParameters(
  ADR_CONTEXT_ATTR attr,
  const char* parameters);

/*
 * opaque --
 *   opaque handle passed into file I/O functions
 *
 */
void ADR_CALL AdrContextAttrSetOpaque(
  ADR_CONTEXT_ATTR attr,
  void* opaque);

/*
 * file callbacks
 *
 */
void ADR_CALL AdrContextAttrSetFileCallbacks(
  ADR_CONTEXT_ATTR attr,
  ADR_FILE_OPEN  open,
  ADR_FILE_CLOSE close,
  ADR_FILE_READ  read,
  ADR_FILE_SEEK  seek,
  ADR_FILE_TELL  tell);


/*
 * AdrCreateContext(attributes)
 *
 * Returns a new Audiere context or NULL if failure.
 *
 * attributes - set of context attributes, or NULL for defaults
 *
 */
ADR_CONTEXT ADR_CALL AdrCreateContext(
  ADR_CONTEXT_ATTR attr);


/*
 * AdrDestroyContext(context)
 *
 * Destroys a context, stopping the update thread and closing the output device.
 * Contexts aren't actually destroyed until all child streams are closed.
 *
 * context - the context to destroy, of course  ;)
 *
 */
void ADR_CALL AdrDestroyContext(
  ADR_CONTEXT context);


/*
 * AdrOpenStream(context, filename)
 *
 * Returns a new audio stream, or NULL if failure.
 *
 * context  - context within which to create the audio stream
 * filename - UTF-8 filename passed into file open callback
 *
 */
ADR_STREAM ADR_CALL AdrOpenStream(
  ADR_CONTEXT context,
  const char* filename);


/*
 * AdrCloseStream(stream)
 *
 * Closes a stream, halting audio output.
 *
 */
void ADR_CALL AdrCloseStream(
  ADR_STREAM stream);


/*
 * AdrPlayStream(stream)
 *
 * Begins playback of an audio stream.
 *
 */
void ADR_CALL AdrPlayStream(
  ADR_STREAM stream);


/*
 * AdrPauseStream(stream)
 *
 * Halts playback of an audio stream, but does not reset the position to the
 * beginning.
 *
 */
void ADR_CALL AdrPauseStream(
  ADR_STREAM stream);


/*
 * AdrResetStream(stream)
 *
 * Resets the current position within the sound file to the beginning.
 * This may be called at any time.
 *
 */
void ADR_CALL AdrResetStream(
  ADR_STREAM stream);


/*
 * AdrIsPlaying(stream)
 *
 * Returns ADR_TRUE if the stream is currently playing audio.
 *
 */
ADR_BOOL ADR_CALL AdrIsStreamPlaying(
  ADR_STREAM stream);


/*
 * AdrSetStreamRepeat(stream, repeat)
 *
 * If repeat is on and playback reaches the end of the stream, it will
 * automatically reset the stream and continue playback.
 */
void ADR_CALL AdrSetStreamRepeat(
  ADR_STREAM stream,
  ADR_BOOL repeat);


/*
 * AdrGetStreamRepeat(stream)
 *
 * Returns the repeat flag for the given stream.  Repeat defaults to false.
 *
 */
ADR_BOOL ADR_CALL AdrGetStreamRepeat(
  ADR_STREAM);


/*
 * AdrSetStreamVolume(stream, volume)
 *
 * Sets the stream volume.  Defaults to ADR_VOLUME_MAX.
 * ADR_VOLUME_MIN is silence.
 * ADR_VOLUME_MAX is full volume.
 *
 */
void ADR_CALL AdrSetStreamVolume(
  ADR_STREAM stream,
  int volume);


/*
 * AdrGetStreamVolume(stream)
 *
 * Returns the current stream volume.
 *
 */
int ADR_CALL AdrGetStreamVolume(
  ADR_STREAM stream);


#ifdef __cplusplus
}
#endif


#endif