#ifndef queue_h
#define queue_h

#define SIZE 10
 
class Queue {
  private:
    double *arr;       // array to store queue elements
    int capacity;   // maximum capacity of the queue
    int front;      // front points to the front element in the queue (if any)
    int rear;       // rear points to the last element in the queue
    int count;      // current size of the queue

  public:
    Queue(int size = SIZE);     // constructor
    ~Queue();                   // destructor

    void dequeue();
    void enqueue(double x);
    double peek();
    int size();
    bool isEmpty();
    bool isFull();
    double *list();
};

#endif
