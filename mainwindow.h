#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include<QSettings>
#include<QFileDialog>
#include<QLabel>
#include "controller.h"
#include "db_control.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QList<QWidget*>* ListOfTab; // Контейнер со всеми вкладками
    // = = = Сохранение настроек = = = //
    void saveSettings();
    QString fileCheck(QString pathFile, QString extension); // проверка файла на пригодность
    void LoadSettings();



private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
