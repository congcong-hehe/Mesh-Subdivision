#include "QtOpenGLWidget.h"
#include <QDebug>
#include <QFileDialog>

static int x_angle = 0;     // x������ת�Ƕ�
static int y_angle = 0;     // y������ת�Ƕ�
static float zoom = 45.0f;      // ��Ұ����

QtOpenGLWidget::QtOpenGLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{

}

QtOpenGLWidget::~QtOpenGLWidget()
{

}


void QtOpenGLWidget::initializeGL()
{
    ff = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    ff->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);      //���û��Ƶ��߿�ģʽ
}

void QtOpenGLWidget::resizeGL(int width, int height)
{
     ff->glViewport(0, 0, width, height);
}

static bool tag = false;

void QtOpenGLWidget::paintGL()
{
    if (change_mesh_)
    {
        if (mesh_ != nullptr)
            mesh_->destroyGL();
        x_angle = 0;
        y_angle = 0;
        zoom = 45.0f;
        tag = true;
        change_mesh_ = false;
    }

    // ͶӰ�任����
    QMatrix4x4  projection;
    projection.perspective(zoom, (float)width() / height(), 0.1f, 100.0f);

    // ģ�ͱ任����
    QMatrix4x4 model;
    model.rotate(x_angle, QVector3D(0.0f, 1.0f, 0.0f));
    model.rotate(y_angle, QVector3D(1.0f, 0.0f, 0.0f));

    ff->glClearColor(0.2f, 0.3f, 0.3f, 1.0f);   // ������ɫ,(������ɫ)
    ff->glClear(GL_COLOR_BUFFER_BIT);   // �����ɫ����

    if (tag)
    {
        mesh_->setShader();
        mesh_->initBO();
        tag = false;
    }
    if (mesh_ != nullptr && !mesh_->isEmpty())   // ��ֹ�����ļ�Ϊ�յ�����������׵�ַԽ�����
    {
        mesh_->drawMesh(ff, projection, model);
    }
}

void QtOpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        mouse_pressed_ = true;
        last_pos_ = event->pos();
    }
}

void QtOpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    
    mouse_pressed_ = false;
}



void QtOpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (mouse_pressed_)
    {
        int xpos = event->pos().x();
        int ypos = event->pos().y();

        int xoffset = xpos - last_pos_.x();
        int yoffset = ypos - last_pos_.y();
        last_pos_ = event->pos();
        x_angle = (x_angle + xoffset / 2)% 360;
        y_angle = (y_angle + yoffset / 2)% 360;
    }
    update();
}

void QtOpenGLWidget::wheelEvent(QWheelEvent* event)
{
    QPoint offset = event->angleDelta();
    zoom -= (float)offset.y() / 20.0f;
    if (zoom < 10.0f)
        zoom = 10.0f;
    if (zoom > 70.0f)
        zoom = 70.0f;
    update();
}



