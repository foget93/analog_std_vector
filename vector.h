#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>
#include <memory>
#include <algorithm>

template <typename T>
class RawMemory {
public:
    RawMemory() = default;

    explicit RawMemory(size_t capacity)
        : buffer_(Allocate(capacity))
        , capacity_(capacity) {}

    RawMemory(const RawMemory&) = delete;
    RawMemory& operator=(const RawMemory& rhs) = delete;

    RawMemory(RawMemory&& other) noexcept
        : buffer_(Allocate(other.capacity_))
        , capacity_(other.capacity_)
    {
        buffer_ = nullptr;
        std::swap(buffer_, other.buffer_);
    }
//    RawMemory(RawMemory&& other) noexcept
//        : buffer_(other.buffer_)
//        , capacity_(other.capacity_)
//    {
//        other.buffer_ = nullptr;
//        //std::swap(buffer_, other.buffer_);
//    }

    RawMemory& operator=(RawMemory&& rhs) noexcept {
        RawMemory rhs_copy(std::move(rhs));
        this->Swap(rhs_copy);
        return *this;
    }

    ~RawMemory() {
        Deallocate(buffer_);
    }

    T* operator+(size_t offset) noexcept {
        // Разрешается получать адрес ячейки памяти, следующей за последним элементом массива
        assert(offset <= capacity_);
        return buffer_ + offset;
    }

    const T* operator+(size_t offset) const noexcept {
        return const_cast<RawMemory&>(*this) + offset;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<RawMemory&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < capacity_);
        return buffer_[index];
    }

    void Swap(RawMemory& other) noexcept {
        std::swap(buffer_, other.buffer_);
        std::swap(capacity_, other.capacity_);
    }

    const T* GetAddress() const noexcept {
        return buffer_;
    }

    T* GetAddress() noexcept {
        return buffer_;
    }

    size_t Capacity() const {
        return capacity_;
    }

private:
    // Выделяет сырую память под n элементов и возвращает указатель на неё
    static T* Allocate(size_t n) {
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }

    // Освобождает сырую память, выделенную ранее по адресу buf при помощи Allocate
    static void Deallocate(T* buf) noexcept {
        operator delete(buf);
    }

    T* buffer_ = nullptr;
    size_t capacity_ = 0;
};// RawMemory

template <typename T>
class Vector {
public:
    Vector() = default;
/*
Этот конструктор сначала выделяет в сырой памяти буфер, достаточный для хранения  элементов в количестве, равном size.
Затем конструирует в сырой памяти элементы массива.
Для этого он вызывает их конструктор по умолчанию, используя размещающий оператор new.
*/
    explicit Vector(size_t size)
            : data_(size)
            , size_(size)  //
        {
//            size_t i = 0;
//            try {
//                for (; i != size; ++i) {
//                    new (data_ + i) T();
//                    //new (buf) T(elem);
//                }
//            } catch (...) {
//                DestroyN(data_.GetAddress(), i);
//                // Деструктор поля data_ освободит сырую память
//                // автоматически при перевыбрасывании исключения
//                throw;
//            }
            std::uninitialized_value_construct_n(data_.GetAddress(), size);
        }
//    explicit Vector(size_t size)
//        : data_(Allocate(size))
//        , capacity_(size)
//        , size_(size)
//    {
//        size_t i = 0;
//        try {
//            for (; i != size; ++i) {
//                new (data_ + i) T();
//            }
//        } catch (...) {
//            // В переменной i содержится количество созданных элементов.
//            // Теперь их надо разрушить
//            DestroyN(data_, i);
//            // Освобождаем память, выделенную через Allocate
//            Deallocate(data_);
//            // Перевыбрасываем пойманное исключение, чтобы сообщить об ошибке создания объекта
//            throw;
//        }
//    }
/*
Чтобы создать копию контейнера Vector, выделим память под нужное количество элементов,
а затем сконструируем в ней копию элементов оригинального контейнера, используя функцию CopyConstruct.

Здесь вместимость копии равна размеру оригинального вектора.
Это экономит память: независимо от вместимости оригинального вектора копия будет занимать столько памяти,
сколько нужно для хранения его элементов. Кроме того, если оригинальный вектор был пустым,
функция Allocate не станет выделять память в куче вообще.

*/
//    explicit Vector(const Vector& other)
//            : data_(other.data_)
//            , size_(other.size_)  //
//        {
//            size_t i = 0;
//            try {
//                for (; i != other.size_; ++i) {
//                    CopyConstruct(data_.GetAddress() + i, other.data_[i]);
//                }
//            } catch (...) {
//                DestroyN(data_.GetAddress(), i);
//                // Деструктор поля data_ освободит сырую память
//                // автоматически при перевыбрасывании исключения
//                throw;
//            }
//        }
    Vector(const Vector& other)
        : data_(other.size_)
        , size_(other.size_)  //
    {
//        size_t i = 0;
//        try {
//            for (; i != other.size_; ++i) {
//                CopyConstruct(data_ + i, other.data_[i]);
//            }
//        } catch (...) {
//            DestroyN(data_.GetAddress(), i);
//            //Deallocate(data_);
//            throw;
//        }
        std::uninitialized_copy_n(other.data_.GetAddress(), size_, data_.GetAddress());

    }

