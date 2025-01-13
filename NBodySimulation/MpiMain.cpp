#include "MpiMain.h"

using namespace std;

void MpiMain::worker(int me) {
    int start;
    int end;
    int n;
    MPI_Status status;
    MPI_Recv(&start, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&end, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
    MPI_Recv(&n, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);

    int* recvBuffer = new int[n * 5];
    //int* recvBuffer;

    cout << "GOT HERE \n";

    MPI_Recv(recvBuffer, n * 5, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);

    vector<Body> bodies = convertArrayToVector(recvBuffer, n * 5);

    for (int i = start; i < end; i++)
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

    // add compute logic

    MPI_Ssend(convertVectorToArray(bodies), n * 5, MPI_INT, 0, 5, MPI_COMM_WORLD);
}

int* MpiMain::convertVectorToArray(vector<Body> bodies)
{
    int* bigArray = new int[bodies.size() * 5];  // Allocate a large array

    // Copy the contents of each smaller array into the large array
    for (int i = 0; i < bodies.size(); ++i) {
        int* serialised = bodies[i].serialiseData();
        for (int j = 0; j < 5; ++j) {
            bigArray[i * 5 + j] = serialised[j];  // Copy elements into the large array
        }
    }

    return bigArray;
}

vector<Body> MpiMain::convertArrayToVector(int* array, int size)
{
    vector<Body> bodies{};
    int* buffer = new int[5];

    for (int i = 0; i < size; i += 5)
    {
        for (int j = 0; j < 5; j++)
        {
            buffer[j] = array[i + j];
        }
        bodies.push_back(Body::deserialiseData(buffer));
    }

    return bodies;
}

void MpiMain::multipleStepsMPIMasterThread(vector<Body>& bodies, double timeFrame, int size)
{
    int numOfSteps = (int)(timeFrame / dt);
    int n = bodies.size();
    cout << size << "\n";
    // Loop over time steps
    for (int step = 0; step < numOfSteps; step++)
    {

        for (int i = 1; i < size; i++)
        {
            int start = ((i - 1) * n) / size;
            int end = (i * n) / size;

            if (i + 1 == size)
            {
                end = n;
            }

            if (end > n || start > n)
                break;

            MPI_Ssend(&start, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Ssend(&end, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
            MPI_Ssend(&n, 1, MPI_INT, i, 3, MPI_COMM_WORLD);
            cout << "vectorSum::bSend 2\n";
            MPI_Ssend(convertVectorToArray(bodies), n * 5, MPI_INT, i, 4, MPI_COMM_WORLD);
            cout << "vectorSum::bSend done\n";
        }
        
        for (int i = 1; i < size; i++)
        {
            MPI_Status status;

            int start = ((i - 1) * n) / size;
            int end = (i * n) / size;

            if (i + 1 == size)
            {
                end = n;
            }

            if (end > n || start > n)
                break;

            int* recvBuffer = new int[n * 5];
            //int* recvBuffer;
            MPI_Recv(recvBuffer, n * 5, MPI_INT, i, 5, MPI_COMM_WORLD, &status);
            vector<Body> recvBodies = convertArrayToVector(recvBuffer, n * 5);


            for (int j = start; j < end; j++)
                bodies[j] = recvBodies[j];
        }
    }
}


void MpiMain::executeMain(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

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

    // Window initialization (only for rank 0)
    if (rank == 0)
    {
        unsigned int maxWeight = 0;
        const int windowWidth = 1920;
        const int windowHeight = 1080;
        sf::RenderWindow window(sf::VideoMode({ windowWidth, windowHeight }), "N Body Simulation");

        while (window.isOpen())
        {
            while (const std::optional event = window.pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                    window.close();
            }

            window.clear(sf::Color::Black);

            double maxX = 1;
            double maxY = 1;
            double minX = bodies[0].position.x;
            double minY = bodies[0].position.y;

            for (auto obj : bodies)
            {
                if (obj.position.x > maxX) maxX = obj.position.x;
                if (obj.position.y > maxY) maxY = obj.position.y;
                if (obj.position.x < minX) minX = obj.position.x;
                if (obj.position.y < minY) minY = obj.position.y;
                maxWeight = max(maxWeight, obj.mass);
            }

            double scaleX = windowWidth / maxX;
            double scaleY = windowHeight / maxY;

            for (auto obj : bodies)
            {
                double massCoeff = ((double)obj.mass / maxWeight);
                double sideLength = 50 * massCoeff;

                if (sideLength < 5)
                    sideLength = 5;

                sf::RectangleShape square(sf::Vector2f(sideLength, sideLength));
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

            cout << "CALLED DISPLAY \n";
            window.display();
            multipleStepsMPIMasterThread(std::ref(bodies), 5, size);
        }
    }
    else
    {
        cout << "WORKER CALLED \n";
        worker(rank);
    }

    // Finalize MPI
    MPI_Finalize();
}