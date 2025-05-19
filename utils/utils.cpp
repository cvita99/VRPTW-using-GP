#include <bits/stdc++.h>
#include "utils.h"
typedef long long ll;
using namespace std;

int find(vector<int> list, int elem) {
   for (int i = 0; i < list.size(); i++) {
      if (list[i] == elem) {
         return i;
      }
   }
   return -1;
}

double distance(pair<int, int> x1, pair<int, int> x2) {
   return sqrt((x1.first - x2.first) * (x1.first - x2.first) +
               (x1.second - x2.second) * (x1.second - x2.second));
}

template <typename T> void printList(const vector<T> &vec) {
   for (size_t i = 0; i < vec.size(); ++i) {
      cout << vec[i] << " ";
   }
   cout << endl;
}

template <typename T> vector<T> reverseList(vector<T> vec) {
   vector<T> result;
   for (size_t i = vec.size() - 1; i >= 0; i--) {
      result.push_back(vec[i]);
   }
   return result;
}

void remove(vector<int> &list, vector<int> shouldBeRemoved) {
   // O(list.size * shouldBeRemoved.size)
   vector<int> pom;
   for (int i = 0; i < list.size(); i++) {
      if (find(shouldBeRemoved, list[i]) == -1) {
         pom.push_back(list[i]);
      }
   }
   list = pom;
}
