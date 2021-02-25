#include <benchmark/benchmark.h>
#include <intrin.h>
#include <memory>

struct foo
{
    virtual int bar(int i) = 0;
    virtual ~foo() = default;
};

class foo_impl final : public foo
{
    int a;

public:
    foo_impl(int x) noexcept : a(x) { }
    int bar(int i) override { return a + i; }
};

static const std::unique_ptr<foo> foo_test = std::make_unique<foo_impl>(100);

__declspec(guard(nocf))
static void Benchmark_NoCFGNoBarrier(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(foo_test->bar(20));
    }
}

__declspec(guard(nocf))
static void Benchmark_NoCFGBarrier(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(foo_test->bar(20));
        _mm_lfence();
    }
}


static void Benchmark_CFGNoBarrier(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(foo_test->bar(20));
    }
}


static void Benchmark_CFGBarrier(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(foo_test->bar(20));
        _mm_lfence();
    }
}

BENCHMARK(Benchmark_NoCFGNoBarrier);
BENCHMARK(Benchmark_NoCFGBarrier);
BENCHMARK(Benchmark_CFGNoBarrier);
BENCHMARK(Benchmark_CFGBarrier);

BENCHMARK_MAIN();