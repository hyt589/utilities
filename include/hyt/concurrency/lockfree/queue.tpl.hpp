#pragma once

#include <atomic>
#include <memory>
namespace hyt {

namespace concurrency {

namespace lockfree {

// reference:
// https://github.com/craflin/LockFreeQueue/blob/master/LockFreeQueueCpp11.h
template <typename T> class Queue {
public:
  explicit Queue(size_t capacity) {
    m_capacity_mask = capacity - 1;
    for (size_t i = 1; i <= sizeof(void *) * 4; i <<= 1)
      m_capacity_mask |= m_capacity_mask >> i;
    m_capacity = m_capacity_mask + 1;

    m_queue = (Node *)new char[sizeof(Node) * m_capacity];
    for (size_t i = 0; i < m_capacity; ++i) {
      m_queue[i].tail.store(i, std::memory_order_relaxed);
      m_queue[i].head.store(-1, std::memory_order_relaxed);
    }

    m_tail.store(0, std::memory_order_relaxed);
    m_head.store(0, std::memory_order_relaxed);
  }

  ~Queue() {
    for (size_t i = m_head; i != m_tail; ++i)
      (&m_queue[i & m_capacity_mask].data)->~T();

    delete[] (char *)m_queue;
  }

  size_t Capacity() const { return m_capacity; }

  size_t Size() const {
    size_t head = m_head.load(std::memory_order_acquire);
    return m_tail.load(std::memory_order_relaxed) - head;
  }

  bool Push(const T &data) {
    Node *node;
    size_t tail = m_tail.load(std::memory_order_relaxed);
    for (;;) {
      node = &m_queue[tail & m_capacity_mask];
      if (node->tail.load(std::memory_order_relaxed) != tail)
        return false;
      if ((m_tail.compare_exchange_weak(tail, tail + 1,
                                        std::memory_order_relaxed)))
        break;
    }
    new (&node->data) T(data);
    node->head.store(tail, std::memory_order_release);
    return true;
  }

  bool Pop(T &result) {
    Node *node;
    size_t head = m_head.load(std::memory_order_relaxed);
    for (;;) {
      node = &m_queue[head & m_capacity_mask];
      if (node->head.load(std::memory_order_relaxed) != head)
        return false;
      if (m_head.compare_exchange_weak(head, head + 1,
                                       std::memory_order_relaxed))
        break;
    }
    result = node->data;
    (&node->data)->~T();
    node->tail.store(head + m_capacity, std::memory_order_release);
    return true;
  }

private:
  struct Node {
    T data;
    std::atomic<size_t> tail;
    std::atomic<size_t> head;
  };

private:
  size_t m_capacity_mask;
  Node *m_queue;
  size_t m_capacity;
  char m_cache_line_pad1[64];
  std::atomic<size_t> m_tail;
  char m_cache_line_pad2[64];
  std::atomic<size_t> m_head;
  char m_cache_line_pad3[64];
};

} // namespace lockfree

} // namespace concurrency

} // namespace hyt
