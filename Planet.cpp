#include "Planet.h"

#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

Planet::Planet(double radius, double mass) 
	: mRadius(radius), mMass(mass)
{
	// sphere
	auto sphere = vtkSmartPointer<vtkSphereSource>::New();
	sphere->SetRadius(radius);
	sphere->SetPhiResolution(32);
	sphere->SetThetaResolution(32);
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
