#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QDebug>
#include <QTimer>

#define MANUAL_MODE  0x80
#define AUTO1_MODE   0x40
#define AUTO2_MODE   0x20
#define AUTO3_MODE   0x10
#define TIMER_CLOSE  0x08
#define TIMER_OPEN   0x04

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
    void on_connectButton_clicked();
    void UDP_read ();

    void on_disconnectButton_clicked();

    void on_tap_open_button_clicked();

    void on_tap_close_button_clicked();

    void send_data_UDP (QByteArray udp_msg);

    void on_stream_meas_button_clicked();

    void timer_buk_state_slot ();

    void on_set_manual_checkBox_stateChanged(int arg1);

    void on_set_auto1_checkBox_stateChanged(int arg1);

    void on_set_auto2_checkBox_stateChanged(int arg1);

    void on_set_auto3_checkBox_stateChanged(int arg1);

    void on_set_timer_open_checkBox_stateChanged(int arg1);

    void on_set_timer_close_checkBox_stateChanged(int arg1);

    void on_sendState_Button_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket udpSocket;
    quint16 eth_port;
    QHostAddress tp_address;
    QByteArray UDP_send_data, UDP_rec_data;
    QTimer *buk_state_timer;
    bool is_connect;
    bool is_buk_state,is_manual_mode, is_timer_on, is_timer_off, no_timer, is_auto2_mode;
    uint8_t send_buk_state;
    QString time_val;
    bool timer_is_set;

};
#endif // MAINWINDOW_H
