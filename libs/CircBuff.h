#include <cstddef>
#include <iterator>
#include <memory>

class const_iterator;
template<typename T, typename Allocator = std::allocator<T>>
class CircBuff {
 public:
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using difference_type = int32_t;
  using size_type = size_t;
  using iterator_category = std::random_access_iterator_tag;

  enum IteratorStatus {
    begin_it, // just returned from begin()
    end_it, // just returned from end()
    regular_it // others
  };

  template<typename ReturnedValueT>
  class CircBuffIterator : public std::iterator<ReturnedValueT, iterator_category> {
   public:
    using value_type = ReturnedValueT;
    using iterator_category = std::random_access_iterator_tag;
    CircBuffIterator<ReturnedValueT>() = default;
    explicit CircBuffIterator<ReturnedValueT>(T* pointer)
        : pointer_(pointer), buff_begin_(nullptr), buff_capacity_(0), buff_end_(0) {}
    CircBuffIterator<ReturnedValueT>(T* pointer, const CircBuff<T>& buff)
        : pointer_(pointer), buff_begin_(buff.begin_), buff_capacity_(buff.capacity_), buff_end_(buff.end_) {}
    CircBuffIterator<ReturnedValueT>(const CircBuffIterator& other)
        : pointer_(other.pointer_),
          buff_begin_(other.buff_begin_),
          buff_capacity_(other.buff_capacity_),
          buff_end_(other.buff_end_),
          status_(other.status_) {}
    ReturnedValueT& operator*() const { return *pointer_; }
    ReturnedValueT* operator->() const {
      return pointer_;
    }
    ReturnedValueT& operator[](difference_type n) const {
      return *(buff_begin_ + (pointer_ - buff_begin_ + n) % buff_capacity_);
    }
    bool operator>(const CircBuffIterator& rhs) const {
      return pointer_ > rhs.pointer_;
    }
    bool operator<(const CircBuffIterator& rhs) const {
      return pointer_ < rhs.pointer_;
    }
    bool operator>=(const CircBuffIterator& rhs) const {
      return pointer_ >= rhs.pointer_;
    }
    bool operator<=(const CircBuffIterator& rhs) const {
      return pointer_ <= rhs.pointer_;
    }
    bool operator==(const CircBuffIterator& it) const {
      if (status_ == begin_it && it.status_ == end_it)
        return false; // used for cycling in loops to prevent begin() == end()

      return (pointer_ == it.pointer_);
    }
    bool operator!=(const CircBuffIterator& it) const {
      if (status_ == begin_it && it.status_ == end_it)
        return true; // used for cycling in loops to prevent begin() == end()

      return pointer_ != it.pointer_;
    }
    const CircBuffIterator<ReturnedValueT> operator++(int) {
      CircBuffIterator<ReturnedValueT> Temp = *this;
      if (pointer_ + 1 != buff_end_) ++pointer_;
      else pointer_ = buff_begin_;
      Temp.status_ = regular_it;
      status_ = regular_it;

      return Temp;
    }
    const CircBuffIterator<ReturnedValueT> operator--(int) {
      CircBuffIterator<ReturnedValueT> Temp = *this;
      if (pointer_ - 1 != buff_begin_) --pointer_;
      else pointer_ = buff_end_ - 1;
      Temp.status_ = regular_it;
      status_ = regular_it;

      return Temp;
    }
    CircBuffIterator<ReturnedValueT>& operator++() {
      if (pointer_ + 1 == buff_end_) pointer_ = buff_begin_;
      else ++pointer_;
      status_ = regular_it;

      return *this;
    }
    CircBuffIterator<ReturnedValueT>& operator--() {
      if (pointer_ - 1 >= buff_begin_) --pointer_;
      else pointer_ = buff_end_ - 1;
      status_ = regular_it;

      return *this;
    }
    CircBuffIterator<ReturnedValueT> operator+(difference_type n) const {
      CircBuffIterator<ReturnedValueT> result(*this);
      result.pointer_ = (buff_begin_ + (pointer_ - buff_begin_ + n) % buff_capacity_);
      return result;
    }
    CircBuffIterator<ReturnedValueT> operator-(difference_type n) const {
      CircBuffIterator<ReturnedValueT> result(*this);
      result.pointer_ = (buff_begin_ + (pointer_ - buff_begin_ + buff_capacity_ - n) % buff_capacity_);
      return result;
    }
    difference_type operator-(const CircBuffIterator<ReturnedValueT>& rhs) const {
      return pointer_ - rhs.pointer_;
    }
    CircBuffIterator<ReturnedValueT>& operator+=(difference_type n) {
      status_ = regular_it;
      pointer_ = buff_begin_ + (pointer_ - buff_begin_ + n) % buff_capacity_;

      return *this;
    }
    CircBuffIterator<ReturnedValueT>& operator-=(difference_type n) {
      status_ = regular_it;
      pointer_ = buff_begin_ + (pointer_ - buff_begin_ + buff_capacity_ - n) % buff_capacity_;

      return *this;
    }
    friend CircBuffIterator<ReturnedValueT> operator+(difference_type n, CircBuffIterator<ReturnedValueT> it) {
      return it + n;
    }

