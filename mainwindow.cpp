#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serialtest = new QSerialPort(this);
    msgBox = new QMessageBox(this);
    sendtime = new QTimer(this);
    ui->SendButton->hide();
    ui->uartoutput->hide();
    ui->uartdevice->setText("/dev/ttyAMA2");
    ui->textBrowser->document()->setMaximumBlockCount(512);
    ui->textBrowser->moveCursor(QTextCursor::End);
    ui->textBrowser->setText("serial port received:\n");
    this->uport = "";
    QObject::connect(serialtest,SIGNAL(readyRead()),this,SLOT(uartread()));
    QObject::connect(sendtime,SIGNAL(timeout()),this,SLOT(on_SendButton_clicked()));
}

MainWindow::~MainWindow()
{
    serialtest->close();
    delete ui;
}

void MainWindow::on_SubmitButton_clicked()
{
    /*get uart port device from uartdevice qlineedit*/
    this->uport = ui->uartdevice->text();
       if(this->uport != "")
       {
           serialtest->setPortName(this->uport);
           serialtest->setBaudRate(QSerialPort::Baud115200);
           serialtest->setDataBits(QSerialPort::Data8);
           serialtest->setParity(QSerialPort::NoParity);
           serialtest->setStopBits(QSerialPort::OneStop);
           serialtest->setFlowControl(QSerialPort::NoFlowControl);

           this->fd = serialtest->open(QIODevice::ReadWrite);


           if(this->fd == false)
           {
               ui->label->setText("uart port open error!");
           }
           else{
                  /*modify lable output and show send/receive button and uartoutput line edit*/
                  ui->label->setText("uart setup successfully!");
                  ui->SendButton->show();
                  ui->uartoutput->show();
                  ui->uartoutput->setText("\r\nweiqian serial port test\n");

            }
       }
}

void MainWindow::on_SendButton_clicked()
{
    if(ui->SendButton->isHidden()){return;}
    /*capture uartoutput content QString and change to char[]*/
        this->len = sprintf(this->data_send, ui->uartoutput->text().toLocal8Bit().constData());
        /*judge if uartoutput lineedit is blank*/
        if(this->len == 0)
        {
            /*config default message for sending once lineedit is blank*/
            this->len = sprintf(this->data_send,"this is a test program");
            this->data_send[this->len]='\0';
            /*popup message box to choose*/

            this->ch = serialtest->write(this->data_send,this->len);
            if(this->ch == -1)
            {
                QMessageBox::warning(NULL, "warning", "send error!");
            }


        }
        else
        {
            /*send input message*/
            if(this->len != -1)
            {
                this->ch = serialtest->write(this->data_send,this->len);
                if(this->ch == -1)
                {
                    QMessageBox::warning(NULL, "warning", "send error!");
                }
            }
            else
                QMessageBox::warning(NULL, "warning", "error set data for sending!");
        }
}

void MainWindow::uartread()
{ui->textBrowser->moveCursor(QTextCursor::End);
    /* stop the timer for read timeout*/
    this->data_read = serialtest->readAll();
    /*check if there is read error*/
    if (serialtest->error() == QSerialPort::ReadError) {
        QMessageBox::warning(NULL, "warning", "Failed to read!");
    }else{
        /* convert data_read from QByteArray to QString and check if "ENTER" is received*/
        QString temp = QString::fromStdString(this->data_read.toStdString());
        if (temp != "\r")
        {
            this->str += temp;
        }else{
            ui->textBrowser->append(this->str);
            this->str.clear();
          }
    }
}



void MainWindow::on_AutoButton_clicked()
{
    if(sendtime->isActive()){
        sendtime->stop();
        ui->AutoButton->setText("auto send");
    }else{
        sendtime->start(150);
        ui->AutoButton->setText("sendding");
    }
}
