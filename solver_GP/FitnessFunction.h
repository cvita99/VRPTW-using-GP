#pragma once
#include <bits/stdc++.h>
#include "Node.h"

class FitnessFunction {
    public:
        virtual double fitness(Node *expr) = 0;
        virtual double fitnessTest(Node *expr) = 0;
    };