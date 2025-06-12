#include <bits/stdc++.h>
#include "../utils/utils.h"
#include "VRPTW.h"
using namespace std;

VRPTW::VRPTW(string solomonInstanceFolder){
    trainingSet = readSolomonInstances(trainingSetPaths, solomonInstanceFolder);
    testSet = readSolomonInstances(testSetPaths, solomonInstanceFolder);
}

double VRPTW::fitness(Node *expr){
    return fitnessFunc(expr);
}

double VRPTW::eval(Node *expr){
    if(!expr->leftChild && !expr->rightChild){
        if(expr->name == "d")
            return evalData.distance;
        else if(expr->name == "rt")
            return evalData.next.tw.first;
        else if(expr->name == "dt")
            return evalData.next.tw.second;
        else if(expr->name == "dem")
            return evalData.next.demand;
        else if(expr->name == "t")
            return evalData.veh_time;
        else if(expr->name == "q")
            return evalData.rem_q;
        else if(expr->name == "t_rem")
            return evalData.next.tw.second - evalData.veh_time;
        else if(expr->name == "d_depot")
            return evalData.dist_from_depot;
        else if(expr->name == "du")
            return evalData.dist_unserved;
        else if(expr->name == "wt")
            return max(0.0, evalData.next.tw.first - evalData.veh_time - evalData.distance);
        else if(expr->name == "t_ready")
            if(evalData.veh_time + evalData.distance >= evalData.next.tw.first){
                return 0;
            } else {
                return evalData.next.tw.first - evalData.veh_time - evalData.distance + evalData.veh_time;
            }
        else if(expr->name == "slack_t"){
            return max(0.0, evalData.next.tw.second - evalData.veh_time - evalData.distance);
        } else if(expr->name == "next_veh") {
            return evalData.dist_of_second_available;
        }
        else 
            cout << "unknown terminal" << endl;
    } else {
        double left = eval(expr->leftChild);
        double right = eval(expr->rightChild);
        if(expr->name == "*")
            return left * right;
        else if(expr->name == "+")
            return left + right;
        else if(expr->name == "-")
            return left - right;
        else if(expr->name == "min")
            return min(left, right);
        else if(expr->name == "max")
            return max(left, right);
        else if(expr->name == "/")
            if(right < 1e-4)
                return 1.0;
            else
                return left / right;
        else 
            cout << "unknown function" << endl;
    }
    return 0.0;
}

double VRPTW::findNearestUnserved(int id, bool notVisited[], SolomonInstance *instance){
    int nearest = -1;
    double m = DBL_MAX;
    for(int i=1; i <= N; i++){
        if(i==id || !notVisited[i]) continue;
        if(m > instance->c[id][i]){
            m = instance->c[id][i];
            nearest = i;
        }
    }
    if(nearest == -1) return 0;
    return m;
}

solution VRPTW::metaAlgorithmSerial(Node *expr, SolomonInstance *instance){
    bool notVisited[N+1];
    for(int i=0; i < N+1; i++){
        notVisited[i] = true;
    }
    int len = N;
    solution sol;
    int r = 0;
    while (len > 0){

        vector<int> route = {0};
        int r_demand = 0;
        double time = 0;
        double r_distance = 0;
        bool flag = false;
        while (len > 0){
            double bestNext = DBL_MAX;
            int bestNextId = -1;
            evalData.rem_q = instance->Q - r_demand;
            evalData.veh_time = time;
            for(int id=1; id < N+1; id++){
                if(!notVisited[id]) continue;
                evalData.next = instance->customers[id];
                evalData.distance = instance->c[route.back()][id];
                evalData.dist_from_depot = instance->c[0][id];
                //evalData.dist_unserved = findNearestUnserved(id, notVisited, instance);
                double fit = eval(expr);
                if(fit < bestNext){
                    bestNext = fit;
                    bestNextId = id;
                }
            }

            Customer cust = instance->customers[bestNextId];
            if(cust.demand + r_demand > instance->Q) 
                break;
            double dist = instance->c[route.back()][bestNextId];
            if(time + dist > cust.tw.second)
                break;
            if(time + dist + cust.service + instance->c[0][bestNextId] > instance->customers[0].tw.second)
                break;

            if(notVisited[bestNextId] == false){
                cout << "GRESKAAAAA ";
                cout << bestNextId << endl;
                exit(1);
            }
            notVisited[bestNextId] = false;
            len--;

            r_demand += cust.demand;
            time += (instance->c[route.back()][bestNextId]);
            if(time < cust.tw.first){
                time = cust.tw.first + cust.service;
            } else {
                time += cust.service;
            }
            r_distance += instance->c[route.back()][bestNextId];
            route.push_back(bestNextId);

        }

        time += instance->c[route.back()][0];
        r_distance += instance->c[route.back()][0];
        sol.routes.push_back(route);
        sol.demand.push_back(r_demand);
        sol.distance.push_back(r_distance);
        r++;
        sol.used_vehicles = r;
    }

    return sol;
}