    IteratorStatus status_ = regular_it;
   private:
    size_t buff_capacity_ = 0;
    T* buff_begin_ = nullptr;
    T* buff_end_ = nullptr;
    T* pointer_ = nullptr;
  };

  using iterator = CircBuffIterator<T>;
  using const_iterator = CircBuffIterator<const T>;

  CircBuff() = default;

  explicit CircBuff(size_type capacity) : capacity_(capacity) {
    data_ = alloc_.allocate(capacity);
    begin_ = data_;
    end_ = data_ + capacity;
  }

  CircBuff(size_type capacity, const T& default_value) : capacity_(capacity) {
    data_ = alloc_.allocate(capacity);
    begin_ = data_;
    end_ = data_ + capacity_;
    for (size_t i = 0; i < capacity_; ++i) {
      alloc_.construct(data_ + i, default_value);
    }
    size_ = capacity;
    if (capacity_ != 0)
      tail_ = size_ - 1;
  }

  CircBuff(const std::initializer_list<T>& elements) {
    if (elements.size() != 0) {
      capacity_ = elements.size();
      size_ = capacity_;
      data_ = alloc_.allocate(capacity_);
      begin_ = data_;
      end_ = data_ + capacity_;
      head_ = 0;
      tail_ = size_ - 1;
      std::uninitialized_copy(elements.begin(), elements.end(), data_);
    }
  }

  CircBuff(const iterator& range_start, const iterator& range_end) {
    capacity_ = (range_end - 1) - range_start + 1;
    size_ = capacity_;
    data_ = alloc_.allocate(capacity_);
    begin_ = data_;
    end_ = data_ + capacity_;
    head_ = 0;
    tail_ = size_ - 1;
    size_t i = 0;
    for (auto it = range_start; it != range_end; ++it, ++i) {
      alloc_.construct(data_ + i, *it);
    }
  }

  CircBuff(const CircBuff& other)
      : capacity_(other.capacity_), size_(other.size_), head_(other.head_), tail_(other.tail_) {
    alloc_ = other.alloc_;
    data_ = alloc_.allocate(capacity_);
    std::uninitialized_copy(other.begin_, other.end_, data_);
    begin_ = data_;
    end_ = data_ + capacity_;
  }

  ~CircBuff() {
    for (size_t i = 0; i < capacity_; ++i) {
      alloc_.destroy(data_ + i);
    }
    alloc_.deallocate(data_, capacity_);
  }

  iterator begin() const {
    if (capacity_ == 0) return iterator();
    iterator result(begin_ + head_, *this);
    result.status_ = begin_it;

    return result;
  }

  iterator end() const {
    if (capacity_ == 0) return iterator();
    iterator result(begin_ + (tail_ + 1) % capacity_, *this);
    result.status_ = end_it;

    return result;
  }

  const_iterator cbegin() const {
    if (capacity_ == 0) return const_iterator();
    const_iterator result(begin_ + head_, *this);
    result.status_ = begin_it;

    return result;
  }

  const_iterator cend() const {
    if (capacity_ == 0) return const_iterator();
    const_iterator result(begin_ + (tail_ + 1) % capacity_, *this);
    result.status_ = end_it;

    return result;
  }

  [[nodiscard]] bool empty() const {
    return size_ == 0;
  }

