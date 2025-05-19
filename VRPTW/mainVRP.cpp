#include "VRPTW.h"
#include "../solver_GP/GeneticProgramming.h"
#include <bits/stdc++.h>
using namespace std;

void serial();
void parallel();

int main(){
    auto start = chrono::high_resolution_clock::now();

    serial();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Program ran for " << elapsed.count() << " seconds.\n";

    return 0;
}

void serial(){
    VRPTW *vrptw = new VRPTW();

    vrptw->fitnessFunc = [vrptw](Node* expr) -> double {
            double sum = 0;
            for(int i=0; i < vrptw->solomonInstances.size(); i++){
                sum += vrptw->cost(vrptw->metaAlgorithmSerial(expr, &vrptw->solomonInstances[i]));
            }
            return sum / vrptw->solomonInstances.size();
    };

    GeneticProgramming *solver = new GeneticProgramming(5, 200, 20000, 0.2, vrptw);
    solver->functions = { "+", "*", "/", "-", "min", "max"};
    solver->terminals = {"d", "rt", "dt", "dem", "t", "q", "t_rem", "d_depot", "wt", "slack_t", "t_ready"}; 
    solver->run();
    for(int i=0; i < vrptw->solomonInstances.size(); i++){
        cout << vrptw->trainingSetPaths[i] << endl;
        solution sol = vrptw->metaAlgorithmSerial(solver->best, &vrptw->solomonInstances[i]);
        vrptw->printRoutes(sol);
    }
}

void parallel(){
    VRPTW *vrptw = new VRPTW();

    vrptw->fitnessFunc = [vrptw](Node* expr) -> double {
        double sum = 0;
        for (int i = 0; i < vrptw->solomonInstances.size(); i++) {
            int veh = vrptw->expectedNumOfVehicles[i];
            bool flag = false;
            solution sol;
            while (!flag) {
                veh++;
                sol = vrptw->metaAlgorithmParallel(expr, &vrptw->solomonInstances[i], veh, flag);
            }
            sum += vrptw->cost(sol);
        }
        return sum / vrptw->solomonInstances.size();
    };

    GeneticProgramming *solver = new GeneticProgramming(5, 200, 6000, 0.5, vrptw);
    solver->functions = { "+", "*", "/", "-", "min", "max"};
    solver->terminals = {"d", "rt", "dt", "dem", "t", "q", "t_rem", "d_depot", "wt", "slack_t", "t_ready", "next_veh"}; 
    solver->run();
    for(int i=0; i < vrptw->solomonInstances.size(); i++){
        cout << vrptw->trainingSetPaths[i] << endl;
        int veh = vrptw->expectedNumOfVehicles[i];
        bool flag = false;
        solution sol;
        while(!flag){
            veh++;
            sol = vrptw->metaAlgorithmParallel(solver->best, &vrptw->solomonInstances[i], veh, flag);
            
        }
        vrptw->printRoutes(sol);
    }
}

void cloesetFirst(){
    VRPTW *vrptw = new VRPTW();
    Node *closestFirst = new Node("d");

    double sum = 0;
    for(int i=0; i < vrptw->solomonInstances.size(); i++){
        cout << vrptw->trainingSetPaths[i] << endl;
        solution sol = vrptw->closestFirst(&vrptw->solomonInstances[i], closestFirst);
        sum += vrptw->cost(sol);
        vrptw->printRoutes(sol);
    }

    cout << "COST: " << sum / vrptw->solomonInstances.size() << endl; 
}

void urgentOrEarliestFirst(){
    VRPTW *vrptw = new VRPTW();
    Node *urgentFirst = new Node("slack_t");
    Node *earliestFirst = new Node("rt");

    double sum = 0;
    for(int i=0; i < vrptw->solomonInstances.size(); i++){
        cout << vrptw->trainingSetPaths[i] << endl;
        int veh = vrptw->expectedNumOfVehicles[i];
        bool flag = false;
        solution sol;
        while(!flag){
            veh++;
            sol = vrptw->earliestOrUrgentFirst(&vrptw->solomonInstances[i], earliestFirst, veh, flag);
        }
        sum+=vrptw->cost(sol);
        vrptw->printRoutes(sol);
    }

    cout << "COST: " << sum / vrptw->solomonInstances.size() << endl;
}

// best after gp serial: 
// best after gp: 
// (((((t - t_ready) / (t_ready max dem)) * ((d max t) * (wt / slack_t))) + ((wt max t_rem) max ((q + t_ready) / (t max dem)))) * (wt + d))
// best fitness: 89465.4

// Mean best-known cost across training set: 81349.12
// Mean best-known cost across test set: 72151.80

// Training Set:
//     Average number of vehicles: ~7.96
//     Average distance: ~1072.70

// Test Set:
//     Average number of vehicles: ~7.82
//     Average distance: ~987.56


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