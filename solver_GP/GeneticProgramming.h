#include <bits/stdc++.h>
#include "FitnessFunction.h"
using namespace std;


class GeneticProgramming {
    public: 
        GeneticProgramming(int maxDepth, int populationSize, int iterations, double mutationRate, FitnessFunction *fit);
        // ~GeneticProgramming();

        vector<string> functions;
        vector<string> terminals;
        int maxDepth = 4;
        int populationSize = 200;
        int iterations = 30000;
        double mutationRate = 0.5;
        FitnessFunction *fitnessFunction;

        int getFuncSize();
        int getTermSize();

        void run();

        //generating initial population
        void rampedHalfAndHalf();
        Node *genRndExpr(int depth, string method);

        //mutation and crossover
        bool threeTorunamentSelection();
        Node *onePointCrossover(int par1, int par2);
        void mutation(Node *expr);

        //utils
        Node* deepCopyOfNode(Node* node, Node *parent);
        int calculateCommonNodes(Node *expr1, Node *expr2);
        bool checkParentPointers(Node* node, Node* parent);
        pair<Node *, Node *> findCrossNode(Node *expr1, Node *expr2, bool &found, int &cnt, int value);
        int findNode(Node *expr, int depth, int &cnt, int mutationNode, Node*& result);
        void deleteExpr(Node *expr);

        vector<Node*> population;
        vector<double> populationFitness; 

        double bestFitness = DBL_MAX;
        Node *best = nullptr;

        std::mt19937 rng; 
        std::uniform_int_distribution<int> rnd_popul;
};
