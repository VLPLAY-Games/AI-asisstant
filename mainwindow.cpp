#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <algorithm>
#include <QDir>
#include <QTimer>
#include <QStatusBar>
#include <QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isProcessing(false)
{
    ui->setupUi(this);

    QString configFilePath = "C:/Users/ssselery/Desktop/coursework/AI-assistant/files/config.cfg";
    if (!Config::loadConfig(configFilePath.toStdString())) {
        updateStatus("Error: Failed to load configuration.");
        QMessageBox::critical(this, "Error", "Failed to load config.cfg. Check the file.");
        return;
    }

    initializeComponents();
    setupConnections();
    loadSettings();
    updateMicrophoneList();
    updateStatus("Waiting for command...");

    QString windowTitle = QString::fromStdString(Config::app_name);
    setWindowTitle(windowTitle);

    // Установка иконки
    QIcon appIcon("C:/Users/ssselery/Desktop/coursework/AI-assistant/files/mainlogo.png");
    if (appIcon.isNull()) {
        log->warning("Icon file not found or failed to load.");
    } else {
        setWindowIcon(appIcon);
        log->info("Application icon set successfully.");
    }
}

MainWindow::~MainWindow()
{
    delete deviceConnection;
    delete koboldClient;
    delete tts;
    delete recognizer;
    delete recorder;
    delete log;
    delete ui;
}

void MainWindow::initializeComponents()
{
    log = new Log(Config::log_path);
    log->info("Initialization started.");

    recorder = new Recorder();
    recognizer = new Recognizer(Config::whisper_cli_path, Config::whisper_model_path, *log);
    tts = new TextToSpeech(*log);
    koboldClient = new KoboldClient(Config::koboldcpp_link, Config::koboldcpp_path,
                                    Config::koboldcpp_cfg_path, Config::koboldcpp_model_path, *log);
    deviceConnection = new DC(*log);

    log->info("Components initialized.");
}

void MainWindow::setupConnections()
{
    disconnect(ui->sendTextButton, &QPushButton::clicked, this, &MainWindow::on_sendTextButton_clicked);
    disconnect(ui->recordButton, &QPushButton::clicked, this, &MainWindow::on_recordButton_clicked);
    disconnect(ui->listMicrophonesButton, &QPushButton::clicked, this, &MainWindow::on_listMicrophonesButton_clicked);
    disconnect(ui->showLogButton, &QPushButton::clicked, this, &MainWindow::on_showLogButton_clicked);
    disconnect(ui->scanNetworkButton, &QPushButton::clicked, this, &MainWindow::on_scanNetworkButton_clicked);
    disconnect(ui->saveSettingsButton, &QPushButton::clicked, this, &MainWindow::on_saveSettingsButton_clicked);

    connect(ui->recordButton, &QPushButton::clicked, this, &MainWindow::on_recordButton_clicked);
    connect(ui->listMicrophonesButton, &QPushButton::clicked, this, &MainWindow::on_listMicrophonesButton_clicked);
    connect(ui->showLogButton, &QPushButton::clicked, this, &MainWindow::on_showLogButton_clicked);
    connect(ui->scanNetworkButton, &QPushButton::clicked, this, &MainWindow::on_scanNetworkButton_clicked);
    connect(ui->saveSettingsButton, &QPushButton::clicked, this, &MainWindow::on_saveSettingsButton_clicked);
    connect(ui->sendTextButton, &QPushButton::clicked, this, &MainWindow::on_sendTextButton_clicked);
}

