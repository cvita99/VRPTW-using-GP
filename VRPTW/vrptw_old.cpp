#include <bits/stdc++.h>
typedef long long ll;
using namespace std;
#include "./utils/utils.h"

double e = 2.718281;

const int N = 100; // number of customers
const int K = 3;  // number of vehicles
int vehicles = K;
int Q = 1000; // capacity and velocity of vehicle

struct Customer {
   int id;
   pair<int, int> x;
   int demand;
   int service;
   pair<int, int> tw;
};

struct solution { // feasible solution
   vector<int> routes[K];
   double delay[K]; // only if the vehicle is late (dont count it if its early)
   vector<double> arrivalTime[K];
   vector<double> couldDelay[K];
   int demand[K];
   int numOfCustomers[K];
   double totalDelay;
   double totalDistance;
};

Customer customers[N + 1];
double c[N + 1][N + 1];
solution bestSolution;
vector<int> currenNotVisitedCustomers;

void calculateDistancesBetweenCustomers() {
   for (int i = 0; i < N + 1; i++) {
      c[i][i] = 0;
      for (int j = i + 1; j < N + 1; j++) {
         c[i][j] = distance(customers[i].x, customers[j].x);
         c[j][i] = c[i][j];
      }
   }
}

int farestCustomerFrom(vector<int> custIds) {
   int farestCust = 0;
   double maks = 0;
   for (int i = 1; i < N + 1; i++) {
      double suma = 0;
      for (auto custId : custIds) {
         suma += c[i][custId];
         if (custId == i) {
            suma = -1;
            break;
         }
      }
      if (suma > maks) {
         maks = suma;
         farestCust = i;
      }
   }
   return farestCust;
}

void cost(solution &sol, int routeNum = -1) { // O(N)
   double total = 0;
   sol.totalDistance = 0;
   for (int r = 0; r < vehicles; r++) {
      vector<int> route = sol.routes[r];
      if (routeNum != -1 && routeNum != r) {
         continue;
      }
      double time = 0, del = 0, distance = 0;
      int prev = 0, routeDemand = 0;
      vector<double> arrival;
      for (auto customerId : route) {
         Customer cust = customers[customerId];
         routeDemand += cust.demand;
         time += (c[prev][customerId]);
         distance += c[prev][customerId];
         arrival.push_back(time);
         // del += max((double)0, cust.tw.first - time);
         del += max((double)0, time - cust.tw.second);
         time += cust.service;
         prev = customerId;
      }
      sol.delay[r] = del;
      sol.demand[r] = routeDemand;
      sol.arrivalTime[r] = arrival;
      sol.numOfCustomers[r] = route.size();
      total += del;
      sol.totalDistance += distance;
   }
   sol.totalDelay = total;
}

void backwardsPrecompute(solution &sol) { // O(N)
   for (int r = 0; r < vehicles; r++) {
      vector<int> route = sol.routes[r];
      double delay = 1e6;
      vector<double> possibleDelay;
      for (int i = route.size() - 1; i >= 0; i--) {
         Customer cust = customers[route[i]];
         // double currentDelay =
         //     max(cust.tw.second - sol.arrivalTime[r][i], (double)0);
         delay = min(delay, cust.tw.second - sol.arrivalTime[r][i]);
         //cout << delay << " ";
         possibleDelay.push_back(delay);
      }
      reverse(possibleDelay.begin(), possibleDelay.end());
      sol.couldDelay[r] = possibleDelay;
   }
}

void randomRemoval(solution &sol, int removeNum) { // O(N * removeNum)
   int l[101];
   memset(l, 0, sizeof(l));
   while (currenNotVisitedCustomers.size() < removeNum) {
      int num = rand() % 100 + 1; // Generate a number between 1 and 100
      if (l[num] == 0) {
         currenNotVisitedCustomers.push_back(num);
         l[num] = 1;
      }
   }
   for (auto &route : sol.routes) {
      remove(route, currenNotVisitedCustomers);
   }
   for (int r = 0; r < vehicles; r++) {
      remove(sol.routes[r], currenNotVisitedCustomers);
   }
}

