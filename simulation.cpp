#include <iostream>

#include <QObject>

#include <vtkRenderWindow.h>

#include "simulation.h"

using namespace std;

Simulation::Simulation()//QVTKOpenGLWidget* renderWindow) 
	//: mRenderWindow(renderWindow)
{
	mTimer = new QTimer;

	// make the connection using the timer variable
	connect(mTimer, SIGNAL(timeout()), this, SLOT(NextIteration()));

	mTimer->setInterval(100);
}

Simulation::~Simulation() 
{
	cout << "Simulation destructor" << std::endl;
	mTimer->stop();

	for (unsigned int i = 0; i < mPlanets.size(); i++)
		delete mPlanets[i];
	
}

void Simulation::Start()
{
	// start the timer object by first dereferencing that object first
	mTimer->start();
	cout << "Timer started " << std::endl;
}

void Simulation::Stop()
{
	mTimer->stop();
	cout << "Timer stoped " << std::endl;
}

void Simulation::AddNewPlanet(Planet* planet)
{
	mPlanets.push_back(planet);
}

void Simulation::NextIteration()
{
	static unsigned long it = 0;
	cout << "It " << ++it << std::endl;

	bool verbose = true;

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
				mPlanets[p1]->mPosition[2] << std::endl;
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
				cout << " partial acc = " << acc << "m/s2" << std::endl;

			// add acceleration
			for (int i = 0; i < 3; i++)
				totalAcc[i] += acc * dir[i];
		}

		// compute and store velocities
		for (int i = 0; i < 3; i++)
			planet1->mSpeed[i] = planet1->mSpeed[i] + totalAcc[i] * DT;

		if (verbose)
			cout << " Total speed = " << vtkMath::Norm(planet1->mSpeed) << "m/s" << std::endl;
	}

	// update all positions at the same time
	for (unsigned int p = 0; p < mPlanets.size(); p++)
	{
		double newPos[3];
		for (int i = 0; i < 3; i++)
			newPos[i] = mPlanets[p]->mPosition[i] + mPlanets[p]->mSpeed[i] * DT;
		mPlanets[p]->SetPosition(newPos);
	}

	//mRenderWindow->GetRenderWindow()->Render();
	emit Render();
}


