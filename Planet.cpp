#include "Planet.h"

#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkTexturedSphereSource.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkTransformTextureCoords.h>

#include "view.h"

using namespace std;

const double Planet::EARTH_MASS = 5.972*10e24;
const double Planet::EARTH_RADIUS = 6371*10e3;

Planet::Planet(std::string name, double radius, double mass, SurfaceTexture texture)
	: mName(name), mRadius(radius), mMass(mass)
{
	// For visualization, all bodies are scaled to earth radius unit (eru)
	// Create a sphere with texture coordinates
	vtkSmartPointer<vtkTexturedSphereSource> sphere = vtkSmartPointer<vtkTexturedSphereSource>::New();
	sphere->SetPhiResolution(32);
	sphere->SetRadius(radius / EARTH_RADIUS);
	sphere->SetThetaResolution(32);

	// mapper
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	// actor
	mActor = vtkSmartPointer<vtkActor>::New();


	if (texture != None)
	{
		string filename("C:/Users/axel/work/gravity/bin/textures/");

		switch (texture)
		{
		case Earth:
			filename += string("earth.png");
			break;

		case Sun:
			filename += string("sun.png");
			break;

		case Moon:
			filename += string("moon.png");
			break;

		default:
			break;
		}

		// Read texture file
		vtkSmartPointer<vtkImageReader2Factory> readerFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
		vtkImageReader2 *imageReader = readerFactory->CreateImageReader2(filename.c_str());
		imageReader->SetFileName(filename.c_str());

		// Create texture
		vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
		texture->SetInputConnection(imageReader->GetOutputPort());

		vtkSmartPointer<vtkTransformTextureCoords> transformTexture = vtkSmartPointer<vtkTransformTextureCoords>::New();
		transformTexture->SetInputConnection(sphere->GetOutputPort());
		transformTexture->SetPosition(0, 0, 0);

		mapper->SetInputConnection(transformTexture->GetOutputPort());
		mActor->SetTexture(texture);

		imageReader->Delete();
	}
	else
		mapper->SetInputConnection(sphere->GetOutputPort());


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
	cout << "Planet destructor: " << mName << std::endl;
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