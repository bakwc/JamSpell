#pragma once

#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <utility>
#include <iostream>
#include <streambuf>
#include <tuple>

namespace NHandyPack {

template<class T, typename E = void>
class TSerializer {
public:
    static void Dump(std::ostream& out, const T& object) {
        object.Dump(out);
    }
    static void Load(std::istream& in, T& object) {
        object.Load(in);
    }
};

template <class T>
static inline void Dump(std::ostream& out, const T& t);

template <class T>
static inline void Load(std::istream& in, T& t);


template <class A, class B>
class TSerializer<std::pair<A, B> > {
public:
    static void Dump(std::ostream& out, const std::pair<A, B>& object) {
        NHandyPack::Dump(out, object.first);
        NHandyPack::Dump(out, object.second);
    }
    static void Load(std::istream& in, std::pair<A, B>& object) {
        NHandyPack::Load(in, object.first);
        NHandyPack::Load(in, object.second);
    }
};

template<std::size_t> struct int_{};

template <class Tuple, size_t Pos>
void DumpTuple(std::ostream& out, const Tuple& tuple, int_<Pos>) {
    DumpTuple(out, tuple, int_<Pos-1>());
    NHandyPack::Dump(out, std::get<std::tuple_size<Tuple>::value-Pos>(tuple));
}

template <class Tuple>
void DumpTuple(std::ostream& out, const Tuple& tuple, int_<1>) {
    NHandyPack::Dump(out, std::get<std::tuple_size<Tuple>::value-1>(tuple));
}

template <class Tuple, size_t Pos>
void LoadTuple(std::istream& in, Tuple& tuple, int_<Pos>) {
    LoadTuple(in, tuple, int_<Pos-1>());
    NHandyPack::Load(in, std::get<std::tuple_size<Tuple>::value-Pos>(tuple));
}

template <class Tuple>
void LoadTuple(std::istream& in, Tuple& tuple, int_<1>) {
    NHandyPack::Load(in, std::get<std::tuple_size<Tuple>::value-1>(tuple));
}

template <class... Args>
class TSerializer<std::tuple<Args...>> {
public:
    static void Dump(std::ostream& out, const std::tuple<Args...>& object) {
        DumpTuple(out, object, int_<sizeof...(Args)>());
    }
    static void Load(std::istream& in, std::tuple<Args...>& object) {
        LoadTuple(in, object, int_<sizeof...(Args)>());
    }
};

template<class TVec, class TObj>
class TVectorSerializer {
public:
    static inline void Dump(std::ostream& out, const TVec& object) {
        uint32_t size = object.size();
        out.write((const char*)(&size), sizeof(size));
        for (const auto& obj: object) {
            NHandyPack::Dump(out, obj);
        }
    }

    static inline void Load(std::istream& in, TVec& object) {
        uint32_t size;
        in.read((char*)(&size), sizeof(size));
        object.clear();
        object.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            TObj obj;
            NHandyPack::Load(in, obj);
            object.push_back(std::move(obj));
        }
    }
};

template<class TVec, class TKey, class TValue>
class TMapSerializer {
public:
    static inline void Dump(std::ostream& out, const TVec& object) {
        uint32_t size = object.size();
        out.write((const char*)(&size), sizeof(size));
        for (const auto& obj: object) {
            NHandyPack::Dump(out, obj);
        }
    }

    static inline void Load(std::istream& in, TVec& object) {
        uint32_t size;
        in.read((char*)(&size), sizeof(size));
        object.clear();
        for (size_t i = 0; i < size; ++i) {
            std::pair<TKey, TValue> obj;
            NHandyPack::Load(in, obj);
            object.insert(std::move(obj));
        }
    }
};

template<class TVec, class TKey, class TValue>
class TUnorderedMapSerializer {
public:
    static inline void Dump(std::ostream& out, const TVec& object) {
        uint32_t size = object.size();
        out.write((const char*)(&size), sizeof(size));
        for (const auto& obj: object) {
            NHandyPack::Dump(out, obj);
        }
    }

