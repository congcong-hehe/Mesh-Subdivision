#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShader>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMouseEvent>
#include <QWheelEvent>
#include <memory>
#include "MixMesh.h"

class QtOpenGLWidget : public QOpenGLWidget , protected QOpenGLFunctions_3_3_Core
{
	Q_OBJECT

public:
	QtOpenGLWidget(QWidget *parent);
	~QtOpenGLWidget();

	virtual void initializeGL() Q_DECL_OVERRIDE;
	virtual void resizeGL(int width, int height) Q_DECL_OVERRIDE;
	virtual void paintGL() Q_DECL_OVERRIDE;

	virtual void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;

	MixMesh* mesh_{ nullptr };
	bool change_mesh_{ false };

private:
	bool mouse_pressed_{ false };
	QPoint last_pos_;

	QOpenGLFunctions_3_3_Core* ff{ nullptr };
};