void worseRemoval(solution &sol, int removeNum) { // O(N * removeNum)
   vector<pair<double, int>> maksDelays;
   cost(sol);
   for (int i = 0; i < vehicles; i++) {
      for (int j = 0; j < sol.routes[i].size() - 1; j++) {
         int customerId = sol.routes[i][j];
         Customer customer = customers[customerId];
         double del = max((double)0, customer.tw.first - sol.arrivalTime[i][j]);
         del += max((double)0, sol.arrivalTime[i][j] - customer.tw.second);
         maksDelays.push_back(pair(del, customerId));
      }
   }
   sort(maksDelays.begin(), maksDelays.end());
   reverse(maksDelays.begin(), maksDelays.end());
   for (int i = 0; i < removeNum; i++) {
      currenNotVisitedCustomers.push_back(maksDelays[i].second);
   }
   for (int r = 0; r < vehicles; r++) {
      remove(sol.routes[r], currenNotVisitedCustomers);
   }
}

void worseRouteRemoval(solution &sol, int removeNum) { // O(N * removeNum)
   cost(sol);
   int worseRoute;
   double maxDelay = -1;
   for (int i = 0; i < vehicles; i++) {
      if (sol.delay[i] > maxDelay) {
         maxDelay = sol.delay[i];
         worseRoute = i;
      }
   }
   vector<pair<double, int>> maksDelays;
   for (int i = 0; i < sol.routes[worseRoute].size() - 1; i++) {
      int customerId = sol.routes[worseRoute][i];
      Customer customer = customers[customerId];
      double del =
          max((double)0, customer.tw.first - sol.arrivalTime[worseRoute][i]);
      del +=
          max((double)0, sol.arrivalTime[worseRoute][i] - customer.tw.second);
      maksDelays.push_back(pair(del, customerId));
   }
   sort(maksDelays.begin(), maksDelays.end());
   reverse(maksDelays.begin(), maksDelays.end());
   for (int i = 0; i < min(removeNum, (int)maksDelays.size()); i++) {
      currenNotVisitedCustomers.push_back(maksDelays[i].second);
   }
   remove(sol.routes[worseRoute], currenNotVisitedCustomers);
}

void regretTwo(solution &sol) { // O(N * removeNum^2)
   // regret2 = cost(second best option) - cost(best option)
   // insert customer with maximum regret
   cost(sol);
   while (currenNotVisitedCustomers.size() > 0) {
      backwardsPrecompute(sol);

      double maxRegret = -1e6;
      int custWithMaxRegret;
      int route, position;

      for (auto notVisitedId : currenNotVisitedCustomers) {
         Customer customer = customers[notVisitedId];
         double firstMini = -1e6, secondMini = -1e6;
         int x1 = 0, y1 = 0;

         for (int i = 0; i < vehicles; i++) {
            if (sol.demand[i] + customer.demand > Q ||
                sol.arrivalTime[i].back() > customers[0].tw.second) {
               continue;
            }
            for (int j = 0; j < sol.routes[i].size(); j++) {
               double possibleDelay = sol.couldDelay[i][j];
               int prev = (j == 0 ? 0 : sol.routes[i][j - 1]);
               double arrivalPrevTime =
                   (j == 0 ? 0 : sol.arrivalTime[i][j - 1]);
               Customer prevCustomer = customers[prev];
               int next = sol.routes[i][j];
               double delta = c[prev][notVisitedId] + c[notVisitedId][next] +
                              customer.service - c[prev][next];
               double delayOfInsertedCust =
                   customer.tw.second - arrivalPrevTime - prevCustomer.service -
                   c[prev][notVisitedId];
               double result = possibleDelay - delta - delayOfInsertedCust;
               bool pom = false;
               if (result > firstMini) {
                  x1 = i;
                  y1 = j;
                  firstMini = result;
                  pom = true;
               }
               if (result > secondMini && pom == false) {
                  secondMini = result;
               }
            }
         }

         double customerRegret = firstMini - secondMini;

         if (customerRegret > maxRegret) {
            maxRegret = customerRegret;
            custWithMaxRegret = notVisitedId;
            route = x1;
            position = y1;
         }
      }

      sol.routes[route].insert(sol.routes[route].begin() + position,
                               custWithMaxRegret);

      int pom = find(currenNotVisitedCustomers, custWithMaxRegret);
      currenNotVisitedCustomers.erase(currenNotVisitedCustomers.begin() + pom);
      cost(sol);
   }
}