solution VRPTW::metaAlgorithmParallel(Node *expr, SolomonInstance *instance, int vehicles, bool &success){
    bool notVisited[N+1];
    for(int i=0; i < N+1; i++){
        notVisited[i] = true;
    }
    int len = N, available_veh = vehicles;
    bool available[vehicles];
    double veh_time[vehicles];
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;
    for (int i = 0; i < vehicles; ++i) {
        pq.push({0.0, i});
    }
    memset(veh_time, 0, sizeof(veh_time));
    memset(available, true, sizeof(available));
    solution sol;
    sol.used_vehicles = vehicles;
    for(int i=0; i < vehicles; i++){
        vector<int> pom = {0};
        sol.routes.push_back(pom);
        sol.demand.push_back(0);
        sol.distance.push_back(0);
    }
    while(len > 0 && available_veh > 0){

        auto [least_time, first_available] = pq.top();
        pq.pop();

        //cout << first_available << " " << least_time << endl;

        double bestNext = DBL_MAX;
        int bestNextId = -1;
        evalData.rem_q = instance->Q - sol.demand[first_available];
        evalData.veh_time = veh_time[first_available];
        for(int i=1; i < N+1; i++){
            if(!notVisited[i]) continue;
            //if(instance->customers[i].orderVisibleTime > veh_time[first_available]) continue;
            evalData.next = instance->customers[i];
            evalData.distance = instance->c[sol.routes[first_available].back()][i];
            evalData.dist_from_depot = instance->c[0][i];
            auto [least_time, second_available] = pq.top();
            evalData.dist_of_second_available = instance->c[sol.routes[second_available].back()][i];
            double fit = eval(expr);
            if(fit < bestNext){
                bestNext = fit;
                bestNextId = i;
            }
        }

        // if(bestNextId == -1){
        //     cout << "tu sam" << endl;
        //     veh_time[first_available] += instance->c[sol.routes[first_available].back()][0];
        //     sol.distance[first_available] += instance->c[sol.routes[first_available].back()][0];
        //     available_veh--;
        //     available[first_available] = false;
        //     continue;
        // }

        Customer cust = instance->customers[bestNextId];
        double dist = instance->c[sol.routes[first_available].back()][bestNextId];

        if ((cust.demand + sol.demand[first_available] > instance->Q) || 
            (veh_time[first_available] + dist > cust.tw.second) || 
            (veh_time[first_available] + dist + cust.service + instance->c[0][bestNextId] > instance->customers[0].tw.second)){
                veh_time[first_available] += instance->c[sol.routes[first_available].back()][0];
                sol.distance[first_available] += instance->c[sol.routes[first_available].back()][0];
                available_veh--;
                available[first_available] = false;
                continue;
        }

        notVisited[bestNextId] = false;
        len--;

        sol.demand[first_available] += cust.demand;
        veh_time[first_available] += (instance->c[sol.routes[first_available].back()][bestNextId]);
        if(veh_time[first_available] < cust.tw.first){
            veh_time[first_available] = cust.tw.first + cust.service;
        } else {
            veh_time[first_available] += cust.service;
        }
        pq.push({veh_time[first_available], first_available});
        sol.distance[first_available] += instance->c[sol.routes[first_available].back()][bestNextId];
        sol.routes[first_available].push_back(bestNextId);
    }

    if(available_veh > 0){
        for(int i=0; i < vehicles; i++){
            if(!available[i]) continue;
            sol.distance[i] += instance->c[sol.routes[i].back()][0];
            available_veh--;
            available[i] = false;
        }
    }
    if(len > 0){
        //nisam uspio obici sve kupce sa dostupnim vozilima
        success = false;
        return sol;
    }
    success = true;
    return sol;
}

