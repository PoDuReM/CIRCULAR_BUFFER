#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <cstddef>
#include <algorithm>
#include <cstring>

template<typename T>
class CircularBuffer {
private:
    T *deque;
    size_t size_, start_, end_, capacity;

    size_t getPrev(size_t i) {
        if (i == 0) {
            return capacity - 1;
        }
        return i - 1;
    }

    size_t getNext(size_t i) {
        if (i == capacity - 1) {
            return 0;
        }
        return i + 1;
    }

public:
    explicit CircularBuffer(size_t n = 0) {
        deque = operator new(sizeof(T) * n);
        capacity = n;
        start_ = end_ = size_ = 0;
    }

    CircularBuffer(CircularBuffer const &other) : CircularBuffer(other.size_) {
        std::copy(other.deque, other.deque + size_, deque);
    }

    CircularBuffer &operator=(CircularBuffer const &other) {
        CircularBuffer(other).swap(*this);
        return *this;
    }

    void swap(CircularBuffer &other) {
        std::swap(deque, other.deque);
        std::swap(size_, other.size_);
        std::swap(start_, other.start_);
        std::swap(end_, other.end_);
    }

    ~CircularBuffer() {
        for (size_t i = 0; i < capacity; ++i) {
            (deque + i)->~T();
        }
        operator delete(deque);
    };

    void resize(size_t new_cap) {
        CircularBuffer newBuf = CircularBuffer(new_cap);
        for (size_t i = 0; i < size_; ++i) {
            newBuf.push_back(operator[](i));
        }
        swap(newBuf);
    }

    void push_front(T const &a) { //strong
        if (capacity == size_) {
            resize(capacity ? capacity * 2 : 1);
        }
        deque[getPrev(start_)] = a;
        start_ = getPrev(start_);
        ++size_;
    }

    void push_back(T const &a) { //strong
        if (capacity == size_) {
            resize(capacity ? capacity * 2 : 1);
        }
        deque[end_] = a;
        end_ = getNext(end_);
        ++size_;
    }

    void pop_front() noexcept {
        start_ = getNext(start_);
        --size_;
    }

    void pop_back() noexcept {
        end_ = getPrev(end_);
        --size_;
    }

    void insert(Iterator const &pos_iter, T const &value) { //basic
        try {
            size_t pos = pos_iter.getInd();
            if (capacity == size_) {
                resize(capacity ? capacity * 2 : 1);
            }
            if (pos < size_ - pos) {
                push_front(front());
                for (size_t i = 0; i < pos; ++i) {
                    operator[](i) = operator[](getNext(i));
                }
            } else {
                push_back(back());
                for (size_t i = size_ - 1; i > pos; --i) {
                    operator[](i) = operator[](getPrev(i));
                }
            }
            operator[](pos) = value;
        } catch (...) {}
    }

    void erase(Iterator const &pos_iter) { //basic
        try {
            size_t pos = pos_iter.getInd();
            if (pos < size_ - pos) {
                for (size_t i = pos; i > 0; --i) {
                    operator[](i) = operator[](getPrev(i));
                }
                pop_front();
            } else {
                if (size_ > 1) {
                    for (size_t i = pos; i < size_ - 2; ++i) {
                        operator[](i) = operator[](getNext(i));
                    }
                }
                pop_back();
            }
        } catch (...) {}
    }

    T &operator[](size_t i) noexcept {
        if (start_ + i >= capacity)
            return deque[start_ + i - capacity];
        return deque[start_ + i];
    }

    T operator[](size_t i) const noexcept {
        if (start_ + i >= capacity)
            return deque[start_ + i - capacity];
        return deque[start_ + i];
    }

    T front() const noexcept {
        return operator[](0);
    }

    T &front() noexcept {
        return operator[](0);
    }

    T back() const noexcept {
        return operator[](size_ - 1);
    }

    T &back() noexcept {
        return operator[](size_ - 1);
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    void clear() noexcept {
        for (size_t i = 0; i < capacity; ++i) {
            (deque + i)->~T();
        }
        operator delete(deque);
        size_ = capacity = start_ = end_ = 0;
    }
};

template <typename T>
void swap(CircularBuffer<T>& lhs, CircularBuffer<T>& rhs) noexcept {
    lhs.swap(rhs);
}

#endif //CIRCULAR_BUFFER_H