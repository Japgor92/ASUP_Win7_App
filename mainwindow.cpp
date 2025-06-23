#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    is_connect=false;
    tp_address.setAddress("192.168.1.69");
    QString Eth_Port="1554";
    eth_port=Eth_Port.toUShort();
    ui->disconnectButton->setEnabled(false);
    ui->connectButton->setEnabled(true);
    ui->state_label->setText("не установлено");
    ui->state_label->setStyleSheet("color: red;");
    ui->tap_close_button->setEnabled(false);
    ui->tap_open_button->setEnabled(false);
    ui->stream_meas_button->setEnabled(false);

    ui->set_timer1_value_lineEdit->setEnabled(false);
    ui->sendState_Button->setEnabled(false);
    ui->timer1_label->setEnabled(false);
    ui->timer2_label->setEnabled(false);

    ui->set_manual_checkBox->setEnabled(false);
    ui->set_auto1_checkBox->setEnabled(false);
    ui->set_auto2_checkBox->setEnabled(false);
    ui->set_auto3_checkBox->setEnabled(false);
    ui->set_timer_open_checkBox->setEnabled(false);
    ui->set_timer_close_checkBox->setEnabled(false);

   ui->mode_title->setEnabled(false);
   ui->timer_title->setEnabled(false);
   ui->state_BUK_label->setEnabled(false);
   ui->timer_state_label->setEnabled(false);
   ui->timer_open_close_label->setEnabled(false);

    buk_state_timer=new QTimer ();
    buk_state_timer->setSingleShot(true);
    connect(buk_state_timer,SIGNAL(timeout()),this,SLOT(timer_buk_state_slot()));
    is_buk_state=false;
    is_manual_mode=false;
    is_timer_on=false;
    is_timer_off=false;
    no_timer=false;
    send_buk_state=0;

    // to do!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    timer_is_set=false;
}

MainWindow::~MainWindow()
{
    if (buk_state_timer->isActive())
    {
       buk_state_timer->stop();
    }
    delete ui;
}


void MainWindow::on_connectButton_clicked()
{
    connect (&udpSocket,&QUdpSocket::readyRead,this,&MainWindow::UDP_read);
    udpSocket.bind(QHostAddress::AnyIPv4,eth_port);
    if (udpSocket.state()==QAbstractSocket::BoundState)
    {
        qDebug () << "Попытка открыть UDP-сокет...";
         ui->disconnectButton->setEnabled(true);
         ui->connectButton->setEnabled(false);
    }
    else
    {
        qDebug () << "Проблемы при открытии UDP-сокета";
         ui->disconnectButton->setEnabled(false);
         ui->connectButton->setEnabled(true);
    }


    UDP_send_data="Hello from win7";
    qint64 byteswritten=udpSocket.writeDatagram (UDP_send_data,tp_address,eth_port);


   if (byteswritten==-1)
   {
       qDebug () << "Ошибка отправки данных: "<< udpSocket.errorString();

   }
   else if (byteswritten !=UDP_send_data.size ())
   {
       qDebug () << "Не все данные были отправлены";

   }
   else
   {
       qDebug () << "Данные были успешно отправлены";
   }
}

