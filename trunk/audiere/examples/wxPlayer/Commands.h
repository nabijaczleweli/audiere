#ifndef ADR_COMMANDS_H
#define ADR_COMMANDS_H


enum {
  DEVICE_NEW_DEVICE,
  DEVICE_OPEN_STREAM,
  DEVICE_OPEN_SOUND,
  DEVICE_CREATE_TONE,
  DEVICE_CREATE_SQUARE_WAVE,
  DEVICE_CREATE_WHITE_NOISE,
  DEVICE_CREATE_PINK_NOISE,
  DEVICE_OPEN_SINGLE_EFFECT,
  DEVICE_OPEN_MULTIPLE_EFFECT,
  DEVICE_CLOSE_WINDOW,
  DEVICE_CLOSE,

  HELP_ABOUT,

  STREAM_PLAY,
  STREAM_STOP,
  STREAM_RESET,
  STREAM_REPEAT,
  STREAM_VOLUME,
  STREAM_PAN,
  STREAM_PITCH,
  STREAM_POS,
  STREAM_UPDATE,

  STREAM_VIEW_INFO,
  STREAM_EDIT_LOOP_POINTS,
  STREAM_VIEW_TAGS,

  EFFECT_PLAY,
  EFFECT_STOP,
  EFFECT_VOLUME,
  EFFECT_PAN,
  EFFECT_PITCH,
};


#endif