    static inline void Load(std::istream& in, TVec& object) {
        uint32_t size;
        in.read((char*)(&size), sizeof(size));
        object.clear();
        object.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            std::pair<TKey, TValue> obj;
            NHandyPack::Load(in, obj);
            object.insert(std::move(obj));
        }
    }
};

template<class TVec, class TObj>
class TSetSerializer {
public:
    static inline void Dump(std::ostream& out, const TVec& object) {
        uint32_t size = object.size();
        out.write((const char*)(&size), sizeof(size));
        for (const auto& obj: object) {
            NHandyPack::Dump(out, obj);
        }
    }

    static inline void Load(std::istream& in, TVec& object) {
        uint32_t size;
        in.read((char*)(&size), sizeof(size));
        object.clear();
        for (size_t i = 0; i < size; ++i) {
            TObj obj;
            NHandyPack::Load(in, obj);
            object.insert(std::move(obj));
        }
    }
};

template <class T> class TSerializer<std::vector<T> >: public TVectorSerializer<std::vector<T>, T > {};
template <class T> class TSerializer<std::list<T> >: public TVectorSerializer<std::list<T>, T > {};
template <> class TSerializer<std::string>: public TVectorSerializer<std::string, char> {};
template <> class TSerializer<std::wstring>: public TVectorSerializer<std::wstring, wchar_t> {};
template <class K, class V> class TSerializer<std::map<K, V> >: public TMapSerializer<std::map<K, V>, K, V > {};
template <class K, class V, class H> class TSerializer<std::unordered_map<K, V, H> >: public TUnorderedMapSerializer<std::unordered_map<K, V, H>, K, V > {};
template <class T> class TSerializer<std::set<T> >: public TSetSerializer<std::set<T>, T > {};
template <class T> class TSerializer<std::unordered_set<T> >: public TSetSerializer<std::unordered_set<T>, T > {};

template <class T>
class TPodSerializer {
public:
    static inline void Dump(std::ostream& out, const T& object) {
        out.write((const char*)(&object), sizeof(T));
    }
    static inline void Load(std::istream& in, T& object) {
        in.read((char*)(&object), sizeof(T));
    }
};

template<class T>
class TSerializer<T, typename std::enable_if<!std::is_class<T>::value>::type>: public TPodSerializer<T> {};

template <class T>
static inline void Dump(std::ostream& out, const T& t) {
    TSerializer<T>::Dump(out, t);
}

template<class T, class... Args>
static inline void Dump(std::ostream& out, const T& first, const Args&... args) {
    NHandyPack::Dump(out, first);
    NHandyPack::Dump(out, args...);
}

template <class T>
static inline void Load(std::istream& in, T& t) {
    TSerializer<T>::Load(in, t);
}

template <class T, class... Args>
static inline void Load(std::istream& in, T& first, Args&... args) {
    NHandyPack::Load(in, first);
    NHandyPack::Load(in, args...);
}

#define HANDYPACK(...) \
    inline virtual void Dump(std::ostream& out) const { \
        NHandyPack::Dump(out, __VA_ARGS__);             \
    } \
 \
    inline virtual void Load(std::istream& in) { \
        NHandyPack::Load(in, __VA_ARGS__);             \
    }


struct membuf: std::streambuf {
    membuf(char const* base, size_t size) {
        char* p(const_cast<char*>(base));
        this->setg(p, p, p + size);
    }
};
struct imemstream: virtual membuf, std::istream {
    imemstream(char const* base, size_t size)
        : membuf(base, size)
        , std::istream(static_cast<std::streambuf*>(this)) {
    }
};

#define HANDYPACK_POD(TypeName) template <> class TSerializer<TypeName>: public TPodSerializer<TypeName> {};

} // NHandyPack
