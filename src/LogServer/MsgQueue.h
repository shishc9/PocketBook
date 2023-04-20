#ifndef POCKET_MSG_QUEUE_H
#define POCKET_MSG_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>
#include <sys/time.h>

template <class T>
class MsgQueue {
public:

  explicit MsgQueue(size_t MaxCapacity = 100);
  ~MsgQueue();

  void clear();
  bool empty();
  bool full();
  void close();
  size_t size();
  size_t capacity();
  T front();
  T back();
  void push_back(const T &item);
  void push_front(const T &item);
  bool pop(T &item);
  bool pop(T &item, int timeout = 3);
  void flush();

private:
  std::deque<T> deq_;
  size_t capacity_;
  bool isClose_;

  std::mutex mutex_;
  std::condition_variable condConsumer_;
  std::condition_variable condProducer_;
};

#endif