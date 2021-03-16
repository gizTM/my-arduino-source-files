#include "Queue.h"

// Constructor to initialize a queue
Queue::Queue(int size) {
  arr = new double[size];
  capacity = size;
  front = 0;
  rear = -1;
  count = 0;
}
Queue::~Queue() { delete[] arr; }

void Queue::dequeue() {
  if (isEmpty()) return;
  // front = (front + 1) % capacity;
  for (int x=0; x<(count - 1); x++) {
    arr[x] = arr[x+1];
  }
  count--;
}
void Queue::enqueue(double item) {
  if (isFull()) return;
  rear = ((rear == -1 ? rear : (count - 1)) + 1) % capacity;
  arr[rear] = item;
  count++;
}
double Queue::peek() {
  if (isEmpty()) return -1;
  return arr[front];
}
double* Queue::list() {
  return arr;
}

int Queue::size() { return count; }
bool Queue::isEmpty() { return (size() == 0); }
bool Queue::isFull() { return (size() == capacity); }
