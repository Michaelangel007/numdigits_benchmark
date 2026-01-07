#define _CRT_SECURE_NO_WARNINGS 1

#include "util_benchmark.h"

#include <assert.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <random>
#include <type_traits>
#include <vector>

inline static uint32_t digits10_alexandrescu_v3( uint64_t );
#define digits10_fixed digits10_alexandrescu_v3

#include "bug_fix.h"
#include "numdigits.h"

#if 0 // positive int: 0 .. 2'147'483'647
    std::vector<std::int32_t> prepare_samples()
    {
        // I know this is broken, but who cares.
        std::mt19937 rg{ std::random_device{}() };

        std::vector<std::int32_t> samples(BENCHMARK_SAMPLE_SIZE);
        for (auto& s : samples)
            s = std::uniform_int_distribution<std::int32_t>{0, std::numeric_limits<std::int32_t>::max()}(rg);

        return samples;
    }

    static std::vector<std::int32_t> samples = prepare_samples();
#else // all int: -2'147'483'648 .. 2'147'483'647
    std::vector<std::uint32_t> prepare_samples()
    {
        // I know this is broken, but who cares.
        std::mt19937 rg{ std::random_device{}() };

        std::vector<std::uint32_t> samples(BENCHMARK_SAMPLE_SIZE);
        for (auto& s : samples)
            s = std::uniform_int_distribution<std::uint32_t>{0, std::numeric_limits<std::uint32_t>::max()}(rg);

        return samples;
    }

    static std::vector<std::uint32_t> samples = prepare_samples();
#endif

template <int (*func)(int)>
static void bench(benchmark::State& state) {
    std::size_t idx = 0;

    for (auto _ : state) {
        auto result = func(samples[idx]);
        void *p = (void*)(uint64_t) result; // We don't care about the actual pointer, just need to cache it

        // Make sure the result or function is not optimized away by the compiler
        benchmark::DoNotOptimize(p);

        if (++idx == samples.size())
            idx = 0;
    }
}

// ------------------------------------------------------------

static void bench_numdigits_alexandrescu_v1(benchmark::State& state) {
    bench<numdigits_alexandrescu_v1>(state);
}
BENCHMARK(bench_numdigits_alexandrescu_v1);

static void bench_numdigits_alexandrescu_pohoreski_v2(benchmark::State& state) {
    bench<numdigits_alexandrescu_pohoreski_v2>(state);
}
BENCHMARK(bench_numdigits_alexandrescu_pohoreski_v2);

static void bench_numdigits_alexandrescu_pohoreski_v3(benchmark::State& state) {
    bench<numdigits_alexandrescu_pohoreski_v3>(state);
}
BENCHMARK(bench_numdigits_alexandrescu_pohoreski_v3);

static void bench_numdigits_alink_buggy_v1(benchmark::State& state) {
    bench<numdigits_alink_buggy_v1>(state);
}
BENCHMARK(bench_numdigits_alink_buggy_v1);

static void bench_numdigits_alink_buggy_v2(benchmark::State& state) {
    bench<numdigits_alink_buggy_v2>(state);
}
BENCHMARK(bench_numdigits_alink_buggy_v2);

static void bench_numdigits_alink_fixed_v1a(benchmark::State& state) {
    bench<numdigits_alink_fixed_v1a>(state);
}
BENCHMARK(bench_numdigits_alink_fixed_v1a);

static void bench_numdigits_alink_fixed_v1b(benchmark::State& state) {
    bench<numdigits_alink_fixed_v1b>(state);
}
BENCHMARK(bench_numdigits_alink_fixed_v1b);

static void bench_numdigits_alink_fixed_v2a(benchmark::State& state) {
    bench<numdigits_alink_fixed_v2a>(state);
}
BENCHMARK(bench_numdigits_alink_fixed_v2a);

static void bench_numdigits_alink_fixed_v2b(benchmark::State& state) {
    bench<numdigits_alink_fixed_v2b>(state);
}
BENCHMARK(bench_numdigits_alink_fixed_v2b);

static void bench_numdigits_clifford_buggy(benchmark::State& state) {
    bench<numdigits_clifford_buggy>(state);
}
BENCHMARK(bench_numdigits_clifford_buggy);

static void bench_numdigits_clifford_fixed(benchmark::State& state) {
    bench<numdigits_clifford_fixed>(state);
}
BENCHMARK(bench_numdigits_clifford_fixed);

static void bench_numdigits_dumb_sprintf_strlen(benchmark::State& state) {
    bench<numdigits_dumb_sprintf_strlen>(state);
}
BENCHMARK(bench_numdigits_dumb_sprintf_strlen);

static void bench_numdigits_gabriel_v1a(benchmark::State& state) {
    bench<numdigits_gabriel_v1a>(state);
}
BENCHMARK(bench_numdigits_gabriel_v1a);

