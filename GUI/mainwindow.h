#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void addChildren(QTreeWidgetItem* item,QString filePath);

    void initLocalDir(Ui::MainWindow *ui);
    
private:
    Ui::MainWindow *ui;

public slots:
    void openConnect();
    void player_play();
    void player_pause();
    void player_stop();

};

#endif // MAINWINDOW_H
