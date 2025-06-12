#include "VRPTW.h"
#include "../solver_GP/GeneticProgramming.h"
#include <bits/stdc++.h>
using namespace std;

void serial();
void parallel();

int main(){
    auto start = chrono::high_resolution_clock::now();

    for(int i = 0; i < 10; i++){
        printf("################################");
        printf("################################");
        printf("################################");
        printf("################################");
        printf("################################");
        printf("RUN %d", i);
        serial();
    }
    

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Program ran for " << elapsed.count() << " seconds.\n";

    return 0;
}

void serial(){
    VRPTW *vrptw = new VRPTW("./solomonInstances_notDyn");

    vrptw->fitnessFunc = [vrptw](Node* expr) -> double {
            double sum = 0;
            for(int i=0; i < vrptw->trainingSet.size(); i++){
                sum += vrptw->cost(vrptw->metaAlgorithmSerial(expr, &vrptw->trainingSet[i]));
            }
            return sum / vrptw->trainingSet.size();
    };

    GeneticProgramming *solver = new GeneticProgramming(5, 200, 20000, 0.2, vrptw);
    solver->functions = { "+", "*", "/", "-", "min", "max"};
    solver->terminals = {"d", "rt", "dt", "dem", "t", "q", "t_rem", "d_depot", "wt", "slack_t", "t_ready"}; 
    solver->run();

    cout << "[Training fitness: " << vrptw->fitness(solver->best) << "]\n";

    double testSetFitness = 0;
    for(int i=0; i < vrptw->testSet.size(); i++){
        cout << vrptw->trainingSetPaths[i] << endl;
        solution sol = vrptw->metaAlgorithmSerial(solver->best, &vrptw->testSet[i]);
        testSetFitness += vrptw->cost(sol);
        vrptw->printRoutes(sol);
    }

    cout << "[Training fitness: " << vrptw->fitness(solver->best) << "]\n";
    cout << "[Test fitness: " << testSetFitness / vrptw->testSet.size() << "]\n";
}

void parallel(){
    VRPTW *vrptw = new VRPTW("./solomonInstances_dynamic");

    vrptw->fitnessFunc = [vrptw](Node* expr) -> double {
        double sum = 0;
        for (int i = 0; i < vrptw->trainingSet.size(); i++) {
            int veh = vrptw->expectedNumOfVehicles[i];
            bool flag = false;
            solution sol;
            while (!flag) {
                veh++;
                sol = vrptw->metaAlgorithmParallel(expr, &vrptw->trainingSet[i], veh, flag);
            }
            sum += vrptw->cost(sol);
        }
        return sum / vrptw->trainingSet.size();
    };

    GeneticProgramming *solver = new GeneticProgramming(5, 200, 6000, 0.2, vrptw);
    solver->functions = { "+", "*", "/", "-", "min", "max"};
    solver->terminals = {"d", "rt", "dt", "dem", "t", "q", "t_rem", "d_depot", "wt", "slack_t", "t_ready", "next_veh"}; 
    solver->run();

    double testSetFitness = 0;
    for(int i=0; i < vrptw->testSet.size(); i++){
        cout << vrptw->testSetPaths[i] << endl;
        int veh = vrptw->expectedNumOfVehicles[i];
        bool flag = false;
        solution sol;
        while(!flag){
            veh++;
            sol = vrptw->metaAlgorithmParallel(solver->best, &vrptw->testSet[i], veh, flag);
            
        }
        testSetFitness += vrptw->cost(sol);
        vrptw->printRoutes(sol);
    }

    cout << "[Training fitness: " << vrptw->fitness(solver->best) << "]\n";
    cout << "[Test fitness: " << testSetFitness / vrptw->testSet.size() << "]\n";
}

void cloesetFirst(){
    VRPTW *vrptw = new VRPTW("./solomonInstances_notDyn");
    Node *closestFirst = new Node("d");

    double testSetFitness = 0;
    for(int i=0; i < vrptw->testSet.size(); i++){
        cout << vrptw->trainingSetPaths[i] << endl;
        solution sol = vrptw->metaAlgorithmSerial(closestFirst, &vrptw->testSet[i]);
        testSetFitness += vrptw->cost(sol);
        vrptw->printRoutes(sol);
    }

    cout << "Closest First" << endl; 
    cout << "[Training fitness: " << vrptw->fitness(closestFirst) << "]\n";
    cout << "[Test fitness: " << testSetFitness / vrptw->testSet.size() << "]\n";
}

void urgentOrEarliestFirst(){
    VRPTW *vrptw = new VRPTW("./solomonInstances_notDyn");
    Node *urgentFirst = new Node("slack_t");
    Node *earliestFirst = new Node("rt");

    string earlyOrUrgent = "early"; //urget

    double sum = 0;
    for(int i=0; i < vrptw->testSet.size(); i++){
        cout << vrptw->trainingSetPaths[i] << endl;
        int veh = vrptw->expectedNumOfVehicles[i];
        bool flag = false;
        solution sol;
        while(!flag){
            veh++;
            if(earlyOrUrgent == "early"){
                sol = vrptw->metaAlgorithmParallel(earliestFirst, &vrptw->testSet[i], veh, flag);
            } else {
                sol = vrptw->metaAlgorithmParallel(urgentFirst, &vrptw->testSet[i], veh, flag);
            }
        }

        sum+=vrptw->cost(sol);
        vrptw->printRoutes(sol);
    }

    cout << "COST: " << sum / vrptw->testSet.size() << endl;
}

// best after gp serial: 
// best after gp: 
// (((((t - t_ready) / (t_ready max dem)) * ((d max t) * (wt / slack_t))) + ((wt max t_rem) max ((q + t_ready) / (t max dem)))) * (wt + d))
// best fitness: 89465.4

// Training Set

//     Average number of vehicles: 7.64

//     Average distance: 1075.74

//     Average fitness: 77,504.32

// Test Set

//     Average number of vehicles: 6.82

//     Average distance: 966.64

//     Average fitness: 69,180.93


// best after gp parallel: 
// ((d - (((d_depot - t_rem) min (wt * dt)) min ((d - wt) * d_depot))) + (t + d))
// best fitness: 115812

// d - distance of vehicle to customer
// rt - customer ready time, dt - customer due time
// dem - demand, st - service time, t - current time
// q - remaining vehicle capacity
// t_rem - time remaining until being late
// d_depot - distance of customer to depot
// wt - Waiting time for vehicle until customer j becomes ready
// slack_t - Slack (time until late) of vehicle for customer j
// t_ready - time until customer j is ready
// near_veh - second nearest vehicle O(n)
// next_veh - next_available_veh O(1)