  [[nodiscard]] size_type size() const {
    return size_;
  }

  [[nodiscard]] size_type capacity() const {
    return capacity_;
  }

  [[nodiscard]] size_type max_size() const {
    return alloc_.max_size();
  }

  CircBuff& operator=(const CircBuff& other) {
    if (this != &other) { // avoiding self copy
      if (data_ != nullptr) {
        for (size_t i = 0; i < capacity_; ++i) {
          alloc_.destroy(data_ + i);
        }
        alloc_.deallocate(data_, capacity_);
      }
      alloc_ = other.alloc_;
      capacity_ = other.capacity_;
      data_ = alloc_.allocate(capacity_);
      if (other.data_ != nullptr) std::uninitialized_copy(other.begin(), other.end(), data_);
      begin_ = data_;
      end_ = data_ + capacity_;
      size_ = other.size_;
      head_ = other.head_;
      tail_ = other.tail_;
    }

    return *this;
  }
  value_type& operator[](size_type n) {
    return *(data_ + n % size_);
  }

  CircBuff& operator=(std::initializer_list<T> elements) {
    if (data_ != nullptr) {
      for (size_t i = 0; i < capacity_; ++i) {
        alloc_.destroy(data_ + i);
      }
      alloc_.deallocate(data_, capacity_);
    }
    capacity_ = elements.size();
    size_ = capacity_;
    data_ = alloc_.allocate(capacity_);
    begin_ = data_;
    end_ = data_ + capacity_;
    head_ = 0;
    if (elements.size() != 0) {
      tail_ = size_ - 1;
      std::uninitialized_copy(elements.begin(), elements.end(), data_);
    } else tail_ = 0;

    return *this;
  }

  virtual void push(const T& el) {
    if (capacity_ == 0) throw std::runtime_error("push to capacity=0 buffer");
    if (!empty()) {
      tail_ = (++tail_) % capacity_;
      if (head_ == tail_) head_ = (++head_) % capacity_;
    }
    *(begin_ + tail_) = el;
    if (size_ < capacity_) ++size_;
  }

  void pop() {
    if (empty()) throw std::runtime_error("pop from empty buffer");
    head_ = (++head_) % capacity_;
    if (size_ > 0) --size_;
  }

  void reserve(size_type new_capacity) {
    if (new_capacity > capacity_) {
      T* new_data = alloc_.allocate(new_capacity);
      for (size_t i = 0; i < size_; ++i) {
        alloc_.construct(new_data + i, *(begin_ + i));
        alloc_.destroy(begin_ + i);
      }
      alloc_.deallocate(data_, capacity_);
      capacity_ = new_capacity;
      data_ = new_data;
      begin_ = data_;
      end_ = data_ + capacity_;
    }
  }

  void resize(size_type new_capacity, const T& default_value = T()) {
    if (new_capacity == capacity_) return;
    T* new_data = alloc_.allocate(new_capacity);
    size_t i = 0;
    size_t new_size = 0;
    for (auto it = begin(); it != end() && new_size < new_capacity; ++it, ++i, ++new_size) {
      alloc_.construct(new_data + i, *it);
    }
    for (i = size_; i < new_capacity; ++i) {
      alloc_.construct(new_data + i, default_value);
      ++new_size;
    }
    for (size_t j = 0; j < capacity_; ++j) {
      alloc_.destroy(data_ + j);
    }
    alloc_.deallocate(data_, capacity_);
    data_ = new_data;
    capacity_ = new_capacity;
    begin_ = data_;
    end_ = data_ + capacity_;
    head_ = 0;
    size_ = new_size;
    if (size_ != 0) tail_ = size_ - 1;
  }

