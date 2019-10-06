#include "Planet.h"

#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

using namespace std;

const double Planet::EARTH_MASS = 5.972*10e24;
const double Planet::EARTH_RADIUS = 6371*10e3;

Planet::Planet(std::string name, double radius, double mass)
	: mName(name), mRadius(radius), mMass(mass)
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

	for (int i = 0; i < 3; i++)
		mSpeed[i] = 0.0;

	// orbit
	mOrbit = vtkSmartPointer<vtkPolyData>::New();
	mOrbitPoints = vtkSmartPointer <vtkPoints>::New();
	mOrbitActor = vtkSmartPointer<vtkActor>::New();
	auto orbitMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	orbitMapper->SetInputData(mOrbit);
	mOrbitActor->SetMapper(orbitMapper);
}


Planet::~Planet()
{
}

void Planet::SetPosition(double* arr, bool computeOrbit)
{
	SetPosition(arr[0], arr[1], arr[2], computeOrbit);
}

void Planet::SetPosition(double x, double y, double z, bool computeOrbit)
{
	// real position
	mPosition[0] = x;
	mPosition[1] = y;
	mPosition[2] = z;

	// position in visualization scaled to earth radius
	mActor->SetPosition(x / EARTH_RADIUS, y / EARTH_RADIUS, z / EARTH_RADIUS);

	// compute orbit
	if (computeOrbit)
	{
		mOrbitPoints->InsertNextPoint(mActor->GetPosition());
		if (mOrbitPoints->GetNumberOfPoints() > 2)
			UpdateOrbit();
	}
}

void Planet::UpdateOrbit()
{
	vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
	unsigned long nP = mOrbitPoints->GetNumberOfPoints();
	lines->InsertNextCell(nP);
	for (unsigned long i = 0; i < nP; i++)
	{
		lines->InsertCellPoint(i);
	}
	
	mOrbit->SetPoints(mOrbitPoints);
	mOrbit->SetLines(lines);

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(mOrbit);
	mOrbitActor->SetMapper(mapper);

	//cout << "Updated orbit. Number of points: " << nP << endl;
}