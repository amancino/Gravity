#include "Planet.h"

#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

Planet::Planet(double radius, double mass) 
	: mRadius(radius), mMass(mass)
{
	auto sphere = vtkSmartPointer<vtkSphereSource>::New();
	sphere->SetRadius(radius);
	sphere->SetPhiResolution(32);
	sphere->SetThetaResolution(32);
	sphere->SetCenter(0, 0, 0);
	sphere->Update();

	mActor = vtkSmartPointer<vtkActor>::New();
}


Planet::~Planet()
{
}
