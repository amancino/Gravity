#pragma once
#include <vtkActor.h>
#include <vtkSmartPointer.h>

class Planet
{
public:

	static const double EARTH_MASS;
	static const double EARTH_RADIUS;

	// radius in m, mass in kg
	Planet(double radius,double mass);
	~Planet();

	void SetPosition(double* arr);
	void SetPosition(double x, double y, double z);

	inline vtkActor* const GetActor() { return mActor; }

	double mMass;
	double mRadius;
	double mSpeed[3];
	double mPosition[3];

private:
	vtkSmartPointer<vtkActor> mActor;

};

