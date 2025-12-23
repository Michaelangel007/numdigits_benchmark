/*
// v1.5 Add Best of N support
// v1.4 Cache output of benchmark test and warn if an implementation is buggy
// v1.3 Output no optimize on same line as totals
// v1.2 Calculate Total Calls at higher precision
// v1.1 Fixed MS output incorrectly being 0.0
Mini Benchmark replacement for Google's bloated benchmark
https://github.com/google/benchmark/
https://google.github.io/benchmark/user_guide.html
*/
#if 1 // Use ours
    #define _CRT_SECURE_NO_WARNINGS 1

    #include <stdio.h>
    #include <string.h> // strcpy()

    #include <algorithm>
    #include <numeric>    // iota()
    #include <chrono>
    #include <vector>

#define BENCHMARK_SAMPLE_SIZE 1000000
#if _DEBUG
    #undef  BENCHMARK_SAMPLE_SIZE
    #define BENCHMARK_SAMPLE_SIZE 100
#endif

namespace benchmark
{
    static void *ResultNoOptimize;
    static void *FirstNoOptimize;
    static size_t MaximumName;

    struct Benchmark;
    std::vector<Benchmark*> RegisteredBenchmarks;

    static int                     iRun;
    static int                     nRuns;
    static std::vector<Benchmark*> aRuns[9];

    struct BenchmarkState
    {
        Benchmark *Parent;

        BenchmarkState(Benchmark* InParent)
        {
            Parent = InParent;
        }
    };

    typedef std::vector<int> State;
    typedef void (*BenchmarkFuncPtr)(benchmark::State& state);

    struct MetricData
    {
        double           ElapsedMS; // milli
        double           ElapsedNS; // nano
        double           CallerNS;  // nanoseconds per call
        double           PercentFaster;
        double           AverageNSPerCall;
        double           AveragePercentFaster;
        double           FirstCallerNS;

        void Reset()
        {
            ElapsedMS            = 0.0;
            ElapsedNS            = 0.0;
            CallerNS             = 0.0;
            PercentFaster        = 0.0;
            AverageNSPerCall     = 0.0; // none (yet)
            AveragePercentFaster = 0.0;
            FirstCallerNS        = 0.0;
        }

        void Update(double ns, double ooTotalCalls, bool firstCaller, double firstCallerNS)
        {
            ElapsedNS     = ns;
            ElapsedMS     = ElapsedNS / 1'000'000; // 1 million nanoseconds per 1 millisecond
            CallerNS      = ElapsedNS * ooTotalCalls;
            if (firstCaller)
            {
                PercentFaster = (100.0 * (firstCallerNS - CallerNS)) / firstCallerNS;
                FirstCallerNS = firstCallerNS;
            }
        }
        void UpdateAverage( double averageNSPerCall, bool firstTest )
        {
            AverageNSPerCall = averageNSPerCall;
            if (!firstTest)
                AveragePercentFaster = (100.0 * (FirstCallerNS - AverageNSPerCall)) / FirstCallerNS;
        }
    };

    struct Benchmark
    {
        BenchmarkFuncPtr Func;
        const char      *Name;
        MetricData       Metrics;

        int              Passes;
        int              MinPasses;
        State            States;
        bool             WarnBadBenchmarkResults;

        Benchmark(const BenchmarkFuncPtr InFunc, const char* InName)
        {
            Func = InFunc;
            Name = InName;

            const size_t N = BENCHMARK_SAMPLE_SIZE;
            States.resize( N ); // BUG? reserve
            std::iota( States.begin(), States.end(), 0 );

            Metrics.Reset();
            Passes = 0;
            WarnBadBenchmarkResults = false;
            MinPasses = 500;
        }
    };

    static void Initialize(int *nArg, char **Argv)
    {
        printf( "Registered %zu functions...\n\n", RegisteredBenchmarks.size() );
        ResultNoOptimize = 0;
        MaximumName = 0;

        iRun  = 0;
        nRuns = 1;

        if (*nArg > 1)
        {
            nRuns = atoi( Argv[1] );
            nRuns = std::min( nRuns, 9 );
            nRuns = std::max( nRuns, 1 );
            if (nRuns == 2) // We can't do a "Best of" with only 2 tests
                nRuns = 1; // since we discard the best case and worst case
            if (nRuns > 1)
                printf( "Best of %d\n", nRuns );
        }
    }

