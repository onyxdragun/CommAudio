#include "connectdlg.h"
#include "ui_connectdlg.h"
#include <QMessageBox>

ConnectDlg::ConnectDlg(QWidget *parent) :
    QDialog(parent),
       ui(new Ui::ConnectDlg)
    {
        ui->setupUi(this);
        connect(ui->pushButton_cancel, SIGNAL(pressed()), this, SLOT(close())); 
        connect(ui->pushButton_connect, SIGNAL(pressed()), this, SLOT(doConnect()));
    }   

ConnectDlg::~ConnectDlg()
{
    delete ui;
}

void ConnectDlg::doConnect()
{
    QMessageBox qb;
    qb.setText("Connecting...");
    qb.exec();
    
}