#include "Planet.h"

#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

const double Planet::EARTH_MASS = 5.972*10e24;
const double Planet::EARTH_RADIUS = 6371*10e3;

Planet::Planet(double radius, double mass) 
	: mRadius(radius), mMass(mass)
{
	// For visualization, all bodies are scaled to earth radius unit (eru)
	auto sphere = vtkSmartPointer<vtkSphereSource>::New();
	sphere->SetRadius(radius/ EARTH_RADIUS);
	sphere->SetPhiResolution(16);
	sphere->SetThetaResolution(16);
	sphere->SetCenter(0, 0, 0);
	sphere->Update();

	// mapper
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphere->GetOutputPort());
	
	// actor
	mActor = vtkSmartPointer<vtkActor>::New();
	mActor->SetMapper(mapper);
	mActor->GetProperty()->SetColor(1, 1, 0);

	for (int i = 0; i < 3; i++)
		mSpeed[i] = 0.0;
}


Planet::~Planet()
{
}

void Planet::SetPosition(double* arr)
{
	SetPosition(arr[0], arr[1], arr[2]);
}

void Planet::SetPosition(double x, double y, double z)
{
	// real position
	mPosition[0] = x;
	mPosition[1] = y;
	mPosition[2] = z;

	// position in visualization scaled to earth radius
	mActor->SetPosition(x / EARTH_RADIUS, y / EARTH_RADIUS, z / EARTH_RADIUS);
}