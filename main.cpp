#include <iostream>

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkProperty.h>
#include <vtkCamera.h>


#include "Planet.h"
using namespace std;

const int DELAY_MS = 100;

class TimerCallback : public vtkCommand
{
public:
	TimerCallback() = default;

	static TimerCallback* New()
	{
		TimerCallback* cb = new TimerCallback;
		cb->TimerCount = 0;
		return cb;
	}

	void ComputeDisplacements();

	virtual void Execute(vtkObject* caller, unsigned long eventId,
		void* vtkNotUsed(callData))
	{
		if (vtkCommand::TimerEvent == eventId)
			++this->TimerCount;
		
		std::cout << this->TimerCount << std::endl;

		ComputeDisplacements();

		if (this->TimerCount < this->maxCount)
		{

			vtkRenderWindowInteractor* iren =
				dynamic_cast<vtkRenderWindowInteractor*>(caller);
			iren->GetRenderWindow()->Render();
		}
		else
		{
			vtkRenderWindowInteractor* iren =
				dynamic_cast<vtkRenderWindowInteractor*>(caller);
			if (this->timerId > -1)
			{
				iren->DestroyTimer(this->timerId);
			}
		}
	}

private:
	int TimerCount = 0;
	const double DT = 60*60*24;		// time resolution

public:

	void AddNewPlanet(Planet* planet) 
	{
		mPlanets.push_back(planet);
	}

	std::vector<Planet*> mPlanets;

	int timerId = 0;
	int maxCount = -1;
};

void TimerCallback::ComputeDisplacements()
{
	bool verbose = false;

	// Newton's law of universal gravitation
	const double G = 6.67430e-11;

	// iterate through all bodies
	for (unsigned int p1 = 0; p1 < mPlanets.size(); p1++)
	{
		if (verbose)
		{
			cout << std::setprecision(9) <<
				mPlanets[p1]->mName << " position: " <<
				mPlanets[p1]->mPosition[0] << ", " <<
				mPlanets[p1]->mPosition[1] << ", " <<
				mPlanets[p1]->mPosition[2] << endl;
		}
		Planet* planet1 = mPlanets[p1];
		double totalAcc[3] = { 0.0 };

		// for each body, compute the forces to other existing bodies and afterwards change its position
		for (unsigned int p2 = 0; p2 < mPlanets.size(); p2++)
		{
			if (p1 == p2)
				continue;

			Planet* planet2 = mPlanets[p2];
			// to do: avoid computing twice the same forces

			// compute distance between bodies
			double dist2 = vtkMath::Distance2BetweenPoints(planet1->mPosition, planet2->mPosition);
			double dir[3] = { 0.0 };
			for (int i = 0; i < 3; i++)
				dir[i] = planet2->mPosition[i] - planet1->mPosition[i];
			vtkMath::Normalize(dir);

			// compute force between bodies
			const double m1 = planet1->mMass;
			const double m2 = planet2->mMass;
			double F = G * m1*m2 / dist2;
			
			if (verbose)
				cout << " F = " << F << " N";

			// compute acceleration
			double acc = F / m1;
			if (verbose)
				cout << " partial acc = " << acc << "m/s2" << endl;

			// add acceleration
			for (int i = 0; i < 3; i++)
				totalAcc[i] += acc * dir[i];
		}

		// compute and store velocities
		for (int i = 0; i < 3; i++)
			planet1->mSpeed[i] = planet1->mSpeed[i] + totalAcc[i] * DT;
		
		if (verbose)
			cout << " Total speed = " << vtkMath::Norm(planet1->mSpeed) << "m/s" << endl;
	}

	// update all positions at the same time
	for (unsigned int p = 0; p < mPlanets.size(); p++)
	{
		double newPos[3];
		for (int i = 0; i < 3; i++)
			newPos[i] = mPlanets[p]->mPosition[i] + mPlanets[p]->mSpeed[i] * DT;
		mPlanets[p]->SetPosition(newPos);
	}
}


int main (int, char *[])
{
  // Create a renderer and render window
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);

  // An interactor
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

	// Add planets
	Planet sun("Sun", 6.95700e8, 2e30);
	// make earth and moon 10 times bigger
	Planet earth("Earth", 10 * 6.371e6,	5.972e24);
	Planet moon("Moon",		10 * 1.7371e6, 7.34767309e22);

  // Add the actors to the scene
  renderer->AddActor(sun.GetActor());
  renderer->AddActor(earth.GetActor());
	renderer->AddActor(moon.GetActor());
	renderer->AddActor(earth.mOrbitActor);
	renderer->AddActor(moon.mOrbitActor);
	renderer->AddActor(sun.mOrbitActor);

	sun.GetActor()->GetProperty()->SetColor(1, 1, 0);
	earth.GetActor()->GetProperty()->SetColor(0, 0.6, 1);
	moon.GetActor()->GetProperty()->SetColor(0.6, 0.6, 0.6);
	moon.mOrbitActor->GetProperty()->SetColor(1, 0, 0);
	sun.mOrbitActor->GetProperty()->SetColor(1, 1, 0);

  // Render
	renderer->SetBackground(0, 0, 0);

	renderWindow->Render();

	

	// mouse interactor
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  renderWindowInteractor->SetInteractorStyle( style );
  
	// initial positions
	sun.SetPosition(0, 0, 0, false);
	earth.SetPosition(149.6e9, 0, 0, false);
	moon.SetPosition(earth.mPosition[0] + 3.844e8, 0, 0, false);
	
	// 30 km per second
	earth.mSpeed[1] = 30.0e3;
	moon.mSpeed[1] = earth.mSpeed[1] - 1023.056;

	// Sign up to receive TimerEvent
	auto cb = vtkSmartPointer<TimerCallback>::New();
	cb->AddNewPlanet(&sun);
	cb->AddNewPlanet(&earth);
	cb->AddNewPlanet(&moon);

	// earth as the center of the universe
	auto camera = renderer->GetActiveCamera();
	camera->SetPosition(earth.mPosition[0]/Planet::EARTH_RADIUS, 0, 100);
	camera->SetFocalPoint(earth.mPosition[0] / Planet::EARTH_RADIUS, 0, 0);
	camera->SetViewUp(0, 1, 0);
	renderer->ResetCameraClippingRange();
	
	renderer->Render();
	renderWindow->Render();

	renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, cb);

	int timerId = renderWindowInteractor->CreateRepeatingTimer(DELAY_MS);
	std::cout << "timerId: " << timerId << std::endl;
	// Destroy the timer when maxCount is reached.
	cb->maxCount = 365;
	cb->timerId = timerId;

	cout << "Starting simulation" << endl;
  // Begin mouse interaction
  renderWindowInteractor->Start();

	cout << "End of simulation" << endl;
  
  return EXIT_SUCCESS;
}