#include "VRPTW.h"
#include "../solver_GP/GeneticProgramming.h"
#include <bits/stdc++.h>
using namespace std;

void serial();
void parallel();
void cloesetFirst();
void urgentOrEarliestFirst();

Node *funcForBest();

Node* funcForBest() {
    Node* expr1 = new Node("max");
    expr1->leftChild = new Node("q");
    expr1->rightChild = new Node("slack_t");
    
    Node* expr2 = new Node("-");
    expr2->leftChild = new Node("d");
    expr2->rightChild = new Node("q");
    
    Node* expr3 = new Node("*");
    expr3->leftChild = expr1;
    expr3->rightChild = expr2;
    
    Node* expr4 = new Node("*");
    expr4->leftChild = expr3;
    expr4->rightChild = new Node("q");
    
    // Right side of the denominator
    Node* expr5 = new Node("*");
    expr5->leftChild = new Node("slack_t");
    expr5->rightChild = new Node("dt");
    
    Node* expr6 = new Node("*");
    expr6->leftChild = new Node("t_rem");
    expr6->rightChild = new Node("q");
    
    Node* expr7 = new Node("*");
    expr7->leftChild = expr5;
    expr7->rightChild = expr6;
    
    Node* expr8 = new Node("+");
    expr8->leftChild = new Node("d");
    expr8->rightChild = new Node("wt");
    
    Node* expr9 = new Node("*");
    expr9->leftChild = expr8;
    expr9->rightChild = new Node("t_rem");
    
    Node* expr10 = new Node("min");
    expr10->leftChild = expr7;
    expr10->rightChild = expr9;
    
    // Final division
    Node* root = new Node("/");
    root->leftChild = expr4;
    root->rightChild = expr10;

    return root;
}

