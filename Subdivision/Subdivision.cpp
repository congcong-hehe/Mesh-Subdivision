#include "Subdivision.h"
#include <QFileDialog>
#include "MixMesh.h"

Subdivision::Subdivision(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

void Subdivision::slot_openFile()
{
    QString file_name = QFileDialog::getOpenFileName(this, "mesh", ".", "meshs(*.obj)");

    if (file_name.isEmpty())   // 如果没有打开文件
    {
        return;
    }

    file_name_ = file_name;

    if (ui.openGLWidget->mesh_ != nullptr)
    {
        delete ui.openGLWidget->mesh_;
        ui.openGLWidget->mesh_ = nullptr;
    }
    ui.openGLWidget->mesh_ = new MixMesh;
    int type = ui.openGLWidget->mesh_->loadFile(file_name.toStdString());
    if (type == -1)
    {
        qDebug() << "can't open the file";
    }
    else if (type == 0)
    {
        meshtype = MeshType::TRI;
    }
    else if(type == 1)
    {
        meshtype = MeshType::QUAD;
    }
    else
    {
        meshtype = MeshType::MIX;
    }
    qDebug() << "open file: " << file_name_;
    qDebug() << "vertice num: " << ui.openGLWidget->mesh_->getVerticesNum() << " face num: " << ui.openGLWidget->mesh_->getFaceNum();
    ui.openGLWidget->change_mesh_ = true;
    ui.openGLWidget->update();

    std::string text_string =
        "open file: " + file_name_.toStdString() + "\n\n"
        "vertice num: " + std::to_string(ui.openGLWidget->mesh_->getVerticesNum()) + "\n\n"
        "face num: " + std::to_string(ui.openGLWidget->mesh_->getFaceNum());
    ui.textBrowser->setText(QString::fromStdString(text_string));
}

void Subdivision::slot_reload()
{
    if (file_name_.isEmpty())
    {
        return;
    }

    if (ui.openGLWidget->mesh_ != nullptr)
    {
        delete ui.openGLWidget->mesh_;
        ui.openGLWidget->mesh_ = nullptr;
    }

    ui.openGLWidget->mesh_ = new MixMesh;

    ui.openGLWidget->mesh_->loadFile(file_name_.toStdString());
    qDebug() << "reload file: " << file_name_;
    qDebug() << "vertice num: " << ui.openGLWidget->mesh_->getVerticesNum() << " face num: " << ui.openGLWidget->mesh_->getFaceNum();
    ui.openGLWidget->change_mesh_ = true;
    ui.openGLWidget->update();

    std::string text_string =
        "reload file: " + file_name_.toStdString() + "\n\n"
        "vertice num: " + std::to_string(ui.openGLWidget->mesh_->getVerticesNum()) + "\n\n"
        "face num: " + std::to_string(ui.openGLWidget->mesh_->getFaceNum());
    ui.textBrowser->setText(QString::fromStdString(text_string));
}

void Subdivision::slot_saveFile()
{
    // 如果没有读取网格，则不执行
    if (ui.openGLWidget->mesh_ == nullptr)
    {
        return;
    }

    QString file_name = QFileDialog::getSaveFileName(this, "mesh", ".", "meshs(*.obj)");
    if (file_name.isEmpty())   // 如果没有打开文件
    {
        return;
    }
    ui.openGLWidget->mesh_->saveFile(file_name.toStdString());
    qDebug() << "save file: " << file_name_;
    qDebug() << "vertice num: " << ui.openGLWidget->mesh_->getVerticesNum() << " face num: " << ui.openGLWidget->mesh_->getFaceNum();

    std::string text_string =
        "save file: " + file_name_.toStdString() + "\n\n"
        "vertice num: " + std::to_string(ui.openGLWidget->mesh_->getVerticesNum()) + "\n\n"
        "face num: " + std::to_string(ui.openGLWidget->mesh_->getFaceNum());
    ui.textBrowser->setText(QString::fromStdString(text_string));
}

void Subdivision::slot_loopSubdivision()
{
    if (meshtype != MeshType::TRI)
    {
        ui.textBrowser->setText("The mesh can't excute loop subdivision!");
        return;
    }

    // 如果没有读取网格，则不执行
    if (ui.openGLWidget->mesh_ == nullptr)
    {
        return;
    }

    MixMesh* mixMesh = (MixMesh*)ui.openGLWidget->mesh_;

    time_t begin = clock();
    mixMesh->loopSubdivision();     // 细分一次
    time_t end = clock();

    mixMesh->initBO();
    ui.openGLWidget->update();
    qDebug() << "loop subdivision success, cost time: " << end - begin << "ms";
    qDebug() << "vertice num: " << ui.openGLWidget->mesh_->getVerticesNum() << " face num: " << ui.openGLWidget->mesh_->getFaceNum();

    std::string text_string =
        "loop subdivision success, cost time: " + std::to_string(end - begin) +  "ms" + "\n\n"
        "vertice num: " + std::to_string(ui.openGLWidget->mesh_->getVerticesNum()) + "\n\n"
        "face num: " + std::to_string(ui.openGLWidget->mesh_->getFaceNum());
    ui.textBrowser->setText(QString::fromStdString(text_string));
}

void Subdivision::slot_catmullClarkSubdivision()
{
    if (meshtype != MeshType::QUAD)
    {
        ui.textBrowser->setText("The mesh can't excute catmull-clark subdivision!");
        return;
    }

    // 如果没有读取网格，则不执行
    if (ui.openGLWidget->mesh_ == nullptr)
    {
        return;
    }

    MixMesh* mixMesh = (MixMesh*)ui.openGLWidget->mesh_;

    time_t begin = clock();
    mixMesh->catmullClarkSubdivision();     // 细分一次
    time_t end = clock();

    mixMesh->initBO();
    ui.openGLWidget->update();
    qDebug() << "catmull-clark subdivision success, cost time :" << end - begin << "ms";
    qDebug() << "vertice num: " << ui.openGLWidget->mesh_->getVerticesNum() << " face num: " << ui.openGLWidget->mesh_->getFaceNum();

    std::string text_string =
        "catmull-clark subdivision success, cost time: " + std::to_string(end - begin) + "ms" + "\n\n"
        "vertice num: " + std::to_string(ui.openGLWidget->mesh_->getVerticesNum()) + "\n\n"
        "face num: " + std::to_string(ui.openGLWidget->mesh_->getFaceNum());
    ui.textBrowser->setText(QString::fromStdString(text_string));
}

void Subdivision::slot_dooSabinSubdivision()
{
    if (meshtype == MeshType::TRI)
    {
        ui.textBrowser->setText("The mesh can't excute doo-sabin subdivision!");
        return;
    }

    // 如果没有读取网格，则不执行
    if (ui.openGLWidget->mesh_ == nullptr)
    {
        return;
    }

    MixMesh* mixMesh = (MixMesh*)ui.openGLWidget->mesh_;

    time_t begin = clock();
    mixMesh->dooSabinSubdivision();     // 细分一次
    time_t end = clock();

    mixMesh->initBO();
    ui.openGLWidget->update();
    qDebug() << "doo-sabin subdivision success, cost time :" << end - begin << "ms";
    qDebug() << "vertice num: " << ui.openGLWidget->mesh_->getVerticesNum() << " face num: " << ui.openGLWidget->mesh_->getFaceNum();

    std::string text_string =
        "doo-sabin subdivision success, cost time: " + std::to_string(end - begin) + "ms" + "\n\n"
        "vertice num: " + std::to_string(ui.openGLWidget->mesh_->getVerticesNum()) + "\n\n"
        "face num: " + std::to_string(ui.openGLWidget->mesh_->getFaceNum());
    ui.textBrowser->setText(QString::fromStdString(text_string));
}