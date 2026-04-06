#include <catch2/catch_test_macros.hpp>
#include <rng/random.hpp>
#include <string>
#include <vector>

// --- COMPILE-TIME TESTS ---
static_assert(rng::URBG<std::mt19937>, "std::mt19937 must satisfy URBG");
static_assert(!rng::URBG<std::string>, "std::string must not satisfy URBG");
static_assert(requires { typename rng::BitGen<std::mt19937>; });
static_assert(!rng::URBG<double>, "URBG Must reject int");

TEST_CASE("BitGen initialization and generation", "[BitGen]")
{
    SECTION("Deterministic seeding yields identical results")
    {
        rng::BitGen<> engine1(42);
        rng::BitGen<> engine2(42);

        REQUIRE(engine1() == engine2());
        REQUIRE(engine1() == engine2());
    }

    SECTION("Constructing from an existing random engine yields identical results")
    {
        std::mt19937 stdeng(42);
        rng::BitGen rngeng(stdeng);

        REQUIRE(stdeng() == rngeng());
        REQUIRE(stdeng() == rngeng());
    }

    SECTION("Constructing using in place constructor")
    {
        std::mt19937 stdeng(42);
        rng::BitGen<std::mt19937> rngeng(std::in_place_t{}, 42);

        REQUIRE(stdeng() == rngeng());
        REQUIRE(stdeng() == rngeng());
    }

    SECTION("Engine limits match underlying std::mt19937")
    {
        REQUIRE(rng::BitGen<>::min() == std::mt19937::min());
        REQUIRE(rng::BitGen<>::max() == std::mt19937::max());
    }
}

TEST_CASE("Uniform number generation respects bounds", "[uniform]")
{
    SECTION("Integer uniform distribution")
    {
        for (int i = 0; i < 100; ++i) {
            int val = rng::uniform(1, 5);
            REQUIRE(val >= 1);
            REQUIRE(val <= 5);
        }
    }

    SECTION("Floating point uniform distribution")
    {
        for (int i = 0; i < 100; ++i) {
            double val = rng::uniform(1.0, 5.0);
            REQUIRE(val >= 1.0);
            REQUIRE(val <= 5.0);
        }
    }
}

TEST_CASE("choice() selects valid elements from ranges", "[choice]")
{
    std::vector<int> v = {10, 20, 30};

    SECTION("Elements are always from the original range")
    {
        for (int i = 0; i < 50; ++i) {
            int c = rng::choice(v);
            REQUIRE((c == 10 || c == 20 || c == 30));
        }
    }

    SECTION("Empty range throws invalid_argument")
    {
        std::vector<int> empty_vec;
        REQUIRE_THROWS_AS(rng::choice(empty_vec), std::invalid_argument);
    }
}

TEST_CASE("choices() with weighted probabilities", "[choices]")
{
    std::vector<std::string> v = {"Common", "Rare", "Legendary"};

    SECTION("discrete_distribution correctly biases selection")
    {
        // 100% chance to pick "Common"
        std::discrete_distribution<size_t> dist1({100, 0, 0});
        REQUIRE(rng::choices(v, dist1) == "Common");

        // 100% chance to pick "Legendary"
        std::discrete_distribution<size_t> dist2({0, 0, 100});
        REQUIRE(rng::choices(v, dist2) == "Legendary");
    }

    SECTION("initializer_list correctly biases selection")
    {
        // 100% chance to pick "Rare"
        REQUIRE(rng::choices(v, {0, 100, 0}) == "Rare");
    }

    SECTION("Size mismatch throws invalid_argument")
    {
        // Range has 3 items, but distribution only has 2 weights
        std::discrete_distribution<size_t> bad_dist({50, 50});
        REQUIRE_THROWS_AS(rng::choices(v, bad_dist), std::invalid_argument);
    }
}

TEST_CASE("Custom URBG overrides the default engine", "[custom_engine]")
{
    std::vector<int> v = {1, 2, 3};
    rng::BitGen<> custom_engine(1337); // Seeded for deterministic testing

    SECTION("Passing custom engine to uniform")
    {
        int val1 = rng::uniform(1, 100, custom_engine);
        custom_engine.seed(1337); // Reset state
        int val2 = rng::uniform(1, 100, custom_engine);
        REQUIRE(val1 == val2);
    }
}
