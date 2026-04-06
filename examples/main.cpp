#include <print>
#include <random>
#include <rng/random.hpp>
#include <vector>

int main()
{
    std::vector<std::string> names_vec{"A", "B", "C"};
    std::discrete_distribution<int> dist({75.5, 4.5, 20});
    int const max_iter = 100000;
    int na = 0;
    int nb = 0;
    int nc = 0;
    for (int i = 0; i < max_iter; i++) {

        auto const result = rng::choices(names_vec, dist);
        if (result == "A")
            na += 1;
        else if (result == "B")
            nb += 1;
        else if (result == "C")
            nc += 1;
        else
            throw "bad result";
    }
    std::println("na = {}, nb = {}, nc={}", na, nb, nc);
    auto pa = (float(na) / max_iter) * 100.;
    auto pb = (float(nb) / max_iter) * 100.;
    auto pc = (float(nc) / max_iter) * 100.;
    std::println("[A] = {} - [B] = {} - [C] = {}", pa, pb, pc);
}
