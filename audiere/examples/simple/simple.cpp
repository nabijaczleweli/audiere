#include <iostream>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <audiere.h>
using namespace std;
using namespace audiere;


#ifdef WIN32

#include <windows.h>
inline void sleepSecond() {
  Sleep(1000);
}

#else  // assume POSIX

#include <unistd.h>
inline void sleepSecond() {
  sleep(1);
}

#endif


int main(int argc, const char** argv) {

  const char* args[] = { "", "/desktop/Midworld.ogg", "null" };
  argc = 3;
  argv = args;

  if (argc != 2 && argc != 3) {
    cerr << "usage: simple <filename> [<device>]" << endl;
    return EXIT_FAILURE;
  }

  cerr << "initializing..." << endl;

  const char* device_name = "";
  if (argc == 3) {
    device_name = argv[2];
  }

  RefPtr<AudioDevice> device(OpenDevice(device_name));
  if (!device) {
    cerr << "OpenDevice() failed" << endl;
    return EXIT_FAILURE;
  }

  cerr << "opened device" << endl;

  RefPtr<OutputStream> sound(OpenSound(device.get(), argv[1]));
  if (!sound) {
    cerr << "OpenSound() failed" << endl;
    return EXIT_FAILURE;
  }

  cerr << "opened sound" << endl;

  sound->play();

  cerr << "started playback" << endl;
  while (sound->isPlaying()) {
    sleepSecond();
  }
}
