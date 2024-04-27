#include <iostream>
#include <memory>
#include <type_traits>

struct BaseControlBlock {
    int cnt_shrd = 0;
    int cnt_weak = 0;
    BaseControlBlock() = default;
    virtual void useDeleter() = 0;
    virtual void BlockDeleter() = 0;
    virtual void* getPtr() = 0;
    virtual ~BaseControlBlock() = default;
};

template <typename T>
class SharedPtr {
    friend int main();
    template <typename Y, typename... Args>
    friend SharedPtr<Y> makeShared(Args&&... args);
    template <typename Y, typename Alloc, typename... Args>
    friend SharedPtr<Y> allocateShared(Alloc alloc, Args&&... args);
    template <typename Y>
    friend class WeakPtr;

    template <typename Y>
    friend class SharedPtr;

    template <typename Y, typename Deleter, typename Alloc>
    struct ControlBlockRegular : BaseControlBlock {
      private:
        using BlockAlloc =
            typename std::allocator_traits<Alloc>::template rebind_alloc<
                ControlBlockRegular<Y, Deleter, Alloc>>;
        using BlockAllocTraits = typename std::allocator_traits<BlockAlloc>;
        Y* ptr;
        Alloc alloc;
        Deleter deleter;

      public:
        ControlBlockRegular(Y* ptr, Deleter del, Alloc alloc)
            : ptr(ptr), alloc(alloc), deleter(del) {}

        void useDeleter() override {
            deleter(ptr);
            ptr = nullptr;
        }

        void* getPtr() override {
            return ptr;
        }

        void BlockDeleter() override {
            auto ths = this;
            BlockAlloc newal = std::move(alloc);
            BlockAllocTraits::deallocate(newal, ths, 1);
        }
        ~ControlBlockRegular() override {}
    };

    template <typename Alloc>
    struct ControlBlockMakeShared : BaseControlBlock {
      private:
        Alloc alloc;
        T obj;
        using ObjAlloc =
            typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
        using ObjTraits = typename std::allocator_traits<ObjAlloc>;

      public:
        using BlockAlloc = typename std::allocator_traits<
            Alloc>::template rebind_alloc<ControlBlockMakeShared<Alloc>>;
        using BlockAllocTraits = typename std::allocator_traits<BlockAlloc>;

        template <typename... Args>
        ControlBlockMakeShared(Alloc alloc, Args&&... args)
            : alloc(alloc), obj(std::forward<Args>(args)...) {}

        void BlockDeleter() override {
            auto ths = this;
            BlockAlloc newal = std::move(alloc);
            BlockAllocTraits::deallocate(newal, ths, 1);
        }

        void* getPtr() override {
            return &obj;
        }

        void useDeleter() override {
            ObjAlloc newAl(alloc);
            ObjTraits::destroy(newAl, &obj);
        }
        ~ControlBlockMakeShared() override {}
    };

    BaseControlBlock* block = nullptr;

    SharedPtr(BaseControlBlock* block) : block(block) {
        if (block != nullptr) {
            this->block->cnt_shrd++;
        }
    }

  public:
    SharedPtr() = default;

    template <typename Y, typename Deleter = std::default_delete<Y>,
              typename Alloc = std::allocator<Y>>
    SharedPtr(Y* ptr, Deleter dlt = Deleter(), Alloc alloc = Alloc()) {
        using BlockAlloc =
            typename std::allocator_traits<Alloc>::template rebind_alloc<
                ControlBlockRegular<Y, Deleter, Alloc>>;
        using BlockAllocTraits = typename std::allocator_traits<BlockAlloc>;
        BlockAlloc nwAlloc = alloc;
        auto newptr = BlockAllocTraits::allocate(nwAlloc, 1);
        ::new (newptr) ControlBlockRegular<Y, Deleter, Alloc>(ptr, dlt, alloc);
        this->block = newptr;
        this->block->cnt_shrd++;
    }

