#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <algorithm>
#include <QDir>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isProcessing(false)
{
    ui->setupUi(this);

    QString configFilePath = QDir("../files/config.cfg").absolutePath();
    if (!Config::loadConfig(configFilePath.toStdString())) {
        updateStatus("Ошибка: Не удалось загрузить конфигурацию.");
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить config.cfg. Проверьте файл.");
        return;
    }

    initializeComponents();
    setupConnections();
    loadSettings();
    updateMicrophoneList();
    updateStatus("Ожидание команды...");

    // Устанавливаем начальный заголовок окна
    QString windowTitle = QString::fromStdString(Config::app_name) + " v" + QString::fromStdString(Config::app_version);
    setWindowTitle(windowTitle);
    log->info("Установлен заголовок окна: " + windowTitle.toStdString());
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
    log->info("Инициализация начата.");

    recorder = new Recorder();
    recognizer = new Recognizer(Config::whisper_cli_path, Config::whisper_model_path, *log);
    tts = new TextToSpeech(*log);
    koboldClient = new KoboldClient(Config::koboldcpp_link, Config::koboldcpp_path,
                                    Config::koboldcpp_cfg_path, Config::koboldcpp_model_path, *log);
    deviceConnection = new DC(*log);

    log->info("Компоненты инициализированы.");
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
        updateStatus("Запись начата..."); // Обновляем статус на "Запись начата..."
        if (recorder->record(Config::wav_path, Config::silence_db)) {
            ui->recordButton->setText("Остановить запись");
            isRecording = true;
            updateStatus("Идёт запись..."); // Обновляем статус на "Идёт запись..."
            ui->consoleTextEdit->append(formatResponse("Запись начата."));
            log->info("Запись начата.");
        } else {
            updateStatus("Ошибка: Не удалось начать запись."); // Обновляем статус на ошибку
            ui->consoleTextEdit->append(formatResponse("Ошибка: Не удалось начать запись."));
            log->error("Не удалось начать запись.");
            QMessageBox::warning(this, "Ошибка", "Не удалось начать запись. Проверьте настройки микрофона и PortAudio.");
            return;
        }
    } else {
        recorder->stopRecording();
        ui->recordButton->setText("Начать запись");
        isRecording = false;
        QString message = "Запись остановлена. Файл сохранён в: " + QString::fromStdString(Config::wav_path);
        updateStatus("Распознавание..."); // Обновляем статус на "Распознавание..."
        ui->consoleTextEdit->append(formatResponse(message));
        log->info("Запись остановлена.");

        log->info("Начало распознавания речи.");
        std::string speech = recognizer->recognize(Config::wav_path);

        if (!speech.empty()) {
            log->info("Речь распознана: " + speech);
            ui->consoleTextEdit->append(formatResponse("Речь распознана: " + QString::fromStdString(speech)));

            std::string processedSpeech = speech;
            std::transform(processedSpeech.begin(), processedSpeech.end(), processedSpeech.begin(), ::tolower);

            if (processedSpeech.find("stop") != std::string::npos || processedSpeech.find("exit") != std::string::npos) {
                log->info("Команда stop/exit обнаружена. Завершение работы.");
                ui->consoleTextEdit->append(formatResponse("Команда stop/exit обнаружена. Завершение работы."));
                QApplication::quit();
                return;
            }

            if (processedSpeech.find("send to device") != std::string::npos) {
                std::string remote_command = processedSpeech.substr(processedSpeech.find("send to device") + 15);
                auto devices = deviceConnection->getDiscoveredDevices();

                if (devices.empty()) {
                    log->warning("Устройства не найдены.");
                    ui->consoleTextEdit->append(formatResponse("Устройства не найдены."));
                    updateStatus("Устройства не найдены."); // Обновляем статус на "Устройства не найдены"
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
                            log->warning("Не удалось определить номер устройства. Используется первое устройство.");
                            ui->consoleTextEdit->append(formatResponse("Не удалось определить номер устройства. Используется первое устройство."));
                            deviceIndex = 0;
                        }
                    }
                }

                log->info("Отправка команды устройству #" + std::to_string(deviceIndex + 1));
                ui->consoleTextEdit->append(formatResponse(QString("Отправка команды устройству #%1").arg(deviceIndex + 1)));

                std::string response = deviceConnection->sendCommand(deviceIndex, remote_command, Config::dc_port);

                if (response != "0") {
                    log->info("Команда успешно отправлена устройству #" + std::to_string(deviceIndex + 1));
                    log->info("Ответ устройства: " + response);
                    ui->consoleTextEdit->append(formatResponse(QString("Команда успешно отправлена устройству #%1").arg(deviceIndex + 1)));
                    ui->consoleTextEdit->append(formatResponse("Ответ устройства: " + QString::fromStdString(response)));
                    std::wstring w_response(response.begin(), response.end());
                    tts->speak(w_response);
                    updateStatus("Ответ нейросети..."); // Временный статус
                    QTimer::singleShot(2000, this, [this]() { updateStatus("Ожидание следующей команды..."); }); // Возвращаем к ожиданию
                } else {
                    log->warning("Не удалось отправить команду устройству #" + std::to_string(deviceIndex + 1));
                    ui->consoleTextEdit->append(formatResponse(QString("Не удалось отправить команду устройству #%1").arg(deviceIndex + 1)));
                    updateStatus("Ошибка при отправке команды устройству. Ожидание следующей команды..."); // Обновляем статус на ошибку
                }
            } else {
                log->info("Отправка запроса в нейросеть: " + speech);
                updateStatus("Ответ нейросети..."); // Обновляем статус на "Ответ нейросети..."
                std::string response = koboldClient->sendRequest(speech);

                if (!response.empty()) {
                    log->info("Ответ от нейросети: " + response);
                    ui->consoleTextEdit->append(formatResponse("Ответ от нейросети: " + QString::fromStdString(response)));
                    std::wstring w_response(response.begin(), response.end());
                    tts->speak(w_response);
                    QTimer::singleShot(2000, this, [this]() { updateStatus("Ожидание следующей команды..."); }); // Возвращаем к ожиданию
                } else {
                    log->error("Ошибка: Нейросеть не вернула ответ для запроса: " + speech);
                    ui->consoleTextEdit->append(formatResponse("Ошибка: Нейросеть не вернула ответ."));
                    updateStatus("Ошибка: Нейросеть не вернула ответ. Ожидание следующей команды..."); // Обновляем статус на ошибку
                }
            }
        } else {
            log->error("Ошибка распознавания или пустой результат.");
            ui->consoleTextEdit->append(formatResponse("Ошибка распознавания или пустой результат."));
            updateStatus("Ошибка распознавания. Ожидание следующей команды..."); // Обновляем статус на ошибку
        }
    }
}