double VRPTW::cost(solution sol){
    double totalDist = 0;
    for(auto i: sol.distance){
        totalDist += i;
    }
    return (10000.0 * sol.used_vehicles) + totalDist;
}

void VRPTW::calculateDistancesBetweenCustomers(SolomonInstance *instance) {
    for (int i = 0; i < N + 1; i++) {
        instance->c[i][i] = 0;
        for (int j = i + 1; j < N + 1; j++) {
            instance->c[i][j] = distance(instance->customers[i].x, instance->customers[j].x);
            instance->c[j][i] = instance->c[i][j];
        }
    }
}

void VRPTW::printRoutes(solution sol) {
    cout << "Printing routes for " << sol.routes.size() << " vehicles" << endl;
    for (int r = 1; r <= sol.routes.size(); r++) {
        cout << r << ". route: ";
        for (auto customerId : sol.routes[r - 1]) {
            cout << customerId << ", ";
        }
        cout << endl;
    }
    double sum = 0;
    for(auto dist: sol.distance){
        sum += dist;
    }
    cout << "Total distance: " << sum << endl;
    cout << "-----------------------------------------" << endl;
}

vector<SolomonInstance> VRPTW::readSolomonInstances(vector<string> paths, string folder) {
    vector<SolomonInstance> solomonInstances;
    for(int i=0; i < paths.size(); i++){

        string filename = folder + paths[i];
        ifstream file(filename);
        string line;
    
        if (!file.is_open()) {
            cout << "Failed to open file: " << filename << endl;
            exit(1);
            return solomonInstances;
        }

        SolomonInstance instance;
    
        while (getline(file, line)) {
        if (line.find("CAPACITY") != string::npos) {
            getline(file, line);
            istringstream iss(line);
            string temp; // number of vehicles
            iss >> temp >> instance.Q;
            break;
        }
        }
    
        while (getline(file, line)) {
        if (line.find("CUST NO.") != string::npos) {
            break;
        }
        }
    
        while (getline(file, line)) {
            istringstream iss(line);
            Customer customer;
            int x, y, readyTime, dueTime;
            int i = 0;
            if (iss >> customer.id >> x >> y >> customer.demand >> readyTime >>
                dueTime >> customer.service >> customer.orderVisibleTime) {
                customer.x = {x, y};
                customer.tw = {readyTime, dueTime};
                instance.customers[customer.id] = customer;
            }
        }

        calculateDistancesBetweenCustomers(&instance);
        solomonInstances.push_back(instance);

        file.close();
    }

    return solomonInstances;
 }

// solution VRPTW::closestFirst(SolomonInstance *instance, Node *expr){
//     bool notVisited[N+1];
//     for(int i=0; i < N+1; i++){
//         notVisited[i] = true;
//     }
//     int len = N;
//     solution sol;
//     int r = 0;
//     while (len > 0){
//         vector<int> route = {0};
//         int r_demand = 0;
//         double time = 0;
//         double r_distance = 0;
//         bool flag = false;

//         while (len > 0){

//             vector<int> feasible = {};
//             for(int i=1; i < N+1; i++){
//                 if(!notVisited[i]) continue;
//                 Customer cust = instance->customers[i];
//                 if(cust.demand + r_demand > instance->Q) 
//                     continue;;
//                 double dist = instance->c[route.back()][i];
//                 if(time + dist > cust.tw.second)
//                     continue;
//                 if(time + dist + cust.service + instance->c[0][i] > instance->customers[0].tw.second)
//                     continue;
//                 feasible.push_back(i);
//             }

//             if(feasible.size() == 0) break;


//             double bestNext = DBL_MAX;
//             int bestNextId = -1;
//             evalData.rem_q = instance->Q - r_demand;
//             evalData.veh_time = time;
//             for(auto id: feasible){
//                 evalData.next = instance->customers[id];
//                 evalData.distance = instance->c[route.back()][id];
//                 evalData.dist_from_depot = instance->c[0][id];
//                 double fit = eval(expr);
//                 if(fit < bestNext){
//                     bestNext = fit;
//                     bestNextId = id;
//                 }
//             }

//             notVisited[bestNextId] = false;
//             len--;