    Vector& operator=(const Vector& rhs) {
        if (this != &rhs) {
            if (size_ >= rhs.Size()) {
                size_t delta = size_ - rhs.size_;
                std::copy(rhs.data_.GetAddress(), rhs.data_.GetAddress() + rhs.size_, data_.GetAddress());
                std::destroy_n(data_.GetAddress() + rhs.size_, delta);
                size_ = rhs.size_;
            }
            else {
                if (data_.Capacity() < rhs.size_) {
                    Vector rhs_copy(rhs);
                    Swap(rhs_copy);
                }
                else {

                    size_t delta = rhs.size_ - size_;
                    std::copy(rhs.data_.GetAddress(), rhs.data_.GetAddress() + size_, data_.GetAddress());
                    std::uninitialized_copy_n(rhs.data_.GetAddress() + size_, delta, data_.GetAddress() + size_);
                    size_ = rhs.size_;
                }
            }
        }
        return *this;
    }

    Vector(Vector&& other) noexcept
//        : data_(std::move(other.data_))
//        , size_(other.size_)
    {
        data_.Swap(other.data_);
        size_ = other.size_;
        other.size_ = 0;
        //std::uninitialized_move_n(other.data_.GetAddress(), size_, data_.GetAddress());
        //size_ = other.size_;
    }

    Vector& operator=(Vector&& rhs) noexcept {
        if (this != &rhs) {
            size_ = 0;
            data_ = RawMemory<T>();
            this->Swap(rhs);
        }
        return *this;
    }

    void Swap(Vector& other) noexcept {
        data_.Swap(other.data_);
        std::swap(size_, other.size_);
    }
/*
Сначала необходимо вызвать деструкторы у size_ элементов массива, используя функцию DestroyN.
Затем нужно освободить выделенную динамическую память, используя функцию Deallocate.

Для корректного разрушения контейнера Vector нужно сначала вызвать DestroyN,
передав ей указатель data_ и количество элементов size_, а затем Deallocate,
чтобы вернуть память обратно в кучу:

*/
    ~Vector() {
        //DestroyN(data_.GetAddress(), size_);
        std::destroy_n(data_.GetAddress(), size_);
    }
//    ~Vector() {
//        DestroyN(data_, size_);
//        Deallocate(data_);
//    }
/*
Если требуемая вместимость больше текущей, Reserve выделяет нужный объём сырой памяти.
На следующем шаге из массива data_ копируются значения в только что выделенную область памяти
*/
    void Reserve(size_t new_capacity) {
        if (new_capacity <= data_.Capacity()) {
            return;
        }

        RawMemory<T> new_data(new_capacity);// = Allocate(new_capacity);
//        size_t i = 0;
//        try {
//            for (; i != size_; ++i) {
//                CopyConstruct(new_data + i, data_[i]); // можно без гетадресс походу изза оператор +
//            }
//        } catch (...) {
//            DestroyN(new_data.GetAddress(), i);
//            throw;
//        }

//        DestroyN(data_.GetAddress(), size_);

        // Конструируем элементы в new_data, копируя их из data_
//        std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
        //уместнее будет использовать перемещение
        //std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());

        // constexpr оператор if Шаблоны std::is_copy_constructible_v и std::is_nothrow_move_constructible_v
        //помогают узнать, есть ли у типа копирующий конструктор и noexcept-конструктор перемещения.
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            //уместнее будет использовать перемещение
            std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
        } else {
            // Конструируем элементы в new_data, копируя их из data_ если
            std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
        }