void MainWindow::on_recordButton_clicked()
{
    ui->consoleTextEdit->clear();
    static bool isRecording = false;
    if (!isRecording) {
        recorder->setMicrophone(Config::microphone);
        updateStatus("Recording started...");
        if (recorder->record(Config::wav_path, Config::silence_db)) {
            ui->recordButton->setText("Stop Recording");
            isRecording = true;
            updateStatus("Recording in progress...");
            ui->consoleTextEdit->append(formatResponse("Recording started."));
            log->info("Recording started.");
        } else {
            updateStatus("Error: Failed to start recording.");
            ui->consoleTextEdit->append(formatResponse("Error: Failed to start recording."));
            log->error("Failed to start recording.");
            QMessageBox::warning(this, "Error", "Failed to start recording. Check microphone settings and PortAudio.");
            return;
        }
    } else {
        recorder->stopRecording();
        ui->recordButton->setText("Start Recording");
        isRecording = false;
        updateStatus("Recognizing...");

        // Асинхронное распознавание
        QFuture<std::string> future = QtConcurrent::run([this]() {
            return recognizer->recognize(Config::wav_path);
        });

        QFutureWatcher<std::string> *watcher = new QFutureWatcher<std::string>(this);
        connect(watcher, &QFutureWatcher<std::string>::finished, this, [this, watcher]() {
            std::string speech = watcher->result();
            watcher->deleteLater();

            if (!speech.empty()) {
                log->info("Speech recognized: " + speech);
                ui->consoleTextEdit->append(formatResponse("Speech recognized: " + QString::fromStdString(speech)));

                std::string processedSpeech = speech;
                std::transform(processedSpeech.begin(), processedSpeech.end(), processedSpeech.begin(), ::tolower);

                if (processedSpeech.find("stop") != std::string::npos || processedSpeech.find("exit") != std::string::npos) {
                    log->info("Stop/exit command detected. Terminating application.");
                    ui->consoleTextEdit->append(formatResponse("Stop/exit command detected. Terminating application."));
                    QApplication::quit();
                    return;
                }

                if (processedSpeech.find("send to device") != std::string::npos) {
                    std::string remote_command = processedSpeech.substr(processedSpeech.find("send to device") + 15);
                    auto devices = deviceConnection->getDiscoveredDevices();

                    if (devices.empty()) {
                        log->warning("No devices found.");
                        ui->consoleTextEdit->append(formatResponse("No devices found."));
                        updateStatus("No devices found.");
                        return;
                    }

                    int deviceIndex = 0;
                    if (devices.size() > 1 && processedSpeech.length() > 15) {
                        size_t pos = processedSpeech.find("device");
                        if (pos != std::string::npos) {
                            std::string numStr = processedSpeech.substr(14, pos - 14);
                            try {
                                deviceIndex = std::stoi(numStr) - 1;
                                remote_command = processedSpeech.substr(pos + 6);
                            } catch (...) {
                                log->warning("Failed to determine device number. Using first device.");
                                ui->consoleTextEdit->append(formatResponse("Failed to determine device number. Using first device."));
                                deviceIndex = 0;
                            }
                        }
                    }

                    log->info("Sending command to device #" + std::to_string(deviceIndex + 1));
                    ui->consoleTextEdit->append(formatResponse(QString("Sending command to device #%1").arg(deviceIndex + 1)));

                    // Асинхронная отправка команды устройству
                    QFuture<std::string> deviceFuture = QtConcurrent::run([this, deviceIndex, remote_command]() {
                        return deviceConnection->sendCommand(deviceIndex, remote_command, Config::dc_port);
                    });

                    QFutureWatcher<std::string> *deviceWatcher = new QFutureWatcher<std::string>(this);
                    connect(deviceWatcher, &QFutureWatcher<std::string>::finished, this, [this, deviceWatcher, deviceIndex]() {
                        std::string response = deviceWatcher->result();
                        deviceWatcher->deleteLater();

                        if (response != "0") {
                            log->info("Command successfully sent to device #" + std::to_string(deviceIndex + 1));
                            log->info("Device response: " + response);
                            ui->consoleTextEdit->append(formatResponse(QString("Command successfully sent to device #%1").arg(deviceIndex + 1)));
                            ui->consoleTextEdit->append(formatResponse("Device response: " + QString::fromStdString(response)));

                            // Асинхронный вызов tts->speak()
                            QFuture<void> ttsFuture = QtConcurrent::run([this, response]() {
                                std::wstring w_response(response.begin(), response.end());
                                tts->speak(w_response);
                            });

                            QFutureWatcher<void> *ttsWatcher = new QFutureWatcher<void>(this);
                            connect(ttsWatcher, &QFutureWatcher<void>::finished, this, [this, ttsWatcher]() {
                                ttsWatcher->deleteLater();
                                updateStatus("Neural network response...");
                                QTimer::singleShot(2000, this, [this]() { updateStatus("Waiting for next command..."); });
                            });
                            ttsWatcher->setFuture(ttsFuture);
                        } else {
                            log->warning("Failed to send command to device #" + std::to_string(deviceIndex + 1));
                            ui->consoleTextEdit->append(formatResponse(QString("Failed to send command to device #%1").arg(deviceIndex + 1)));
                            updateStatus("Error sending command to device. Waiting for next command...");
                        }
                    });

                    deviceWatcher->setFuture(deviceFuture);
                } else {
                    log->info("Sending request to neural network: " + speech);
                    updateStatus("Neural network response...");

                    // Асинхронный запрос к нейронной сети
                    QFuture<std::string> nnFuture = QtConcurrent::run([this, speech]() {
                        return koboldClient->sendRequest(speech);
                    });

                    QFutureWatcher<std::string> *nnWatcher = new QFutureWatcher<std::string>(this);
                    connect(nnWatcher, &QFutureWatcher<std::string>::finished, this, [this, nnWatcher, speech]() {
                        std::string response = nnWatcher->result();
                        nnWatcher->deleteLater();

                        if (!response.empty()) {
                            log->info("Neural network response: " + response);
                            ui->consoleTextEdit->append(formatResponse("Neural network response: " + QString::fromStdString(response)));

                            // Асинхронный вызов tts->speak()
                            QFuture<void> ttsFuture = QtConcurrent::run([this, response]() {
                                std::wstring w_response(response.begin(), response.end());
                                tts->speak(w_response);
                            });

                            QFutureWatcher<void> *ttsWatcher = new QFutureWatcher<void>(this);
                            connect(ttsWatcher, &QFutureWatcher<void>::finished, this, [this, ttsWatcher]() {
                                ttsWatcher->deleteLater();
                                QTimer::singleShot(2000, this, [this]() { updateStatus("Waiting for next command..."); });
                            });
                            ttsWatcher->setFuture(ttsFuture);
                        } else {
                            log->error("Error: Neural network did not return a response for request: " + speech);
                            ui->consoleTextEdit->append(formatResponse("Error: Neural network did not return a response."));
                            updateStatus("Error: Neural network did not return a response. Waiting for next command...");
                        }
                    });

                    nnWatcher->setFuture(nnFuture);
                }
            } else {
                log->error("Recognition error or empty result.");
                ui->consoleTextEdit->append(formatResponse("Recognition error or empty result."));
                updateStatus("Recognition error. Waiting for next command...");
            }
        });

        watcher->setFuture(future);
    }
}

