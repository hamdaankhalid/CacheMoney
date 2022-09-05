#include <map>
#include "./lru-cache.h"
#include <iostream>
#include <mutex>


LinkedList::LinkedList(int k, int v)
{
  key = k;
  val = v;
  prev = nullptr;
  next = nullptr;
}

LRUCache::LRUCache(int cap)
{
  size = 0;
  capacity = cap;
  dummyHead = std::make_shared<LinkedList>(-9909, -9909);
  dummyTail = std::make_shared<LinkedList>(9909, 9909);
  dummyHead->next = dummyTail;
  dummyTail->prev = dummyHead;
}

int LRUCache::get(int key)
{
  std::lock_guard<std::mutex> lck (mu);
  std::cout << "Get Cmd " << key << std::endl;
  if (valToNode.find(key) == valToNode.end())
  {
    std::cout << "No key found" << std::endl;
    return -1;
  }

  std::shared_ptr<LinkedList>nodeForKey = valToNode.at(key);

  // move the key's node to the right most part of the linkedlist
  // remove it from it's current spot
  std::shared_ptr<LinkedList> olderNodeForKeyPrev = nodeForKey->prev;
  std::shared_ptr<LinkedList> olderNodeForKeyNext = nodeForKey->next;

  // connect the gap it leaves when removed from current spot
  olderNodeForKeyPrev->next = olderNodeForKeyNext;
  olderNodeForKeyNext->prev = olderNodeForKeyPrev;

  // add to the front most part between dummyTail and actualy one before dummy tail

  std::shared_ptr<LinkedList> oneBeforeRightMost = dummyTail->prev;

  oneBeforeRightMost->next = nodeForKey;
  nodeForKey->prev = oneBeforeRightMost;
  nodeForKey->next = dummyTail;
  dummyTail->prev = nodeForKey;
  int val = nodeForKey->val;
  std::cout << "Val for " << key << ": " << val << std::endl;
  return val;
}

void LRUCache::put(int key, int value)
{
  std::lock_guard<std::mutex> lck (mu);
  std::cout << "Put Cmd " << key << ": " << value << std::endl;
  if (valToNode.find(key) != valToNode.end())
  {
    // duplicate exists so remove from the map
    std::shared_ptr<LinkedList> nodeForKey = valToNode.at(key);

    std::shared_ptr<LinkedList> leftOfDuplicate = nodeForKey->prev;
    std::shared_ptr<LinkedList> rightOfDuplicate = nodeForKey->next;
    leftOfDuplicate->next = rightOfDuplicate;
    rightOfDuplicate->prev = leftOfDuplicate;

    valToNode.erase(key);
    size--;
  }

  std::shared_ptr<LinkedList> nodeToBeAdded = std::make_shared<LinkedList>(key, value);
  if (size >= capacity)
  {
    // evict the left most
    std::shared_ptr<LinkedList>leftMost = dummyHead->next;

    std::shared_ptr<LinkedList>oneAfterLeftMost = leftMost->next;

    dummyHead->next = oneAfterLeftMost;
    oneAfterLeftMost->prev = dummyHead;
    valToNode.erase(leftMost->key);

    size--;
  }

  // always add node to the right most part of the list
  std::shared_ptr<LinkedList> oneBeforeRightMost = dummyTail->prev;
  nodeToBeAdded->next = dummyTail;
  nodeToBeAdded->prev = oneBeforeRightMost;

  oneBeforeRightMost->next = nodeToBeAdded;
  dummyTail->prev = nodeToBeAdded;

  // add the node in the map val to node
  valToNode.insert(make_pair(key, nodeToBeAdded));

  size++;
}