    static void RunSpecifiedBenchmarks()
    {
        for (int iRun = 0; iRun < nRuns; iRun++ )
        {
            if (nRuns > 1)
                printf( "--- Run %d of %d ---\n", iRun+1, nRuns );

            for (Benchmark* bench : RegisteredBenchmarks)
            {
                const size_t len = strlen( bench->Name);
                if (MaximumName < len)
                    MaximumName = len;

                printf( "Running '%s'...\n", bench->Name );
                State& states = bench->States;

                auto start = std::chrono::high_resolution_clock::now();
                    do
                    {
                        bench->Passes++;
                        bench->Func( states );
                    } while (bench->Passes < bench->MinPasses);
                auto stop  = std::chrono::high_resolution_clock::now();

                const double ooTotalCalls = 1.0 / ((double)bench->Passes * (double)states.size());
                const double ns = (double) std::chrono::duration_cast<std::chrono::nanoseconds >(stop - start).count();
                const bool   firstCaller   = (bench != RegisteredBenchmarks[0]);
                const double firstCallerNS = RegisteredBenchmarks[0]->Metrics.CallerNS;
                bench->Metrics.Update( ns, ooTotalCalls, firstCaller, firstCallerNS );

                if (firstCaller)
                {
                    if (ResultNoOptimize && (FirstNoOptimize != ResultNoOptimize))
                    {
                        bench->WarnBadBenchmarkResults = true;
                    }
                }
                else
                {
                    if (ResultNoOptimize )
                    {
                        FirstNoOptimize = ResultNoOptimize;
                    }
                }

                printf( "    Total: %7.3f ms, Passes: %d, Pass Size: %zu", bench->Metrics.ElapsedMS, bench->Passes, states.size() );
                if (ResultNoOptimize)
                {
                    printf( "  ForceOpt0: %08p", (char*)ResultNoOptimize );
                    if (bench->WarnBadBenchmarkResults)
                        printf( " WARNING implementation buggy?");
                    printf( "\n");
                }
                printf( "    ns/call: %7.3f ns\n", bench->Metrics.CallerNS       );
                printf( "    %%faster: %6.2f%%\n", bench->Metrics.PercentFaster  );
            }

            if (nRuns > 1)
            {
                // We need a deep copy not a shallow copy of pointers
                // We copy the existing pointers and allocate a new copy for RegisteredFunctions
                aRuns[ iRun ] = RegisteredBenchmarks;

                RegisteredBenchmarks.clear();
                for (Benchmark* bench : aRuns[ iRun ])
                {
                    Benchmark *copy = new Benchmark( bench->Func, bench->Name );
                    RegisteredBenchmarks.push_back( copy );
                }
            }
        }
    }

    static void Summary()
    {
        struct
        {
            bool operator()(const Benchmark* a, const Benchmark* b) const
            {
                return a->Metrics.ElapsedNS < b->Metrics.ElapsedNS;
            };
        } CompareElapsedNS;
        std::vector<Benchmark*> sorted = RegisteredBenchmarks;
        std::sort( sorted.begin(), sorted.end(), CompareElapsedNS );

        printf( "\n" );
        printf( "=== Summary (In order of appearance) ===\n" );
        for (Benchmark* bench : RegisteredBenchmarks)
        {
            printf( "%*s  ", -(int)MaximumName, bench->Name );
            if (bench->Metrics.AverageNSPerCall > 0.0)
                printf( "~%7.3f avg ns/call  %7.2f%%", bench->Metrics.AverageNSPerCall, bench->Metrics.AveragePercentFaster );
            else
                printf( "%7.3f ns/call  %7.2f%%", bench->Metrics.CallerNS, bench->Metrics.PercentFaster );
            printf( "\n" );
        }

        printf( "\n" );
        printf( "=== Summary (Best to Worst) ===\n" );
        for (Benchmark* bench : sorted)
        {
            printf( "%*s  ", -(int)MaximumName, bench->Name );
            if (bench->Metrics.AverageNSPerCall > 0.0)
                printf( "~%7.3f avg ns/call  %7.2f%%", bench->Metrics.AverageNSPerCall, bench->Metrics.AveragePercentFaster );
            else
                printf( "%7.3f ns/call  %7.2f%%", bench->Metrics.CallerNS, bench->Metrics.PercentFaster );
            printf( "\n" );
        }
    }