        // Разрушаем элементы в data_
        std::destroy_n(data_.GetAddress(), size_);
        // Избавляемся от старой сырой памяти, обменивая её на новую
        data_.Swap(new_data);
        // При выходе из метода старая память будет возвращена в кучу
//        data_ = new_data;
//        capacity_ = new_capacity;
    }

    void Resize(size_t new_size) {
        if (new_size == size_) {
            return;
        }

        else if (size_ > new_size) {
            std::destroy_n(data_.GetAddress() + new_size, size_ - new_size);
        }
        else {
            Reserve(new_size);
            std::uninitialized_value_construct_n(data_.GetAddress() + size_, new_size - size_);
        }
        size_ = new_size;
    }

    template <typename... Args>
    T& EmplaceBack(Args&&... args) {
        if (size_ == Capacity()) {
            RawMemory<T> new_data((size_ == 0) ? 1 : 2 * size_);

            new (new_data.GetAddress() + size_) T(std::forward<Args>(args)...);

            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
            } else {
                std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
            }
            std::destroy_n(data_.GetAddress(), size_);
            data_.Swap(new_data);

        } else {
            new (data_.GetAddress() + size_) T(std::forward<Args>(args)...);
        }
        ++size_;
        return *(data_.GetAddress() + size_ - 1);
    }

    template <typename S>
    void PushBack(S&& value) { //универсальная ссылка и вуаля
        if (this->Capacity() > size_) {
            new (data_ + size_) T(std::forward<S>(value));
        }
        else if (size_ == 0) {
            Reserve(1);
            new (data_.GetAddress()) T(std::forward<S>(value));
        }
        else {
            RawMemory<T> new_data(size_ * 2);
            new (new_data + size_) T(std::forward<S>(value));
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
            }
            else {
                std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
            }
            std::destroy_n(data_.GetAddress(), size_);
            data_.Swap(new_data);
        }
        ++size_;
    }

    void PopBack() noexcept {
        T* deleted = data_.GetAddress() + size_ - 1;
        deleted->~T();
        --size_;
    }

    size_t Size() const noexcept {
        return size_;
    }

    size_t Capacity() const noexcept {
        return data_.Capacity();
    }

/*
В константном операторе [] используется оператор  const_cast, чтобы снять константность
с ссылки на текущий объект и вызвать неконстантную версию оператора [].
Так получится избавиться от дублирования проверки assert(index < size).
Оператор const_cast позволяет сделать то, что нельзя, но, если очень хочется, можно.
В данном случае нельзя вызвать неконстантный метод из константного.
Но неконстантный оператор [] тут не модифицирует состояние объекта, поэтому его можно вызвать, предварительно сняв константность с объекта.
*/
    const T& operator[](size_t index) const noexcept {
        return const_cast<Vector&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

private: //methods
    //static:
//    alignas(Cat) char buf[sizeof(Cat)];
//    Cat* cat = new (&buf[0]) Cat("Luna"s, 1);
//    cat->SayHello();
//    cat->~Cat();

    // Выделяет сырую память под n элементов и возвращает указатель на неё
    // operator new вернет void*
    //    void* buf_dynamic = operator new(sizeof(Cat)); <---пример
    //    Cat* cat_d = new(buf_dynamic) Cat("Murka"s, 4);
    //    cat_d->SayHello();
    //    cat->~Cat();
    //    operator delete(buf_dynamic); //static void Deallocate(T* buf) noexcept
//    static T* Allocate(size_t n) {
//        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
//    }

//    // Освобождает сырую память, выделенную ранее по адресу buf при помощи Allocate
//    static void Deallocate(T* buf) noexcept {
//        operator delete(buf);
//    }

    // Вызывает деструкторы n объектов массива по адресу buf
    static void DestroyN(T* buf, size_t n) noexcept {
        for (size_t i = 0; i != n; ++i) {
            Destroy(buf + i);
        }
    }

    // Создаёт копию объекта elem в сырой памяти по адресу buf
    static void CopyConstruct(T* buf, const T& elem) {
        new (buf) T(elem);
    }

    // Вызывает деструктор объекта по адресу buf
    static void Destroy(T* buf) noexcept {
        buf->~T();
    }

private:
    RawMemory<T> data_;
    size_t size_ = 0;
//    size_t capacity_ = 0;
//    T*  data_ = nullptr;

};
/*
Если будете разрабатывать и отлаживать программу в IDE на вашем компьютере,
рекомендуем использовать статический анализатор clang-tidy совместно с UB и Address санитайзерами.
*/
