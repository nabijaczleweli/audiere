#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <set>
#include <string>
#include <vector>
#include "Commands.h"
#include "DeviceFrame.h"
#include "SoundEffectFrame.h"
#include "StreamFrame.h"
#include "wxPlayer.h"


template<typename T>
std::string Join(
  T cont,
  const std::string& joiner,
  const std::string& prefix = "")
{
  std::string result;

  typename T::iterator i = cont.begin();
  for (;;) {
    result += prefix + *i++;
    if (i == cont.end()) {
      break;
    } else {
      result += joiner;
    }
  }

  return result;
}


BEGIN_EVENT_TABLE(DeviceFrame, wxMDIParentFrame)
  EVT_MENU(DEVICE_NEW_DEVICE,           DeviceFrame::OnDeviceNewDevice)
  EVT_MENU(DEVICE_NEW_CDDEVICE,         DeviceFrame::OnDeviceNewCDDevice)
  EVT_MENU(DEVICE_NEW_MIDIDEVICE,       DeviceFrame::OnDeviceNewMIDIDevice)
  EVT_MENU(DEVICE_OPEN_STREAM,          DeviceFrame::OnDeviceOpenStream)
  EVT_MENU(DEVICE_OPEN_SOUND,           DeviceFrame::OnDeviceOpenSound)
  EVT_MENU(DEVICE_CREATE_TONE,          DeviceFrame::OnDeviceCreateTone)
  EVT_MENU(DEVICE_CREATE_SQUARE_WAVE,   DeviceFrame::OnDeviceCreateSquareWave)
  EVT_MENU(DEVICE_CREATE_WHITE_NOISE,   DeviceFrame::OnDeviceCreateWhiteNoise)
  EVT_MENU(DEVICE_CREATE_PINK_NOISE,    DeviceFrame::OnDeviceCreatePinkNoise)
  EVT_MENU(DEVICE_OPEN_SINGLE_EFFECT,   DeviceFrame::OnDeviceOpenSingleEffect)
  EVT_MENU(DEVICE_OPEN_MULTIPLE_EFFECT, DeviceFrame::OnDeviceOpenMultipleEffect)
  EVT_MENU(DEVICE_CLOSE_WINDOW,         DeviceFrame::OnDeviceCloseCurrentWindow)
  EVT_MENU(DEVICE_CLOSE,                DeviceFrame::OnDeviceClose)
  EVT_MENU(HELP_ABOUT,                  DeviceFrame::OnHelpAbout)
END_EVENT_TABLE()


DeviceFrame::DeviceFrame(audiere::AudioDevice* device)
: wxMDIParentFrame(0, -1, "Audio Device - " + wxString(device->getName()))
{
  m_device = device;

  wxMenu* deviceMenu = new wxMenu();
  deviceMenu->Append(DEVICE_NEW_DEVICE,           "&New Device...");
  deviceMenu->Append(DEVICE_NEW_CDDEVICE,         "New C&D Device...");
  deviceMenu->Append(DEVICE_NEW_MIDIDEVICE,       "New &MIDI Device...");
  deviceMenu->AppendSeparator();
  deviceMenu->Append(DEVICE_OPEN_STREAM,          "&Open Stream...");
  deviceMenu->Append(DEVICE_OPEN_SOUND,           "Open &Sound...");
  deviceMenu->AppendSeparator();
  deviceMenu->Append(DEVICE_CREATE_TONE,          "Create &Tone...");
  deviceMenu->Append(DEVICE_CREATE_SQUARE_WAVE,   "Create S&quare Wave...");
  deviceMenu->Append(DEVICE_CREATE_WHITE_NOISE,   "Create &White Noise");
  deviceMenu->Append(DEVICE_CREATE_PINK_NOISE,    "Create &Pink Noise");
  deviceMenu->AppendSeparator();
  deviceMenu->Append(DEVICE_OPEN_SINGLE_EFFECT,   "Open &Effect (Single)...");
  deviceMenu->Append(DEVICE_OPEN_MULTIPLE_EFFECT, "Open Effect (&Multiple)...");
  deviceMenu->AppendSeparator();
  deviceMenu->Append(DEVICE_CLOSE_WINDOW,         "Close C&urrent Window");
  deviceMenu->Append(DEVICE_CLOSE,                "&Close Device");

  wxMenu* helpMenu = new wxMenu();
  helpMenu->Append(HELP_ABOUT, "&About...");

  wxMenuBar* menuBar = new wxMenuBar();
  menuBar->Append(deviceMenu, "&Device");
  menuBar->Append(helpMenu,   "&Help");
  SetMenuBar(menuBar);

  SetFocus();
}


