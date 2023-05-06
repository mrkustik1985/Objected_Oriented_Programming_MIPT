#pragma once

#include <iostream>
#include <memory>
#include <type_traits>

using std::cout;
using std::endl;

template <typename T, typename Allocator = std::allocator<T>>
class List {
  private:
    struct BaseNode {
        BaseNode* prev = this;
        BaseNode* next = this;
    };

    struct Node : public BaseNode {
        T value;
        Node() = default;
        Node(const T& value)
            : value(value){};
    };

    void clear() {
        BaseNode* help = start.next;
        while (help != &start) {
            BaseNode* nxt = help->next;
            alloc_traits::destroy(our_alloc, static_cast<Node*>(help));
            alloc_traits::deallocate(our_alloc, static_cast<Node*>(help), 1);
            help = nxt;
        }
    }

    void prt() {
        cout << " FUCCCK " << &start << endl;
        id += 1;
        BaseNode* help = start.next;
        while (help != &start) {
            cout << help << endl;
            BaseNode* nxt = help->next;
            help = nxt;
        }
        cout << id << " THE END ETTTTT \n\n";
    }

    using NodeAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using alloc_traits = std::allocator_traits<NodeAlloc>;

    NodeAlloc our_alloc = Allocator();
    BaseNode start;
    size_t sz = 0;
    int id = 0;

  public:
    template <bool IsConst>
    struct common_iterator {
      private:
      public:
        BaseNode* ptr;
        using value_type = std::conditional_t<IsConst, const T, T>;
        using pointer = std::conditional_t<IsConst, const T*, T*>;
        using reference = std::conditional_t<IsConst, const T&, T&>;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = int;

        common_iterator(const BaseNode* nd)
            // sorry
            : ptr(const_cast<BaseNode*>(nd))  // NOLINT
        {}

        operator common_iterator<true>() const {
            return common_iterator<true>(ptr);
        }

        common_iterator& operator++() {
            ptr = ptr->next;
            return *this;
        }

        common_iterator operator++(int) {
            common_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        common_iterator& operator--() {
            ptr = ptr->prev;
            return *this;
        }

        common_iterator operator--(int) {
            common_iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const common_iterator& other) const {
            return ptr == other.ptr;
        };

        bool operator!=(const common_iterator& other) const {
            return ptr != other.ptr;
        };

        std::conditional_t<IsConst, const T&, T&> operator*() const {
            return static_cast<std::conditional_t<IsConst, const Node*, Node*>>(ptr)->value;
        };

        typename std::conditional_t<IsConst, const T*, T*> operator->() const {
            return &(static_cast<std::conditional_t<IsConst, const Node*, Node*>>(ptr)->value);
        };
    };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;

    iterator begin() {
        return iterator(start.next);
    }
    iterator end() {
        return iterator(&start);
    }

    const_iterator begin() const {
        return const_iterator(start.next);
    }
    const_iterator end() const {
        return const_iterator(&start);
    }

    const_iterator cbegin() const {
        return const_iterator(start.next);
    }
    const_iterator cend() const {
        return const_iterator(&start);
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

    List() = default;
    List(size_t count, const Allocator& allocator = Allocator())
        : our_alloc(allocator) {
        size_t i = 0;
        try {
            BaseNode* help = &start;
            for (; i < count; ++i) {
                Node* node = alloc_traits::allocate(our_alloc, 1);
                try {
                    alloc_traits::construct(our_alloc, node);
                } catch (...) {
                    alloc_traits::deallocate(our_alloc, node, 1);
                    throw;
                }
                help->next = node;
                node->prev = help;
                node->next = &start;
                help = node;
            }
            sz = count;
            start.prev = help;
        } catch (...) {
            clear();
            throw;
        }
    }

    List(size_t count, const T& val, const Allocator& allocator = Allocator())
        : our_alloc(allocator) {
        size_t i = 0;
        try {
            BaseNode* help = &start;
            for (; i < count; ++i) {
                Node* node = alloc_traits::allocate(our_alloc, 1);
                try {
                    alloc_traits::construct(our_alloc, node, val);
                } catch (...) {
                    alloc_traits::deallocate(our_alloc, node, 1);
                    throw;
                }
                help->next = node;
                node->prev = help;
                node->next = &start;
                help = node;
            }
            sz = count;
            start.prev = help;
        } catch (...) {
            clear();
            throw;
        }
    }

    List(const Allocator& alloc)
        : our_alloc(alloc) {}

    List(const List& other)
        : sz(other.sz) {
        BaseNode* help = &start;
        try {
            our_alloc = std::allocator_traits<Allocator>::select_on_container_copy_construction(other.get_allocator());

            for (const auto& to : other) {
                Node* node = alloc_traits::allocate(our_alloc, 1);
                try {
                    alloc_traits::construct(our_alloc, node, to);
                } catch (...) {
                    alloc_traits::deallocate(our_alloc, node, 1);
                    throw;
                }
                help->next = node;
                node->prev = help;
                node->next = &start;
                help = node;
            }
            start.prev = help;
            sz = other.sz;
        } catch (...) {
            clear();
            throw;
        }
    }
    void swap(List<T, Allocator> other) {
        std::swap(sz, other.sz);
        std::swap(our_alloc, other.our_alloc);
        start.next->prev = &other.start;
        other.start.next->prev = &start;
        start.prev->next = &other.start;
        other.start.prev->next = &start;
        std::swap(start, other.start);
    }

    List<T, Allocator>& operator=(const List& other) {
        try {
            swap(other);
            if (alloc_traits::propagate_on_container_copy_assignment::value && our_alloc != other.our_alloc) {
                our_alloc = other.our_alloc;
            }
            return *this;
        } catch (...) {
            throw;
        };
    }

    void insert(const_iterator a, const T& b) {
        Node* node;
        try {
            node = alloc_traits::allocate(our_alloc, 1);
            alloc_traits::construct(our_alloc, node, b);
            BaseNode* prf = a.ptr->prev;
            prf->next = node;
            node->prev = prf;
            node->next = a.ptr;
            a.ptr->prev = node;
            sz++;
        } catch (...) {
            alloc_traits::deallocate(our_alloc, static_cast<Node*>(node), 0);
            throw;
        }
    }
    void push_back(const T& b) {
        insert(end(), b);
    }

    void push_front(const T& b) {
        insert(begin(), b);
    }

    void erase(const_iterator a) {
        a.ptr->next->prev = a.ptr->prev;
        a.ptr->prev->next = a.ptr->next;
        alloc_traits::destroy(our_alloc, static_cast<Node*>(a.ptr));
        alloc_traits::deallocate(our_alloc, static_cast<Node*>(a.ptr), 0);
        sz--;
    }

    void pop_back() {
        iterator dlt = end();
        dlt--;
        erase(dlt);
    }

    void pop_front() {
        erase(begin());
    }

    Allocator get_allocator() const {
        return our_alloc;
    }

    void print() {
        BaseNode* st = &start;
        while (st->next != &start) {
            Node* nxt = static_cast<Node*>(st->next);
            std::cout << nxt->value << ' ';
            st = st->next;
        }
        std::cout << std::endl;
    }

    size_t size() const {
        return sz;
    }

    ~List() {
        clear();
    }
};