    SharedPtr(const SharedPtr& other) : block(other.block) {
        if (block != nullptr) {
            block->cnt_shrd++;
        }
    }

    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other) : block(other.block) {
        if (block != nullptr) {
            block->cnt_shrd++;
        }
    }

    SharedPtr(SharedPtr&& other) : block(other.block) {
        other.block = nullptr;
    }

    template <typename Y>
    SharedPtr(SharedPtr<Y>&& other) : block(other.block) {
        other.block = nullptr;
    }

    void swap(SharedPtr<T>& other) {
        std::swap(block, other.block);
    }

    SharedPtr<T>& operator=(const SharedPtr<T>& other) {
        if (this == &other) {
            return *this;
        }
        SharedPtr<T> copy(other);
        swap(copy);
        return *this;
    }

    template <typename Y>
    SharedPtr<T>& operator=(const SharedPtr<Y>& other) {
        SharedPtr<T> copy(other);
        swap(copy);
        return *this;
    }

    SharedPtr<T>& operator=(SharedPtr<T>&& other) {
        SharedPtr<T> copy(std::move(other));
        swap(copy);
        return *this;
    }

    template <typename Y>
    SharedPtr<T>& operator=(SharedPtr<Y>&& other) {
        SharedPtr<T> copy(std::move(other));
        swap(copy);
        return *this;
    }

    T* get() const {
        if (block == nullptr) {
            return nullptr;
        }
        return static_cast<T*>(block->getPtr());
    }

    T& operator*() const {
        return *(get());
    }
    T* operator->() const {
        return get();
    }

    size_t use_count() const {
        if (block == nullptr) {
            return 0;
        }
        return block->cnt_shrd;
    }
    template <typename Y>
    void reset(Y* ptr) {
        SharedPtr<T> copy(ptr);
        swap(copy);
    }

    void reset() {
        SharedPtr<T> to;
        swap(to);
    }

    ~SharedPtr() {
        if (block != nullptr) {
            block->cnt_shrd--;
            if (block->cnt_shrd == 0) {
                block->useDeleter();
                if (block->cnt_weak == 0) {
                    block->BlockDeleter();
                    block = nullptr;
                }
            }
        }
    }
};

template <typename Y, typename Alloc, typename... Args>
SharedPtr<Y> allocateShared(Alloc alloc, Args&&... args) {
    using ControlBlock =
        typename SharedPtr<Y>::template ControlBlockMakeShared<Alloc>;
    using BlockAlloc = typename SharedPtr<Y>::template ControlBlockMakeShared<
        Alloc>::BlockAlloc;

    BlockAlloc blockAlloc = alloc;

    ControlBlock* blockPtr =
        ControlBlock::BlockAllocTraits::allocate(blockAlloc, 1);
    ControlBlock::BlockAllocTraits::construct(blockAlloc, blockPtr, alloc,
                                              std::forward<Args>(args)...);

    return SharedPtr<Y>(dynamic_cast<BaseControlBlock*>(blockPtr));
}

template <typename Y, typename... Args>
SharedPtr<Y> makeShared(Args&&... args) {
    return allocateShared<Y, std::allocator<Y>, Args...>(
        std::allocator<Y>(), std::forward<Args>(args)...);
}
template <typename T>
class WeakPtr {
  private:
    BaseControlBlock* ptr = nullptr;
    template <typename Y>
    friend class WeakPtr;

  public:
    void swap(WeakPtr& other) {
        std::swap(ptr, other.ptr);
    }
    WeakPtr() = default;
    template <typename Y>
    WeakPtr(const SharedPtr<Y>& ptr) : ptr(ptr.block) {
        this->ptr->cnt_weak++;
    }

    WeakPtr(const WeakPtr& ptr) : ptr(ptr.ptr) {
        this->ptr->cnt_weak++;
    }
    template <typename Y>
    WeakPtr(const WeakPtr<Y>& ptr) : ptr(ptr.ptr) {
        this->ptr->cnt_weak++;
    }
    WeakPtr(WeakPtr&& ptr) : ptr(ptr.ptr) {
        ptr.ptr = nullptr;
    }
    template <typename Y>
    WeakPtr(WeakPtr<Y>&& ptr) : ptr(ptr.ptr) {
        ptr.ptr = nullptr;
    }

    WeakPtr& operator=(const WeakPtr& ptr) {
        WeakPtr<T>(ptr).swap(*this);
        return *this;
    }

    template <typename Y>
    WeakPtr& operator=(const WeakPtr<Y>& ptr) {
        WeakPtr<T>(ptr).swap(*this);
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& ptr) {
        WeakPtr(std::move(ptr)).swap(*this);
        return *this;
    }

    template <typename Y>
    WeakPtr& operator=(WeakPtr<Y>&& ptr) {
        WeakPtr<T>(std::move(ptr)).swap(*this);
        return *this;
    }

    template <typename Y>
    WeakPtr& operator=(const SharedPtr<Y>& ptr) {
        WeakPtr<T>(ptr).swap(*this);
        return *this;
    }

    bool expired() const {
        if (ptr == nullptr) {
            return true;
        }
        return ptr->cnt_shrd == 0;
    }
    SharedPtr<T> lock() const {
        if (ptr == nullptr) {
            return SharedPtr<T>();
        }
        return SharedPtr<T>(ptr);
    }
    size_t use_count() const {
        if (ptr == nullptr) {
            return 0;
        }
        return ptr->cnt_shrd;
    }
    ~WeakPtr() {
        if (ptr != nullptr) {
            ptr->cnt_weak--;
            if (ptr->cnt_weak == 0 && ptr->cnt_shrd == 0) {
                ptr->BlockDeleter();
            }
        }
    }
};
