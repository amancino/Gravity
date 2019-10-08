#pragma once

#include <qobject.h>
#include <qpushbutton.h>
#include <qwidget.h>

#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKOpenGLWidget.h>

#include "simulation.h"

class View : public QObject
{
	Q_OBJECT

public:
	View();
	~View();

protected:
	void ConnectActionsAndFunctions();
	void ConfigureSimulation(vtkRenderer* renderer);

protected slots:
	void OnStart();
	void OnWindowClosed();

private:
	QPushButton* pbStart;
	QVTKOpenGLWidget* mRrenderWindowWidget;
	QWidget* mWindow;

	Simulation* mSim;

};