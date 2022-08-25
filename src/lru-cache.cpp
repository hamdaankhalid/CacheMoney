#include <map>
#include "./lru-cache.h"
#include <iostream>
#include <mutex>

using namespace std;

LinkedList::LinkedList(int k, int v)
{
  key = k;
  val = v;
  prev = NULL;
  next = NULL;
}

LRUCache::LRUCache(int cap)
{
  size = 0;
  capacity = cap;
  dummyHead = new LinkedList(-9909, -9909);
  dummyTail = new LinkedList(9909, 9909);
  dummyHead->next = dummyTail;
  dummyTail->prev = dummyHead;
}

LRUCache::~LRUCache()
{
  for (auto const& entry : valToNode) {
    // cout << entry.first << " , " << entry.second->key << endl;
    delete entry.second;
  }

  delete dummyHead;
  delete dummyTail;
}

int LRUCache::get(int key)
{
  mu.lock();
  cout << "Get Cmd " << key << endl;
  // cout << "get " << key << endl;
  if (valToNode.find(key) == valToNode.end())
  {
    // cout << "key not in map" << endl;
    mu.unlock();
    cout << "No key found" << endl;
    return -1;
  }

  LinkedList *nodeForKey = valToNode.at(key);

  // move the key's node to the right most part of the linkedlist

  // remove it from it's current spot
  LinkedList *olderNodeForKeyPrev = nodeForKey->prev;
  LinkedList *olderNodeForKeyNext = nodeForKey->next;

  // connect the gap it leaves when removed from current spot
  olderNodeForKeyPrev->next = olderNodeForKeyNext;
  olderNodeForKeyNext->prev = olderNodeForKeyPrev;

  // add to the front most part between dummyTail and actualy one before dummy tail

  LinkedList *oneBeforeRightMost = dummyTail->prev;

  oneBeforeRightMost->next = nodeForKey;
  nodeForKey->prev = oneBeforeRightMost;
  nodeForKey->next = dummyTail;
  dummyTail->prev = nodeForKey;
  int val = nodeForKey->val;
  cout << "Val for " << key << ": " << val << endl;
  mu.unlock();
  return val;
}

void LRUCache::put(int key, int value)
{
  mu.lock();
  cout << "Put Cmd " << key << ": " << value << endl;
  if (valToNode.find(key) != valToNode.end())
  {
    // duplicate exists so remove from the map
    LinkedList *nodeForKey = valToNode.at(key);

    LinkedList *leftOfDuplicate = nodeForKey->prev;
    LinkedList *rightOfDuplicate = nodeForKey->next;
    leftOfDuplicate->next = rightOfDuplicate;
    rightOfDuplicate->prev = leftOfDuplicate;

    valToNode.erase(key);
    delete nodeForKey;

    size--;
  }

  LinkedList *nodeToBeAdded = new LinkedList(key, value);
  if (size >= capacity)
  {
    // evict the left most
    LinkedList *leftMost = dummyHead->next;

    LinkedList *oneAfterLeftMost = leftMost->next;

    dummyHead->next = oneAfterLeftMost;
    oneAfterLeftMost->prev = dummyHead;
    valToNode.erase(leftMost->key);
    delete leftMost;

    size--;
  }

  // always add node to the right most part of the list
  LinkedList *oneBeforeRightMost = dummyTail->prev;
  nodeToBeAdded->next = dummyTail;
  nodeToBeAdded->prev = oneBeforeRightMost;

  oneBeforeRightMost->next = nodeToBeAdded;
  dummyTail->prev = nodeToBeAdded;

  // add the node in the map val to node
  valToNode.insert(make_pair(key, nodeToBeAdded));

  size++;

  mu.unlock();
}