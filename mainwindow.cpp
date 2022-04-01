#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ListOfTab = new QList<QWidget*>();
    Controller* ctr_tab = new Controller();
    DB_Control* ctr_db = new DB_Control();

    ListOfTab->append(ctr_tab);
    ListOfTab->append(ctr_db);
    ui->tabWidget->addTab(ctr_tab,"Controller");
    ui->tabWidget->addTab(ctr_db,"Control DB");

}

MainWindow::~MainWindow()
{
    QList<QWidget*>::iterator i;
    for(i = ListOfTab->begin();i!=ListOfTab->end();i++){
        QWidget* tab = *i;
        tab->close();
    }
    delete ui;
}

void MainWindow::saveSettings()
{
    QString saveFileName;

    saveFileName = QFileDialog::getSaveFileName(this,
                                                "Save file",
                                                QDir::currentPath(),
                                                "INI files (*.ini);;All files (*.*)");

    QSettings* Settings_Save = new QSettings(saveFileName,QSettings::IniFormat);
    Settings_Save->beginGroup("Settings");

        //=== Настройки соединения ===//
        //Settings_Save->beginGroup("USB");
        //Settings_Save->setValue("USB_ID",USB_ID);
       // Settings_Save->endGroup();

    Settings_Save->endGroup();
    Settings_Save->sync();
}

QString MainWindow::fileCheck(QString pathFile, QString extension)
{
    QFile file(pathFile,this);
    QFileInfo fileInfo(file);
    // Проверка расширения файла
    if(fileInfo.suffix() != extension){
       // m_debugConsole->print("Invalid file extension!","e");
        return "";
    }

    // Проверка на существование файла рядом с программой или по полному пути
    if(file.exists(QDir::currentPath()+ "/" + pathFile)) return QDir::currentPath()+ "/" + pathFile ;
    else if(file.exists(pathFile)) return pathFile;
    else{
        //m_debugConsole->print("The file does not exist!","e");
        return "";
    }
}
void MainWindow::LoadSettings()
{
    QString openFileName = QFileDialog::getOpenFileName(this,
                                                        "Open file",
                                                        QDir::currentPath(),
                                                        "INI files (*.ini);;All files (*.*)");

    QSettings* Settings_Save = new QSettings(fileCheck(openFileName,"ini"),QSettings::IniFormat);
    Settings_Save->beginGroup("Settings");

        //=== Настройки соединения ===//
       // Settings_Save->beginGroup("USB");
       // USB_ID = Settings_Save->value("USB_ID").toString();
       // Settings_Save->endGroup();

    Settings_Save->endGroup();

}

void MainWindow::on_pushButton_clicked()
{

}