void MainWindow::on_listMicrophonesButton_clicked()
{
    ui->consoleTextEdit->clear();
    updateMicrophoneList();
    updateStatus("Список микрофонов обновлён. Ожидание следующей команды...");
    log->info("Кнопка List Microphones нажата, список обновлён.");
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
        updateStatus("Лог отображён. Ожидание следующей команды...");
    } else {
        updateStatus("Ошибка: Не удалось открыть лог. Ожидание следующей команды...");
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл лога.");
    }
}

void MainWindow::on_scanNetworkButton_clicked()
{
    ui->consoleTextEdit->clear();
    log->info("Начало сканирования сети с подсетью: " + Config::dc_subnet + ", порт: " + std::to_string(Config::dc_port));
    deviceConnection->scanLocalNetwork(Config::dc_subnet, Config::dc_port);
    std::vector<std::string> devices = deviceConnection->getDiscoveredDevices();
    QString result;
    if (devices.empty()) {
        result = "Не найдено устройств в сети.";
        updateStatus("Сеть просканирована. Устройства не найдены. Ожидание следующей команды...");
        log->warning("Сканирование завершено. Устройства не найдены.");
    } else {
        result = "Обнаружено устройств: " + QString::number(devices.size()) + "<br>";
        for (const auto& device : devices) {
            result += QString::fromStdString(device) + "<br>";
        }
        updateStatus("Сеть просканирована. Ожидание следующей команды...");
        log->info("Сканирование завершено. Обнаружено устройств: " + std::to_string(devices.size()));
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
        log->info("Повторный вызов on_sendTextButton_clicked заблокирован (isProcessing = true).");
        return;
    }

    log->info("Кнопка Send Text нажата (первый вызов).");
    isProcessing = true;

    ui->sendTextButton->setEnabled(false);
    QTimer::singleShot(1000, this, [this]() {
        ui->sendTextButton->setEnabled(true);
        isProcessing = false;
        log->info("Кнопка Send Text разблокирована после таймера.");
    });

    ui->consoleTextEdit->clear();
    QString text = ui->inputLineEdit->text().trimmed();
    if (text.isEmpty()) {
        updateStatus("Ошибка: Введите текст для отправки. Ожидание следующей команды...");
        ui->consoleTextEdit->append(formatResponse("Ошибка: Введите текст для отправки."));
        log->warning("Попытка отправить пустой текст.");
        return;
    }

    log->info("Отправлен текстовый запрос: " + text.toStdString());
    updateStatus("Обработка текста..."); // Изменяем статус на "Обработка текста..."
    QTimer::singleShot(1000, this, [this, text]() {
        updateStatus("Ответ нейросети..."); // Изменяем статус на "Ответ нейросети..."
        std::string response = koboldClient->sendRequest(text.toStdString());

        if (!response.empty()) {
            log->info("Ответ от нейросети: " + response);
            ui->consoleTextEdit->append(formatResponse("Ваш запрос: " + text));
            ui->consoleTextEdit->append(formatResponse("Ответ от нейросети: " + QString::fromStdString(response)));
            std::wstring w_response(response.begin(), response.end());
            tts->speak(w_response);
            QTimer::singleShot(2000, this, [this]() { updateStatus("Ожидание следующей команды..."); }); // Возвращаем к ожиданию
        } else {
            log->error("Ошибка: Нейросеть не вернула ответ для текстового запроса: " + text.toStdString());
            ui->consoleTextEdit->append(formatResponse("Ошибка: Нейросеть не вернула ответ."));
            updateStatus("Ошибка: Нейросеть не вернула ответ. Ожидание следующей команды...");
        }
    });

    ui->inputLineEdit->clear();
}

