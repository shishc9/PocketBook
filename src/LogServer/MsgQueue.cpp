#include <cassert>
#include <chrono>

#include "MsgQueue.h"

template <class T>
MsgQueue<T>::MsgQueue(size_t MaxCapacity) {
  capacity_ = MaxCapacity;
  assert(MaxCapacity > 0);
  isClose_ = false;
}

template <class T>
MsgQueue<T>::~MsgQueue() {
  close();
}

template <class T>
void MsgQueue<T>::close() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    deq_.clear();
    isClose_ = true;
  }
  condConsumer_.notify_all();
  condProducer_.notify_all();
}

template <class T>
void MsgQueue<T>::flush() {
  condConsumer_.notify_one();
}

template <class T>
void MsgQueue<T>::clear() {
  deq_.clear();
}

template <class T>
T MsgQueue<T>::front() {
  std::lock_guard<std::mutex> lock(mutex_);
  return deq_.front();
}

template <class T>
T MsgQueue<T>::back() {
  std::lock_guard<std::mutex> lock(mutex_);
  return deq_.back();
}

template <class T>
size_t MsgQueue<T>::size() {
  std::lock_guard<std::mutex> lock(mutex_);
  return deq_.size();
}

template <class T>
size_t MsgQueue<T>::capacity() {
  std::lock_guard<std::mutex> lock(mutex_);
  return capacity_;
}

template <class T>
void MsgQueue<T>::push_back(const T &item) {
  std::unique_lock<std::mutex> lock(mutex_);
  while (deq_.size() >= capacity_) {
    condProducer_.wait(lock);
  }
  deq_.push_back(item);
  condConsumer_.notify_one();
}

template <class T>
void MsgQueue<T>::push_front(const T &item) {
  std::unique_lock<std::mutex> lock(mutex_);
  while (deq_.size() >= capacity_) {
    condProducer_.wait(lock);
  }
  deq_.push_front(item);
  condConsumer_.notify_one();
}

template <class T>
bool MsgQueue<T>::empty() {
  std::lock_guard<std::mutex> lock(mutex_);
  return deq_.empty();
}

template <class T>
bool MsgQueue<T>::full() {
  std::lock_guard<std::mutex> lock(mutex_);
  return deq_.size() >= capacity_;
}

template <class T>
bool MsgQueue<T>::pop(T &item) {
  std::unique_lock<std::mutex> lock(mutex_);
  while (deq_.empty()) {
    condConsumer_.wait(lock);
    if (isClose_) {
      return false;
    }
  }
  item = deq_.front();
  deq_.pop_front();
  condProducer_.notify_one();
  return true;
}

template <class T>
bool MsgQueue<T>::pop(T &item, int timeout) {
  std::unique_lock<std::mutex> lock(mutex_);
  while (deq_.empty()) {
    if (condConsumer_.wait_for(lock, std::chrono::seconds(timeout)) == std::cv_status::timeout) {
      return false;
    }
    if (isClose_) {
      return false;
    }
  }
  item = deq_.front();
  deq_.pop_front();
  condProducer_.notify_one();
  return true;
}
