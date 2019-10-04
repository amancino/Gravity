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
	const double DT = 0.1;

public:
	Planet* sun;
	Planet* earth;

	int timerId = 0;
	int maxCount = -1;
};

void TimerCallback::ComputeDisplacements()
{
	// compute distance between bodies

	double earthPos[3];
	earth->mActor->GetPosition(earthPos);
	cout << "Earth pos: " << earthPos[0] << ", " << earthPos[1] << ", " << earthPos[2] << endl;
	
	double sunPos[3];
	sun->mActor->GetPosition(sunPos);
	cout << "Sun pos: " << sunPos[0] << ", " << sunPos[1] << ", " << sunPos[2] << endl;


	double dist2 = vtkMath::Distance2BetweenPoints(earthPos, sunPos);
	double dir[3] = { 0.0 };
	for (int i = 0; i < 3; i++)
		dir[i] = sunPos[i] - earthPos[i];
	vtkMath::Normalize(dir);
	
	// compute force between bodies
	double F = 100000.0/dist2;
	cout << " F = " << F;
	
	// compute acceleration
	double earthAcc = F / earth->mMass;
	double sunAcc = F / sun->mMass;

	cout << " acc = " << earthAcc;

	// compute and store velocities
	for (int i = 0; i < 3; i++)
		earth->mSpeed[i] = earth->mSpeed[i] + earthAcc*DT*dir[i];

	cout << " speed = " << vtkMath::Norm(earth->mSpeed) << endl;

	// update position
	double newEarthPos[3];
	for (int i = 0; i < 3; i++)
		newEarthPos[i] = earthPos[i] + earth->mSpeed[i]*DT;

	earth->mActor->SetPosition(newEarthPos);
}


int main (int, char *[])
{
	Planet sun(10,100);
	Planet earth(10, 10);

  // Create a renderer and render window
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);

  // An interactor
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(sun.mActor);
  renderer->AddActor(earth.mActor);
  renderer->SetBackground(0,0,0);

  // Render
  renderWindow->Render();

	// mouse interactor
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  renderWindowInteractor->SetInteractorStyle( style );
  
	// initisal positions
	sun.mActor->SetPosition(0, 0, 0);
	earth.mActor->SetPosition(50, 0, 0);
	earth.mSpeed[1] = 10;

	// Sign up to receive TimerEvent
	auto cb = vtkSmartPointer<TimerCallback>::New();
	cb->sun = &sun;
	cb->earth = &earth;

	renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, cb);

	int timerId = renderWindowInteractor->CreateRepeatingTimer(100);
	std::cout << "timerId: " << timerId << std::endl;
	// Destroy the timer when maxCount is reached.
	cb->maxCount = 2000;
	cb->timerId = timerId;

	cout << "Starting simulation" << endl;
  // Begin mouse interaction
  renderWindowInteractor->Start();

	cout << "End of simulation" << endl;
  
  return EXIT_SUCCESS;
}