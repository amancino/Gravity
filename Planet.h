#pragma once
#include <vtkActor.h>
#include <vtkSmartPointer.h>

class Planet
{
public:
	Planet(double radius,double mass);
	~Planet();

	double mMass;
	double mRadius;
	double mSpeed[3];

	vtkSmartPointer<vtkActor> mActor;

};