static void bench_numdigits_gabriel_v1b(benchmark::State& state) {
    bench<numdigits_gabriel_v1b>(state);
}
BENCHMARK(bench_numdigits_gabriel_v1b);

static void bench_numdigits_gabriel_v1c(benchmark::State& state) {
    bench<numdigits_gabriel_v1c>(state);
}
BENCHMARK(bench_numdigits_gabriel_v1c);

static void bench_numdigits_if_naive(benchmark::State& state) {
    bench<numdigits_if_naive>(state);
}
BENCHMARK(bench_numdigits_if_naive);

static void bench_numdigits_log(benchmark::State& state) {
    bench<numdigits_log>(state);
}
BENCHMARK(bench_numdigits_log);

static void bench_numdigits_microsoft_itoa_strlen(benchmark::State& state) {
    bench<numdigits_microsoft_itoa_strlen>(state);
}
BENCHMARK(bench_numdigits_microsoft_itoa_strlen);

static void bench_numdigits_pohoreski_v1a(benchmark::State& state) {
    bench<numdigits_pohoreski_v1a>(state);
}
BENCHMARK(bench_numdigits_pohoreski_v1a);

static void bench_numdigits_pohoreski_v1b(benchmark::State& state) {
    bench<numdigits_pohoreski_v1b>(state);
}
BENCHMARK(bench_numdigits_pohoreski_v1b);

static void bench_numdigits_pohoreski_v2a(benchmark::State& state) {
    bench<numdigits_pohoreski_v2a>(state);
}
BENCHMARK(bench_numdigits_pohoreski_v2a);

static void bench_numdigits_pohoreski_v2b(benchmark::State& state) {
    bench<numdigits_pohoreski_v2b>(state);
}
BENCHMARK(bench_numdigits_pohoreski_v2b);

static void bench_numdigits_pohoreski_v3a(benchmark::State& state) {
    bench<numdigits_pohoreski_v3a>(state);
}
BENCHMARK(bench_numdigits_pohoreski_v3a);

static void bench_numdigits_pohoreski_v3b(benchmark::State& state) {
    bench<numdigits_pohoreski_v3b>(state);
}
BENCHMARK(bench_numdigits_pohoreski_v3b);

static void bench_numdigits_pohoreski_v4a(benchmark::State& state) {
    bench<numdigits_pohoreski_v4a>(state);
}
BENCHMARK(bench_numdigits_pohoreski_v4a);

static void bench_numdigits_pohoreski_v4b(benchmark::State& state) {
    bench<numdigits_pohoreski_v4b>(state);
}
BENCHMARK(bench_numdigits_pohoreski_v4b);

static void bench_numdigits_ransom(benchmark::State& state) {
    bench<numdigits_ransom>(state);
}
BENCHMARK(bench_numdigits_ransom);

static void bench_numdigits_reference_int(benchmark::State& state) {
    bench<numdigits10_int>(state);
}
BENCHMARK(bench_numdigits_reference_int);

static void bench_numdigits_reference_mixed_int(benchmark::State& state) {
    bench<numdigits10_mixed_int>(state);
}
BENCHMARK(bench_numdigits_reference_mixed_int);

static void bench_numdigits_simple(benchmark::State& state) {
    bench<numdigits_simple>(state);
}
BENCHMARK(bench_numdigits_simple);

static void bench_numdigits_simple_reverse(benchmark::State& state) {
    bench<numdigits_simple_reverse>(state);
}
BENCHMARK(bench_numdigits_simple_reverse);

static void bench_numdigits_thomas(benchmark::State& state) {
    bench<numdigits_thomas>(state);
}
BENCHMARK(bench_numdigits_thomas);

static void bench_numdigits_user42690_buggy(benchmark::State& state) {
    bench<numdigits_user42690_buggy>(state);
}
BENCHMARK(bench_numdigits_user42690_buggy);

static void bench_numdigits_user42690_fixed_a(benchmark::State& state) {
    bench<numdigits_user42690_fixed_a>(state);
}
BENCHMARK(bench_numdigits_user42690_fixed_a);

static void bench_numdigits_user42690_fixed_b(benchmark::State& state) {
    bench<numdigits_user42690_fixed_b>(state);
}
BENCHMARK(bench_numdigits_user42690_fixed_b);

static void bench_numdigits_vitali(benchmark::State& state) {
    bench<numdigits_vitali>(state);
}
BENCHMARK(bench_numdigits_vitali);

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
int main(int argc, char** argv)
{
    printf( "Benchmarking numdigits()\n" );
    printf( "    sizeof( int     ) = %zu bytes\n", sizeof( int     ) );
    printf( "    sizeof( int32   ) = %zu bytes\n", sizeof( int32_t ) );
    printf( "    sizeof(long long) = %zu bytes\n", sizeof(long long) );
    printf( "    sizeof( int64_t ) = %zu bytes\n", sizeof( int64_t ) );
    printf( "    sizeof(uint64_t ) = %zu bytes\n", sizeof(uint64_t ) );

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}
