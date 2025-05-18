#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include "config.h"
#include "log.h"
#include "recorder.h"
#include "recognizer.h"
#include "tts.h"
#include "kobold_client.h"
#include "device_connection.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_recordButton_clicked();
    void on_listMicrophonesButton_clicked();
    void on_showLogButton_clicked();
    void on_scanNetworkButton_clicked();
    void on_saveSettingsButton_clicked();
    void on_sendTextButton_clicked();

private:
    Ui::MainWindow *ui;
    Log *log;
    Recorder *recorder;
    Recognizer *recognizer;
    TextToSpeech *tts;
    KoboldClient *koboldClient;
    DC *deviceConnection;
    bool isProcessing;

    void initializeComponents();
    void setupConnections();
    void loadSettings();
    void saveSettings();
    void updateMicrophoneList();
    void updateStatus(const QString& message);
    QString formatResponse(const QString& text);
};

#endif // MAINWINDOW_H