void greedyInsertion(solution &sol) { // O(N * Number of not visited)
   cost(sol);
   random_device rd;
   mt19937 rng(rd());
   shuffle(currenNotVisitedCustomers.begin(), currenNotVisitedCustomers.end(),
           rng);
   while (currenNotVisitedCustomers.size() > 0) {
      int customerId = currenNotVisitedCustomers.back();
      backwardsPrecompute(sol);

      Customer customer = customers[customerId];
      double smallestChange = -1e9;
      int x = 0, y = 0;
      for (int i = 0; i < vehicles; i++) {
         if (sol.demand[i] + customer.demand > Q ||
             sol.arrivalTime[i].back() > customers[0].tw.second) {
            continue;
         }
         for (int j = 0; j < sol.routes[i].size(); j++) {
            double possibleDelay = sol.couldDelay[i][j];
            int prev = (j == 0 ? 0 : sol.routes[i][j - 1]);
            double arrivalPrevTime = (j == 0 ? 0 : sol.arrivalTime[i][j - 1]);
            Customer prevCustomer = customers[prev];
            int next = sol.routes[i][j];
            double delta = c[prev][customerId] + c[customerId][next] +
                           customer.service - c[prev][next];
            double delayOfInsertedCust = customer.tw.second - arrivalPrevTime -
                                         prevCustomer.service -
                                         c[prev][customerId];
            if ((possibleDelay - delta - delayOfInsertedCust) >
                smallestChange) { // - delayOfInsertedCust
               x = i;
               y = j;
               smallestChange = possibleDelay - delta - delayOfInsertedCust;
            }
         }
      }
      if (smallestChange == -1e9)
         cerr << "nigdje nisam greedy insertao kupca: " << customerId << endl;
      sol.routes[x].insert(sol.routes[x].begin() + y, customerId);

      currenNotVisitedCustomers.pop_back();
      cost(sol);
   }
}

vector<function<void(solution &)>> reapirOp = {greedyInsertion, regretTwo};
vector<function<void(solution &, int)>> destroyOp = {
    randomRemoval, worseRemoval, worseRouteRemoval};

vector<string> reapirOperatorNames = {"greedy insertion", "regret two"};
vector<string> destroyOperatorNames = {"random removal", "worse removal", "worse route removal"};

const int numOfDestroyOp = 3;
const int numOfRepairOp = 2;

double operatorWeights[numOfDestroyOp][numOfRepairOp];
double probabilities[numOfDestroyOp][numOfRepairOp];

void calculateTheProbability() {
   double sum = 0;
   for (int i = 0; i < numOfDestroyOp; i++) {
      for (int j = 0; j < numOfRepairOp; j++) {
         sum += operatorWeights[i][j];
      }
   }
   double pom = 0;
   for (int i = 0; i < numOfDestroyOp; i++) {
      for (int j = 0; j < numOfRepairOp; j++) {
         pom += (operatorWeights[i][j] / sum);
         probabilities[i][j] = pom;
      }
   }
}

void printOperatorWeights(){
   for (int i = 0; i < numOfDestroyOp; i++) {
      for (int j = 0; j < numOfRepairOp; j++) {
        cout << "(" + reapirOperatorNames[j] + ", " + destroyOperatorNames[i] +  "): " + to_string(probabilities[i][j]) << endl;
      }
   }
}

void memsetOperatorWeights(){
   for (int i = 0; i < numOfDestroyOp; i++) {
      for (int j = 0; j < numOfRepairOp; j++) {
        operatorWeights[i][j] = 1;
      }
   }
}

void ALNS(solution &sol) {
   double Temp = 10000;
   double Tmin = 0.0001;
   double alfa = 0.95;

   solution wipSol = sol;
   memsetOperatorWeights();

   while (Temp > Tmin) {
      int i = 0;
      calculateTheProbability();
     
      while (i < 100) {
         random_device rd;
         mt19937 gen(rd());
         uniform_real_distribution<> dis(0.0, 1.0);
         uniform_int_distribution<int> dist(1, 100);
         // double randomNumber = dis(gen);

         // int dest, rep;

         // for (int i = 0; i < numOfDestroyOp; i++) {
         //    for (int j = 0; j < numOfRepairOp; j++) {
         //       if (probabilities[i][j] >= randomNumber) {
         //          dest = i; rep = j;
         //          break;
         //       }
         //    }
         // }

         int dest = dist(gen) % 3;
         int rep = dist(gen) % 2;

         destroyOp[dest](wipSol, (dist(gen) % 10 + 5));
         reapirOp[rep](wipSol);
         cost(wipSol);
         int gama = 5;
         if ((wipSol.totalDelay) < (sol.totalDelay)) {
            operatorWeights[dest][rep] *= 1.02;
            sol = wipSol;
            if ((bestSolution.totalDelay) > (wipSol.totalDelay))
               bestSolution = wipSol;
         } else {
            int događaj = rand() % 101;
            double deltaCost = sol.totalDelay - wipSol.totalDelay;
            double exsp = deltaCost / Temp;
            if (događaj < (pow(e, exsp) * 100)) {
               sol = wipSol;
            } else {
               wipSol = sol;
            }
         }
         // if (bestSolution.totalDelay == 0) {
         //    return;
         // }
         i++;
      }

      Temp = Temp * alfa;
   }
}

