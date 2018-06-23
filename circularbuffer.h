#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <cstddef>
#include <algorithm>
#include <cstring>

template<typename T>
class CircularBuffer {
public:
    template<typename U>
    class Iterator {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = U;
        using pointer = U *;
        using reference = U &;
        using iterator_category = std::random_access_iterator_tag;

        friend class CircularBuffer;

        template<typename V>
        Iterator(const Iterator<V> &other);

        Iterator &operator++();

        Iterator operator++(int);

        Iterator &operator--();

        Iterator operator--(int);

        Iterator operator+(ptrdiff_t);

        Iterator operator-(ptrdiff_t);

        Iterator &operator+=(ptrdiff_t);

        Iterator &operator-=(ptrdiff_t);

        U &operator*() const;

        U *operator->() const;

        friend bool operator==(Iterator const &lhs, Iterator const &rhs);

        friend bool operator!=(Iterator const &lhs, Iterator const &rhs);

        friend ptrdiff_t operator-(Iterator const &lhs, Iterator const &rhs);

        friend bool operator<(Iterator const &lhs, Iterator const &rhs);

        friend bool operator>=(Iterator const &lhs, Iterator const &rhs);

        friend bool operator>(Iterator const &lhs, Iterator const &rhs);

        friend bool operator<=(Iterator const &lhs, Iterator const &rhs);

    private:
        explicit Iterator(U *, size_t, size_t, size_t);

        U *ptr_;
        size_t index, beg, capacity;
    };

    using iterator = Iterator<T>;
    using const_iterator = Iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin();

    const_iterator begin() const;

    iterator end();

    const_iterator end() const;

    reverse_iterator rbegin();

    const_reverse_iterator rbegin() const;

    reverse_iterator rend();

    const_reverse_iterator rend() const;

private:
    T *deque;
    size_t size_, start_, end_, capacity;

    size_t getPrev(size_t);

    size_t getNext(size_t);

public:
    explicit CircularBuffer(size_t = 0);

    CircularBuffer(CircularBuffer const &);

    CircularBuffer &operator=(CircularBuffer const &);

    void swap(CircularBuffer &);

    ~CircularBuffer();

    void resize(size_t);

    void push_front(T const &); //strong

    void push_back(T const &a); //strong

    void pop_front() noexcept;

    void pop_back() noexcept;

    void insert(iterator &pos_iter, T const &value); //basic

    void erase(iterator const &pos_iter); //basic

    T &operator[](size_t i) noexcept;

    T operator[](size_t i) const noexcept;

    T front() const noexcept;

    T &front() noexcept;

    T back() const noexcept;

    T &back() noexcept;

    bool empty() const noexcept;

    void clear() noexcept;
};

template<typename T>
template<typename U>
template<typename V>
CircularBuffer<T>::Iterator<U>::Iterator(const CircularBuffer::Iterator<V> &other) :
        ptr_(other.ptr_), beg(other.beg), index(other.index), capacity(other.capacity) {}

template<typename T>
template<typename U>
CircularBuffer::Iterator &CircularBuffer<T>::Iterator<U>::operator++() {
    *this += 1;
    return *this;
}

template<typename T>
template<typename U>
CircularBuffer::Iterator CircularBuffer<T>::Iterator<U>::operator++(int) {
    Iterator tmp(ptr_);
    *this += 1;
    return tmp;
}

template<typename T>
template<typename U>
CircularBuffer::Iterator &CircularBuffer<T>::Iterator<U>::operator--() {
    *this -= 1;
    return *this;
}

template<typename T>
template<typename U>
CircularBuffer::Iterator CircularBuffer<T>::Iterator<U>::operator--(int) {
    Iterator tmp(ptr_);
    *this -= 1;
    return tmp;
}

template<typename T>
template<typename U>
CircularBuffer::Iterator CircularBuffer<T>::Iterator<U>::operator+(ptrdiff_t k) {
    Iterator tmp(ptr_, index, beg, capacity);
    return tmp += k;
}

template<typename T>
template<typename U>
CircularBuffer::Iterator CircularBuffer<T>::Iterator<U>::operator-(ptrdiff_t k) {
    Iterator tmp(ptr_, index, beg, capacity);
    return tmp -= k;
}

