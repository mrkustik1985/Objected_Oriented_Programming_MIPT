#include <cmath>
#include <exception>
#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::vector;

template <typename T>
class Deque {
  private:
    static const size_t size_block = 64;

    T* GetNewArray() {
        T* x = reinterpret_cast<T*>(new char[size_block * sizeof(T)]);
        return x;
    }

    void Delete(int i) const {
        for (int j = start_index; j < i; j++) {
            int id1 = j / size_block, id2 = j % size_block;
            points[id1][id2].~T();
        }
    }

    void DeleteConstructor() const {
        for (size_t i = 0; i < points.size(); i++) {
            delete[] reinterpret_cast<char*>(points[i]);
        }
    }

    void clear() const {
        Delete(end_index);
        DeleteConstructor();
    }

    T& GetByIndex(int id) const {
        int id1 = id / size_block;
        int id2 = id % size_block;
        return points[id1][id2];
    }

    T* GetByIndexP(int id) const {
        int id1 = id / size_block;
        int id2 = id % size_block;
        return &points[id1][id2];
    }

  public:
    vector<T*> points;
    int start_index = 0, end_index = 0;
    int sz = 0;
    int cap = 0;

    template <bool IsConst>
    struct common_iterator {
      private:
      public:
        typename std::conditional_t<IsConst, const T**, T**> ptr;
        int id = 0;
        int pos = 0;
        using value_type = std::conditional_t<IsConst, const T, T>;
        using pointer = std::conditional_t<IsConst, const T*, T*>;
        using reference = std::conditional_t<IsConst, const T&, T&>;
        using difference_type = int;
        using iterator_category = std::random_access_iterator_tag;

        common_iterator(
            const int id, const int count,
            typename std::conditional_t<IsConst, const T**, T**> deq)
            : ptr(deq), id(id), pos(count) {}

        operator common_iterator<true>() {
            return common_iterator<true>(id, pos, ptr);
        }

        common_iterator& operator+=(const int n1) {
            int n = n1;
            if (static_cast<int>(size_block) - id <= n) {
                n -= size_block - id;
                ptr++;
                id = 0;
            }
            int cnt = n / size_block;
            ptr += cnt;
            n -= cnt * size_block;
            id += n;
            pos += n1;
            return *this;
        }
        common_iterator& operator-=(const int& n1) {
            int n = n1;
            if (id + 1 <= n) {
                n -= (id + 1);
                ptr--;
                id = size_block - 1;
            }
            int cnt = n / size_block;
            ptr -= cnt;
            n -= cnt * size_block;
            id -= n;
            pos -= n1;
            return *this;
        }
        common_iterator& operator++() {
            *this += 1;
            return *this;
        }

        common_iterator operator++(int) {
            common_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        common_iterator& operator--() {
            *this -= 1;
            return *this;
        }

        common_iterator operator--(int) {
            common_iterator tmp = *this;
            --(*this);
            return tmp;
        }
        common_iterator operator+(const int& n) const {
            common_iterator tmp = *this;
            tmp += n;
            return tmp;
        };

        common_iterator operator-(const int& n) const {
            common_iterator tmp = *this;
            tmp -= n;
            return tmp;
        };

        int operator-(const common_iterator& other) const {
            return pos - other.pos;
        };

        bool operator==(const common_iterator& other) const {
            return pos == other.pos;
        };

        bool operator!=(const common_iterator& other) const {
            return pos != other.pos;
        };

        bool operator<(const common_iterator& other) const {
            return pos < other.pos;
        };

        bool operator>(const common_iterator& other) const {
            return pos > other.pos;
        };

        bool operator>=(const common_iterator& other) const {
            return pos >= other.pos;
        };

        bool operator<=(const common_iterator& other) const {
            return pos <= other.pos;
        };
        std::conditional_t<IsConst, const T&, T&> operator*() const {
            return (*ptr)[id];
        };

        typename std::conditional_t<IsConst, const T*, T*> operator->() const {
            return &(*ptr)[id];
        };
    };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;

    iterator begin() {
        return iterator(start_index % size_block, start_index,
                        &points[start_index / size_block]);
    }
    iterator end() {
        if (end_index == cap) {
            points.push_back(GetNewArray());
            cap += size_block;
        }
        return iterator(end_index % size_block, end_index,
                        &points[end_index / size_block]);
    }

    const_iterator begin() const {
        return const_iterator(
            start_index % size_block, start_index,
            const_cast<const T**>(&points[end_index / size_block]));
    }
    const_iterator end() const {
        return const_iterator(
            end_index % size_block, end_index,
            const_cast<const T**>(&points[end_index / size_block]));
    }

    const_iterator cbegin() const {
        return const_iterator(
            start_index % size_block, start_index,
            const_cast<const T**>(&points[start_index / size_block]));
    }
    const_iterator cend() const {
        return const_iterator(
            end_index % size_block, end_index,
            const_cast<const T**>((&points[end_index / size_block])));
    }

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }

