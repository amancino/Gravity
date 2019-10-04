#pragma once
class vtkActor;

class Planet
{
public:
	Planet(double radius,double mass);
	~Planet();



	double mMass;
	double mRadius;


private:
	vtkActor* mActor;
};

