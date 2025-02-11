#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <cctype>

using namespace std;

// Пути к Whisper.cpp и модели
const string WHISPER_PATH = "software\\whisper.cpp\\bin\\whisper-cli.exe";
const string MODEL_PATH = "software\\whisper.cpp\\models\\ggml-medium.bin";

// Функция записи голоса через FFmpeg
void record_audio(const string& filename, int duration = 3) {
    cout << "Listening...\n";

    string microphone_name = "Microphone (2- USB PnP Audio Device)";
    string command = "ffmpeg -f dshow -i audio=\"" + microphone_name +
        "\" -t " + to_string(duration) +
        " -ac 1 -ar 16000 " + filename + " -y -loglevel error";

    system(command.c_str());
    cout << "Done listening.\n";
}

// Функция распознавания речи через Whisper.cpp
string recognize_speech(const string& filename) {
    cout << "Processing...\n";

    string input_wav_file = filename;  // Добавляем .wav
    string output_txt_file = filename + ".txt"; // Файл с текстом

    string command = WHISPER_PATH + " -m " + MODEL_PATH +
        " -f " + input_wav_file +  // Передаём полный путь к файлу .wav
        " --language en --output-txt" + 
        " --no-prints";

    system(command.c_str());

    ifstream file(output_txt_file); // Открываем файл с результатом
    if (!file) return "Recognition failed!";

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    while (true) {
        record_audio("voice_record.wav");
        string text = recognize_speech("voice_record.wav");
        // cout << text << endl;
        transform(text.begin(), text.end(), text.begin(), ::tolower);

        // Проверяем на наличие "stop" или "exit" в нижнем регистре
        if (text.find("stop") != string::npos || text.find("exit") != string::npos) {
            break;
        }
    }

    return 0;
}