  iterator insert(const iterator& it, const value_type& value) {
    bool found = false;
    size_type new_capacity = capacity_;
    if (size_ == capacity_) ++new_capacity;
    T* new_data = alloc_.allocate(new_capacity);
    size_t i = 0;
    iterator result;
    for (auto cur = begin(); cur != end() && i < new_capacity; ++cur) {
      if (cur == it && i == 0 && !found) {
        found = true;
        alloc_.construct(new_data + i, value);
        result = iterator(new_data + i);
        ++i;
      }
      alloc_.construct(new_data + i, *cur);
      ++i;
      if (cur + 1 == it && !found) {
        found = true;
        alloc_.construct(new_data + i, value);
        result = iterator(new_data + i);
        ++i;
      }
    }
    if (!found) {
      throw std::runtime_error("cannot insert before not found iterator");
    }
    for (size_t j = 0; j < capacity_; ++j) {
      alloc_.destroy(data_ + j);
    }
    alloc_.deallocate(data_, capacity_);
    capacity_ = new_capacity;
    data_ = new_data;
    begin_ = data_;
    end_ = data_ + capacity_;
    ++size_;
    head_ = 0;
    tail_ = size_ - 1;
    return result;
  }

  iterator insert(const iterator& it, size_type n, const value_type& value) {
    iterator before = it;
    iterator result = it;
    bool first_inserted = false;
    for (size_t i = 0; i < n; ++i) {
      before = insert(before, value);
      if (!first_inserted) result = before;
      first_inserted = true;
    }
    return result;
  }

  iterator insert(const iterator& it, const iterator& range_start, const iterator& range_end) {
    iterator before = it;
    iterator result = it;
    bool first_inserted = false;
    size_t i = 0;
    for (auto range_val = range_end - 1; i <= (range_end - 1 - range_start); --range_val, ++i) {
      before = insert(before, *range_val);
      if (!first_inserted) result = before;
      first_inserted = true;
    }
    return result;
  }

  iterator insert(const iterator& it, std::initializer_list<T> range) {
    iterator before = it;
    iterator result = it;
    bool first_inserted = false;
    size_t i = 0;
    if (range.size() != 0) {
      for (auto range_val = range.end() - 1; i < range.size(); --range_val, ++i) {
        before = insert(before, *range_val);
        if (!first_inserted) result = before;
        first_inserted = true;
      }
    }
    return result;
  }

  iterator erase(const iterator& it) {
    bool found = false;
    T* new_data = alloc_.allocate(capacity_);
    size_t i = 0;
    iterator result = end();
    for (auto cur = begin(); cur != end(); ++cur) {
      if (cur == it && !found) {
        found = true;
        --size_;
        result = cur + 1;
      } else {
        alloc_.construct(new_data + i, *cur);
        ++i;
      }
    }
    if (!found) {
      throw std::runtime_error("cannot erase not found iterator");
    }
    for (size_t j = 0; j < capacity_; ++j) {
      alloc_.destroy(data_ + j);
    }
    alloc_.deallocate(data_, capacity_);
    data_ = new_data;
    begin_ = data_;
    end_ = data_ + capacity_;
    head_ = 0;
    if (!empty()) tail_ = size_ - 1;
    else tail_ = 0;
    return result;
  }

  iterator erase(const iterator& range_start, const iterator& range_end) {
    bool found = false;
    T* new_data = alloc_.allocate(capacity_);
    size_t i = 0;
    iterator result = end();
    for (auto cur = begin(); cur != end(); ++cur) {
      if (cur == range_start) {
        cur = range_end - 1;
        size_ -= ((cur - range_start) + 1);
        found = true;
        result = cur + 1;
      } else {
        alloc_.construct(new_data + i, *cur);
        ++i;
      }
    }
    if (!found) {
      throw std::runtime_error("cannot erase not found iterator");
    }
    for (size_t j = 0; j < capacity_; ++j) {
      alloc_.destroy(data_ + j);
    }
    alloc_.deallocate(data_, capacity_);
    data_ = new_data;
    begin_ = data_;
    end_ = data_ + capacity_;
    head_ = 0;
    if (!empty()) tail_ = size_ - 1;
    else tail_ = 0;
    return result;
  }

  void assign(const iterator& range_start, const iterator& range_end) {
    *this = CircBuff<T, Allocator>(range_start, range_end);
  }
  void assign(const std::initializer_list<T>& elements) {
    *this = CircBuff<T, Allocator>(elements);
  }
  void assign(size_type capacity, const T& default_value) {
    *this = CircBuff<T, Allocator>(capacity, default_value);
  }

  void clear() {
    for (size_t i = 0; i < capacity_; ++i) {
      alloc_.destroy(data_ + i);
    }
    head_ = 0;
    tail_ = 0;
    size_ = 0;
  }

