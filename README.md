# 🎲 rng

A lightweight, modern C++23 header-only library that makes random number
generation ergonomic, safe, and blazingly fast.

`rng` wraps the verbose C++ `<random>` library into a clean, Python-like
API (I took also some inspirations from abseil library)
and fully supporting C++20 Ranges.

## ✨ Features

* **Ergonomic API**: Python-inspired `choice`, `choices`, and `uniform` functions.
* **Mathematically Robust Seeding**: Safely warms up `std::mt19937` using an
8-word `std::seed_seq` entropy pool, avoiding the common 32-bit
state bottleneck.
* **C++20 Ranges Support**: Works seamlessly with any sized forward range
(`std::vector`, `std::array`, etc.).
* **Hybrid Error Handling**: Uses C++20 `<source_location>` to provide
accurate error tracing. Dynamically adapts to your compiler flags
(throws catchable exceptions normally, or safely aborts in `-fno-exceptions`
environments).
* **Header-Only**: Drop it in and go.

## 🚀 Quick Start

```cpp
#include <rng/rng.h>
#include <iostream>
#include <vector>
#include <string>

int main() {
    // 1. Uniform Generation (Integers and Floats)
    int dice_roll = rng::uniform(1, 6);
    double angle = rng::uniform(0.0, 360.0);

    // 2. Random Choice from a Range
    std::vector<std::string> enemies = {"Goblin", "Orc", "Dragon"};
    std::string spawn = rng::choice(enemies);

    // 3. Weighted Random Choices
    std::vector<std::string> loot = {"Common", "Rare", "Legendary"};
    
    // 80% Common, 15% Rare, 5% Legendary
    auto item = rng::choices(loot, {80, 15, 5}); 
    
    std::cout << "Spawned: " << spawn << " carrying a " << item << " item!\n";
    return 0;
}
```
