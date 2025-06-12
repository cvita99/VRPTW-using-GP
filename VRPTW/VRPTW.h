#include <bits/stdc++.h>
#include "../solver_GP/FitnessFunction.h"
using namespace std;

struct Customer {
    int id;
    pair<int, int> x;
    int demand;
    int service;
    pair<int, int> tw;
    int orderVisibleTime = 0;
};

struct EvalData{
    Customer next;
    double distance;
    int rem_q;
    double veh_time;
    double dist_unserved;
    double dist_from_depot;
    double dist_of_second_available;
};

struct solution { 
    int used_vehicles = 0;
    vector<vector<int>> routes;
    // vector<double> delay;
    vector<int> demand;
    // double totalDelay;
    vector<double> distance;
};

struct SolomonInstance {
    static const int N = 100;
    //static const int K = 15;  
    Customer customers[N+1];
    double c[N + 1][N + 1];
    int Q = 1000; 
   
};

class VRPTW : public FitnessFunction{
    public:
        double e = 2.718281;

        vector<string> trainingSetPaths = {"/c1/c101.txt", "/c1/c102.txt", "/c1/c103.txt", "/c1/c104.txt", 
            "/c2/c201.txt", "/c2/c202.txt", "/c2/c203.txt", "/c2/c204.txt",
            "/r1/r101.txt", "/r1/r102.txt", "/r1/r103.txt", "/r1/r104.txt", "/r1/r105.txt", "/r1/r106.txt", 
            "/r2/r201.txt", "/r2/r202.txt", "/r2/r203.txt", "/r2/r204.txt", "/r2/r205.txt", "/r2/r206.txt",
            "/rc1/rc101.txt", "/rc1/rc102.txt", "/rc1/rc103.txt", "/rc1/rc104.txt",
            "/rc2/rc201.txt", "/rc2/rc202.txt", "/rc2/rc203.txt", "/rc2/rc204.txt"
        }; //28 instances

        vector<string> testSetPaths = {"/c1/c105.txt", "/c1/c106.txt", "/c1/c107.txt", "/c1/c108.txt", "/c1/c109.txt", 
            "/c2/c205.txt", "/c2/c206.txt", "/c2/c207.txt", "/c2/c208.txt",
            "/r1/r107.txt", "/r1/r108.txt", "/r1/r109.txt", "/r1/r110.txt", "/r1/r111.txt", "/r1/r112.txt", 
            "/r2/r207.txt", "/r2/r208.txt", "/r2/r209.txt", "/r2/r210.txt", "/r2/r211.txt", 
            "/rc1/rc105.txt", "/rc1/rc106.txt", "/rc1/rc107.txt", "/rc1/rc108.txt",
            "/rc2/rc205.txt", "/rc2/rc206.txt", "/rc2/rc207.txt", "/rc2/rc208.txt"
        }; //28 instances


        vector<int> expectedNumOfVehicles = {10, 10, 10, 10, 3, 3, 3, 3, 18, 18, 14, 11, 15, 14, 4, 3, 4, 3, 3, 3, 16, 14, 12, 11, 4, 3, 3, 3};

        static const int N = 100; 
    
        vector<SolomonInstance> trainingSet;
        vector<SolomonInstance> testSet;

        // Customer customers[N + 1];
        // double c[N + 1][N + 1];

        VRPTW(string solomonInstanceFolder);

        double fitness(Node *expr) override;

        solution metaAlgorithmSerial(Node *expr, SolomonInstance *instance);
        solution metaAlgorithmParallel(Node *expr, SolomonInstance *instance, int vehicles, bool &success);

        // solution closestFirst(SolomonInstance *instance, Node *expr);
        // solution earliestOrUrgentFirst(SolomonInstance *instance, Node *expr, int vehicles, bool &success);

        double eval(Node *expr);
        double cost(solution sol);
        
        function<double(Node *)> fitnessFunc;

        void calculateDistancesBetweenCustomers(SolomonInstance *instance);
        void printRoutes(solution sol);
        double findNearestUnserved(int id, bool notVisited[], SolomonInstance *instance);

        EvalData evalData;

        vector<SolomonInstance> readSolomonInstances(vector<string> paths, string folder);
};