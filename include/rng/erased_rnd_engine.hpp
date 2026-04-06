#pragma once

#include <random>
#include <utility>
namespace ntd
{
namespace _bitgen_impl
{
enum class Action
{
    Destroy,
    Copy,
    Move,
    Seed,
    Call
};

template <typename Tp, typename Rs = typename Tp::result_type>
struct handler;
} // namespace _bitgen_impl

template <typename T>
concept URBG = std::uniform_random_bit_generator<T>;

class BitGenErased
{
public:
    using result_type = std::uint64_t;

public:
    BitGenErased() : _h(nullptr), _ptr(nullptr)
    {
    }

    template <URBG Tp, typename Decayed = std::decay_t<Tp>>
        requires(!std::same_as<Decayed, BitGenErased> &&
                 std::copy_constructible<Decayed>)
    BitGenErased(Tp &&eng)
    {
        _bitgen_impl::handler<Tp>::_create(*this, std::forward<Tp>(eng));
    }

    template <URBG Tp, typename... Args, typename Decayed = std::decay_t<Tp>>
        requires(std::constructible_from<Decayed, Args...> &&
                 std::copy_constructible<Decayed>)
    BitGenErased(std::in_place_type_t<Tp>, Args &&...args)
    {
        _bitgen_impl::handler<Tp>::_create(*this, std::forward<Args>(args)...);
    }

    ~BitGenErased()
    {
        if (_h)
            _h(Action::Destroy, this, nullptr, nullptr);
    }

    BitGenErased(BitGenErased const &other) : _h(nullptr)
    {
        if (other._h)
            other._h(Action::Copy, &other, this, nullptr);
    }

    BitGenErased(BitGenErased &&other) noexcept : _h(nullptr)
    {
        if (other._h)
            other._h(Action::Move, &other, this, nullptr);
    }

    BitGenErased &operator=(BitGenErased const &other)
    {
        if (this != &other) {
            BitGenErased tmp(other);
            this->swap(tmp);
        }
        return *this;
    }

    BitGenErased &operator=(BitGenErased &&other) noexcept
    {
        if (this != &other) {
            if (_h) {
                _h(Action::Destroy, this, nullptr, nullptr);
            }
            _ptr = other._ptr;
            _h = other._h;
            other._ptr = nullptr;
            other._h = nullptr;
        }
        return *this;
    }
    void seed(result_type _seed)
    {
        if (_h) {
            auto cp = _seed;
            _h(Action::Seed, this, nullptr, &cp);
        }
    }

    result_type operator()()
    {
        result_type ret{};
        if (_h) {
            _h(Action::Call, this, nullptr, &ret);
        }
        return ret;
    }

    void swap(BitGenErased &other)
    {
        using std::swap;
        std::swap(_h, other._h);
        std::swap(_ptr, other._ptr);
    }

    using Action = _bitgen_impl::Action;
    using _ptr_handler = void (*)(Action, BitGenErased const *, BitGenErased *,
                                  void *seed);

    template <typename Tp, typename Rs>
    friend struct _bitgen_impl::handler;

private:
    _ptr_handler _h;
    void *_ptr;
};

namespace _bitgen_impl
{
template <typename Tp, typename Rs>
struct handler
{
    static void handle(Action act, BitGenErased const *self, BitGenErased *other,
                       void *args)
    {
        switch (act) {
        case Action::Destroy:
            _destroy(const_cast<BitGenErased &>(*self));
            break;
        case Action::Copy:
            _copy(*self, *other);
            break;
        case Action::Move:
            _move(const_cast<BitGenErased &>(*self), *other);
            break;
        case Action::Seed: {
            Tp *_ptr = static_cast<Tp *>(self->_ptr);
            _ptr->seed(*static_cast<Rs *>(args));
            break;
        }
        case Action::Call:
            Tp *_ptr = static_cast<Tp *>(self->_ptr);
            *static_cast<Rs *>(args) = (*_ptr)();
            break;
        }
    }
    template <typename... Args>
    static Tp &_create(BitGenErased &dest, Args &&...args)
    {
        Tp *_ptr = static_cast<Tp *>(
            ::operator new(sizeof(Tp), std::align_val_t{alignof(Tp)}));
        try {
            ::new (_ptr) Tp(std::forward<Args>(args)...);
        }
        catch (...) {
            ::operator delete(_ptr, std::align_val_t{alignof(Tp)});
            throw;
        }
        dest._ptr = _ptr;
        dest._h = &handler::handle;
        return *_ptr;
    }

    static void _destroy(BitGenErased &self)
    {
        Tp *_ptr = static_cast<Tp *>(self._ptr);
        std::destroy_at(_ptr);
        ::operator delete(_ptr, std::align_val_t{alignof(Tp)});
        self._h = nullptr;
    }

    static void _copy(BitGenErased const &self, BitGenErased &dest)
    {
        handler::_create(dest, *static_cast<Tp const *>(self._ptr));
    }

    static void _move(BitGenErased &self, BitGenErased &dest)
    {
        dest._ptr = self._ptr;
        dest._h = &handler::handle;
        self._h = nullptr;
    }
};
} // namespace _bitgen_impl
} // namespace ntd
