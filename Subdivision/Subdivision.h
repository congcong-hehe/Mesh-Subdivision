#pragma once

#include <QtWidgets/QMainWindow>
#include <QDebug>
#include <QString>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "ui_Subdivision.h"
#include <time.h>

class Subdivision : public QMainWindow
{
    Q_OBJECT

public:
    Subdivision(QWidget *parent = Q_NULLPTR);

    enum class MeshType{TRI, QUAD, MIX};
private:
    Ui::SubdivisionClass ui;

    QString file_name_;  // �򿪵������ļ�����
    MeshType meshtype{ MeshType::TRI };     // �򿪵���������

private slots:
    void slot_openFile();   // ���ļ�
    void slot_reload();     // �����ļ�
    void slot_saveFile();   // �����ļ�
    void slot_loopSubdivision();    // ִ��loopϸ��
    void slot_catmullClarkSubdivision();    // ִ��catmullclarkϸ��
    void slot_dooSabinSubdivision();    // ִ��doo-sabinϸ��
};
