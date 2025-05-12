  //  Copyright MIT License 2025 VL_PLAY Games


#pragma once
#ifndef RECORDER_H
#define RECORDER_H
#include <string>
#include <vector>
#include <portaudio.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>

  class Recorder {
  public:
      Recorder();
      ~Recorder();

      void setMicrophone(const std::string& microphone);
      bool record(const std::string& wav_path, int silence_db);
      void stopRecording();
      std::vector<std::string> listAvailableMicrophones() const;

  private:
      std::string microphone_name;
      PaDeviceIndex findInputDeviceByName(const std::string& name);
      bool stopRequested;
  };

#endif