void reduceNumberOfVehicles(solution &sol) {
   for(int i = 0; i < sol.routes[vehicles - 1].size() - 1; i++){
      currenNotVisitedCustomers.push_back(sol.routes[vehicles - 1][i]);
   }
   sol.routes[vehicles - 1].clear();
   vehicles--;
   cout << "smanjen broj vozila na " << vehicles << endl;
   regretTwo(sol);
}

solution initialize() {
   solution sol;
   int farest = farestCustomerFrom({0});
   sol.routes[0].push_back(farest);
   vector<int> farestCustomers;
   farestCustomers.push_back(farest);
   for (int i = 1; i < vehicles; i++) {
      farest = farestCustomerFrom(farestCustomers);
      sol.routes[i].push_back(farest);
      farestCustomers.push_back(farest);
      // cout << farest << ' ';
   }
   for (int i = 0; i < vehicles; i++) {
      sol.routes[i].push_back(0);
   }
   for (int i = 1; i <= N; i++) {
      if (find(farestCustomers, i) == -1) {
         currenNotVisitedCustomers.push_back(i);
      }
   }
   regretTwo(sol);
   return sol;
}

void readSolomonInstance(const string &filename) {
   ifstream file(filename);
   string line;

   if (!file.is_open()) {
      cerr << "Failed to open file: " << filename << endl;
      return;
   }

   while (getline(file, line)) {
      if (line.find("CAPACITY") != string::npos) {
         getline(file, line);
         istringstream iss(line);
         string temp; // number of vehicles
         iss >> temp >> Q;
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
          dueTime >> customer.service) {
         customer.x = {x, y};
         customer.tw = {readyTime, dueTime};
         customers[customer.id] = customer;
      }
   }

   file.close();
   return;
}

void printRoutes(solution &sol) {
   cout << "-----------------------------------------" << endl;
   cout << "Printing routes for " << vehicles << " vehicles" << endl;
   for (int r = 1; r <= vehicles; r++) {
      cout << r << ". route: ";
      for (auto customerId : sol.routes[r - 1]) {
         cout << customerId << ", ";
      }
      cout << endl;
   }
}

void printCustomers() {
   for (const auto &customer : customers) {
      cout << "ID: " << customer.id << ", Coordinates: (" << customer.x.first
           << ", " << customer.x.second << ")"
           << ", Demand: " << customer.demand << ", Time Window: ["
           << customer.tw.first << ", " << customer.tw.second << "]"
           << ", Service Time: " << customer.service << endl;
   }
}

void printDelayAndArrival(solution &sol) {
   cout << endl << "DELAY ";
   for (int r = 0; r < vehicles; r++) {
      cout << sol.delay[r] << " ";
   }
   cout << endl << "ARRIVAL ";
   for (int r = 0; r < vehicles; r++) {
      cout << sol.arrivalTime[r].back() << " ";
   }
   cout << endl << "Total delay: " << sol.totalDelay << endl;
   cout << "TOTAL DISTANCE: " << sol.totalDistance << endl;
}

int main() {
   readSolomonInstance("../solomonInstances/C2/c201.txt");
   srand(time(0));

   calculateDistancesBetweenCustomers();
   solution sol = initialize();
   cout << "initial solution: " << endl;
   printRoutes(sol);
   cost(sol);
   printDelayAndArrival(sol);

   bestSolution = sol;

   for(int i = 0; i < 1; i++) {
      ALNS(sol);
      printRoutes(bestSolution);
      printDelayAndArrival(bestSolution);
      reduceNumberOfVehicles(bestSolution);
      printOperatorWeights();
      sol = bestSolution;
   }

   return 0;
}
