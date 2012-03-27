#ifndef CONNECTDLG_H
#define CONNECTDLG_H

#include <QDialog>

namespace Ui {
class ConnectDlg;
}

class ConnectDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConnectDlg(QWidget *parent = 0);
    ~ConnectDlg();
    
private:
    Ui::ConnectDlg *ui;

public slots:
    void doConnect();
};


#endif