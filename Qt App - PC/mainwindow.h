#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QListWidgetItem>
#include <QBluetoothSocket>
#include <QPushButton>
#include <QList>
#include "stateindicator.h"
#include <QLCDNumber>
#include <QButtonGroup>
#include <QSpinBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void initBluetooth();
    void initPanel();
    void on_find_clicked();
    void onButton_clicked(int id);
    void offButton_clicked(int id);
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void deviceConnected();
    void deviceDisconnected();
    void connectionError(QBluetoothSocket::SocketError error);
    void socketStateChanged(QBluetoothSocket::SocketState state);
    void readyRead();
    void sendState(char channel, char state, int delay);
    void checkState();
    void readState(QString command);
    void readTimer(QString command);

    void on_devicesList_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QBluetoothDeviceDiscoveryAgent *agent;
    QBluetoothSocket *socket;
    QString adress;
    char* buffer;
    QButtonGroup *onButtons;
    QButtonGroup *offButtons;
    QList<QSpinBox *> delays;
    QList<stateIndicator *> indicators;
    QList<QLCDNumber *> lcdDisplays;

};

#endif // MAINWINDOW_H