  void swap(CircBuff& other) {
    std::swap(alloc_, other.alloc_);
    std::swap(begin_, other.begin_);
    std::swap(end_, other.end_);
    std::swap(capacity_, other.capacity_);
    std::swap(size_, other.size_);
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
    std::swap(data_, other.data_);
  }

  friend void swap(CircBuff& lhs, CircBuff& rhs) {
    lhs.swap(rhs);
  }

 protected:
  size_type capacity_ = 0;
  size_type size_ = 0;
  size_type tail_ = 0;
  size_type head_ = 0;
  value_type* begin_ = nullptr;
  value_type* end_ = nullptr;
  value_type* data_ = nullptr;
  Allocator alloc_;
};

template<typename T, typename Allocator = std::allocator<T>>
class CircBuffExtended : public CircBuff<T, Allocator> {
 public:
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using difference_type = int32_t;
  using size_type = size_t;
  using iterator_category = std::random_access_iterator_tag;

  using iterator = typename CircBuff<T, Allocator>::template CircBuffIterator<T>;
  using const_iterator = typename CircBuff<T, Allocator>::template CircBuffIterator<const T>;

  CircBuffExtended() : CircBuff<T, Allocator>() {};

  explicit CircBuffExtended(size_type capacity) : CircBuff<T, Allocator>(capacity) {}

  CircBuffExtended(size_type capacity, const T& default_value) : CircBuff<T, Allocator>(capacity, default_value) {}

  CircBuffExtended(const std::initializer_list<T>& elements) : CircBuff<T, Allocator>(elements) {};

  CircBuffExtended(const iterator& range_start, const iterator& range_end) : CircBuff<T, Allocator>(range_start,
                                                                                                    range_end) {};

  explicit CircBuffExtended(const CircBuff<T, Allocator>& other) : CircBuff<T, Allocator>(other) {}

  void push(const T& el) {
    if (CircBuff<T, Allocator>::size() + 1 > CircBuff<T, Allocator>::capacity_) {
      size_type new_capacity;
      if (CircBuff<T, Allocator>::capacity_ == 0) new_capacity = 1;
      else new_capacity = CircBuff<T, Allocator>::capacity_ * 2;
      value_type* new_data = CircBuff<T, Allocator>::alloc_.allocate(new_capacity);
      size_t i = 0;
      for (auto it = CircBuff<T, Allocator>::begin(); it != CircBuff<T, Allocator>::end(); ++it, ++i) {
        CircBuff<T, Allocator>::alloc_.construct(new_data + i, *it);
      }
      for (size_t j = 0; j < CircBuff<T, Allocator>::capacity_; ++j) {
        CircBuff<T, Allocator>::alloc_.destroy(CircBuff<T, Allocator>::data_ + j);
      }
      CircBuff<T, Allocator>::alloc_.deallocate(CircBuff<T, Allocator>::data_, CircBuff<T, Allocator>::capacity_);
      CircBuff<T, Allocator>::data_ = new_data;
      CircBuff<T, Allocator>::capacity_ = new_capacity;
      CircBuff<T, Allocator>::begin_ = CircBuff<T, Allocator>::data_;
      CircBuff<T, Allocator>::end_ = CircBuff<T, Allocator>::data_ + CircBuff<T, Allocator>::capacity_;
      CircBuff<T, Allocator>::head_ = 0;
      if (CircBuff<T, Allocator>::size_ != 0) CircBuff<T, Allocator>::tail_ = CircBuff<T, Allocator>::size_ - 1;
    }
    if (!CircBuff<T, Allocator>::empty()) {
      CircBuff<T, Allocator>::tail_ = (++CircBuff<T, Allocator>::tail_) % CircBuff<T, Allocator>::capacity_;
      if (CircBuff<T, Allocator>::head_ == CircBuff<T, Allocator>::tail_)
        CircBuff<T, Allocator>::head_ = (++CircBuff<T, Allocator>::head_) % CircBuff<T, Allocator>::capacity_;
    }
    *(CircBuff<T, Allocator>::begin_ + CircBuff<T, Allocator>::tail_) = el;
    if (CircBuff<T, Allocator>::size_ < CircBuff<T, Allocator>::capacity_) ++CircBuff<T, Allocator>::size_;
  }

};