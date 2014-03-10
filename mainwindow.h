#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void loadPNG();
    void loadPLIST();
    void getPixtures();
private:
    Ui::MainWindow *ui;
    void parsePlistFile(QString fileName);
    QPixmap pix;
    QString pixFileName;
    QString plistFileName;
    QString outputPathName;
};

#endif // MAINWINDOW_H