void DeviceFrame::OnDeviceNewDevice() {
  wxGetApp().OnNewDevice(this);
}


void DeviceFrame::OnDeviceNewCDDevice() {
  wxGetApp().OnNewCDDevice(this);
}


void DeviceFrame::OnDeviceNewMIDIDevice() {
  wxGetApp().OnNewMIDIDevice(this);
}


wxString DeviceFrame::GetSoundFile() {
  std::vector<audiere::FileFormatDesc> formats;
  audiere::GetSupportedFileFormats(formats);

  // combine all of the supported extensions into one collection
  std::set<std::string> all_extensions;
  for (unsigned i = 0; i < formats.size(); ++i) {
    for (unsigned j = 0; j < formats[i].extensions.size(); ++j) {
      all_extensions.insert("*." + formats[i].extensions[j]);
    }
  }

  // build a string of wildcards for wxWindows
  std::string wildcards;
  wildcards = "Sound Files (" + Join(all_extensions, ",") + ")|";
  wildcards += Join(all_extensions, ";") + "|";

  for (unsigned i = 0; i < formats.size(); ++i) {
    audiere::FileFormatDesc& ffd = formats[i];
    wildcards += ffd.description + " ";
    wildcards += "(" + Join(ffd.extensions, ",", "*.") + ")|";
    wildcards += Join(ffd.extensions, ";", "*.") + "|";
  }

  wildcards += "All Files (*.*)|*.*";

  return wxFileSelector(
    "Select a sound file", "", "", "",
    wildcards.c_str(), wxOPEN, this);
}


void DeviceFrame::OnDeviceOpenStream() {
  wxString filename(GetSoundFile());
  if (filename.empty()) {
    return;
  }

  audiere::SampleSourcePtr source = audiere::OpenSampleSource(filename);
  if (!source) {
    wxMessageBox(
      "Could not open sample source: " + filename,
      "Open Stream", wxOK | wxCENTRE, this);
    return;
  }

  audiere::LoopPointSourcePtr loop_source =
    audiere::CreateLoopPointSource(source);
  if (loop_source) {
    source = loop_source.get();
  }

  audiere::OutputStreamPtr stream = audiere::OpenSound(
    m_device,
    source,
    true);
  if (!stream) {
    wxMessageBox(
      "Could not open output stream: " + filename,
      "Open Stream", wxOK | wxCENTRE, this);
    return;
  }

  // get the basename of the path for the window title
  wxString basename = wxFileNameFromPath(filename);
  new StreamFrame(this, "Stream: " + basename, stream.get(), source.get(), loop_source.get());
}


void DeviceFrame::OnDeviceOpenSound() {
  wxString filename(GetSoundFile());
  if (filename.empty()) {
    return;
  }

  audiere::SampleSourcePtr source = audiere::OpenSampleSource(filename);
  if (!source) {
    wxMessageBox(
      "Could not open source: " + filename,
      "Open Sound", wxOK | wxCENTRE, this);
    return;
  }

  audiere::OutputStreamPtr stream = audiere::OpenSound(m_device, source);
  if (!stream) {
    wxMessageBox(
      "Could not open sound: " + filename,
      "Open Sound", wxOK | wxCENTRE, this);
    return;
  }

  // get the basename of the path for the window title
  wxString basename = wxFileNameFromPath(filename);
  new StreamFrame(this, "Sound: " + basename, stream.get(), source.get());
}


