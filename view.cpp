#include <iostream>

#include <QApplication>
#include <QPushbutton.h>
#include <QtWidgets>

#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkProperty.h>
#include <vtkCamera.h>

// qt and vtk
#include <QSurfaceFormat>
#include <QVTKOpenGLWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>

#include "Planet.h"
#include "view.h"

using namespace std;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	View* view = new View();

	return app.exec();
}

View::View()
{
	// render window
	// needed to ensure appropriate OpenGL context is created for VTK rendering.
	QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());

	mRrenderWindowWidget = new QVTKOpenGLWidget();
	vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
	mRrenderWindowWidget->SetRenderWindow(renderWindow);

	// create renderer and configure simulation
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	ConfigureSimulation(renderer);
	renderer->SetBackground(0, 0, 0);
	renderWindow->AddRenderer(renderer);

	pbStart = new QPushButton();
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(mRrenderWindowWidget);
	layout->addWidget(pbStart);

	mWindow = new QWidget();
	mWindow->setLayout(layout);
	mWindow->setWindowTitle(QApplication::translate("window", "Window layout"));
	mWindow->resize(800, 600);
	mWindow->show();

	ConnectActionsAndFunctions();
}

View::~View()
{
	cout << "View destructor called" << std::endl;
	delete mSim;
	delete mRrenderWindowWidget;
	
}

void View::ConnectActionsAndFunctions()
{
	connect(pbStart, SIGNAL(clicked()), this, SLOT(OnStart()));

	// delete window when closed
	mWindow->setAttribute(Qt::WA_DeleteOnClose);

	connect(mWindow, SIGNAL(destroyed(QObject*)), this, SLOT(OnWindowClosed()));
}

void View::OnWindowClosed()
{
	cout << "Window closed" << std::endl;

	delete this;
}

void View::OnStart()
{
	cout << "Starting simulation" << std::endl;

	mSim->Start();
}

void View::ConfigureSimulation(vtkRenderer* renderer)
{
	// Add planets
	Planet* sun = new Planet("Sun", 6.95700e8, 2e30, Planet::SurfaceTexture::Sun);
	// make earth and moon 10 times bigger
	Planet* earth = new Planet("Earth", 10 * 6.371e6, 5.972e24, Planet::SurfaceTexture::Earth);
	Planet* moon = new Planet("Moon", 10 * 1.7371e6, 7.34767309e22, Planet::SurfaceTexture::Moon);

	// Add the actors to the scene
	renderer->AddActor(sun->GetActor());
	renderer->AddActor(earth->GetActor());
	renderer->AddActor(moon->GetActor());
	renderer->AddActor(earth->mOrbitActor);
	renderer->AddActor(moon->mOrbitActor);
	renderer->AddActor(sun->mOrbitActor);

	sun->GetActor()->GetProperty()->SetColor(1, 1, 0);
	earth->GetActor()->GetProperty()->SetColor(0, 0.6, 1);
	moon->GetActor()->GetProperty()->SetColor(0.6, 0.6, 0.6);
	moon->mOrbitActor->GetProperty()->SetColor(1, 0, 0);
	sun->mOrbitActor->GetProperty()->SetColor(1, 1, 0);

	// Render
	renderer->SetBackground(0, 0, 0);

	// initial positions
	sun->SetPosition(0, 0, 0, false);
	earth->SetPosition(149.6e9, 0, 0, false);
	moon->SetPosition(earth->mPosition[0] + 3.844e8, 0, 0, false);

	// 30 km per second
	earth->mSpeed[1] = 30.0e3;
	moon->mSpeed[1] = earth->mSpeed[1] - 1023.056;

	
	auto camera = renderer->GetActiveCamera();
	camera->SetPosition(earth->mPosition[0] / Planet::EARTH_RADIUS, 0, 100);
	camera->SetFocalPoint(earth->mPosition[0] / Planet::EARTH_RADIUS, 0, 0);
	camera->SetViewUp(0, 1, 0);
	renderer->ResetCameraClippingRange();

	cout << "End of configuration" << std::endl;

	mSim = new Simulation();//mRrenderWindowWidget);
	mSim->AddNewPlanet(sun);
	mSim->AddNewPlanet(earth);
	mSim->AddNewPlanet(moon);
}