int main(){
    auto start = chrono::high_resolution_clock::now();

    for(int i = 0; i < 5; i++){
        printf("################################\n");
        printf("################################\n");
        printf("################################\n");
        printf("RUN %d\n", i);
        parallel();
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Elapsed time: " << fixed << setprecision(3)
              << elapsed.count() << " seconds" << endl;

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

    vrptw->fitnessFuncTest = [vrptw](Node* expr) -> double {
        double sum = 0;
        for(int i=0; i < vrptw->testSet.size(); i++){
            sum += vrptw->cost(vrptw->metaAlgorithmSerial(expr, &vrptw->testSet[i]));
        }
        return sum / vrptw->testSet.size();
    };

    GeneticProgramming *solver = new GeneticProgramming(4, 200, 20000, 0.2, vrptw);
    solver->functions = { "+", "*", "/", "-", "min", "max"};
    solver->terminals = {"d", "rt", "dt", "dem", "t", "q", "t_rem", "d_depot", "wt", "slack_t", "t_ready"}; 
    solver->run();

    cout << "[Training fitness: " << vrptw->fitness(solver->best) << "]\n";

    double testSetFitness = 0;
    for(int i=0; i < vrptw->testSet.size(); i++){
        cout << vrptw->testSetPaths[i] << endl;
        solution sol = vrptw->metaAlgorithmSerial(solver->best, &vrptw->testSet[i]);
        testSetFitness += vrptw->cost(sol);
        vrptw->printRoutes(sol);
    }

    cout << "[Training fitness: " << vrptw->fitness(solver->best) << "]\n";
    cout << "[Test fitness: " << testSetFitness / vrptw->testSet.size() << "]\n";
}

void parallel(){
    VRPTW *vrptw = new VRPTW("./solomonInstances_notDyn"); 

    vrptw->fitnessFunc = [vrptw](Node* expr) -> double {
        double sum = 0;
        for (int i = 0; i < vrptw->trainingSet.size(); i++) {
            int veh = vrptw->expectedNumOfVehiclesTrain[i];
            bool flag = false;
            solution sol;
            while (!flag) {
                sol = vrptw->metaAlgorithmParallel(expr, &vrptw->trainingSet[i], veh, flag);
                if(veh > vrptw->N){
                    cout << "nemoguce obici sve kupce na vrijeme" << endl;
                    cout << vrptw->trainingSetPaths[i] << endl;
                    exit(1);
                }
                veh++;
            }
            sum += vrptw->cost(sol);
        }
        return sum / vrptw->trainingSet.size();
    };

    vrptw->fitnessFuncTest = [vrptw](Node* expr) -> double {
        double testSetFitness = 0;
        for(int i=0; i < vrptw->testSet.size(); i++){
            int veh = vrptw->expectedNumOfVehiclesTest[i];
            bool flag = false;
            solution sol;
            while(!flag){
                sol = vrptw->metaAlgorithmParallel(expr, &vrptw->testSet[i], veh, flag);
                if(veh > vrptw->N){
                    cout << "nemoguce obici sve kupce na vrijeme" << endl;
                    exit(1);
                }
                veh++;
            }
            testSetFitness += vrptw->cost(sol);
        }
        return testSetFitness / vrptw->testSet.size();
    };

    GeneticProgramming *solver = new GeneticProgramming(4, 200, 3000, 0.2, vrptw);
    solver->functions = { "+", "*", "/", "-", "min", "max"};
    solver->terminals = {"d", "rt", "dt", "dem", "t", "q", "t_rem", "d_depot", "wt", "slack_t", "t_ready", "next_veh"}; 
    solver->run();

    double testSetFitness = 0;
    for(int i=0; i < vrptw->testSet.size(); i++){
        cout << vrptw->testSetPaths[i] << endl;
        int veh = vrptw->expectedNumOfVehiclesTest[i];
        bool flag = false;
        solution sol;
        while(!flag){
            sol = vrptw->metaAlgorithmParallel(solver->best, &vrptw->testSet[i], veh, flag);
            veh++;
        }
        testSetFitness += vrptw->cost(sol);
        vrptw->printRoutes(sol);
    }

    cout << "[Training fitness: " << vrptw->fitness(solver->best) << "]\n";
    cout << "[Test fitness: " << testSetFitness / vrptw->testSet.size() << "]\n";
}

void cloesetFirst(){
    VRPTW *vrptw = new VRPTW("./solomonInstances_notDyn");
    Node *closestFirstTree = new Node("d");

    double testSetFitness = 0;
    for(int i=0; i < vrptw->testSet.size(); i++){
        cout << vrptw->testSetPaths[i] << endl;
        solution sol = vrptw->metaAlgorithmSerial(closestFirstTree, &vrptw->testSet[i]);
        testSetFitness += vrptw->cost(sol);
        vrptw->printRoutes(sol);
    }

    cout << "Closest First" << endl; 
    //cout << "[Training fitness: " << vrptw->fitness(closestFirstTree) << "]\n";
    cout << "[Test fitness: " << testSetFitness / vrptw->testSet.size() << "]\n";
}

void urgentOrEarliestFirst(){
    VRPTW *vrptw = new VRPTW("./solomonInstances_notDyn");
    Node *urgentFirst = new Node("slack_t");
    Node *earliestFirst = new Node("rt");

    string earlyOrUrgent = "urgent"; //urget
    cout << earlyOrUrgent << endl;

    double sum = 0;
    for(int i=0; i < vrptw->trainingSetPaths.size(); i++){
        cout << vrptw->trainingSetPaths[i] << endl;
        int veh = vrptw->expectedNumOfVehiclesTrain[i];
        bool flag = false;
        solution sol;
        while(!flag){
            veh++;
            if(earlyOrUrgent == "early"){
                sol = vrptw->metaAlgorithmParallel(earliestFirst, &vrptw->trainingSet[i], veh, flag);
            } else {
                sol = vrptw->metaAlgorithmParallel(urgentFirst, &vrptw->trainingSet[i], veh, flag);
            }
        }

        sum+=vrptw->cost(sol);
        //vrptw->printRoutes(sol);
    }

    cout << "[Test fitness: " << sum / vrptw->testSet.size() << "]\n";
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