template<typename T>
template<typename U>
CircularBuffer::Iterator &CircularBuffer<T>::Iterator<U>::operator+=(ptrdiff_t k) {
    index += k;
    ptr_ += k;
    if (index >= capacity) {
        index -= capacity;
        ptr_ -= capacity;
    }
    return *this;
}

template<typename T>
template<typename U>
CircularBuffer::Iterator &CircularBuffer<T>::Iterator<U>::operator-=(ptrdiff_t k) {
    if (index < k) {
        index += capacity;
        ptr_ += capacity;
    }
    index -= k;
    ptr_ -= k;
    return *this;
}

template<typename T>
template<typename U>
U &CircularBuffer<T>::Iterator<U>::operator*() const {
    return *ptr_;
}

template<typename T>
template<typename U>
U *CircularBuffer<T>::Iterator<U>::operator->() const {
    return ptr_;
}

bool operator==(const CircularBuffer::Iterator &lhs, const CircularBuffer::Iterator &rhs) {
    return lhs.ptr_ == rhs.ptr_;
}

bool operator!=(const CircularBuffer::Iterator &lhs, const CircularBuffer::Iterator &rhs) {
    return lhs.ptr_ != rhs.ptr_;
}

template<typename T>
template<typename U>
CircularBuffer<T>::Iterator<U>::Iterator(U *ptr, size_t ind, size_t beg, size_t cap)
        : ptr_(ptr), index(ind), beg(beg), capacity(cap) {}

ptrdiff_t operator-(const CircularBuffer::Iterator &lhs, const CircularBuffer::Iterator &rhs) {
    if ((lhs.index >= lhs.beg) ^ (rhs.index >= rhs.beg)) {
        return lhs.index - rhs.index;
    }
    if (lhs.index < rhs.index) {
        return lhs.index - rhs.index + lhs.capacity;
    }
    return lhs.index - rhs.index - lhs.capacity;
}

bool operator<(const CircularBuffer::Iterator &lhs, const CircularBuffer::Iterator &rhs) {
    if ((lhs.index >= lhs.beg) ^ (rhs.index >= rhs.beg)) {
        return lhs.index < rhs.index;
    }
    return lhs.index > rhs.index;
}

bool operator>=(const CircularBuffer::Iterator &lhs, const CircularBuffer::Iterator &rhs) {
    return !(lhs < rhs);
}

bool operator>(const CircularBuffer::Iterator &lhs, const CircularBuffer::Iterator &rhs) {
    return rhs < lhs;
}

bool operator<=(const CircularBuffer::Iterator &lhs, const CircularBuffer::Iterator &rhs) {
    return !(lhs > rhs);
}

template<typename T>
size_t CircularBuffer<T>::getPrev(size_t i) {
    if (i == 0) {
        return capacity - 1;
    }
    return i - 1;
}

template<typename T>
size_t CircularBuffer<T>::getNext(size_t i) {
    if (i == capacity - 1) {
        return 0;
    }
    return i + 1;
}

template<typename T>
CircularBuffer<T>::CircularBuffer(size_t n) {
    deque = operator new(sizeof(T) * n);
    capacity = n;
    start_ = end_ = size_ = 0;
}

template<typename T>
CircularBuffer<T>::CircularBuffer(CircularBuffer const &other) : CircularBuffer(other.size_) {
    std::copy(other.deque, other.deque + size_, deque);
}

template<typename T>
CircularBuffer<T> &CircularBuffer<T>::operator=(CircularBuffer const &other) {
    CircularBuffer(other).swap(*this);
    return *this;
}

template<typename T>
void CircularBuffer<T>::swap(CircularBuffer &other) {
    std::swap(deque, other.deque);
    std::swap(size_, other.size_);
    std::swap(start_, other.start_);
    std::swap(end_, other.end_);
}

template<typename T>
CircularBuffer<T>::~CircularBuffer() {
    for (size_t i = 0; i < capacity; ++i) {
        (deque + i)->~T();
    }
    operator delete(deque);
}

