#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "deviceobject.h"
#include <QtBluetooth>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    initBluetooth();
    initPanel();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete [] buffer;
}
/*!
 * @brief MainWindow::initBluetooth
 * Konfikguracja urządzenia Bluetooth po stronie komputera
 */
void MainWindow::initBluetooth()
{
    ui->setupUi(this);
    agent = new QBluetoothDeviceDiscoveryAgent();
    connect(agent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));
    agent->start();
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(deviceConnected()));
    connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(connectionError(QBluetoothSocket::SocketError)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deviceDisconnected()));
    connect(socket, SIGNAL(stateChanged(QBluetoothSocket::SocketState)), this, SLOT(socketStateChanged(QBluetoothSocket::SocketState)));
    buffer = new char[20];  // bufor na komunikaty
}
/*!
 * \brief MainWindow::initPanel
 * Konfiguracja okna użytkownika
 */
void MainWindow::initPanel()
{
    onButtons = new QButtonGroup();     // przyciski właczania
    onButtons->addButton(ui->on_1, 0);
    onButtons->addButton(ui->on_2, 1);
    onButtons->addButton(ui->on_3, 2);
    onButtons->addButton(ui->on_4, 3);
    connect(onButtons, SIGNAL(buttonClicked(int)), this, SLOT(onButton_clicked(int)));
    offButtons = new QButtonGroup();
    offButtons->addButton(ui->off_1, 0);    // przyciski wyłączania
    offButtons->addButton(ui->off_2, 1);
    offButtons->addButton(ui->off_3, 2);
    offButtons->addButton(ui->off_4, 3);
    connect(offButtons, SIGNAL(buttonClicked(int)), this, SLOT(offButton_clicked(int)));
    indicators.append(ui->devState_1);      // wskaźniki stanu
    indicators.append(ui->devState_2);
    indicators.append(ui->devState_3);
    indicators.append(ui->devState_4);      // wyświetlacze liczników
    lcdDisplays.append(ui->display_1);
    lcdDisplays.append(ui->display_2);
    lcdDisplays.append(ui->display_3);
    lcdDisplays.append(ui->display_4);
    foreach(QLCDNumber *disp, lcdDisplays)
        disp->display("00:00");
    delays.append(ui->delay_1);             // zadane opóźnienia
    delays.append(ui->delay_2);
    delays.append(ui->delay_3);
    delays.append(ui->delay_4);
}
/*!
 * \brief MainWindow::on_find_clicked
 * Odświeżenie listy dostępnych urządzeń Bluetooth
 */
void MainWindow::on_find_clicked()
{
    ui->devicesList->clear();
    agent->stop();
    agent->start();
}
/*!
 * \brief MainWindow::onButton_clicked
 * Klikniecie przez użytkownika przycisku włączania sterowanego urządzenia
 * \param id numer sterowanego urządzenia
 */
void MainWindow::onButton_clicked(int id)
{
    if (socket->state() != QBluetoothSocket::ConnectedState)
        return;
    sendState('1'+char(id), 't', delays.at(id)->value());
}
/*!
 * @brief MainWindow::offButton_clicked
 * Klikniecie przez użytkownika przycisku wyłączania sterowanego urządzenia
 * \param id numer sterowanego urządzenia
 */
void MainWindow::offButton_clicked(int id)
{
    if (socket->state() != QBluetoothSocket::ConnectedState)
        return;
    sendState('1'+char(id), 'f', delays.at(id)->value());
}
/*!
 * @brief MainWindow::deviceDiscovered
 * Obsługa wykrycia nowego urządzenia Bluetooth
 * @param device - informacja o urzadzeniu
 */
void MainWindow::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    ui->devicesList->addItem(device.address().toString() + "\t" + device.name());   // adres MAC + nazwa
    //ui->devicesList->addItem(device.name());
    //ui->devicesList->addItem(device.address().toString());
}
/*!
 * @brief MainWindow::deviceConnected
 * Połączono z urządzeniem Bluetooth
 */
void MainWindow::deviceConnected()
{
    qDebug() << "connected to " << adress;
    QListWidgetItem *item = ui->devicesList->findItems(adress, Qt::MatchStartsWith).at(0);
    item->setForeground(Qt::red);
    checkState();
}
/*!
 * @brief MainWindow::deviceDisconnected
 * Rozłączono z urządzeniem Bluetooth
 */