void DeviceFrame::OnDeviceCreateTone() {
  int frequency = wxGetNumberFromUser(
    "Value must be between 1 and 30000.", "Enter frequency in Hz",
    "Create Tone", 256, 1, 30000, this);
  if (frequency != -1) {
    audiere::SampleSourcePtr source = audiere::CreateTone(frequency);
    if (!source) {
      wxMessageBox(
        "Could not create tone",
        "Create Tone", wxOK | wxCENTRE, this);
      return;
    }

    audiere::OutputStreamPtr stream = m_device->openStream(source.get());
    if (!stream) {
      wxMessageBox(
        "Could not open output stream",
        "Create Tone", wxOK | wxCENTRE, this);
      return;
    }

    wxString title;
    title.sprintf("Tone: %d Hz", frequency);
    new StreamFrame(this, title, stream.get(), source.get());
  }
}


void DeviceFrame::OnDeviceCreateSquareWave() {
  int frequency = wxGetNumberFromUser(
    "Value must be between 1 and 30000.", "Enter frequency in Hz",
    "Create Square Wave", 256, 1, 30000, this);
  if (frequency != -1) {
    audiere::SampleSourcePtr source = audiere::CreateSquareWave(frequency);
    if (!source) {
      wxMessageBox(
        "Could not create square wave",
        "Create Square Wave", wxOK | wxCENTRE, this);
      return;
    }

    audiere::OutputStreamPtr stream = m_device->openStream(source.get());
    if (!stream) {
      wxMessageBox(
        "Could not open output stream",
        "Create Square Wave", wxOK | wxCENTRE, this);
      return;
    }

    wxString title;
    title.sprintf("Square Wave: %d Hz", frequency);
    new StreamFrame(this, title, stream.get(), source.get());
  }
}


void DeviceFrame::OnDeviceCreateWhiteNoise() {
  audiere::SampleSourcePtr source = audiere::CreateWhiteNoise();
  if (!source) {
    wxMessageBox(
      "Could not create white noise",
      "Create White Noise", wxOK | wxCENTRE, this);
    return;
  }

  audiere::OutputStreamPtr stream = m_device->openStream(source.get());
  if (!stream) {
    wxMessageBox(
      "Could not open output stream",
      "Create White Noise", wxOK | wxCENTRE, this);
    return;
  }

  new StreamFrame(this, "White Noise", stream.get(), source.get());
}


void DeviceFrame::OnDeviceCreatePinkNoise() {
  audiere::SampleSourcePtr source = audiere::CreatePinkNoise();
  if (!source) {
    wxMessageBox(
      "Could not create white noise",
      "Create Pink Noise", wxOK | wxCENTRE, this);
    return;
  }

  audiere::OutputStreamPtr stream = m_device->openStream(source.get());
  if (!stream) {
    wxMessageBox(
      "Could not open output stream",
      "Create Pink Noise", wxOK | wxCENTRE, this);
    return;
  }

  new StreamFrame(this, "Pink Noise", stream.get(), source.get());
}


void DeviceFrame::OnDeviceOpenSingleEffect() {
  DoOpenEffect(audiere::SINGLE, "Single");
}


void DeviceFrame::OnDeviceOpenMultipleEffect() {
  DoOpenEffect(audiere::MULTIPLE, "Multiple");
}


void DeviceFrame::DoOpenEffect(audiere::SoundEffectType type, wxString typestring) {
  wxString filename(GetSoundFile());
  if (filename.empty()) {
    return;
  }

  audiere::SoundEffect* effect = audiere::OpenSoundEffect(m_device, filename, type);
  if (effect) {
    wxString basename = wxFileNameFromPath(filename);
    wxString title;
    title.sprintf("Sound Effect (%s): %s",
                  typestring.c_str(), basename.c_str());
    new SoundEffectFrame(this, title, effect);
  } else {
    wxMessageBox(
      "Could not open sound effect: " + filename,
      "Open Sound Effect", wxOK | wxCENTRE, this);
  }
}


void DeviceFrame::OnDeviceCloseCurrentWindow() {
  wxMDIChildFrame* frame = GetActiveChild();
  if (frame) {
    frame->Close();
  }
}


void DeviceFrame::OnDeviceClose() {
  Close();
}


void DeviceFrame::OnHelpAbout() {
  wxGetApp().ShowAboutDialog(this);
}
