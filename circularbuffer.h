#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <cstddef>
#include <algorithm>
#include <cstring>

template<typename T>
class circular_buffer {
public:
    template<typename U>
    class Iterator {
    public:
        using difference_type = ptrdiff_t;
        using value_type = U;
        using pointer = U *;
        using reference = U &;
        using iterator_category = std::random_access_iterator_tag;

        friend class circular_buffer;

        template<typename V>
        Iterator(const Iterator<V> &other, typename std::enable_if<std::is_same<U, const V>::value>::type * = nullptr) :
                ptr_(other.ptr_), beg(other.beg), index(other.index), capacity(other.capacity) {}

        Iterator<U> &operator++() {
            *this += 1;
            return *this;
        }

        const Iterator<U> operator++(int) {
            Iterator tmp(ptr_, index, beg, capacity);
            *this += 1;
            return tmp;
        }

        Iterator<U> &operator--() {
            *this -= 1;
            return *this;
        }

        const Iterator<U> operator--(int) {
            Iterator tmp(ptr_, index, beg, capacity);
            *this -= 1;
            return tmp;
        }

        Iterator<U> operator+(ptrdiff_t k) {
            Iterator tmp(ptr_, index, beg, capacity);
            return tmp += k;
        }

        Iterator<U> operator-(ptrdiff_t k) {
            Iterator tmp(ptr_, index, beg, capacity);
            return tmp -= k;
        }

        Iterator<U> &operator+=(ptrdiff_t k) {
            if (k < 0) {
                return *this -= -k;
            }
            index += k;
            ptr_ += k;
            if (index >= capacity) {
                index -= capacity;
                ptr_ -= capacity;
            }
            return *this;
        }

        Iterator<U> &operator-=(ptrdiff_t k) {
            if (k < 0) {
                return *this += -k;
            }
            if (index < k) {
                index += capacity;
                ptr_ += capacity;
            }
            index -= k;
            ptr_ -= k;
            return *this;
        }

        U &operator*() const {
            return *ptr_;
        }

        U *operator->() const {
            return ptr_;
        }

        friend bool operator==(Iterator<U> const &lhs, Iterator<U> const &rhs) {
            return lhs.ptr_ == rhs.ptr_;
        }

        friend bool operator!=(Iterator<U> const &lhs, Iterator<U> const &rhs) {
            return lhs.ptr_ != rhs.ptr_;
        }

        friend ptrdiff_t operator-(Iterator<U> const &lhs, Iterator<U> const &rhs) {
            if (((lhs.index < lhs.beg) ^ (rhs.index < rhs.beg)) == 0) {
                return lhs.index - rhs.index;
            }
            if (lhs.index < rhs.index) {
                return lhs.index - rhs.index + lhs.capacity;
            }
            return lhs.index - rhs.index - lhs.capacity;
        }

        friend bool operator<(Iterator<U> const &lhs, Iterator<U> const &rhs) {
            if ((lhs.index >= lhs.beg) ^ (rhs.index >= rhs.beg)) {
                return lhs.index < rhs.index;
            }
            return lhs.index > rhs.index;
        }

        friend bool operator>=(Iterator<U> const &lhs, Iterator<U> const &rhs) {
            return !(lhs < rhs);
        }

        friend bool operator>(Iterator<U> const &lhs, Iterator<U> const &rhs) {
            return rhs < lhs;
        }

        friend bool operator<=(Iterator<U> const &lhs, Iterator<U> const &rhs) {
            return !(lhs > rhs);
        }

    private:
        Iterator(U *ptr, size_t ind, size_t beg, size_t cap) : ptr_(ptr), index(ind), beg(beg), capacity(cap) {}

        U *ptr_;
        size_t index, beg, capacity;
    };

