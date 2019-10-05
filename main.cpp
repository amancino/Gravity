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

#include "Planet.h"
using namespace std;


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
	Planet* sun;
	Planet* earth;

	int timerId = 0;
	int maxCount = -1;
};

void TimerCallback::ComputeDisplacements()
{
	// compute distance between bodies
	cout << "Earth pos: " << earth->mPosition[0] << ", " << earth->mPosition[1] << ", " << earth->mPosition[2] << endl;
	cout << "Sun pos: " << sun->mPosition[0] << ", " << sun->mPosition[1] << ", " << sun->mPosition[2] << endl;


	double dist2 = vtkMath::Distance2BetweenPoints(earth->mPosition, sun->mPosition);
	double dir[3] = { 0.0 };
	for (int i = 0; i < 3; i++)
		dir[i] = sun->mPosition[i] - earth->mPosition[i];
	vtkMath::Normalize(dir);
	
	// compute force between bodies
	// Newton's law of universal gravitation
	const double G = 6.67430e-11;
	const double m1 = earth->mMass;
	const double m2 = sun->mMass;
	double F = G * m1*m2/dist2;
	cout << " F = " << F << " N";
	
	// compute acceleration
	double earthAcc = F / earth->mMass;
	double sunAcc = F / sun->mMass;

	cout << " acc = " << earthAcc << "m/s2";

	// compute and store velocities
	for (int i = 0; i < 3; i++)
		earth->mSpeed[i] = earth->mSpeed[i] + earthAcc*DT*dir[i];

	cout << " speed = " << vtkMath::Norm(earth->mSpeed) << "m/s" << endl;

	// update position
	double newEarthPos[3];
	for (int i = 0; i < 3; i++)
		newEarthPos[i] = earth->mPosition[i] + earth->mSpeed[i]*DT;

	earth->SetPosition(newEarthPos);
}


int main (int, char *[])
{
	Planet sun(6.95700e8, 2e30);

	// make earth 100 times bigger
	Planet earth(100* 6.371e6, 5.972e24);

  // Create a renderer and render window
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);

  // An interactor
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(sun.GetActor());
  renderer->AddActor(earth.GetActor());
  renderer->SetBackground(0,0,0);

  // Render
  renderWindow->Render();

	// mouse interactor
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  renderWindowInteractor->SetInteractorStyle( style );
  
	// initial positions
	sun.SetPosition(0, 0, 0);
	earth.SetPosition(149.6e9, 0, 0);
	
	// 30 km per second
	earth.mSpeed[1] = 30.0e3;

	// Sign up to receive TimerEvent
	auto cb = vtkSmartPointer<TimerCallback>::New();
	cb->sun = &sun;
	cb->earth = &earth;

	renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, cb);

	int timerId = renderWindowInteractor->CreateRepeatingTimer(100);
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