    static void BestOf()
    {
        if (nRuns > 1)
        {
            std::vector< double > bestCaller   ; // [nTests]
            std::vector< double > worstCaller  ; // [nTests]
            std::vector< double > totalCaller  ; // [nTests]
            std::vector< double > averageCaller; // [nTests]

            const int nTests = (int) RegisteredBenchmarks.size();
            bestCaller   .reserve( nTests );
            worstCaller  .reserve( nTests );
            totalCaller  .reserve( nTests );
            averageCaller.reserve( nTests );

            for (int iTest = 0; iTest < nTests; iTest++ )
            {
                const int iRun = 0;
                Benchmark*  pBenchmark0 = aRuns[ iRun ][ iTest ];
                MetricData& tMetrics = pBenchmark0->Metrics;
                totalCaller.push_back( tMetrics.CallerNS );
                worstCaller.push_back( tMetrics.CallerNS );
                bestCaller .push_back( tMetrics.CallerNS );
            }

            for (int iTest = 0; iTest < nTests; iTest++ )
            {
                // We need to pivot the data -- benchmark CallerNS results
                for (int iRun = 1; iRun < nRuns; iRun++ )
                {
                    Benchmark* pBenchmarkI = aRuns[ iRun ][ iTest ];
                    const double CallerNS = pBenchmarkI->Metrics.CallerNS;

                    if (worstCaller[ iTest ] < CallerNS)
                        worstCaller[ iTest ] = CallerNS;
                    if (bestCaller [ iTest ] > CallerNS)
                        bestCaller [ iTest ] = CallerNS;
                    totalCaller[ iTest ] += CallerNS;
                }
            }

            // Totals included both worst and best so subtract them
            for (int iTest = 0; iTest < nTests; iTest++)
            {
                totalCaller[ iTest ] -= worstCaller[ iTest ];
                totalCaller[ iTest ] -= bestCaller [ iTest ];

                double avg = totalCaller[ iTest ] / (nRuns - 2);
                averageCaller.push_back( avg );
            }

            // Copy average into last benchmark results
            for (int iTest = 0; iTest < nTests; iTest++)
            {
                Benchmark *pBenchmarkN = RegisteredBenchmarks[ iTest ];

                Benchmark *pLast = aRuns[ nRuns-1 ][ iTest ];
                pLast->Metrics.UpdateAverage( averageCaller[ iTest ], !iTest );

                // We need to copy the last run results into RegisteredBenchmarks
                // since they were reset when the run ended in preparation for the next run.
                pBenchmarkN->Metrics = pLast->Metrics;
            }
        }
    }

    static void Shutdown()
    {
        BestOf();
        Summary();
        RegisteredBenchmarks.clear();

        for (int iRun = 1; iRun < nRuns; iRun++ )
            aRuns[ iRun ].clear();
    }

    static Benchmark* Register(Benchmark* benchmark)
    {
        RegisteredBenchmarks.push_back( benchmark );
        return benchmark;
    }

    static void DoNotOptimize(void*p)
    {
        ResultNoOptimize = p;
    }
}

#define STRINGIFY(x)           #x
#define BENCH_ID               __LINE__
#define JOIN2(x,y)             x##y
#define JOIN3(x,y,z)           x##y##z
#define MAKE_FUNC_NAME3(x,y,z) JOIN3(x,y,z)
#define MAKE_FUNC_NAME(x)      MAKE_FUNC_NAME3(x,_Benchmark_,BENCH_ID)
#define BENCHMARK(FuncName)    static ::benchmark::Benchmark * MAKE_FUNC_NAME(FuncName) = ::benchmark::Register( new ::benchmark::Benchmark(FuncName, STRINGIFY(FuncName) ));

#else
    #include <benchmark/benchmark.h>
#endif