    using iterator = Iterator<T>;
    using const_iterator = Iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return iterator(deque + start_, start_, start_, capacity);
    }

    const_iterator begin() const {
        return const_iterator(deque + start_, start_, start_, capacity);
    }

    iterator end() {
        return iterator(deque + end_, end_, start_, capacity);
    }

    const_iterator end() const {
        return const_iterator(deque + end_, end_, start_, capacity);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

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

    void resize(size_t new_cap) {
        circular_buffer newBuf = circular_buffer(new_cap);
        for (size_t i = 0; i < size_; ++i) {
            newBuf.push_back(operator[](i));
        }
        swap(newBuf);
    }

public:
    explicit circular_buffer(size_t n = 0) {
        deque = (T *) (malloc(sizeof(T) * n));
        capacity = n;
        start_ = end_ = size_ = 0;
    }

    circular_buffer(circular_buffer<T> const &other) : circular_buffer(other.capacity) {
        //std::copy(other.deque, other.deque + capacity, deque);
        for (size_t i = start_; i != end_; i = getNext(i))
            new (deque + i) T(other.deque[i]);
        start_ = other.start_;
        end_ = other.end_;
        size_ = other.size_;
    }

    circular_buffer<T> &operator=(circular_buffer<T> const &other) {
        circular_buffer(other).swap(*this);
        return *this;
    }

    void swap(circular_buffer<T> &other) {
        std::swap(deque, other.deque);
        std::swap(size_, other.size_);
        std::swap(start_, other.start_);
        std::swap(end_, other.end_);
        std::swap(capacity, other.capacity);
    }

    ~circular_buffer() {
        for (size_t i = start_; i != end_; i = getNext(i)) {
            (deque + i)->~T();
        }
        free(deque);
    }

    void push_front(T const &value) { //strong
        if (capacity <= size_ + 1) {
            resize(capacity ? capacity * 2 : 2);
        }
        new (deque + getPrev(start_)) T(value);
        start_ = getPrev(start_);
        ++size_;
    }

    void push_back(T const &value) { //strong
        if (capacity <= size_ + 1) {
            resize(capacity ? capacity * 2 : 2);
        }
        new (deque + end_) T(value);
        end_ = getNext(end_);
        ++size_;
    }

    void pop_front() noexcept {
        start_ = getNext(start_);
        --size_;
        (deque + getPrev(start_))->~T();
    }

    void pop_back() noexcept {
        end_ = getPrev(end_);
        --size_;
        (deque + end_)->~T();
    }

    iterator insert(const_iterator pos_iter, T const &value) { //basic
        size_t pos = (size_t) (pos_iter - begin());
        if (capacity <= size_ + 1) {
            resize(capacity ? capacity * 2 : 2);
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
        pos += start_;
        if (pos >= capacity) {
            pos -= capacity;
        }
        return iterator(deque + pos, pos, start_, capacity);
    }

    iterator erase(const_iterator pos_iter) { //basic
        size_t pos = (size_t) (pos_iter - begin());
        if (pos < size_ - pos) {
            for (size_t i = pos; i > 0; --i) {
                operator[](i) = operator[](getPrev(i));
            }
            pop_front();
        } else {
            if (size_ > 1) {
                for (size_t i = pos; i < size_ - 1; ++i) {
                    operator[](i) = operator[](getNext(i));
                }
            }
            pop_back();
        }
        pos += start_;
        if (pos >= capacity) {
            pos -= capacity;
        }
        return iterator(deque + pos, pos, start_, capacity);
    }

    T &operator[](size_t i) noexcept {
        if (start_ + i >= capacity) {
            return deque[start_ + i - capacity];
        }
        return deque[start_ + i];
    }

    T const &operator[](size_t i) const noexcept {
        if (start_ + i >= capacity) {
            return deque[start_ + i - capacity];
        }
        return deque[start_ + i];
    }

    T const &front() const noexcept {
        return operator[](0);
    }

    T &front() noexcept {
        return operator[](0);
    }

    T const &back() const noexcept {
        return operator[](size_ - 1);
    }

    T &back() noexcept {
        return operator[](size_ - 1);
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    void clear() noexcept {
        for (size_t i = start_; i != end_; i = getNext(i)) {
            (deque + i)->~T();
        }
        free(deque);
        deque = nullptr;
        size_ = capacity = start_ = end_ = 0;
    }

    size_t size() const noexcept {
        return size_;
    }
};

template<typename T>
void swap(circular_buffer<T> &lhs, circular_buffer<T> &rhs) noexcept {
    lhs.swap(rhs);
}

#endif //CIRCULAR_BUFFER_H