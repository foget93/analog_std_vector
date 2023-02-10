#include "vector.h"
#include <iostream>
#include <vector>

namespace {

struct Obj_1 {
    Obj_1() {
        ++num_default_constructed;
    }

    Obj_1(const Obj_1& /*other*/) {
        ++num_copied;
    }

    Obj_1(Obj_1&& /*other*/) noexcept {
        ++num_moved;
    }

    Obj_1& operator=(const Obj_1& other) = default;
    Obj_1& operator=(Obj_1&& other) = default;

    ~Obj_1() {
        ++num_destroyed;
    }

    static int GetAliveObjectCount() {
        return num_default_constructed + num_copied + num_moved - num_destroyed;
    }

    static void ResetCounters() {
        num_default_constructed = 0;
        num_copied = 0;
        num_moved = 0;
        num_destroyed = 0;
    }

    static inline int num_default_constructed = 0;
    static inline int num_copied = 0;
    static inline int num_moved = 0;
    static inline int num_destroyed = 0;
};

}  // namespace

using namespace std;

void Test1() {
    Obj_1::ResetCounters();
    const size_t SIZE = 100500;
    const size_t INDEX = 10;
    const int MAGIC = 42;
    {
        Vector<int> v;
        assert(v.Capacity() == 0);
        assert(v.Size() == 0);

        v.Reserve(SIZE);
        assert(v.Capacity() == SIZE);
        assert(v.Size() == 0);
    }
    {
        Vector<int> v(SIZE);
        const auto& cv(v);
        assert(v.Capacity() == SIZE);
        assert(v.Size() == SIZE);
        assert(v[0] == 0);
        assert(&v[0] == &cv[0]);
        v[INDEX] = MAGIC;
        assert(v[INDEX] == MAGIC);
        assert(&v[100] - &v[0] == 100);

        v.Reserve(SIZE * 2);
        assert(v.Size() == SIZE);
        assert(v.Capacity() == SIZE * 2);
        assert(v[INDEX] == MAGIC);
    }
    {
        Vector<int> v(SIZE);
        v[INDEX] = MAGIC;
        const auto v_copy(v);
        assert(&v[INDEX] != &v_copy[INDEX]);
        assert(v[INDEX] == v_copy[INDEX]);
    }
    {
        Vector<Obj_1> v;
        v.Reserve(SIZE);
        assert(Obj_1::GetAliveObjectCount() == 0);
    }
    {
        Vector<Obj_1> v(SIZE);
        assert(Obj_1::GetAliveObjectCount() == SIZE);
        v.Reserve(SIZE * 2);
        assert(Obj_1::GetAliveObjectCount() == SIZE);
    }
    assert(Obj_1::GetAliveObjectCount() == 0);
    std::cout << "Test1() - Done."s << std::endl;
}

struct C {
    C() {
        Check();
    }
    C(const C& /*other*/) {
        Check();
    }
    C(C&& /*other*/) = delete;
    C& operator=(const C& /*rhs*/) = default;
    C& operator=(C&& /*rhs*/) = delete;
    ~C() {
        --objects;
    }

    static void Check() {
        if (objects >= 5) {
            throw std::runtime_error("Too many objects"s);
        }
        ++objects;
    }
    inline static size_t objects = 0;
};

#include "tests.h"

int main() {
    //Test1();
    cout << "std::vector"sv << endl;
//    try {
//        std::vector<C> v(10);
//        cout << "OK"sv << endl;
//    } catch (...) {
//        cout << "Exception"sv << endl;
//        cout << "Objects:"sv << C::objects << endl;
//    }
//    cout << "Vector"sv << endl;
//    try {
//        Vector<C> v(10);
//        cout << "OK"sv << endl;
//    } catch (...) {
//        cout << "Exception"sv << endl;
//        cout << "Objects:"sv << C::objects << endl;
//    }
    Test2();
    Test3();
}