//             Customer cust = instance->customers[bestNextId];
//             r_demand += cust.demand;
//             time += (instance->c[route.back()][bestNextId]);
//             if(time < cust.tw.first){
//                 time = cust.tw.first + cust.service;
//             } else {
//                 time += cust.service;
//             }
//             r_distance += instance->c[route.back()][bestNextId];
//             route.push_back(bestNextId);

//         }

//         time += instance->c[route.back()][0];
//         r_distance += instance->c[route.back()][0];
//         sol.routes.push_back(route);
//         sol.demand.push_back(r_demand);
//         sol.distance.push_back(r_distance);
//         r++;
//         sol.used_vehicles = r;
//     }

//     return sol;
// }

// solution VRPTW::earliestOrUrgentFirst(SolomonInstance *instance, Node *expr, int vehicles, bool &success){
//     bool notVisited[N+1];
//     for(int i=0; i < N+1; i++){
//         notVisited[i] = true;
//     }
//     int len = N, available_veh = vehicles;
//     bool available[vehicles];
//     double veh_time[vehicles];
//     priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;
//     for (int i = 0; i < vehicles; ++i) {
//         pq.push({0.0, i});
//     }
//     memset(veh_time, 0, sizeof(veh_time));
//     memset(available, true, sizeof(available));
//     solution sol;
//     sol.used_vehicles = vehicles;
//     for(int i=0; i < vehicles; i++){
//         vector<int> pom = {0};
//         sol.routes.push_back(pom);
//         sol.demand.push_back(0);
//         sol.distance.push_back(0);
//     }
//     while(len > 0 && available_veh > 0){

//         auto [least_time, first_available] = pq.top();
//         pq.pop();

//         vector<int> feasible = {};

//         for(int i=1; i < N+1; i++){
//             if(!notVisited[i]) continue;
//             Customer cust = instance->customers[i];
//             double dist = instance->c[sol.routes[first_available].back()][i];
//             if ((cust.demand + sol.demand[first_available] > instance->Q) || 
//                 (veh_time[first_available] + dist > cust.tw.second) || 
//                 (veh_time[first_available] + dist + cust.service + instance->c[0][i] > instance->customers[0].tw.second)){
//                     continue;
//             }
//             feasible.push_back(i);
//         }

//         if(feasible.size() == 0){
//             veh_time[first_available] += instance->c[sol.routes[first_available].back()][0];
//             sol.distance[first_available] += instance->c[sol.routes[first_available].back()][0];
//             available_veh--;
//             available[first_available] = false;
//             continue;
//         }

//         double bestNext = DBL_MAX;
//         int bestNextId = -1;
//         evalData.rem_q = instance->Q - sol.demand[first_available];
//         evalData.veh_time = veh_time[first_available];
//         for(auto i: feasible){
//             if(!notVisited[i]) continue;
//             evalData.next = instance->customers[i];
//             evalData.distance = instance->c[sol.routes[first_available].back()][i];
//             evalData.dist_from_depot = instance->c[0][i];
//             double fit = eval(expr);
//             if(fit < bestNext){
//                 bestNext = fit;
//                 bestNextId = i;
//             }
//         }

//         Customer cust = instance->customers[bestNextId];

//         notVisited[bestNextId] = false;
//         len--;

//         sol.demand[first_available] += cust.demand;
//         veh_time[first_available] += (instance->c[sol.routes[first_available].back()][bestNextId]);
//         if(veh_time[first_available] < cust.tw.first){
//             veh_time[first_available] = cust.tw.first + cust.service;
//         } else {
//             veh_time[first_available] += cust.service;
//         }
//         pq.push({veh_time[first_available], first_available});
//         sol.distance[first_available] += instance->c[sol.routes[first_available].back()][bestNextId];
//         sol.routes[first_available].push_back(bestNextId);
//     }

//     if(available_veh > 0){
//         for(int i=0; i < vehicles; i++){
//             if(!available[i]) continue;
//             sol.distance[i] += instance->c[sol.routes[i].back()][0];
//             available_veh--;
//             available[i] = false;
//         }
//     }
//     if(len > 0){
//         //nisam uspio obici sve kupce sa dostupnim vozilima
//         success = false;
//         return sol;
//     }
//     success = true;
//     return sol;
// }