void MainWindow::on_listMicrophonesButton_clicked()
{
    ui->consoleTextEdit->clear();
    updateMicrophoneList();
    updateStatus("Microphone list updated. Waiting for next command...");
    log->info("List Microphones button clicked, list updated.");
    ui->microphoneComboBox->update();
}

void MainWindow::on_showLogButton_clicked()
{
    ui->consoleTextEdit->clear();
    QFile file(QString::fromStdString(Config::log_path));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        ui->consoleTextEdit->setText(in.readAll());
        file.close();
        updateStatus("Log displayed. Waiting for next command...");
    } else {
        updateStatus("Error: Failed to open log. Waiting for next command...");
        QMessageBox::warning(this, "Error", "Failed to open log file.");
    }
}

void MainWindow::on_scanNetworkButton_clicked()
{
    ui->consoleTextEdit->clear();
    log->info("Starting network scan with subnet: " + Config::dc_subnet + ", port: " + std::to_string(Config::dc_port));
    deviceConnection->scanLocalNetwork(Config::dc_subnet, Config::dc_port);
    std::vector<std::string> devices = deviceConnection->getDiscoveredDevices();
    QString result;
    if (devices.empty()) {
        result = "No devices found in the network.";
        updateStatus("Network scanned. No devices found. Waiting for next command...");
        log->warning("Scan completed. No devices found.");
    } else {
        result = "Devices found: " + QString::number(devices.size()) + "<br>";
        for (const auto& device : devices) {
            result += QString::fromStdString(device) + "<br>";
        }
        updateStatus("Network scanned. Waiting for next command...");
        log->info("Scan completed. Devices found: " + std::to_string(devices.size()));
    }
    ui->consoleTextEdit->append(formatResponse(result));
}

void MainWindow::on_saveSettingsButton_clicked()
{
    saveSettings();
}

