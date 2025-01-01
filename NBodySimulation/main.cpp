#include <iostream>
#include <vector>
#include <thread>
#include "Body.h"

#include <SFML/Graphics.hpp>

using namespace std;

const double G = 0.000000000066743;
//const double dt = 0.0001;
const double dt = 0.1;
double elapsedTime = 0;

void oneStep(vector<Body>& bodies)
{
	for (int i = 0; i < bodies.size(); i++)
	{
		for (int j = 0; j < bodies.size(); j++)
		{
			if (i != j)
			{
				Vector2D forceDirection = (bodies[j].position - bodies[i].position).normalize();
				double r = forceDirection.magnitude();
				double forceMagnitude = (G * bodies[i].mass * bodies[j].mass) / (r * r);
				Vector2D totalForce = forceDirection * forceMagnitude;
				Vector2D acceleration = totalForce / bodies[i].mass;

				bodies[i].velocity = bodies[i].velocity + (acceleration * dt);
				bodies[i].position = bodies[i].position + (bodies[i].velocity * dt);
			}
		}
	}
}

void oneStepWorkerThread(vector<Body>& bodies, int start, int end)
{
	for (int i = start; i < end; ++i)
	{
		for (int j = 0; j < bodies.size(); ++j)
		{
			if (i != j)
			{
				Vector2D forceDirection = (bodies[j].position - bodies[i].position).normalize();
				double r = forceDirection.magnitude();
				double forceMagnitude = (G * bodies[i].mass * bodies[j].mass) / (r * r);
				Vector2D totalForce = forceDirection * forceMagnitude;
				Vector2D acceleration = totalForce / bodies[i].mass;

				bodies[i].velocity = bodies[i].velocity + (acceleration * dt);
				bodies[i].position = bodies[i].position + (bodies[i].velocity * dt);
			}
		}
	}
}

void oneStepParalelised(vector<Body>& bodies, int nrOfThreads)
{
	std::vector<std::thread> threads;
	int chunkSize = bodies.size() / nrOfThreads;

	for (int i = 0; i < nrOfThreads; ++i)
	{
		int start = i * chunkSize;
		int end = (i == nrOfThreads - 1) ? bodies.size() : (i + 1) * chunkSize;
		threads.push_back(std::thread(oneStepWorkerThread, std::ref(bodies), start, end));
	}

	for (auto& thread : threads)
	{
		thread.join();
	}
}

void multipleSteps(vector<Body>& bodies, double timeFrame)
{
	int numOfSteps = (int) (timeFrame / dt);
	for (int i = 0; i < numOfSteps; i++)
		oneStep(bodies);
}

void multipleStepsParalelised(vector<Body>& bodies, double timeFrame, int threads)
{
	int numOfSteps = (int)(timeFrame / dt);
	for (int i = 0; i < numOfSteps; i++)
	{
		oneStepParalelised(bodies, threads);
		//oneStep(bodies);
	}
	cout << "Done all steps " << numOfSteps << "\n";
}

int main()
{	
	/*vector<Body> bodies = {
		Body{30000, Vector2D{0, 0}, Vector2D{0, 0}},
		Body{20000, Vector2D{50000, 0}, Vector2D{0, 0}},
		Body{20000, Vector2D{0, 50000}, Vector2D{0, 0}}
	};*/

	vector<Body> bodies = {
		Body{30000, Vector2D{0, 0}, Vector2D{0, 0}},
		Body{20000, Vector2D{50000, 0}, Vector2D{0, 0}},
		Body{20000, Vector2D{0, 50000}, Vector2D{0, 0}},
		Body{20000, Vector2D{0, 25000}, Vector2D{0, 0}},
		Body{20000, Vector2D{25000, 50000}, Vector2D{0, 0}},
		Body{20000, Vector2D{15000, 20000}, Vector2D{0, 0}},
		Body{20000, Vector2D{40000, 18000}, Vector2D{0, 0}},
		Body{20000, Vector2D{5000, 6000}, Vector2D{0, 0}}
	};


	unsigned int maxWeight = 0;
	const int windowWidth = 1920;
	const int windowHeight = 1080;
	sf::RenderWindow window(sf::VideoMode({ windowWidth, windowHeight }), "N Body Simulation");

	// run the program as long as the window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		while (const std::optional event = window.pollEvent())
		{
			// "close requested" event: we close the window
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		// clear the window with black color
		window.clear(sf::Color::Black);

		double maxX = 1;
		double maxY = 1;

		double minX = bodies[0].position.x;
		double minY = bodies[0].position.y;

		for (auto obj : bodies)
		{
			if (obj.position.x > maxX)
				maxX = obj.position.x;

			if (obj.position.y > maxY)
				maxY = obj.position.y;

			if (obj.position.x < minX)
				minX = obj.position.x;

			if (obj.position.y < minY)
				minY = obj.position.y;

			maxWeight = max(maxWeight, obj.mass);
		}

		double scaleX = windowWidth / maxX;
		double scaleY = windowHeight / maxY;

		for (auto obj : bodies) {
			double massCoeff = ((double) obj.mass / maxWeight);
			double sideLength = 50 * massCoeff;
			
			if (sideLength < 5)
				sideLength = 5;

			sf::RectangleShape square(sf::Vector2f(sideLength, sideLength));
			// Map the object's position to the window coordinates
			sf::Vector2f position((obj.position.x * scaleX), (obj.position.y * scaleY));

			if (position.x <= 0.001)
				position.x += 30;

			if (position.x - 1920 <= 0.001)
				position.x -= 30;

			if (position.y <= 0.001)
				position.y += 30;

			if (position.y - 1080 <= 0.001)
				position.y -= 30;

			square.setPosition(position);
			square.setFillColor(sf::Color::Blue);
			window.draw(square);
		}

		window.display();
		
		//multipleSteps(bodies, 500);
		multipleStepsParalelised(bodies, 500, 3);
		cout << "DOne one pass \n";
	}

	return 0;
}
