#pragma once

#include <vector>

#include <QVTKOpenGLWidget.h>

#include <QObject>
#include <QTimer>

#include "Planet.h"

class Simulation : public QObject
{
	Q_OBJECT

public:
	Simulation(QVTKOpenGLWidget* renderWindow);
	~Simulation();

	const double DT = 60 * 60 * 24;		// time resolution

	void Start();

	void Stop();

	void AddNewPlanet(Planet* planet);

protected slots:
	void NextIteration();

private:

	QVTKOpenGLWidget* mRenderWindow;
	QTimer* mTimer;

	std::vector<Planet*> mPlanets;
};