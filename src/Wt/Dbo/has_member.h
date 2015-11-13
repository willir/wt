#ifndef GENERATE_HAS_MEMBER

#define GENERATE_HAS_MEMBER(X)                                                   \
template<typename T> class has_member_##X {                                         \
    struct Fallback { int X; };                                                     \
    struct Derived : T, Fallback { };                                               \
                                                                                    \
    template<typename U, U> struct Check;                                           \
                                                                                    \
    typedef char ArrayOfOne[1];                                                     \
    typedef char ArrayOfTwo[2];                                                     \
                                                                                    \
    template<typename U> static ArrayOfOne & func(Check<int Fallback::*, &U::X> *); \
    template<typename U> static ArrayOfTwo & func(...);                             \
  public:                                                                           \
    typedef has_member_##X type;                                                        \
    enum { value = sizeof(func<Derived>(0)) == 2 };                                 \
}

#endif