void MainWindow::on_sendTextButton_clicked()
{
    if (isProcessing) {
        log->info("Repeated call to on_sendTextButton_clicked blocked (isProcessing = true).");
        return;
    }

    log->info("Send Text button clicked (first call).");
    isProcessing = true;

    ui->sendTextButton->setEnabled(false);
    QTimer::singleShot(1000, this, [this]() {
        ui->sendTextButton->setEnabled(true);
        isProcessing = false;
        log->info("Send Text button unblocked after timer.");
    });

    ui->consoleTextEdit->clear();
    QString text = ui->inputLineEdit->text().trimmed();
    if (text.isEmpty()) {
        updateStatus("Error: Enter text to send. Waiting for next command...");
        ui->consoleTextEdit->append(formatResponse("Error: Enter text to send."));
        log->warning("Attempt to send empty text.");
        return;
    }

    log->info("Text request sent: " + text.toStdString());
    updateStatus("Processing text...");
    QTimer::singleShot(1000, this, [this, text]() {
        updateStatus("Neural network response...");

        // Асинхронный запрос к нейронной сети
        QFuture<std::string> future = QtConcurrent::run([this, text]() {
            return koboldClient->sendRequest(text.toStdString());
        });

        QFutureWatcher<std::string> *watcher = new QFutureWatcher<std::string>(this);
        connect(watcher, &QFutureWatcher<std::string>::finished, this, [this, watcher, text]() {
            std::string response = watcher->result();
            watcher->deleteLater();

            if (!response.empty()) {
                log->info("Neural network response: " + response);
                ui->consoleTextEdit->append(formatResponse("Your request: " + text));
                ui->consoleTextEdit->append(formatResponse("Neural network response: " + QString::fromStdString(response)));

                // Асинхронный вызов tts->speak()
                QFuture<void> ttsFuture = QtConcurrent::run([this, response]() {
                    std::wstring w_response(response.begin(), response.end());
                    tts->speak(w_response);
                });

                QFutureWatcher<void> *ttsWatcher = new QFutureWatcher<void>(this);
                connect(ttsWatcher, &QFutureWatcher<void>::finished, this, [this, ttsWatcher]() {
                    ttsWatcher->deleteLater();
                    QTimer::singleShot(2000, this, [this]() { updateStatus("Waiting for next command..."); });
                });
                ttsWatcher->setFuture(ttsFuture);
            } else {
                log->error("Error: Neural network did not return a response for text request: " + text.toStdString());
                ui->consoleTextEdit->append(formatResponse("Error: Neural network did not return a response."));
                updateStatus("Error: Neural network did not return a response. Waiting for next command...");
            }
        });

        watcher->setFuture(future);
    });

    ui->inputLineEdit->clear();
}

QString MainWindow::formatResponse(const QString& text)
{
    return "<div style='border: 1px solid #555; padding: 5px;'><p style='color: #FFFFFF; font-family: Arial; font-size: 16px; margin: 5px;'>" + text + "</p></div>";
}

void MainWindow::updateMicrophoneList()
{
    ui->microphoneComboBox->clear();
    std::vector<std::string> microphones = recorder->listAvailableMicrophones();
    if (microphones.empty()) {
        updateStatus("Error: Failed to retrieve microphone list. Waiting for next command...");
        ui->consoleTextEdit->append(formatResponse("Error: Failed to retrieve microphone list."));
        log->error("Failed to retrieve microphone list. Check PortAudio.");
        QMessageBox::warning(this, "Error", "Failed to retrieve microphone list. Check PortAudio.");
    } else {
        QString micList = "Available microphones:<br>";
        for (const auto& mic : microphones) {
            ui->microphoneComboBox->addItem(QString::fromStdString(mic));
            micList += QString::fromStdString(mic) + "<br>";
        }
        ui->consoleTextEdit->append(formatResponse(micList));
        int index = ui->microphoneComboBox->findText(QString::fromStdString(Config::microphone));
        if (index != -1) {
            ui->microphoneComboBox->setCurrentIndex(index);
        } else {
            ui->microphoneComboBox->setCurrentIndex(0);
            Config::microphone = ui->microphoneComboBox->currentText().toStdString();
            log->info("Current microphone not found, selected: " + Config::microphone);
        }
        ui->microphoneComboBox->update();
    }
}

void MainWindow::updateStatus(const QString& message)
{
    ui->statusTextEdit->setText("<p style='font-family: Arial; font-size: 16px; color: #FFFFFF; margin: 5px;'>" + message + "</p>");
}

