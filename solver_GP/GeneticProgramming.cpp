#include "GeneticProgramming.h"
typedef long long ll;
using namespace std;

random_device rd;  
mt19937 gen(rd());  
uniform_real_distribution<double> dist(0.0, 1.0); 
uniform_int_distribution<int> intDist(0, INT32_MAX);

GeneticProgramming::GeneticProgramming(int maxDepth, int populationSize, int iterations, double mutationRate, FitnessFunction *fit)
    : rng(std::random_device{}()), rnd_popul(0, populationSize-1){

    this->maxDepth = maxDepth;
    this->populationSize = populationSize;
    this->iterations = iterations;
    this->mutationRate = mutationRate;
    this->fitnessFunction = fit;
}

// GeneticProgramming::~GeneticProgramming() {
//     for(int i = 0; i < populationSize; i++){
//         deleteExpr(population[i]);
//     }
//     deleteExpr(best);
// }

void GeneticProgramming::run(){
    cout << "initializing" << endl;
    rampedHalfAndHalf();

    int best_expr;
    cout << "finding best expr in initial population" << endl;
    for(int i=0; i < populationSize; i++){
        double fit = populationFitness[i];
        if(fit < bestFitness) {
            bestFitness = fit;
            best_expr = i;
        }
    }
    best = deepCopyOfNode(population[best_expr], nullptr);
    cout << "best in initial population: \n"<< best->printExpr() << endl;
    cout << "fitness " << bestFitness << endl;

    for(int i=0; i < iterations; i++){
        if(threeTorunamentSelection()){
            cout << "iteration " << i << endl;
            break;
        };
        if (i % 500 == 0) {
            cout << "iteration " << i << ": best sol so far: " << bestFitness << endl;
        }
    }

    cout << "best after gp: \n"<< best->printExpr() << endl;
    cout << "best fitness: " << bestFitness << endl;

    // for(int i = 0; i < populationSize; i++){
    //     deleteExpr(population[i]);
    // }
    // deleteExpr(best);
}

void GeneticProgramming::deleteExpr(Node *expr){
    if(!expr) return;
    deleteExpr(expr->leftChild);
    deleteExpr(expr->rightChild);
    delete expr;
}

Node* GeneticProgramming::genRndExpr(int depth, string method){
    double rand_num = dist(gen);
    if(depth == 0 || (depth != maxDepth && method == "grow" && rand_num < (((double)getTermSize()) / (getTermSize() + getFuncSize())))){
        int rand_term = intDist(gen) % getTermSize();
        //Node *node = new Node(terminals[rand_term], depth);
        return new Node(terminals[rand_term], depth);
    } else {
        int rand_func = intDist(gen) % getFuncSize();
        Node *left = genRndExpr(depth - 1, method);
        Node *right = genRndExpr(depth - 1, method);
        Node *expr = new Node(functions[rand_func], depth);
        right->parent = expr;
        left->parent = expr;
        expr->leftChild = left;
        expr->rightChild = right;
        return expr;
    }
}

bool GeneticProgramming::checkParentPointers(Node* node, Node* parent = nullptr) {
    if (node == nullptr) return true;

    if (node->parent != parent) {
        std::cout << "Parent mismatch at node: " << node->name << std::endl;
        cout << node->printExpr() << endl;
        return false;
    }

    bool leftOk = checkParentPointers(node->leftChild, node);
    bool rightOk = checkParentPointers(node->rightChild, node);

    return leftOk && rightOk;
}

void GeneticProgramming::rampedHalfAndHalf(){
    for(int i=0; i < populationSize; i++){
        string method = (i % 2 == 0) ? "grow" : "full";
        Node *expr = genRndExpr(maxDepth, method);
        population.push_back(expr);
        double fit = fitnessFunction->fitness(expr);
        populationFitness.push_back(fit);
    }
}

bool GeneticProgramming::threeTorunamentSelection(){
    int rand_expr1 = rnd_popul(rng);
    int rand_expr2 = rnd_popul(rng);
    while(rand_expr2 == rand_expr1){
        rand_expr2 = rnd_popul(rng);
    }
    int rand_expr3 = rnd_popul(rng);
    while(rand_expr3 == rand_expr2 || rand_expr3 == rand_expr1){
        rand_expr3 = rnd_popul(rng);
    }

    double fitness1 = populationFitness[rand_expr1];
    double fitness2 = populationFitness[rand_expr2];
    double fitness3 = populationFitness[rand_expr3];

    int par1, par2, worst;
    if(fitness1 <= fitness2 || fitness1 <= fitness3){
        par1 = rand_expr1;
        if(fitness2 <= fitness3) {
            par2 = rand_expr2;
            worst = rand_expr3;
        } else { 
            par2 = rand_expr3;
            worst = rand_expr2;
        }
    } else {
        worst = rand_expr1;
        par1 = rand_expr2;
        par2 = rand_expr3;
    }

    Node *pom = population[worst];
    deleteExpr(pom);
    Node *newNode = onePointCrossover(par1, par2);

    if(dist(gen) > mutationRate){
        mutation(newNode);
    }

    double fitnessNew = fitnessFunction->fitness(newNode);
    population[worst] = newNode;
    populationFitness[worst] = fitnessNew;
    
    if(fitnessNew < bestFitness){
        bestFitness = fitnessNew;
        Node *pom = best;
        best = deepCopyOfNode(population[worst], nullptr);
        deleteExpr(pom);
        if(fitnessNew <= 1e-6){
            cout << "found the expression tree " << newNode->printExpr() << endl;
            return true;
        }
    }
    return false;
}

