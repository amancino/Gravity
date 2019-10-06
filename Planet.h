#pragma once
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkLineSource.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>

class Planet
{
public:

	static const double EARTH_MASS;
	static const double EARTH_RADIUS;

	// radius in m, mass in kg
	Planet(std::string name, double radius,double mass);
	~Planet();

	void SetPosition(double* arr, bool computeOrbit=true);
	void SetPosition(double x, double y, double z, bool computeOrbit=true);

	inline vtkActor* GetActor() { return mActor; }

	double mMass;
	double mRadius;
	double mSpeed[3];
	double mPosition[3];
	std::string mName;

	
	vtkSmartPointer<vtkActor> mOrbitActor;

protected:
	void UpdateOrbit();

private:
	vtkSmartPointer<vtkActor> mActor;
	
	vtkSmartPointer<vtkPolyData>				mOrbit;
	vtkSmartPointer <vtkPoints>					mOrbitPoints;

};