void MainWindow::deviceDisconnected()
{
    qDebug() << "disconnected from " << adress;
    QListWidgetItem *item = ui->devicesList->findItems(adress, Qt::MatchStartsWith).at(0);
    //socket->abort();
    adress = "";
    item->setForeground(Qt::black);
    foreach(stateIndicator *ind, indicators)
        ind->setDisabled();
}
/*!
 * @brief MainWindow::connectionError
 * Wystąpił blad zwiazany z połączeniem
 * @param error
 */
void MainWindow::connectionError(QBluetoothSocket::SocketError error)
{
    qDebug() << "Connection error " << error;
}
/*!
 * @brief MainWindow::socketStateChanged
 * Natąpiła zmiana aktualnego stanu połączenia
 * @param state nowy stan
 */
void MainWindow::socketStateChanged(QBluetoothSocket::SocketState state)
{
    qDebug() << "State changed to " << state;
}
/*!
 * @brief MainWindow::on_devicesList_itemClicked
 * Użytkownik kliknął nazwę urżadzenia na liscie
 * @param item element listy
 */
void MainWindow::on_devicesList_itemClicked(QListWidgetItem *item)
{
    if ((socket->state() != QBluetoothSocket::UnconnectedState) &&
            (socket->state() != QBluetoothSocket::ConnectedState))
        return;
    adress = item->text().left(17);     // MAC adress
    static const QString serviceUuid(QStringLiteral("00001101-0000-1000-8000-00805F9B34FB"));
    //socket->disconnectFromService();
    socket->connectToService(QBluetoothAddress(adress), QBluetoothUuid(serviceUuid), QIODevice::ReadWrite); // połaczenie z urządzeniem
}
/*!
 * @brief MainWindow::readyRead
 * Pojawiły się nowe komunikaty od połączonego urządzenia
 */
void MainWindow::readyRead()
{
    int maxCount = 20;
    if(!socket->peek(maxCount).contains('\n'))
        return;
    QString stringBuff = socket->readAll();
    QStringList commands = QStringList(stringBuff.split('\n'));
    QStringListIterator commIter(commands);
    while (commIter.hasNext()) {            // odczyt i dekodowanie kolejnych komunikatów
        QString command = commIter.next();
        if (command == "")
            break;
        qDebug() << "I received: " << command;
        QChar firstCh = command.at(0);
        command.remove(0,1);
        if (firstCh == 's')     // komunikat z aktualnym stanem wyjść
            readState(command);
        else if (firstCh == 't')    // komunikat ze stanem licznika
            readTimer(command);
    }
}
/*!
 * \brief MainWindow::sendState
 * Wysyłanie komunikatów do połączonego urządzenia
 * \param channel numer kanału którego dotyczy komunikat
 * \param state nowy stan ('t' lub 'f')
 * \param delay opóźnienie (opcjonalnie)
 */
void MainWindow::sendState(char channel, char state, int delay=0)
{
    int maxSize = 10;
    QByteArray buffer(maxSize, 0);
    buffer = "s";
    buffer += channel;
    buffer += state;
    if (delay != 0){    // bez opóźnienia
        buffer += QByteArray::number(delay);
    }
    else {
        lcdDisplays.at(channel-'1')->display("00:00");
    }
    qDebug() << buffer.data();
    socket->write(buffer.data(), maxSize);

}
/*!
 * \brief MainWindow::checkState
 * Wysłanie zapytania do połączonego urządzenia
 */
void MainWindow::checkState()
{
    qDebug() << "check current state";
    socket->write("?");
}
/*!
 * \brief MainWindow::readState
 * Dekodowanie odebranego komunikatu stanu urzadzeń
 * \param command odebrany komunikat
 */
void MainWindow::readState(QString command)
{
    qDebug() << "readState: " << command;
    for (int i = 0; i < indicators.size(); ++i) {
        if (command.at(i) == 't')
            indicators.at(i)->setOn();
        else
            indicators.at(i)->setOff();
    }
}
/*!
 * \brief MainWindow::readTimer
 * Dekodowanie odebranego komunikatu o stanie wybranego licznika
 * \param command odebrany komunikat
 */
void MainWindow::readTimer(QString command)
{
    int channel = command.at(0).digitValue();
    int sec = command.remove(0, 1).toInt();
    int min = sec / 60;
    sec %= 60;
    QTime timeLeft = QTime(1, min, sec);
    lcdDisplays.at(channel-1)->display(timeLeft.toString("mm:ss"));
}