int GeneticProgramming::calculateCommonNodes(Node *expr1, Node *expr2){
    if(expr1 && expr2){
        return 1 + calculateCommonNodes(expr1->leftChild, expr2->leftChild) + calculateCommonNodes(expr1->rightChild, expr2->rightChild);
    } else {
        return 0;
    }
}

//this function returns two cross nodes for each tree
pair<Node *, Node *> GeneticProgramming::findCrossNode(Node *expr1, Node *expr2, bool &found, int &cnt, int value){
    if(expr1 && expr2 && cnt == value){
        found = true;
        return make_pair(expr1, expr2);
    } else if(!expr1 || !expr2){
        return make_pair(expr1, expr2);
    } else {
        cnt++;
        auto pairNodes = findCrossNode(expr1->leftChild, expr2->leftChild, found, cnt, value);
        if(found) return pairNodes;
        pairNodes = findCrossNode(expr1->rightChild, expr2->rightChild, found, cnt, value);
        return pairNodes;
    }
}

Node* GeneticProgramming::onePointCrossover(int par1, int par2){

    Node *expr1 = deepCopyOfNode(population[par1], nullptr);
    Node *expr2 = deepCopyOfNode(population[par2], nullptr);

    //cout << par1 << " " << par2 << endl;

    int common = calculateCommonNodes(expr1, expr2);
    int crossNode = intDist(gen) % common;
    while(crossNode == 0){
        crossNode = intDist(gen) % common;
    }

    bool flag = false;
    int cnt = 0;

    pair<Node *, Node *> crossNodes = findCrossNode(expr1, expr2, flag, cnt, crossNode); 

    bool isleft = crossNodes.first->parent->leftChild == crossNodes.first;
    if(isleft){
        Node *pom_parent2 = crossNodes.second->parent;

        crossNodes.first->parent->leftChild = crossNodes.second;
        crossNodes.second->parent = crossNodes.first->parent;

        pom_parent2->leftChild = crossNodes.first;
        crossNodes.first->parent = pom_parent2;
    } else {
        Node *pom_parent2 = crossNodes.second->parent;

        crossNodes.first->parent->rightChild = crossNodes.second;
        crossNodes.second->parent = crossNodes.first->parent;

        pom_parent2->rightChild = crossNodes.first;
        crossNodes.first->parent = pom_parent2;
    }

    double fitness1 = fitnessFunction->fitness(expr1);
    double fitness2 = fitnessFunction->fitness(expr2);

    if(fitness1 < fitness2){
        deleteExpr(expr2);
        return expr1;
    } else {
        deleteExpr(expr1);
        return expr2;
    }
}

Node* GeneticProgramming::deepCopyOfNode(Node* node, Node *parent) {
    if (node == nullptr) return nullptr;
    Node* newNode = new Node(node->name);
    newNode->leftChild = deepCopyOfNode(node->leftChild, newNode);
    newNode->rightChild = deepCopyOfNode(node->rightChild, newNode);
    newNode->parent = parent;
    return newNode;
}

int GeneticProgramming::findNode(Node *expr, int depth, int &cnt, int mutationNode, Node*& result){
    if(!expr) return depth;
    if(cnt == mutationNode) {
        cnt++;
        result = expr;
        return depth;
    } else {
        cnt++;
        int dl = findNode(expr->leftChild, depth+1, cnt, mutationNode, result);
        if(result != nullptr) return dl;
        int dr = findNode(expr->rightChild, depth+1, cnt, mutationNode, result);
        return dr;
    }
}

void GeneticProgramming::mutation(Node *expr){
    int nodes = expr->countNodes();
    int mutationNode = intDist(gen) % nodes;
    while(mutationNode == 0){
        mutationNode = intDist(gen) % nodes;
    } 
    Node *mutatNode = nullptr;
    int cnt = 0;
    int depth = findNode(expr, 0, cnt, mutationNode, mutatNode);
    if(!mutatNode){
        cout << "did not find the node" << endl;
    }
    Node *mutated = genRndExpr(maxDepth - depth, "grow");
    if(!mutatNode->parent) cout << "has no parent" << endl;
    else{
        if(mutatNode->parent->leftChild == mutatNode){
            mutatNode->parent->leftChild = mutated;
        } else {
            mutatNode->parent->rightChild = mutated;
        }
        mutated->parent = mutatNode->parent;
        deleteExpr(mutatNode);
    }

}

int GeneticProgramming::getFuncSize(){
    return functions.size();
}

int GeneticProgramming::getTermSize(){
    return terminals.size();
}



