#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

template <typename T, typename Allocator = std::allocator<T>>
class List {
  private:
    struct BaseNode {
        BaseNode* prev = this;
        BaseNode* next = this;
    };

    template <typename Key, typename Value, typename Hash, typename Equal,
              typename Alloc>
    friend class UnorderedMap;

    struct Node : public BaseNode {
        T value;
        Node() = default;
        Node(const T& value)
            : value(value){};
    };

    void clear() {
        BaseNode* help = start.next;
        while (sz != 0 && help != &start) {
            BaseNode* nxt = help->next;
            alloc_traits::destroy(our_alloc, static_cast<Node*>(help));
            alloc_traits::deallocate(our_alloc, static_cast<Node*>(help), 1);
            help = nxt;
            sz--;
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

    void swapLi(List<T, Allocator>& other) {
        std::swap(sz, other.sz);
        std::swap(our_alloc, other.our_alloc);
        if (start.next == &start) {
            other.start.next->prev = &start;
            other.start.prev->next = &start;
            start.next = other.start.next;
            start.prev = other.start.prev;
            other.start.next = &other.start;
            other.start.prev = &other.start;
            return;
        }
        if (other.start.next == &other.start) {
            return;
        }
        start.next->prev = &other.start;
        other.start.next->prev = &start;
        start.prev->next = &other.start;
        other.start.prev->next = &start;
        std::swap(start, other.start);
    }

    void update(List&& other) {
        sz = other.sz;
        our_alloc = std::move(other.our_alloc);
        if (&other.start == other.start.next && &other.start == other.start.prev) {
            other.sz = 0;
            clear();
            start.next = &start;
            start.prev = &start;
            return;
        }
        BaseNode* ptr = &start;
        other.start.next->prev = ptr;
        other.start.prev->next = ptr;
        other.sz = 0;
        start.prev = other.start.prev;
        start.next = other.start.next;
        other.start.next = &other.start;
        other.start.prev = &other.start;
    }

    using NodeAlloc =
        typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using alloc_traits = std::allocator_traits<NodeAlloc>;

    NodeAlloc our_alloc = Allocator();
    BaseNode start;
    size_t sz = 0;

    template <bool IsConst>
    struct common_iterator {
        BaseNode* ptr;
        using value_type = std::conditional_t<IsConst, const T, T>;
        using pointer = std::conditional_t<IsConst, const T*, T*>;
        using reference = std::conditional_t<IsConst, const T&, T&>;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = int;

        common_iterator(const BaseNode* nd)
            : ptr(const_cast<BaseNode*>(nd))  //NOLINT
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
            return !(ptr == other.ptr);
        };

        std::conditional_t<IsConst, const T&, T&> operator*() const {
            return static_cast<std::conditional_t<IsConst, const Node*, Node*>>(ptr)
                ->value;
        };

        typename std::conditional_t<IsConst, const T*, T*> operator->() const {
            return &(
                static_cast<std::conditional_t<IsConst, const Node*, Node*>>(ptr)
                    ->value);
        };
    };

  public:
    void insertNode(BaseNode* node1, BaseNode* node2) {
        ++sz;
        node1->prev = node2->prev;
        node2->prev->next = node1;
        node2->prev = node1;
        node1->next = node2;
    }

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

    List() {
        start.next = &start;
        start.prev = &start;
    }
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
            our_alloc = std::allocator_traits<Allocator>::
                select_on_container_copy_construction(other.get_allocator());

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

    List<T, Allocator>& operator=(const List& other) {
        try {
            swap(other);
            if (alloc_traits::propagate_on_container_copy_assignment::value &&
                our_alloc != other.our_alloc) {
                our_alloc = other.our_alloc;
            }
            return *this;
        } catch (...) {
            throw;
        };
    }

    List(List&& other) {
        update(std::move(other));
    }

    List<T, Allocator>& operator=(List&& other) {
        update(std::move(other));
        return *this;
    }

    template <typename... Args>
    iterator emplace(const_iterator a, Args&&... args) {
        Node* node = alloc_traits::allocate(our_alloc, 1);
        try {
            alloc_traits::construct(our_alloc, node, std::forward<Args>(args)...);
        } catch (...) {
            alloc_traits::deallocate(our_alloc, static_cast<Node*>(node), 0);
            throw;
        }
        BaseNode* prf = a.ptr->prev;
        prf->next = node;
        node->prev = prf;
        node->next = a.ptr;
        a.ptr->prev = node;
        sz++;
        return iterator(node);
    }

    void insert(const_iterator a, const T& b) {
        try {
            emplace(a, b);
        } catch (...) {
            throw;
        }
    }

    void insert(const_iterator a, T&& b) {
        try {
            emplace(a, std::move(b));
        } catch (...) {
            throw;
        }
    }

    void push_back(const T& b) {
        insert(end(), b);
    }

    void push_front(const T& b) {
        insert(begin(), b);
    }

    void push_back(T&& b) {
        insert(end(), std::move(b));
    }

    void push_front(T&& b) {
        insert(begin(), std::move(b));
    }

    void erase(const_iterator a) {
        a.ptr->next->prev = a.ptr->prev;
        a.ptr->prev->next = a.ptr->next;
        auto copy = a.ptr;
        alloc_traits::destroy(our_alloc, static_cast<Node*>(copy));
        alloc_traits::deallocate(our_alloc, static_cast<Node*>(copy), 1);
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

    size_t size() const {
        return sz;
    }

    ~List() {
        clear();
    }
};

template <typename Key, typename Value, typename Hash = std::hash<Key>,
          typename Equal = std::equal_to<Key>,
          typename Alloc = std::allocator<std::pair<const Key, Value>>>

class UnorderedMap {
  public:
    static const int DEFAULT_CAPACITY = 10;
    constexpr static const float DEFAULT_LOAD_FACTOR = 0.95;
    constexpr static const float DEFAULT_EXPAND_FACTOR = 2;
    using NodeType = std::pair<const Key, Value>;

    template <bool IsConst>
    struct common_iterator;
    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;

    struct Element {
        int hash;
        NodeType key;

        Element(const Element& other)
            : key(other.key), hash(other.hash) {}
        Element(Element&& other)
            : key(std::move(other.key)), hash(std::move(other.hash)) {}
        template <typename... Args>
        Element(Args&&... args)
            : key(std::forward<Args>(args)...) {}
    };

    using ElementAlloc =
        typename std::allocator_traits<Alloc>::template rebind_alloc<Element>;
    using MapList = List<Element, ElementAlloc>;
    using Node = typename MapList::Node;
    using NodePtrAlloc =
        typename std::allocator_traits<Alloc>::template rebind_alloc<Node*>;
    using MPAL = typename MapList::alloc_traits;

  private:
    Hash hasher;
    Equal keyEqual;
    Alloc allocator;
    MapList elements;
    std::vector<Node*, NodePtrAlloc> pointers;
    float maxLoadFactor;

    void check_load() {
        float loadFactor = load_factor();
        float mx = max_load_factor();
        int newSize =
            DEFAULT_CAPACITY * 2 +
            static_cast<int>(std::ceil(DEFAULT_EXPAND_FACTOR * pointers.size()));
        if (loadFactor >= mx) {
            rehash(newSize);
        }
    }

    void rehash(int newPtrSize) {
        MapList copy(std::move(elements));
        elements = MapList();
        pointers.assign(newPtrSize, nullptr);
        auto nxt_it = copy.begin();
        for (auto it = copy.begin(); it != copy.end(); it = nxt_it) {
            nxt_it = std::next(it);
            int pos = get_pos(it->hash);
            if (pointers[pos] == nullptr) {
                const auto& to = &elements.start;
                elements.insertNode(it.ptr, to);
            } else {
                const auto& to = pointers[pos];
                elements.insertNode(it.ptr, to);
            }
            pointers[pos] = static_cast<typename MapList::Node*>(it.ptr);
        }
        copy.sz = 0;
    }

  public:
    UnorderedMap()
        : pointers(DEFAULT_CAPACITY, nullptr, allocator),
          maxLoadFactor(DEFAULT_LOAD_FACTOR) {}

    UnorderedMap(const UnorderedMap& other)
        : allocator(
              std::allocator_traits<Alloc>::select_on_container_copy_construction(
                  other.allocator)),
          pointers(DEFAULT_CAPACITY, nullptr, allocator),
          maxLoadFactor(other.maxLoadFactor) {
        hasher = other.hasher;
        keyEqual = other.keyEqual;
        try {
            for (auto it = other.begin(); it != other.end(); ++it) {
                emplace(*it);
            }
        } catch (...) {
            throw;
        }
    }

    UnorderedMap(UnorderedMap&& other)
        : hasher(std::move(other.hasher)), keyEqual(std::move(other.keyEqual)), allocator(std::move(other.allocator)), elements(std::move(other.elements)), pointers(std::move(other.pointers)), maxLoadFactor(std::move(other.maxLoadFactor)) {}

    void swap(UnorderedMap& other) {
        std::swap(maxLoadFactor, other.maxLoadFactor);
        std::swap(allocator, other.allocator);
        std::swap(hasher, other.hasher);
        std::swap(keyEqual, other.keyEqual);
        std::swap(pointers, other.pointers);
        elements.swapLi(other.elements);
    }

    UnorderedMap& operator=(const UnorderedMap& other) {
        UnorderedMap help(other);
        if (!static_cast<bool>(
                std::allocator_traits<
                    Alloc>::propagate_on_container_move_assignment::value)) {
            help.allocator = allocator;
        }
        swap(help);
        return *this;
    }

    UnorderedMap& operator=(UnorderedMap&& other) {
        UnorderedMap help(std::move(other));
        swap(help);
        return *this;
    }

    Value& at(const Key& key) {
        const auto& it = find(key);
        if (it == end()) {
            throw std::out_of_range("");
        }
        return it->second;
    }

    const Value& at(const Key& key) const {
        const auto& it = find(key);
        if (it == cend()) {
            throw std::out_of_range("");
        }
        return it->second;
    }

    Value& operator[](const Key& key) {
        auto it = find(key);
        if (it == end()) {
            auto to = insert({key, Value()}).first;
            return to->second;
        }
        return it->second;
    }

    std::pair<iterator, bool> insert(const NodeType& node) {
        return emplace(node);
    }

    template <typename... Args>
    std::pair<iterator, bool> insert(Args&&... args) {
        auto to = emplace(std::forward<Args>(args)...);
        return {to.first, to.second};
    }

    template <typename iterator>
    void insert(iterator b, iterator e) {
        while (b != e) {
            insert(*b);
            ++b;
        }
    }

    void erase(const_iterator a) {
        auto copy = a++;
        auto cp = a;
        int pos = get_pos(copy.get_hash());
        auto nodeToErase = static_cast<typename MapList::Node*>(copy.ptr.ptr);
        if (nodeToErase != pointers[pos]) {
            elements.erase(copy.ptr);
            return;
        }
        if (cp.get_hash() == copy.get_hash()) {
            pointers[pos] = static_cast<typename MapList::Node*>(cp.ptr.ptr);
        } else {
            pointers[pos] = nullptr;
        }
        elements.erase(copy.ptr);
    }

    void erase(const_iterator b, const_iterator e) {
        auto need = b;
        for (auto it = b; it != e; it = need) {
            need = std::next(it);
            erase(it);
        }
    }

    void reserve(size_t newCap) {
        if (std::ceil(newCap / maxLoadFactor) < pointers.size()) {
            return;
        }
        rehash(std::ceil(newCap / maxLoadFactor));
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        Node* newNode;
        NodeType* keyValPos;

        try {
            newNode = MapList::alloc_traits::allocate(elements.our_alloc, 1);
            keyValPos = &(newNode->value.key);
            std::allocator_traits<Alloc>::construct(allocator, keyValPos,
                                                    std::forward<Args>(args)...);
            newNode->value.hash = hasher(newNode->value.key.first);
        } catch (...) {
            MPAL::deallocate(elements.our_alloc, newNode, 1);
            throw;
        }

        try {
            if (find(keyValPos->first) != end()) {
                auto answ = find(keyValPos->first);
                std::allocator_traits<Alloc>::destroy(allocator, keyValPos);
                MPAL::deallocate(elements.our_alloc, newNode, 1);
                return {answ, false};
            }

            typename MapList::BaseNode* nextNodePtr =
                pointers[get_pos(newNode->value.hash)];

            if (nextNodePtr == nullptr) {
                nextNodePtr = &elements.start;
            }

            elements.insertNode(newNode, nextNodePtr);
            pointers[get_pos(newNode->value.hash)] = newNode;
            check_load();
            return {iterator(newNode), true};

        } catch (...) {
            std::allocator_traits<Alloc>::destroy(allocator, keyValPos);
            MPAL::deallocate(elements.our_alloc, newNode, 1);
            throw;
        }
    }

    iterator find(const Key& key) {
        int hash = hasher(key);
        if (pointers[get_pos(hash)] == nullptr) {
            return end();
        }
        for (iterator it = iterator(pointers[get_pos(hash)]); it != end(); ++it) {
            if (get_pos(it.get_hash()) != get_pos(hash)) {
                break;
            }
            if (it.get_hash() != hash || !keyEqual(key, it->first)) {
                continue;
            }
            return it;
        }
        return end();
    }

    const_iterator find(const Key& key) const {
        int hash = hasher(key);
        if (pointers[get_pos(hash)] == nullptr) {
            return cend();
        }
        const_iterator it(pointers[get_pos(hash)]);
        for (const_iterator it = const_iterator(pointers[get_pos(hash)]);
             it != end(); ++it) {
            if (get_pos(it.get_hash()) != get_pos(hash)) {
                break;
            }
            if (it.get_hash() != hash || !keyEqual(key, it->first)) {
                continue;
            }
            return it;
        }
        return cend();
    }

    float max_load_factor() const {
        return maxLoadFactor;
    }

    float load_factor() const {
        return static_cast<float>(elements.size()) /
               static_cast<float>(pointers.size());
    }

    int get_pos(int hs) const {
        return hs % pointers.size();
    }

    int size() const {
        return elements.size();
    }

    template <bool IsConst>
    struct common_iterator {
        using iterType =
            std::conditional_t<IsConst, typename MapList::const_iterator,
                               typename MapList::iterator>;

        iterType ptr;

        int get_hash() const {
            return ptr->hash;
        }

        using value_type = std::conditional_t<IsConst, const NodeType, NodeType>;
        using reference =
            typename std::conditional_t<IsConst, const NodeType&, NodeType&>;
        using pointer =
            typename std::conditional_t<IsConst, const NodeType*, NodeType*>;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = int;

        common_iterator(const iterType& other)
            : ptr(other) {}
        common_iterator(const Element* el)
            : ptr(el) {}

        operator common_iterator<true>() const {
            return common_iterator<true>(ptr);
        }

        operator common_iterator<false>() const {
            return common_iterator<false>(ptr);
        }

        common_iterator& operator++() {
            ++ptr;
            return *this;
        }

        common_iterator operator++(int) {
            return common_iterator(ptr++);
        }

        common_iterator& operator--() {
            --ptr;
            return *this;
        }

        common_iterator operator--(int) {
            return common_iterator(ptr--);
        }

        bool operator==(const common_iterator& other) const {
            return ptr == other.ptr;
        }

        bool operator!=(const common_iterator& other) const {
            return ptr != other.ptr;
        }

        reference operator*() const {
            return ptr->key;
        }

        pointer operator->() const {
            return &(ptr->key);
        }
    };

    iterator begin() {
        return iterator(elements.begin());
    }
    iterator end() {
        return iterator(elements.end());
    }

    const_iterator begin() const {
        return const_iterator(elements.begin());
    }
    const_iterator end() const {
        return const_iterator(elements.end());
    }

    const_iterator cbegin() const {
        return const_iterator(elements.begin());
    }
    const_iterator cend() const {
        return const_iterator(elements.end());
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

    ~UnorderedMap() = default;
};