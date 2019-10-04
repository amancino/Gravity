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
	vtkActor* sunActor = nullptr;
	vtkActor* earthActor = nullptr;
	double sunMass = 1.0;
	double earthMass = 1.0;
	double earthSpeed[3] = { 0.0 };
	double sunSpeed[3] = { 0.0 };

	int timerId = 0;
	int maxCount = -1;
};

void TimerCallback::ComputeDisplacements()
{
	// compute distance between bodies

	double earthPos[3];
	earthActor->GetPosition(earthPos);
	cout << "Earth pos: " << earthPos[0] << ", " << earthPos[1] << ", " << earthPos[2] << endl;
	
	double sunPos[3];
	sunActor->GetPosition(sunPos);
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
	double earthAcc = F / earthMass;
	double sunAcc = F / sunMass;

	cout << " acc = " << earthAcc;

	// compute and store velocities
	for (int i = 0; i < 3; i++)
		earthSpeed[i] = earthSpeed[i] + earthAcc*DT*dir[i];

	cout << " speed = " << vtkMath::Norm(earthSpeed) << endl;

	// update position
	double newEarthPos[3];
	for (int i = 0; i < 3; i++)
		newEarthPos[i] = earthPos[i] + earthSpeed[i]*DT;

	earthActor->SetPosition(newEarthPos);
}


int main (int, char *[])
{
  // Sphere 1
  vtkSmartPointer<vtkSphereSource> sun = 
    vtkSmartPointer<vtkSphereSource>::New();
	sun->SetCenter(0.0, 0.0, 0.0);
	sun->SetRadius(5.0);
	sun->SetPhiResolution(32);
	sun->SetThetaResolution(32);
	sun->Update();
  
  vtkSmartPointer<vtkPolyDataMapper> mapper1 = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper1->SetInputConnection(sun->GetOutputPort());
  
  vtkSmartPointer<vtkActor> actor1 = 
    vtkSmartPointer<vtkActor>::New();
  actor1->SetMapper(mapper1);
	actor1->GetProperty()->SetColor(1,1,0);
  
  // Sphere 2
  vtkSmartPointer<vtkSphereSource> earth = 
    vtkSmartPointer<vtkSphereSource>::New();
	earth->SetCenter(0.0, 0.0, 0.0);
	earth->SetRadius(1.0);
	earth->SetPhiResolution(32);
	earth->SetThetaResolution(32);
	earth->Update();
  
  // Create a mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper2 = 
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper2->SetInputConnection(earth->GetOutputPort());

  // Create an actor
  vtkSmartPointer<vtkActor> actor2 = 
    vtkSmartPointer<vtkActor>::New();
  actor2->SetMapper(mapper2);
	actor2->GetProperty()->SetColor(0, 0, 1);

  // A renderer and render window
  vtkSmartPointer<vtkRenderer> renderer = 
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow = 
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);

  // An interactor
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = 
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(actor1);
  renderer->AddActor(actor2);
  renderer->SetBackground(1,1,1); // Background color white

  // Render
  renderWindow->Render();

  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = 
    vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); //like paraview
  
  renderWindowInteractor->SetInteractorStyle( style );
  

	// Sign up to receive TimerEvent
	auto cb = vtkSmartPointer<TimerCallback>::New();
	actor1->SetPosition(0, 0, 0);
	actor2->SetPosition(50, 0, 0);
	cb->sunActor = actor1;
	cb->earthActor = actor2;
	cb->earthMass = 10;
	cb->sunMass = 1000;
	cb->earthSpeed[1] = 10.0;

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