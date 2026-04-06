#pragma once

#include <algorithm>
#include <concepts>
#include <iterator>
#include <random>
#include <ranges>
#include <source_location>
#include <type_traits>

namespace rng
{
namespace _detail
{
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
#define RNG_EXCEPTIONS_ENABLED 1
#else
#define RNG_EXCEPTIONS_ENABLED 0
#endif
inline void expect(bool condition, const char *message,
                   const std::source_location loc = std::source_location::current())
{
    if (condition)
        return; // All good, do nothing

#if RNG_EXCEPTIONS_ENABLED
    throw std::invalid_argument(std::string("RNG Error at ") + loc.file_name() + ":" +
                                std::to_string(loc.line()) + " - " + message);
#else
#include <print>
    std::println(stderr, "Fatal RNG Error at {}: {} - {}", loc.file_name(), loc.line(),
                 message);
    std::abort();
#endif
}
} // namespace _detail
template <typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;

template <typename R>
concept SizedForwardRange =
    std::ranges::forward_range<R> && std::ranges::sized_range<R>;

template <typename T>
concept URBG = std::uniform_random_bit_generator<T>;

template <typename Tp>
    requires std::is_arithmetic<Tp>::value
struct UniformImpl
{
    using uniform_dist = std::conditional_t<std::is_floating_point_v<Tp>,
                                            std::uniform_real_distribution<Tp>,
                                            std::uniform_int_distribution<Tp>>;
};

template <typename Tp>
using uniform_dist = typename UniformImpl<Tp>::uniform_dist;

template <typename Engine = std::mt19937>
    requires URBG<Engine>
class BitGen
{
public:
    using result_type = typename Engine::result_type;

    static constexpr result_type min()
    {
        return Engine::min();
    }

    static constexpr result_type max()
    {
        return Engine::max();
    }

    BitGen()
        : _rnd_engine{}
    {
        std::random_device rd{};
        std::array<unsigned int, 8> seed_data;
        std::generate(seed_data.begin(), seed_data.end(), std::ref(rd));
        std::seed_seq seq(seed_data.begin(), seed_data.end());
        _rnd_engine.seed(seq);
    }

    explicit BitGen(result_type seed)
        : _rnd_engine{seed}
    {
    }

    void seed(result_type s)
    {
        _rnd_engine.seed(s);
    }

    result_type operator()()
    {
        return _rnd_engine();
    }

    void discard(unsigned long long z)
    {
        _rnd_engine.discard(z);
    }

private:
    Engine _rnd_engine;
};

inline thread_local BitGen default_engine{};

template <Arithmetic Tp, Arithmetic Up, URBG Engine = BitGen<>>
inline constexpr std::common_type_t<Tp, Up> uniform(Tp a, Up b,
                                                    Engine &bitgen = default_engine)
{
    uniform_dist<std::common_type_t<Tp, Up>> dist(a, b);
    return dist(bitgen);
}

template <SizedForwardRange Rg, URBG Engine = BitGen<>>
constexpr inline decltype(auto) choice(Rg &&range, Engine &bitgen = default_engine)
{

    _detail::expect(!std::ranges::empty(range), "Range is empty!");
    auto len = std::ranges::size(range);
    auto const ind = uniform(0, len - 1, bitgen);
    return *std::ranges::next(std::ranges::begin(range), ind);
}

template <SizedForwardRange Rg, std::integral IndexType, URBG Engine = BitGen<>>
constexpr inline decltype(auto) choices(Rg &&range,
                                        std::discrete_distribution<IndexType> &weights,
                                        Engine &urbg = default_engine)
{
    auto const _wz = weights.probabilities().size();
    _detail::expect(!std::ranges::empty(range), "Range is empty!");
    _detail::expect(std::ranges::size(range) == _wz, "Size mismatch!");
    auto const ind = weights(urbg);
    return *std::ranges::next(std::ranges::begin(range), ind);
}

// Do not use this in a tight loop (Use the version with std::discret_distribution
// instead)
template <SizedForwardRange Rg, Arithmetic Tp, URBG Engine = BitGen<>>
constexpr inline decltype(auto) choices(Rg &&range, std::initializer_list<Tp> weights,
                                        Engine &urbg = default_engine)
{
    std::discrete_distribution<std::size_t> dist(weights.begin(), weights.end());
    return choices(std::forward<Rg>(range), dist, urbg);
}
} // namespace rng