    Deque() {
        start_index = 0;
        end_index = 0;
        points.push_back(GetNewArray());
        cap = size_block;
    }
    Deque(const Deque<T>& que)
        : start_index(que.start_index),
          end_index(que.end_index),
          sz(que.sz),
          cap(que.cap) {
        points.resize(que.points.size());
        for (size_t i = 0; i < points.size(); i++) {
            points[i] = GetNewArray();
        }
        int i = 0;
        try {
            for (i = start_index; i < end_index; i++) {
                int id1 = i / size_block, id2 = i % size_block;
                new (points[id1] + id2) T(que.points[id1][id2]);
            }
        } catch (...) {
            Delete(i);
            DeleteConstructor();
            throw;
        }
    }
    Deque(const int cnt)
        : start_index(0),
          end_index(cnt),
          sz(cnt),
          cap((cnt / size_block + 1) * size_block) {
        points.resize(cnt / size_block + 1);
        size_t i = 0;
        try {
            for (i = 0; i < points.size(); i++) {
                points[i] = GetNewArray();
                T x = T();
                for (size_t j = 0; j < size_block; j++) {
                    points[i][j] = x;
                }
            }
        } catch (...) {
            Delete(i);
            DeleteConstructor();
            throw;
        }
    }

    Deque(const int cnt, const T& element)
        : start_index(0),
          end_index(cnt),
          sz(cnt),
          cap((cnt / size_block + 1) * size_block) {
        points.resize(cnt / size_block + 1);
        for (size_t i = 0; i < points.size(); i++) {
            points[i] = GetNewArray();
        }
        int i = 0;
        try {
            for (i = 0; i < cnt; i++) {
                int id1 = i / size_block, id2 = i % size_block;
                new (points[id1] + id2) T(element);
            }
        } catch (...) {
            Delete(i);
            DeleteConstructor();
            throw;
        }
    }

    Deque<T>& operator=(const Deque<T>& deq) {
        if (&deq == this) {
            return *this;
        }

        try {
            Deque<T> tmp(deq);
            std::swap(points, tmp.points);
            std::swap(start_index, tmp.start_index);
            std::swap(end_index, tmp.end_index);
            std::swap(cap, tmp.cap);
            std::swap(sz, tmp.sz);
        } catch (...) {
            clear();
            throw;
        }
        return *this;
    }

    size_t size() const {
        return static_cast<size_t>(sz);
    }

    T& operator[](int id) {
        id += start_index;
        int id1 = id / size_block;
        int id2 = id % size_block;
        return points[id1][id2];
    }
    const T& operator[](int id) const {
        id += start_index;
        int id1 = id / size_block;
        int id2 = id % size_block;
        return points[id1][id2];
    }
    T& at(size_t id) {
        id += start_index;
        int id1 = id / size_block;
        int id2 = id % size_block;
        if (id >= static_cast<size_t>(end_index)) {
            throw std::out_of_range("index out of range");
        }
        return points[id1][id2];
    }

    const T& at(size_t id) const {
        id += start_index;
        int id1 = id / size_block;
        int id2 = id % size_block;
        if (id >= static_cast<size_t>(end_index)) {
            throw std::out_of_range("index out of range");
        }
        return points[id1][id2];
    }

    void pop_back() {
        if (start_index != end_index) {
            end_index--;
            GetByIndex(end_index).~T();
            sz--;
        }
    }
    void pop_front() {
        if (start_index != end_index) {
            GetByIndex(start_index).~T();
            start_index++;
            sz--;
        }
    }

    void push_back(const T& x) {
        int where = 0;
        if (cap == end_index) {
            vector<T*> help((points.size() + 1) * 3);
            size_t id_s = points.size() + 1;
            for (size_t i = id_s; i < id_s + points.size(); i++) {
                delete[] reinterpret_cast<char*>(help[i]);
                help[i] = points[i - id_s];
            }
            for (size_t i = 0; i < help.size(); i++) {
                if (help[i] == nullptr) {
                    help[i] = GetNewArray();
                }
            }
            where = (id_s + points.size()) * size_block;
            start_index += id_s * size_block;
            points = help;
            cap = points.size() * size_block;
            sz++;
            end_index = start_index + sz;
            new (points[where / size_block] + where % size_block) T(x);
        } else {
            new (points[end_index / size_block] + end_index % size_block) T(x);
            end_index++;
            sz++;
        }
    }

    void push_front(const T& el) {
        if (start_index > 0) {
            T* to_add = GetByIndexP(start_index - 1);
            new (to_add) T(el);
            --start_index;
            sz++;
            return;
        }
        doubleSize();
        --start_index;
        sz++;
        new (points[start_index / size_block] + start_index % size_block) T(el);
    }

    void doubleSize() {
        std::vector<T*> new_array(cap * 3 / size_block);
        for (size_t i = 0; i < points.size(); ++i) {
            new_array[i + cap / size_block] = points.at(i);
        }
        for (size_t i = 0; i < new_array.size(); ++i) {
            if (new_array[i] == nullptr) {
                new_array[i] = GetNewArray();
            }
        }
        points = new_array;
        end_index = end_index + cap;
        start_index = start_index + cap;
        cap *= 3;
    }

    void insert(iterator a, const T& x) {
        int where = a - begin();
        push_back(x);
        iterator it = begin() + where;
        iterator it1 = end() - 1;
        while (it1 != it) {
            std::swap(*it1, *(it1 - 1));
            --it1;
        }
    }

    void erase(iterator it) {
        while (it != end() - 1) {
            std::swap(*it, *(it + 1));
            ++it;
        }
        (*it).~T();
        end_index--;
        sz--;
    }

    ~Deque() {
        clear();
    }
};