void MainWindow::UDP_read ()
{
    QByteArray datagram;

    while (udpSocket.hasPendingDatagrams())
    {
        datagram.resize(udpSocket.pendingDatagramSize());
        udpSocket.readDatagram(datagram.data(),datagram.size(),&tp_address,&eth_port);


    }
    if (datagram=="Hello from Orange Pi!")
    {
        ui->state_label->setText("установлено");
        ui->state_label->setStyleSheet("color: green;");
        is_connect=true;
        buk_state_timer->start(3000);
        return;
    }

    if (datagram.startsWith("BUK-state="))
    {

        QByteArray subArray=datagram.mid(10);
       uint8_t value = static_cast<uint8_t>(subArray[0]);
       qDebug () <<value;
       if ((value & MANUAL_MODE)==MANUAL_MODE)
       {

            if (!is_manual_mode)
            {
                ui->tap_close_button->setEnabled(true);
                ui->tap_open_button->setEnabled(true);
                ui->stream_meas_button->setEnabled(true);
                ui->state_BUK_label->setText("ручной");
                is_manual_mode=true;
                is_auto2_mode=false;
            }
      }

              if ((value & TIMER_OPEN)==TIMER_OPEN)
              {
                 if (!is_timer_on)
                 {
                   send_data_UDP ("TIME?");
                   is_timer_on=true;
                   no_timer=false;
                   ui->timer_open_close_label->setText("откр");
                   return;
                 }
              }


              if ((value & TIMER_CLOSE)==TIMER_CLOSE)
              {
                 if (!is_timer_off)
                 {
                   send_data_UDP ("TIME?");
                   is_timer_off=true;
                   no_timer=false;
                   ui->timer_open_close_label->setText("закр");
                   return;
                 }
              }

              if (((value & TIMER_CLOSE)==0)&&((value & TIMER_OPEN)==0))
              {
                 if (!no_timer)
                 {
                     is_timer_off=false;
                     is_timer_on=false;
                     no_timer=true;
                     ui->timer_open_close_label->setText("-");
                     ui->timer_state_label->setText("--:--");
                     return;
                 }

              }

              if ((value & AUTO2_MODE)==AUTO2_MODE)
              {
                  if (!is_auto2_mode)
                  {
                      is_timer_off=false;
                      is_timer_on=false;
                      no_timer=false;
                      is_manual_mode=false;
                      is_auto2_mode=true;

                      ui->tap_close_button->setEnabled(false);
                      ui->tap_open_button->setEnabled(false);
                      ui->stream_meas_button->setEnabled(false);
                      ui->state_BUK_label->setText("Авто2");
                      return;
                  }

              }



       if (!is_buk_state)
       {
           ui->mode_title->setEnabled(true);
           ui->timer_title->setEnabled(true);
           ui->state_BUK_label->setEnabled(true);
           ui->timer_state_label->setEnabled(true);
           ui->timer_open_close_label->setEnabled(true);

           ui->set_timer1_value_lineEdit->setEnabled(true);
           ui->sendState_Button->setEnabled(true);
           ui->timer1_label->setEnabled(true);
           ui->timer2_label->setEnabled(true);
           ui->set_manual_checkBox->setEnabled(true);
           ui->set_auto1_checkBox->setEnabled(true);
           ui->set_auto2_checkBox->setEnabled(true);
           ui->set_auto3_checkBox->setEnabled(true);
           ui->set_timer_open_checkBox->setEnabled(true);
           ui->set_timer_close_checkBox->setEnabled(true);
           is_buk_state=true;
       }




    }

    if (datagram.startsWith("TIMER="))
    {
       QByteArray subArray=datagram.mid(6);
       ui->timer_state_label->setText(QString::fromUtf8(subArray));
    }

    if (datagram.startsWith("Alarm_time?"))
    {
       QString res_string="Alarm_time="+time_val;

       send_data_UDP(res_string.toUtf8());
    }

    if (datagram=="TAP is ON")
    {
        ui->open_tap_label->setText("кран открыт");
        ui->tap_open_button->setEnabled(true);
    }
    if (datagram=="TAP is OFF")
    {
        ui->close_tap_label->setText("кран закрыт");
        ui->tap_close_button->setEnabled(true);
    }

    if (datagram.startsWith('F'))
    {

        QByteArray subArray=datagram.mid(2);
        ui->stream_meas_label->setText(QString::fromUtf8(subArray));
        ui->stream_meas_button->setEnabled(true);
    }



}

void MainWindow::on_disconnectButton_clicked()
{
    QByteArray stop_str="Socket close";
    send_data_UDP (stop_str);
    disconnect (&udpSocket,&QUdpSocket::readyRead,this,&MainWindow::UDP_read);
    is_connect=false;
    udpSocket.close();
    qDebug () << "Соединение UDP разорвано.";
    ui->state_label->setText("не установлено");
    ui->state_label->setStyleSheet("color: red;");
    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);
    ui->tap_close_button->setEnabled(false);
    ui->tap_open_button->setEnabled(false);
    ui->stream_meas_button->setEnabled(false);
    ui->stream_meas_label->setText("-");
    ui->state_BUK_label->setText("-");
     ui->timer_state_label->setText("-:-");

     ui->open_tap_label->setText("-");
     ui->close_tap_label->setText("-");

    ui->set_timer1_value_lineEdit->setEnabled(false);
    ui->sendState_Button->setEnabled(false);
    ui->timer1_label->setEnabled(false);
    ui->timer2_label->setEnabled(false);
    ui->set_manual_checkBox->setEnabled(false);
    ui->set_auto1_checkBox->setEnabled(false);
    ui->set_auto2_checkBox->setEnabled(false);
    ui->set_auto3_checkBox->setEnabled(false);
    ui->set_timer_open_checkBox->setEnabled(false);
    ui->set_timer_close_checkBox->setEnabled(false);

    ui->mode_title->setEnabled(false);
    ui->timer_title->setEnabled(false);
    ui->state_BUK_label->setEnabled(false);
    ui->timer_state_label->setEnabled(false);
    ui->timer_open_close_label->setEnabled(false);

    is_buk_state=false;
    is_manual_mode=false;
    is_timer_on=false;
    is_timer_off=false;
    no_timer=false;


    if (buk_state_timer->isActive())
    {
       buk_state_timer->stop();
    }
}