void MainWindow::loadSettings()
{
    QString basePath = "C:/Users/ssselery/Desktop/coursework/AI-assistant/files";
    if (QDir::isRelativePath(QString::fromStdString(Config::log_path))) {
        Config::log_path = (basePath + "/" + QString::fromStdString(Config::log_path).replace("../files/", "")).toStdString();
    }
    if (QDir::isRelativePath(QString::fromStdString(Config::wav_path))) {
        Config::wav_path = (basePath + "/" + QString::fromStdString(Config::wav_path).replace("../files/", "")).toStdString();
    }
    if (QDir::isRelativePath(QString::fromStdString(Config::output_txt_path))) {
        Config::output_txt_path = (basePath + "/" + QString::fromStdString(Config::output_txt_path).replace("../files/", "")).toStdString();
    }
    if (QDir::isRelativePath(QString::fromStdString(Config::whisper_cli_path))) {
        Config::whisper_cli_path = (basePath + "/" + QString::fromStdString(Config::whisper_cli_path).replace("../files/", "")).toStdString();
    }
    if (QDir::isRelativePath(QString::fromStdString(Config::whisper_model_path))) {
        Config::whisper_model_path = (basePath + "/" + QString::fromStdString(Config::whisper_model_path).replace("../files/", "")).toStdString();
    }
    if (QDir::isRelativePath(QString::fromStdString(Config::koboldcpp_path))) {
        Config::koboldcpp_path = (basePath + "/" + QString::fromStdString(Config::koboldcpp_path).replace("../files/", "")).toStdString();
    }
    if (QDir::isRelativePath(QString::fromStdString(Config::koboldcpp_cfg_path))) {
        Config::koboldcpp_cfg_path = (basePath + "/" + QString::fromStdString(Config::koboldcpp_cfg_path).replace("../files/", "")).toStdString();
    }
    if (QDir::isRelativePath(QString::fromStdString(Config::koboldcpp_model_path))) {
        Config::koboldcpp_model_path = (basePath + "/" + QString::fromStdString(Config::koboldcpp_model_path).replace("../files/", "")).toStdString();
    }

    ui->microphoneComboBox->setCurrentText(QString::fromStdString(Config::microphone));
    ui->logPathLineEdit->setText(QString::fromStdString(Config::log_path));
    ui->wavPathLineEdit->setText(QString::fromStdString(Config::wav_path));
    ui->outputTxtPathLineEdit->setText(QString::fromStdString(Config::output_txt_path));
    ui->silenceDbLineEdit->setText(QString::number(Config::silence_db));
    ui->whisperCliPathLineEdit->setText(QString::fromStdString(Config::whisper_cli_path));
    ui->whisperModelPathLineEdit->setText(QString::fromStdString(Config::whisper_model_path));
    ui->koboldcppLinkLineEdit->setText(QString::fromStdString(Config::koboldcpp_link));
    ui->koboldcppPathLineEdit->setText(QString::fromStdString(Config::koboldcpp_path));
    ui->koboldcppCfgPathLineEdit->setText(QString::fromStdString(Config::koboldcpp_cfg_path));
    ui->koboldcppModelPathLineEdit->setText(QString::fromStdString(Config::koboldcpp_model_path));
    ui->dcSubnetLineEdit->setText(QString::fromStdString(Config::dc_subnet));
    ui->dcPortLineEdit->setText(QString::number(Config::dc_port));
}

void MainWindow::saveSettings()
{
    Config::microphone = ui->microphoneComboBox->currentText().toStdString();
    Config::log_path = ui->logPathLineEdit->text().toStdString();
    Config::wav_path = ui->wavPathLineEdit->text().toStdString();
    Config::output_txt_path = ui->outputTxtPathLineEdit->text().toStdString();
    Config::silence_db = ui->silenceDbLineEdit->text().toInt();
    Config::whisper_cli_path = ui->whisperCliPathLineEdit->text().toStdString();
    Config::whisper_model_path = ui->whisperModelPathLineEdit->text().toStdString();
    Config::koboldcpp_link = ui->koboldcppLinkLineEdit->text().toStdString();
    Config::koboldcpp_path = ui->koboldcppPathLineEdit->text().toStdString();
    Config::koboldcpp_cfg_path = ui->koboldcppCfgPathLineEdit->text().toStdString();
    Config::koboldcpp_model_path = ui->koboldcppModelPathLineEdit->text().toStdString();
    Config::dc_subnet = ui->dcSubnetLineEdit->text().toStdString();
    Config::dc_port = ui->dcPortLineEdit->text().toInt();

    log->info("Attempting to save settings to config.cfg");
    QString configPath = "C:/Users/ssselery/Desktop/coursework/AI-assistant/files/config.cfg";
    bool success = Config::saveAllConfig(configPath.toStdString());
    if (success) {
        log->info("Settings saved successfully to " + configPath.toStdString());
        updateStatus("Settings saved. Waiting for next command...");

        QString windowTitle = QString::fromStdString(Config::app_name);
        setWindowTitle(windowTitle);
        QLabel* versionLabel = dynamic_cast<QLabel*>(ui->statusbar->children().last());
        if (versionLabel) {
            versionLabel->setText("v" + QString::fromStdString(Config::app_version));
        }
        log->info("Window title updated: " + windowTitle.toStdString());
    } else {
        log->error("Failed to save settings to " + configPath.toStdString());
        updateStatus("Error: Failed to save settings. Waiting for next command...");
        QMessageBox::warning(this, "Error", "Failed to save settings. Check file permissions or path.");
    }
}
