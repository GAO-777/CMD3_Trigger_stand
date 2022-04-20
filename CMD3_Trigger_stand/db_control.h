#ifndef DB_CONTROL_H
#define DB_CONTROL_H

#include <QWidget>
#include <QList>
#include <QMessageBox>
#include "binp_device.h"
#include "tools.h"
#include <bitset>
#include <QListWidgetItem>
#include <QUdpSocket>

#include "ethernet_interface.h"
namespace Ui {
class DB_Control;
}
using namespace std;

// - - - Адресное пространство Решающего блока - - - //
#define StatusReg_Offset        4336
#define CTR_ERROR_CPU           4337
#define CTR_ERROR_Link          4338
#define CTR_ERROR_Arbiter       4339
#define CTR_RESET               4340
#define ControlReg_offset       4341
#define Start_One_Width_Offset  4342
#define Start_One_Offset        4343

#define TestRAM_BGO_offset      4111
#define TestRAM_LXe_offset      4117
#define TestRAM_CsI_offset      4125

#define CF_Mask_1_offset        6916
#define CF_Mask_2_offset        6918
#define CF_Mask_3_offset        6920
#define CF_Mask_4_offset        6922
#define CF_Mask_5_offset        6924
#define CF_Mask_6_offset        6926
#define CF_Mask_7_offset        6928
#define CF_Mask_8_offset        6930



class DB_Control : public QWidget
{
    Q_OBJECT

public:
    explicit DB_Control(QWidget *parent = nullptr);
    ~DB_Control();
    MCHS_Imitator* MCHS;
    ADIS* ADIS;



    QStringList *CF_RAM_0 = new QStringList();


    ConnectionsBar* Connections_Bar;
    QPushButton* openDevice_pb;
    unsigned int CF_Masks[8];

// - - - Control Page - - - //
    CommandList* WriteList;
    CommandList* ReadList;
    // - - - InData TestRAM Page - - - //


    void openDevice();
    void closeDevice();


// = = = = = Tools Decision Block = = = = = //
    bool rawDataToRAM(QList<unsigned short> *positional, QList<unsigned short> *energy,
                      QList<unsigned short> *addr, QList<unsigned short> *data, QString who); // Раскладывает поз. и энерг. информацию по ячейкам InData_TestRAM

private slots:
    void on_TR_Clear_clicked();

    void on_TR_Send_clicked();

    void on_CTRL_SetCTRLReg_pb_clicked();

    void on_CTRL_ReadErrors_pb_clicked();

    void on_CTRL_CreateWList_pb_clicked();

    void on_CTRL_CreateRList_pb_clicked();

    void on_CTRL_Write_pb_clicked();

    void on_CTRL_Read_pb_clicked();

    void on_CTRL_SaveList_pb_clicked();

    void on_CTRL_LoadList_pb_clicked();

    void on_CTRL_Clear_pb_clicked();

    void on_CTRL_Start_One_pb_clicked();

    void on_CF_Masks_lw_itemClicked(QListWidgetItem *item);

    void on_CF_Mask_NUM_sb_valueChanged(int arg1);

    void on_Send_CF_Masks_pb_clicked();

    void on_IMChS_waitingNIM_clicked();

    void on_Read_193_clicked();

    void on_Console_clear_pb_clicked();

    void on_Unpack_RAM_sb_valueChanged(int arg1);

    void on_Set_DataFL_clicked();

    void on_TestEth_clicked();

private:
    Ui::DB_Control *ui;
};

#endif // DB_CONTROL_H
