#include <iostream>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <audiere.h>
#include <unistd.h>
using namespace std;
using namespace audiere;


int main(int argc, char** argv) {

  if (argc != 2) {
    cerr << "usage: adr_test <filename>" << endl;
    return EXIT_FAILURE;
  }

  cerr << "initializing..." << endl;

  auto_ptr<Context> context(CreateContext(0));
  if (!context.get()) {
    cerr << "CreateContext() failed" << endl;
    return EXIT_FAILURE;
  }

  cerr << "created context" << endl;

  auto_ptr<Stream> stream(context->openStream(argv[1]));
  if (!stream.get()) {
    cerr << "openStream() failed" << endl;
    return EXIT_FAILURE;
  }

  cerr << "created stream" << endl;

  stream->play();

  cerr << "started playback" << endl;
  while (stream->isPlaying()) {
    sleep(1);
  }
}