template<typename T>
void CircularBuffer<T>::resize(size_t new_cap) {
    CircularBuffer newBuf = CircularBuffer(new_cap);
    for (size_t i = 0; i < size_; ++i) {
        newBuf.push_back(operator[](i));
    }
    swap(newBuf);
}

template<typename T>
void CircularBuffer<T>::push_front(const T &a) { //strong
    if (capacity == size_) {
        resize(capacity ? capacity * 2 : 1);
    }
    deque[getPrev(start_)] = a;
    start_ = getPrev(start_);
    ++size_;
}

template<typename T>
void CircularBuffer<T>::push_back(const T &a) { //strong
    if (capacity == size_) {
        resize(capacity ? capacity * 2 : 1);
    }
    deque[end_] = a;
    end_ = getNext(end_);
    ++size_;
}

template<typename T>
void CircularBuffer<T>::pop_front() noexcept {
    start_ = getNext(start_);
    --size_;
}

template<typename T>
void CircularBuffer<T>::pop_back() noexcept {
    end_ = getPrev(end_);
    --size_;
}

template<typename T>
void CircularBuffer<T>::insert(CircularBuffer::iterator &pos_iter, const T &value) { //basic
    size_t pos = (size_t)(pos_iter - begin());
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
}

template<typename T>
void CircularBuffer<T>::erase(const CircularBuffer::iterator &pos_iter) { //basic
    size_t pos = (size_t)(pos_iter - begin());
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
}

template<typename T>
T &CircularBuffer<T>::operator[](size_t i) noexcept {
    if (start_ + i >= capacity)
        return deque[start_ + i - capacity];
    return deque[start_ + i];
}

template<typename T>
T CircularBuffer<T>::operator[](size_t i) const noexcept {
    if (start_ + i >= capacity)
        return deque[start_ + i - capacity];
    return deque[start_ + i];
}

template<typename T>
T CircularBuffer<T>::front() const noexcept {
    return operator[](0);
}

template<typename T>
T &CircularBuffer<T>::front() noexcept {
    return operator[](0);
}

template<typename T>
T CircularBuffer<T>::back() const noexcept {
    return operator[](size_ - 1);
}

template<typename T>
T &CircularBuffer<T>::back() noexcept {
    return operator[](size_ - 1);
}

template<typename T>
bool CircularBuffer<T>::empty() const noexcept {
    return size_ == 0;
}

template<typename T>
void CircularBuffer<T>::clear() noexcept {
    for (size_t i = 0; i < capacity; ++i) {
        (deque + i)->~T();
    }
    operator delete(deque);
    size_ = capacity = start_ = end_ = 0;
}

template<typename T>
CircularBuffer::iterator CircularBuffer<T>::begin() {
    return CircularBuffer::iterator(deque + start_, start_, start_, capacity);
}

template<typename T>
CircularBuffer::const_iterator CircularBuffer<T>::begin() const {
    return CircularBuffer::const_iterator(deque + start_, start_, start_, capacity);
}

template<typename T>
CircularBuffer::reverse_iterator CircularBuffer<T>::rbegin() {
    return CircularBuffer::reverse_iterator(end());
}

template<typename T>
CircularBuffer::const_reverse_iterator CircularBuffer<T>::rbegin() const {
    return CircularBuffer::const_reverse_iterator(end());
}

template<typename T>
CircularBuffer::reverse_iterator CircularBuffer<T>::rend() {
    return CircularBuffer::reverse_iterator(begin());
}

template<typename T>
CircularBuffer::const_reverse_iterator CircularBuffer<T>::rend() const {
    return CircularBuffer::const_reverse_iterator(begin());
}

template<typename T>
CircularBuffer::iterator CircularBuffer<T>::end() {
    return CircularBuffer::iterator(deque + end_, end_, start_, capacity);
}

template<typename T>
CircularBuffer::const_iterator CircularBuffer<T>::end() const {
    return CircularBuffer::iterator(deque + end_, end_, start_, capacity);
}

template<typename T>
void swap(CircularBuffer<T> &lhs, CircularBuffer<T> &rhs) noexcept {
    lhs.swap(rhs);
}

#endif //CIRCULAR_BUFFER_H