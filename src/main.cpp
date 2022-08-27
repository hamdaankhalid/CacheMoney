#include <iostream>
#include "./lru-cache.h"
#include <thread>
#include <memory>

using namespace std;

void testThread(LRUCache* lruc) {
  lruc->put(2, 5);
  lruc->get(2);
  lruc->put(2, 2);
  lruc->get(2);
  lruc->put(4, 3);
  lruc->get(2);
  lruc->get(4);
  lruc->get(69);
}

// g++ main.cpp lru-cache.cpp -o ../cachemoney -lstdc++ -std=c++11
int main() {
  LRUCache* lruc = new LRUCache(2);
  
  thread th2(testThread, lruc);
  thread th1(testThread, lruc);

  cout << "Hello from main thread" << endl;

  th1.join();
  th2.join();
  
  delete lruc;
}
