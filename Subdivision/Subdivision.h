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

    QString file_name_;  // 打开的网格文件名字
    MeshType meshtype{ MeshType::TRI };     // 打开的网格类型

private slots:
    void slot_openFile();   // 打开文件
    void slot_reload();     // 重载文件
    void slot_saveFile();   // 保存文件
    void slot_loopSubdivision();    // 执行loop细分
    void slot_catmullClarkSubdivision();    // 执行catmullclark细分
    void slot_dooSabinSubdivision();    // 执行doo-sabin细分
};
