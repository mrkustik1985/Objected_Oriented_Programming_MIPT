template <size_t N>
class StackStorage {
  private:
    StackStorage(const StackStorage& /*unused*/) {}
    char stat_arr[N];
    size_t sz = 0;

  public:
    StackStorage() = default;
    void* return_memory() {
        return reinterpret_cast<void*>(stat_arr + sz);
    }

    void change_cnt_used(size_t ncnt) {
        sz += ncnt;
    }

    size_t cnt_staied_memory() {
        return N - sz;
    }

    ~StackStorage() = default;
};

template <typename T, size_t N>
class StackAllocator {
  private:
    StackStorage<N>* storage;

  public:
    using value_type = T;

    StackAllocator() = default;
    StackAllocator(StackStorage<N>& storage)
        : storage(&storage) {}
    StackAllocator(const StackAllocator<T, N>& other)
        : storage(other.storage) {}

    template <typename U>
    StackAllocator(const StackAllocator<U, N>& other)
        : storage(other.get_storage()) {}

    T* allocate(size_t cnt) {
        size_t size_type = sizeof(T);
        void* ptr_to_memory = storage->return_memory();
        size_t size_staied = storage->cnt_staied_memory();
        if (std::align(alignof(T), size_type * cnt, ptr_to_memory, size_staied) !=
            nullptr) {
            T* result = reinterpret_cast<T*>(ptr_to_memory);
            storage->change_cnt_used(cnt * size_type);
            return result;
        }
        return nullptr;
    }

    void deallocate(T* /*unused*/, size_t /*unused*/) {}

    bool operator==(const StackAllocator<T, N>& other) const {
        return storage == other.storage;
    }

    bool operator!=(const StackAllocator<T, N>& other) const {
        return storage != other.storage;
    }

    template <typename U>
    struct rebind {
        using other = StackAllocator<U, N>;
    };

    StackStorage<N>* get_storage() const {
        return storage;
    }

    ~StackAllocator() = default;
};