void MainWindow::on_tap_open_button_clicked()  // открыть кран
{
    QByteArray send_str="Open TAP";
    send_data_UDP (send_str);
    ui->open_tap_label->setText("кран открывается...");
    ui->close_tap_label->setText("-");
    ui->tap_open_button->setEnabled(false);
}


void MainWindow::on_tap_close_button_clicked()  // закрыть кран
{
    QByteArray send_str="Close TAP";
    send_data_UDP (send_str);
    ui->close_tap_label->setText("кран закрывается...");
    ui->open_tap_label->setText("-");
    ui->tap_close_button->setEnabled(false);
}

void MainWindow::send_data_UDP (QByteArray udp_msg)
{

    qint64 byteswritten=udpSocket.writeDatagram (udp_msg,tp_address,eth_port);
    if (byteswritten==-1)
    {
        qDebug () << "Ошибка отправки данных: "<< udpSocket.errorString();

    }
    else if (byteswritten !=udp_msg.size ())
    {
        qDebug () << "Не все данные были отправлены";

    }
    else
    {
        qDebug () << "Данные были успешно отправлены";

    }
}


void MainWindow::on_stream_meas_button_clicked() // измерить поток
{
    QByteArray send_str="Frequency measure";
    send_data_UDP (send_str);
    ui->stream_meas_label->setText("идет измерение...");
    ui->stream_meas_button->setEnabled(false);
}

void MainWindow::timer_buk_state_slot ()
{
   send_data_UDP("State");
    buk_state_timer->start(3000);
}

void MainWindow::on_set_manual_checkBox_stateChanged(int arg1)  // выбрали ручной режим
{
    if (arg1 == Qt::Checked)
    {
        ui->set_auto1_checkBox->setChecked(false);
        ui->set_auto2_checkBox->setChecked(false);
        ui->set_auto3_checkBox->setChecked(false);
        ui->set_auto1_checkBox->setEnabled(false);
        ui->set_auto2_checkBox->setEnabled(false);
        ui->set_auto3_checkBox->setEnabled(false);
        ui->set_timer2_value_lineEdit->setEnabled (false);

        send_buk_state|=MANUAL_MODE;
        send_buk_state&=MANUAL_MODE;
    }
    else if (arg1 == Qt::Unchecked)
    {
        ui->set_auto1_checkBox->setEnabled(true);
        ui->set_auto2_checkBox->setEnabled(true);
        ui->set_auto3_checkBox->setEnabled(true);
        send_buk_state&=~MANUAL_MODE;
      //  ui->set_timer2_value_lineEdit->setEnabled (true);
    }
}


void MainWindow::on_set_auto1_checkBox_stateChanged(int arg1)  // выбрали режим Авто1
{
    if (arg1 == Qt::Checked)
    {
        ui->set_manual_checkBox->setChecked(false);
        ui->set_timer_open_checkBox->setChecked(false);
        ui->set_timer_close_checkBox->setChecked(false);
        ui->set_auto2_checkBox->setChecked(false);
        ui->set_auto3_checkBox->setChecked(false);
        ui->set_manual_checkBox->setEnabled(false);
        ui->set_auto2_checkBox->setEnabled(false);
        ui->set_auto3_checkBox->setEnabled(false);
        ui->set_timer_open_checkBox->setEnabled (false);
        ui->set_timer_close_checkBox->setEnabled (false);
        ui->set_timer1_value_lineEdit->setEnabled (false);
       // ui->set_timer2_value_lineEdit->setEnabled (false);

        send_buk_state|=AUTO1_MODE;
        send_buk_state&=AUTO1_MODE;
    }
    else if (arg1 == Qt::Unchecked)
    {
        ui->set_manual_checkBox->setEnabled(true);
        ui->set_auto2_checkBox->setEnabled(true);
        ui->set_auto3_checkBox->setEnabled(true);
        ui->set_timer_open_checkBox->setEnabled (true);
        ui->set_timer_close_checkBox->setEnabled (true);
        ui->set_timer1_value_lineEdit->setEnabled (true);
        send_buk_state&=~AUTO1_MODE;
      //  ui->set_timer2_value_lineEdit->setEnabled (true);
    }
}