QString MainWindow::formatResponse(const QString& text)
{
    return "<div style='border: 1px solid #555; padding: 5px;'><p style='color: #FFFFFF; font-family: Arial; font-size: 14px; margin: 5px;'>" + text + "</p></div>";
}

void MainWindow::updateMicrophoneList()
{
    ui->microphoneComboBox->clear();
    std::vector<std::string> microphones = recorder->listAvailableMicrophones();
    if (microphones.empty()) {
        updateStatus("Ошибка: Не удалось получить список микрофонов. Ожидание следующей команды...");
        ui->consoleTextEdit->append(formatResponse("Ошибка: Не удалось получить список микрофонов."));
        log->error("Не удалось получить список микрофонов. Проверьте PortAudio.");
        QMessageBox::warning(this, "Ошибка", "Не удалось получить список микрофонов. Проверьте PortAudio.");
    } else {
        QString micList = "Доступные микрофоны:<br>";
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
            log->info("Текущий микрофон не найден, выбран: " + Config::microphone);
        }
        ui->microphoneComboBox->update();
    }
}

void MainWindow::updateStatus(const QString& message)
{
    ui->statusTextEdit->setText(message);
}

void MainWindow::loadSettings()
{
    ui->appNameLineEdit->setText(QString::fromStdString(Config::app_name));
    ui->appVersionLineEdit->setText(QString::fromStdString(Config::app_version));
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
    log->info("Сохранение настроек: app_name = " + ui->appNameLineEdit->text().toStdString() + ", app_version = " + ui->appVersionLineEdit->text().toStdString());

    Config::app_name = ui->appNameLineEdit->text().toStdString();
    Config::app_version = ui->appVersionLineEdit->text().toStdString();
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

    log->info("Attempting to save settings to files/config.cfg");
    QString configPath = QDir("../files/config.cfg").absolutePath();
    bool success = Config::saveAllConfig(configPath.toStdString());
    if (success) {
        log->info("Settings saved successfully to " + configPath.toStdString());
        updateStatus("Настройки сохранены. Ожидание следующей команды...");

        QString windowTitle = QString::fromStdString(Config::app_name) + " v" + QString::fromStdString(Config::app_version);
        setWindowTitle(windowTitle);
        log->info("Обновлён заголовок окна: " + windowTitle.toStdString());
    } else {
        log->error("Failed to save settings to " + configPath.toStdString());
        updateStatus("Ошибка: Не удалось сохранить настройки. Ожидание следующей команды...");
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить настройки. Проверь права доступа или путь к файлу.");
    }
}
