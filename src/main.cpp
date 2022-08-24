#include <iostream>
#include "../headers/lru-cache.h"

// g++ main.cpp lru-cache.cpp -o ../main -lstdc++
int main() {
  LRUCache *lruc = new LRUCache(2);

  lruc->put(2, 5);
  cout << lruc->get(2) << endl;
  lruc->put(2, 2);
  cout << lruc->get(2) << endl;
  lruc->put(4, 3);
  cout << lruc->get(2) << endl;
  cout << lruc->get(4) << endl;

  delete lruc;
}