void MainWindow::on_set_auto2_checkBox_stateChanged(int arg1)  // выбрали режим Авто2
{
    if (arg1 == Qt::Checked)
    {
        ui->set_manual_checkBox->setChecked(false);
        ui->set_timer_open_checkBox->setChecked(false);
        ui->set_timer_close_checkBox->setChecked(false);
        ui->set_auto1_checkBox->setChecked(false);
        ui->set_auto3_checkBox->setChecked(false);
        ui->set_manual_checkBox->setEnabled(false);
        ui->set_auto1_checkBox->setEnabled(false);
        ui->set_auto3_checkBox->setEnabled(false);
        ui->set_timer_open_checkBox->setEnabled (false);
        ui->set_timer_close_checkBox->setEnabled (false);
        ui->set_timer1_value_lineEdit->setEnabled (false);
       // ui->set_timer2_value_lineEdit->setEnabled (false);

        send_buk_state|=AUTO2_MODE;
        send_buk_state&=AUTO2_MODE;
    }
    else if (arg1 == Qt::Unchecked)
    {
        ui->set_manual_checkBox->setEnabled(true);
        ui->set_auto1_checkBox->setEnabled(true);
        ui->set_auto3_checkBox->setEnabled(true);
        ui->set_timer_open_checkBox->setEnabled (true);
        ui->set_timer_close_checkBox->setEnabled (true);
        ui->set_timer1_value_lineEdit->setEnabled (true);
        send_buk_state&=~AUTO2_MODE;
      //  ui->set_timer2_value_lineEdit->setEnabled (true);
    }
}




void MainWindow::on_set_auto3_checkBox_stateChanged(int arg1)  // выбрали режим Авто3
{
    if (arg1 == Qt::Checked)
    {
        ui->set_manual_checkBox->setChecked(false);
        ui->set_timer_open_checkBox->setChecked(false);
        ui->set_timer_close_checkBox->setChecked(false);
        ui->set_auto1_checkBox->setChecked(false);
        ui->set_auto2_checkBox->setChecked(false);
        ui->set_manual_checkBox->setEnabled(false);
        ui->set_auto1_checkBox->setEnabled(false);
        ui->set_auto2_checkBox->setEnabled(false);
        ui->set_timer_open_checkBox->setEnabled (false);
        ui->set_timer_close_checkBox->setEnabled (false);
        ui->set_timer1_value_lineEdit->setEnabled (false);
       // ui->set_timer2_value_lineEdit->setEnabled (false);

        send_buk_state|=AUTO3_MODE;
        send_buk_state&=AUTO3_MODE;
    }
    else if (arg1 == Qt::Unchecked)
    {
        ui->set_manual_checkBox->setEnabled(true);
        ui->set_auto1_checkBox->setEnabled(true);
        ui->set_auto2_checkBox->setEnabled(true);
        ui->set_timer_open_checkBox->setEnabled (true);
        ui->set_timer_close_checkBox->setEnabled (true);
        ui->set_timer1_value_lineEdit->setEnabled (true);
        send_buk_state&=~AUTO3_MODE;
      //  ui->set_timer2_value_lineEdit->setEnabled (true);
    }
}


void MainWindow::on_set_timer_open_checkBox_stateChanged(int arg1)  //выбрали таймер откр
{
    if (arg1 == Qt::Checked)
    {
      ui->set_timer_close_checkBox->setChecked(false);
      ui->set_timer_close_checkBox->setEnabled(false);
      send_buk_state|=TIMER_OPEN;
      send_buk_state&=~TIMER_CLOSE;


    }
    else if (arg1 == Qt::Unchecked)
    {

        ui->set_timer_close_checkBox->setEnabled(true);
        send_buk_state&=~TIMER_OPEN;
    }

}


void MainWindow::on_set_timer_close_checkBox_stateChanged(int arg1)  //выбрали таймер закр
{
    if (arg1 == Qt::Checked)
    {
      ui->set_timer_open_checkBox->setChecked(false);
      ui->set_timer_open_checkBox->setEnabled(false);
      send_buk_state|=TIMER_CLOSE;
      send_buk_state&=~TIMER_OPEN;


    }
    else if (arg1 == Qt::Unchecked)
    {

        ui->set_timer_open_checkBox->setEnabled(true);
        send_buk_state&=~TIMER_CLOSE;
    }
}


void MainWindow::on_sendState_Button_clicked()
{
   QByteArray transmit_buk_state;//="Set BUK-state=";
   transmit_buk_state.append("Set BUK-state=");
   transmit_buk_state.append(static_cast<char>(send_buk_state));


   if (((send_buk_state & TIMER_CLOSE)==TIMER_CLOSE)||((send_buk_state & TIMER_OPEN)==TIMER_OPEN))
   {
       time_val="";
       time_val=ui->set_timer1_value_lineEdit->text();
     //  timer_is_set=true;

   }

    send_data_UDP(transmit_buk_state);



     qDebug () <<transmit_buk_state;
}

