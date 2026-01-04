# Introduction

One should NEVER blindly trust code, no matter how simple.

Years ago the extremely knowledgeable and awesome Andrei Alexandrescu presented this _beautiful_ optimized `digits10()` in his excellent presentation [Three Optimization Tips for C++](http://www.slideshare.net/andreialexandrescu1/three-optimization-tips-for-c-15708507) to calculate a `NumberDigitsBase10()` using powers-of-ten constants.

Unfortunately the slides have a bug!  Can you find it without using a debugger? (The first three lines and last lines are OK leaving 13 lines to inspect.)

```c++
uint32_t digits10_buggy(uint64_t v) {
    if (v < P01) return 1;
    if (v < P02) return 2;
    if (v < P03) return 3;
    if (v < P12) {
        if (v < P08) {
            if (v < P06) {
                if (v < P04) return 4;
                return 5 + (v < P05);
            }
            return 7 + (v >= P07);
        }
        if (v < P10) {
            return 9 + (v >= P09);
        }
        return 11 + (v >= P11);
    }
    return 12 + digits10_buggy(v / P12);
}

```

Somewhat ironically the original [Facebook blog](https://engineering.fb.com/2013/03/15/developer-tools/three-optimization-tips-for-c/) had the correct algorithm but mangled most of the code to be unreadable. DOH!

<details><summary>Once we fix `digits10()` ...</summary>

```
                return 5 + (v < P05);
```

</details>

```c++
uint32_t digits10( uint64_t v )
{
    if (v < P01) return 1;
    if (v < P02) return 2;
    if (v < P03) return 3;
    if (v < P12) {
        if (v < P08) {
            if (v < P06) {
                if (v < P04) return 4;
                return 5 + (v >= P05);
            }
            return 7 + (v >= P07);
        }
        if (v < P10) return 9 + (v >= P09);
        return 11 + (v >= P11);
    }
    return 12 + digits10_alexandrescu_v3(v / P12);
}
```

... we can then use it for an optimized `int` version:

```c++
int numdigits_alexandrescu_pohoreski( int n )
{
    const int64_t  i = n;
    const uint64_t x = abs(i);
    return (n < 0) + digits10( x );
}

```

# The problem

Q. How do we optimize `numdigits( int n )` for base 10?

A naïve implementation would be:

```c
int numdigits_dumb_sprintf_strlen( int n )
{
    char buffer[16];               // We can optimize away
    sprintf( buffer, "%d", n );    //     memset( buffer, 0, sizeof(buffer));
    return (int) strlen( buffer ); // since sprintf() will null terminate for us.
}
```

It works but it is S-L-O-W.

These StackOverflow questions ...

* [What's the best way to get the length of the decimal representation of an int in C++?](https://stackoverflow.com/questions/1696086/whats-the-best-way-to-get-the-length-of-the-decimal-representation-of-an-int-in/)
* [Efficient way to determine number of digits in an integer](https://stackoverflow.com/questions/1489830/efficient-way-to-determine-number-of-digits-in-an-integer)

... have all sorts of answers but there are _numerous_ problems. They ALL:

* FAIL to be the fastest,
* FAIL to provide proof, and
* FAIL to provide verification the implementation is correct.

Specifically,

* SOME only provided an uint64_t version so we are left to provide a native `int` version,
* SOME of them have bugs such as
  * not handling INT_MIN,
  * not handling all negative numbers,
  * not handling zero,
  * not handling INT_MAX, and
  * having compile-time errors such as typos.

# Results

I took the most popular answers, fixed them, wrote a verification utility and benchmark 35 (!) implementations to get actual hard data. Timings show average ns/call and which algorithms are faster compared Andrei's "base" version. -% faster means the implementation is slower (worse.)

| Algorithm                 |Apple M2           |%Faster |Ryzen 5600X        |%Faster |Threadripper 3960X |%Faster |
|:--------------------------|------------------:|-------:|------------------:|-------:|------------------:|-------:|
| alexandrescu_v1           |~ 9.360 avg ns/call|   0.00%|~ 9.780 avg ns/call|   0.00%|~11.536 avg ns/call|   0.00%|
| alexandrescu_pohoreski_v2 |~ 5.127 avg ns/call|  45.26%|~ 5.430 avg ns/call|  44.47%|~ 6.202 avg ns/call|  46.19%|
| alexandrescu_pohoreski_v3 |~ 1.375 avg ns/call|  85.32%|~ 2.562 avg ns/call|  73.80%|~ 2.864 avg ns/call|  75.14%|
| alink_buggy_v1            |~ 6.372 avg ns/call|  31.97%|~ 4.903 avg ns/call|  49.86%|~ 6.024 avg ns/call|  47.73%|
| alink_buggy_v2            |~ 9.545 avg ns/call|  -1.90%|~ 7.669 avg ns/call|  21.58%|~ 9.333 avg ns/call|  19.01%|
| alink_fixed_v1a           |~ 5.841 avg ns/call|  37.64%|~ 5.847 avg ns/call|  40.20%|~ 6.751 avg ns/call|  41.42%|
| alink_fixed_v1b           |~ 5.383 avg ns/call|  42.53%|~ 5.149 avg ns/call|  47.35%|~ 5.835 avg ns/call|  49.37%|
| alink_fixed_v2a           |~ 7.142 avg ns/call|  23.75%|~ 8.838 avg ns/call|  24.03%|~ 8.720 avg ns/call|  24.34%|
| alink_fixed_v2b           |~ 5.374 avg ns/call|  42.62%|~ 5.963 avg ns/call|  39.02%|~ 6.904 avg ns/call|  40.09%|
| clifford_buggy            |~ 0.816 avg ns/call|  91.29%|~ 5.563 avg ns/call|  43.11%|~ 6.014 avg ns/call|  47.82%|
| clifford_fixed            |~ 3.873 avg ns/call|  58.65%|~ 5.413 avg ns/call|  44.65%|~ 5.967 avg ns/call|  48.22%|
| dumb_sprintf_strlen       |~48.854 avg ns/call|-421.58%|~56.827 avg ns/call|-481.14%|~87.404 avg ns/call|-658.42%|
| gabriel_v1a               |~ 2.631 avg ns/call|  71.91%|~ 5.803 avg ns/call|  40.65%|~ 7.464 avg ns/call|  35.23%|
| gabriel_v1b               |~ 2.833 avg ns/call|  69.75%|~ 3.599 avg ns/call|  63.20%|~ 4.391 avg ns/call|  61.90%|
| gabriel_v1c               |~ 2.553 avg ns/call|  72.75%|~ 3.435 avg ns/call|  64.87%|~ 4.383 avg ns/call|  61.97%|
| if_naive                  |~ 5.793 avg ns/call|  38.15%|~ 6.677 avg ns/call|  31.71%|~ 7.480 avg ns/call|  35.10%|
| microsoft_itoa_strlen     |~10.803 avg ns/call| -15.34%|~28.799 avg ns/call|-194.51%|~51.214 avg ns/call|-344.40%|
| pohoreski_v1a             |~ 1.871 avg ns/call|  80.02%|~ 6.971 avg ns/call|  28.71%|~ 8.324 avg ns/call|  27.77%|
| pohoreski_v1b             |~ 2.297 avg ns/call|  75.48%|~ 4.700 avg ns/call|  51.94%|~ 5.327 avg ns/call|  53.78%|
| pohoreski_v2a             |~ 1.376 avg ns/call|  85.31%|~ 6.727 avg ns/call|  31.20%|~ 7.920 avg ns/call|  31.28%|
| pohoreski_v2b             |~ 2.294 avg ns/call|  75.51%|~ 4.332 avg ns/call|  55.70%|~ 5.004 avg ns/call|  56.58%|
| pohoreski_v3a             |~ 1.767 avg ns/call|  81.13%|~ 2.926 avg ns/call|  70.08%|~ 3.468 avg ns/call|  69.91%|
| pohoreski_v3b             |~ 1.767 avg ns/call|  81.14%|~ 2.940 avg ns/call|  69.94%|~ 3.489 avg ns/call|  69.73%|
| pohoreski_v4a             |~ 1.940 avg ns/call|  79.29%|~ 8.180 avg ns/call|  16.35%|~ 9.507 avg ns/call|  17.50%|
| pohoreski_v4b             |~ 1.766 avg ns/call|  81.15%|~ 5.498 avg ns/call|  43.77%|~ 6.631 avg ns/call|  42.46%|
| ransom                    |~ 4.990 avg ns/call|  46.72%|~ 8.521 avg ns/call|  12.86%|~10.001 avg ns/call|  13.22%|
| reference_int             |~ 4.945 avg ns/call|  47.20%|~ 4.767 avg ns/call|  51.25%|~ 5.707 avg ns/call|  50.48%|
| reference_mixed_int       |~ 4.941 avg ns/call|  47.25%|                TBD|    TBD |~ 5.609 avg ns/call|  51.33%|
| simple                    |~ 0.815 avg ns/call|  91.30%|~ 7.945 avg ns/call|  18.75%|~ 9.412 avg ns/call|  18.33%|
| simple_reverse            |~ 0.817 avg ns/call|  91.28%|               TBD |    TBD |~10.207 avg ns/call|  11.43%|
| thomas                    |~11.755 avg ns/call| -25.50%|~12.819 avg ns/call| -31.10%|~14.819 avg ns/call| -28.59%|
| user42690_buggy           |~10.945 avg ns/call| -16.85%|~12.523 avg ns/call| -28.06%|~14.361 avg ns/call| -24.62%|
| user42690_fixed_a         |~13.394 avg ns/call| -43.00%|~14.846 avg ns/call| -51.82%|~16.945 avg ns/call| -47.04%|
| user42690_fixed_b         |~ 9.385 avg ns/call|  -0.20%|~10.937 avg ns/call| -11.85%|~12.351 avg ns/call|  -7.17%|
| vitali                    |~ 5.103 avg ns/call|  45.52%|~ 5.185 avg ns/call|  46.97%|~ 6.103 avg ns/call|  47.04%|

**NOTES:**

 * Accurate to roughly +/- 1.0 ns/call.
 * 5 runs, discard best case, worst case, average remaining 3 timings.

Sorted by performance from best to worst:

## Windows 10, AMD Threadripper 3960X

Here we see the weakness of the early Threadripper generations -- poor single-threaded performance, aka _inherently sequential._ Their real strength has always been algorithms that multi-thread nicely, aka _embarrassingly parallel_ implementations.

|Rank| Algorithm                  |Threadripper 3960X |%Faster |
|---:|:---------------------------|------------------:|-------:|
|  1 | alexandrescu_pohoreski_v3  |~ 2.864 avg ns/call|  75.14%|
|  2 | pohoreski_v3a              |~ 3.468 avg ns/call|  69.91%|
|  3 | pohoreski_v3b              |~ 3.489 avg ns/call|  69.73%|
|  4 | gabriel_v1c                |~ 4.383 avg ns/call|  61.97%|
|  5 | gabriel_v1b                |~ 4.391 avg ns/call|  61.90%|
|  6 | pohoreski_v2b              |~ 5.004 avg ns/call|  56.58%|
|  7 | pohoreski_v1b              |~ 5.327 avg ns/call|  53.78%|
|  8 | reference_mixed_int        |~ 5.609 avg ns/call|  51.33%|
|  9 | reference_int              |~ 5.707 avg ns/call|  50.48%|
| 10 | alink_fixed_v1b            |~ 5.835 avg ns/call|  49.37%|
| 11 | clifford_fixed             |~ 5.967 avg ns/call|  48.22%|
| 12 | clifford_buggy             |~ 6.014 avg ns/call|  47.82%|
| 13 | alink_buggy_v1             |~ 6.024 avg ns/call|  47.73%|
| 14 | vitali                     |~ 6.103 avg ns/call|  47.04%|
| 15 | alexandrescu_pohoreski_v2  |~ 6.202 avg ns/call|  46.19%|
| 16 | pohoreski_v4b              |~ 6.631 avg ns/call|  42.46%|
| 17 | alink_fixed_v1a            |~ 6.751 avg ns/call|  41.42%|
| 18 | alink_fixed_v2b            |~ 6.904 avg ns/call|  40.09%|
| 19 | gabriel_v1a                |~ 7.464 avg ns/call|  35.23%|
| 20 | if_naive                   |~ 7.480 avg ns/call|  35.10%|
| 21 | pohoreski_v2a              |~ 7.920 avg ns/call|  31.28%|
| 22 | pohoreski_v1a              |~ 8.324 avg ns/call|  27.77%|
| 23 | alink_fixed_v2a            |~ 8.720 avg ns/call|  24.34%|
| 24 | alink_buggy_v2             |~ 9.333 avg ns/call|  19.01%|
| 25 | simple                     |~ 9.412 avg ns/call|  18.33%|
| 26 | pohoreski_v4a              |~ 9.507 avg ns/call|  17.50%|
| 27 | ransom                     |~10.001 avg ns/call|  13.22%|
| 28 | simple_reverse             |~10.207 avg ns/call|  11.43%|
| 29 | alexandrescu_v1            |~11.536 avg ns/call|   0.00%|
| 30 | user42690_fixed_b          |~12.351 avg ns/call|  -7.17%|
| 31 | user42690_buggy            |~14.361 avg ns/call| -24.62%|
| 32 | thomas                     |~14.819 avg ns/call| -28.59%|
| 33 | user42690_fixed_a          |~16.945 avg ns/call| -47.04%|
| 34 | microsoft_itoa_strlen      |~51.214 avg ns/call|-344.40%|
| 35 | dumb_sprintf_strlen        |~87.404 avg ns/call|-658.42%|

## Windows 10, AMD Ryzen 5600X

Testing 3rd generation Ryzen we see minor disruptions in the middle but nothing too earth shattering.

/!\ **INFO.** The latest 2 tests are missing. Will update later.

|Rank| Algorithm                  |Ryzen 5600X        |%Faster |
|---:|---------------------------:|------------------:|-------:|
|  1 | alexandrescu_pohoreski_v3  |~ 2.562 avg ns/call|  73.80%|
|  2 | pohoreski_v3a              |~ 2.926 avg ns/call|  70.08%|
|  3 | pohoreski_v3b              |~ 2.940 avg ns/call|  69.94%|
|  4 | gabriel_v1c                |~ 3.435 avg ns/call|  64.87%|
|  5 | gabriel_v1b                |~ 3.599 avg ns/call|  63.20%|
|  6 | pohoreski_v2b              |~ 4.332 avg ns/call|  55.70%|
|  7 | pohoreski_v1b              |~ 4.700 avg ns/call|  51.94%|
|  8 | reference_int              |~ 4.767 avg ns/call|  51.25%|
|  9 | alink_buggy_v1             |~ 4.903 avg ns/call|  49.86%|
| 10 | alink_fixed_v1b            |~ 5.149 avg ns/call|  47.35%|
| 11 | vitali                     |~ 5.185 avg ns/call|  46.97%|
| 12 | clifford_fixed             |~ 5.413 avg ns/call|  44.65%|
| 13 | alexandrescu_pohoreski_v2  |~ 5.430 avg ns/call|  44.47%|
| 14 | pohoreski_v4b              |~ 5.498 avg ns/call|  43.77%|
| 15 | clifford_buggy             |~ 5.563 avg ns/call|  43.11%|
| 16 | gabriel_v1a                |~ 5.803 avg ns/call|  40.65%|
| 17 | alink_fixed_v1a            |~ 5.847 avg ns/call|  40.20%|
| 18 | alink_fixed_v2b            |~ 5.963 avg ns/call|  39.02%|
| 19 | if_naive                   |~ 6.677 avg ns/call|  31.71%|
| 20 | pohoreski_v2a              |~ 6.727 avg ns/call|  31.20%|
| 21 | pohoreski_v1a              |~ 6.971 avg ns/call|  28.71%|
| 22 | alink_fixed_v2a            |~ 7.392 avg ns/call|  24.41%|
| 23 | alink_buggy_v2             |~ 7.669 avg ns/call|  21.58%|
| 24 | simple                     |~ 7.945 avg ns/call|  18.75%|
| 25 | pohoreski_v4a              |~ 8.180 avg ns/call|  16.35%|
| 26 | ransom                     |~ 8.521 avg ns/call|  12.86%|
| 27 | alexandrescu_v1            |~ 9.780 avg ns/call|   0.00%|
| 28 | user42690_fixed_b          |~10.937 avg ns/call| -11.85%|
| 29 | user42690_buggy            |~12.523 avg ns/call| -28.06%|
| 30 | thomas                     |~12.819 avg ns/call| -31.10%|
| 31 | user42690_fixed_a          |~14.846 avg ns/call| -51.82%|
| 32 | microsoft_itoa_strlen      |~28.799 avg ns/call|-194.51%|
| 33 | dumb_sprintf_strlen        |~56.827 avg ns/call|-481.14%|

## macOS 13.7.8, Apple M2 Max

Apple's M2 Max performance is _very_ interesting to see!

|Rank| Algorithm                  |Apple M2 Max       |%Faster |
|---:|---------------------------:|------------------:|-------:|
|  1 | simple                     |~ 0.815 avg ns/call|  91.30%|
|  2 | clifford_buggy             |~ 0.816 avg ns/call|  91.29%|
|  3 | simple_reverse             |~ 0.817 avg ns/call|  91.28%|
|  4 | alexandrescu_pohoreski_v3  |~ 1.375 avg ns/call|  85.32%|
|  5 | pohoreski_v2a              |~ 1.376 avg ns/call|  85.31%|
|  6 | pohoreski_v3a              |~ 1.767 avg ns/call|  81.13%|
|  7 | pohoreski_v4b              |~ 1.766 avg ns/call|  81.15%|
|  8 | pohoreski_v3b              |~ 1.767 avg ns/call|  81.14%|
|  9 | pohoreski_v1a              |~ 1.871 avg ns/call|  80.02%|
| 10 | pohoreski_v4a              |~ 1.940 avg ns/call|  79.29%|
| 11 | pohoreski_v2b              |~ 2.294 avg ns/call|  75.51%|
| 12 | pohoreski_v1b              |~ 2.297 avg ns/call|  75.48%|
| 13 | gabriel_v1c                |~ 2.553 avg ns/call|  72.75%|
| 14 | gabriel_v1a                |~ 2.631 avg ns/call|  71.91%|
| 15 | gabriel_v1b                |~ 2.833 avg ns/call|  69.75%|
| 16 | clifford_fixed             |~ 3.873 avg ns/call|  58.65%|
| 17 | reference_mixed_int        |~ 4.941 avg ns/call|  47.25%|
| 18 | reference_int              |~ 4.945 avg ns/call|  47.20%|
| 19 | ransom                     |~ 4.990 avg ns/call|  46.72%|
| 20 | vitali                     |~ 5.103 avg ns/call|  45.52%|
| 21 | alexandrescu_pohoreski_v2  |~ 5.127 avg ns/call|  45.26%|
| 22 | alink_fixed_v2b            |~ 5.374 avg ns/call|  42.62%|
| 23 | alink_fixed_v1b            |~ 5.383 avg ns/call|  42.53%|
| 24 | if_naive                   |~ 5.793 avg ns/call|  38.15%|
| 25 | alink_fixed_v1a            |~ 5.841 avg ns/call|  37.64%|
| 26 | alink_buggy_v1             |~ 6.372 avg ns/call|  31.97%|
| 27 | alink_fixed_v2a            |~ 7.142 avg ns/call|  23.75%|
| 28 | alexandrescu_v1            |~ 9.360 avg ns/call|   0.00%|
| 29 | user42690_fixed_b          |~ 9.385 avg ns/call|  -0.20%|
| 30 | alink_buggy_v2             |~ 9.545 avg ns/call|  -1.90%|
| 31 | microsoft_itoa_strlen      |~10.803 avg ns/call| -15.34%|
| 32 | user42690_buggy            |~10.945 avg ns/call| -16.85%|
| 33 | thomas                     |~11.755 avg ns/call| -25.50%|
| 34 | user42690_fixed_a          |~13.394 avg ns/call| -43.00%|
| 35 | dumb_sprintf_strlen        |~48.854 avg ns/call|-421.58%|

Fixing the broken implementation of `clifford` takes a huge performance hit. This is WHY it is important to verify your algorithm _works correctly!_

# Implementations

On Windows, this `reference_int` implementation beats almost all but one of the SO answers:

```c++
// Reference int
int numdigits10_int( int n )
{
    if (n <=-1'000'000'000ll) return 11; // -2'147'483'648 .. -1'000'000'000  -2^31
    if (n <=  -100'000'000ll) return 10; //   -999'999'999 ..   -100'000'000
    if (n <=   -10'000'000ll) return  9; //    -99'999'999 ..    -10'000'000
    if (n <=    -1'000'000ll) return  8; //     -9'999'999 ..     -1'000'000
    if (n <=      -100'000ll) return  7; //       -999'999 ..       -100'000
    if (n <=       -10'000ll) return  6; //        -99'999 ..        -10'000
    if (n <=        -1'000ll) return  5; //         -9'999 ..         -1'000
    if (n <=          -100ll) return  4; //           -999 ..           -100
    if (n <=           -10ll) return  3; //            -99 ..            -10
    if (n <              0ll) return  2; //             -9 ..             -1
    if (n <             10ll) return  1; //              0 ..              9
    if (n <            100ll) return  2; //             10 ..             99
    if (n <          1'000ll) return  3; //            100 ..           '999
    if (n <         10'000ll) return  4; //          1'000 ..          9'999
    if (n <        100'000ll) return  5; //         10'000 ..         99'999
    if (n <      1'000'000ll) return  6; //        100'000 ..        999'999
    if (n <     10'000'000ll) return  7; //      1'000'000 ..      9'999'999
    if (n <    100'000'000ll) return  8; //     10'000'000 ..     99'999'999
    if (n <  1'000'000'000ll) return  9; //    100'000'000 ..    999'999'999
    /*else<= 2'147'483'647 */ return 10; //  1'000'000'000 ..  2'147'483'647   2^31 - 1
}
```

On macOS the simple version beats everything using the reference uint64_t.
```c++

// Reference uint64_t
uint32_t numdigits10_uint64( uint64_t n )
{
    if (n <                         10ull) return  1;
    if (n <                        100ull) return  2;
    if (n <                      1'000ull) return  3;
    if (n <                     10'000ull) return  4;
    if (n <                    100'000ull) return  5;
    if (n <                  1'000'000ull) return  6;
    if (n <                 10'000'000ull) return  7;
    if (n <                100'000'000ull) return  8;
    if (n <              1'000'000'000ull) return  9;
    if (n <             10'000'000'000ull) return 10;
    if (n <            100'000'000'000ull) return 11;
    if (n <          1'000'000'000'000ull) return 12;
    if (n <         10'000'000'000'000ull) return 13;
    if (n <        100'000'000'000'000ull) return 14;
    if (n <      1'000'000'000'000'000ull) return 15;
    if (n <     10'000'000'000'000'000ull) return 16;
    if (n <    100'000'000'000'000'000ull) return 17;
    if (n <  1'000'000'000'000'000'000ull) return 18;
    if (n < 10'000'000'000'000'000'000ull) return 19;
    /*else<=18'446'744'073'709'551'615*/   return 20; // 2^64 - 1
}

// Simple version
int numdigits_simple_int64(int64_t n)
{
    if (n == INT64_MIN) return 20; // handle edge case of negating int min
    if (n < 0) return 1 + numdigits10_uint64( (uint64_t) -n );
    else       return     numdigits10_uint64( (uint64_t)  n );
}

int numdigits_simple(int n) {
    return numdigits_simple_int64( (int64_t) n );
}
```

Andrei never provided an `int` version so I experimented with various "wrappers".

```c++
    const uint32_t x = (n < 0) ? (uint64_t)-n & 0xFFFFFFFFU : n;
    return (n< < 0) + numdigits( x );

    const int64_t  i = n;
    const uint64_t x = abs(i);
    return (n < 0) + numdigits( x );
```
It turns out the last variation proved to be the fastest.

# Verification

We need to verify each implementation passes:

* int min,
* int max,
* zero
* powers-of-2
* powers-of-10
* nines

Using a data-driven approach makes this trivial to extend tests.  I used these manual tables to verify that each implementation worked correctly (since I couldn't trust ANY implementation) when I discovered a bug in the slide version of Andrei's `digits10()` as discussed in the [Introduction](#introduction).

```c++
    struct IntKeyVal
    {
        int number;
        int expect;
    };

    const IntKeyVal TEST_INT_MINMAX[] = {
        { -2147483647, 11 }, // [0] - 2^31 + 1  INT_MIN+1. See TEST_POW2 for INT_MIN test
        {           0,  1 }, // [1] We include zero since it doesn't really fit in other tables
        { +2147483647, 10 }  // [2] + 2^31 - 1  INT_MAX
    };
    const int NUM_INT_MINMAX = sizeof(TEST_INT_MINMAX) / sizeof(TEST_INT_MINMAX[0]);

    const IntKeyVal TEST_INT_NINES[] = {
    //  { -9999999999, 11 }, // [--] overflows int
    //  { -2147483648, 11 }, // [--] - 2^31 INT_MIN
        {  -999999999, 10 }, // [ 0]
        {   -99999999,  9 }, // [ 1]
        {    -9999999,  8 }, // [ 2]
        {     -999999,  7 }, // [ 3]
        {      -99999,  6 }, // [ 4]
        {       -9999,  5 }, // [ 5]
        {        -999,  4 }, // [ 6]
        {         -99,  3 }, // [ 7]
        {          -9,  2 }, // [ 8]
        {          +9,  1 }, // [ 9]
        {         +99,  2 }, // [10]
        {        +999,  3 }, // [11]
        {       +9999,  4 }, // [12]
        {      +99999,  5 }, // [13]
        {     +999999,  6 }, // [14]
        {    +9999999,  7 }, // [15]
        {   +99999999,  8 }, // [16]
        {  +999999999,  9 }  // [17]
    //  { +2147483647, 10 }  // [--] 2^31 - 1
    //  { +9999999999, 10 }  // [--] overflows int
    };
    const int NUM_INT_NINES = sizeof(TEST_INT_NINES) / sizeof(TEST_INT_NINES[0]);

    const IntKeyVal TEST_INT_POW2[] = {
        { -2147483647-1,11}, // [ 0] - 2^31 INT_MIN
        { -1073741824, 11 }, // [ 1] - 2^30
        {  -536870912, 10 }, // [ 2] - 2^29
        {  -268435456, 10 }, // [ 3] - 2^28
        {  -134217728, 10 }, // [ 4] - 2^27
        {   -67108864,  9 }, // [ 5] - 2^26
        {   -33554432,  9 }, // [ 6] - 2^25
        {   -16777216,  9 }, // [ 7] - 2^24
        {    -8388608,  8 }, // [ 8] - 2^23
        {    -4194304,  8 }, // [ 9] - 2^22
        {    -2097152,  8 }, // [10] - 2^21
        {    -1048576,  8 }, // [11] - 2^20
        {     -524288,  7 }, // [12] - 2^19
        {     -262144,  7 }, // [13] - 2^18
        {     -131072,  7 }, // [14] - 2^17
        {      -65536,  6 }, // [15] - 2^16
        {      -32768,  6 }, // [16] - 2^15
        {      -16384,  6 }, // [17] - 2^14
        {       -8192,  5 }, // [18] - 2^13
        {       -4096,  5 }, // [19] - 2^12
        {       -2048,  5 }, // [20] - 2^11
        {       -1024,  5 }, // [21] - 2^10
        {        -512,  4 }, // [22] - 2^9
        {        -256,  4 }, // [23] - 2^8
        {        -128,  4 }, // [24] - 2^7
        {         -64,  3 }, // [25] - 2^6
        {         -32,  3 }, // [26] - 2^5
        {         -16,  3 }, // [27] - 2^4
        {          -8,  2 }, // [28] - 2^3
        {          -4,  2 }, // [29] - 2^2
        {          -2,  2 }, // [30] - 2^1
        {          -1,  2 }, // [31] - 2^0
        {          +1,  1 }, // [32] + 2^0
        {          +2,  1 }, // [33] + 2^1
        {          +4,  1 }, // [34] + 2^2
        {          +8,  1 }, // [35] + 2^3
        {         +16,  2 }, // [36] + 2^4
        {         +32,  2 }, // [37] + 2^5
        {         +64,  2 }, // [38] + 2^6
        {        +128,  3 }, // [39] + 2^7
        {        +256,  3 }, // [40] + 2^8
        {        +512,  3 }, // [41] + 2^9
        {       +1024,  4 }, // [42] + 2^10
        {       +2048,  4 }, // [43] + 2^11
        {       +4096,  4 }, // [44] + 2^12
        {       +8192,  4 }, // [45] + 2^13
        {      +16384,  5 }, // [46] + 2^14
        {      +32768,  5 }, // [47] + 2^15
        {      +65536,  5 }, // [48] + 2^16
        {     +131072,  6 }, // [49] + 2^17
        {     +262144,  6 }, // [50] + 2^18
        {     +524288,  6 }, // [51] + 2^19
        {    +1048576,  7 }, // [52] + 2^20
        {    +2097152,  7 }, // [53] + 2^21
        {    +4194304,  7 }, // [54] + 2^22
        {    +8388608,  7 }, // [55] + 2^23
        {   +16777216,  8 }, // [56] + 2^24
        {   +33554432,  8 }, // [57] + 2^25
        {   +67108864,  8 }, // [58] + 2^26
        {  +134217728,  9 }, // [59] + 2^27
        {  +268435456,  9 }, // [60] + 2^28
        {  +536870912,  9 }, // [61] + 2^29
        { +1073741824, 10 }  // [62] + 2^30
    //  { +2147483648  10 }  // [--] + 2^31 overflows int
    };
    const int NUM_INT_POW2 = sizeof(TEST_INT_POW2) / sizeof(TEST_INT_POW2[0]);

    const IntKeyVal TEST_INT_POW10[] = {
    //  { -2147483648, 11 }, // [--] - 2^31
        { -1000000000, 11 }, // [ 0]
        {  -100000000, 10 }, // [ 1]
        {   -10000000,  9 }, // [ 2]
        {    -1000000,  8 }, // [ 3]
        {     -100000,  7 }, // [ 4]
        {      -10000,  6 }, // [ 5]
        {       -1000,  5 }, // [ 6]
        {        -100,  4 }, // [ 7]
        {         -10,  3 }, // [ 8]
        {          -1,  2 }, // [ 9]
        {          +1,  1 }, // [10]
        {         +10,  2 }, // [11]
        {        +100,  3 }, // [12]
        {       +1000,  4 }, // [13]
        {      +10000,  5 }, // [14]
        {     +100000,  6 }, // [15]
        {    +1000000,  7 }, // [16]
        {   +10000000,  8 }, // [17]
        {  +100000000,  9 }, // [18]
        { +1000000000, 10 }, // [19]
    //  { +2147483647, 10 }, // [--] + 2^31 - 1
    };
    const int NUM_INT_POW10 = sizeof(TEST_INT_POW10) / sizeof(TEST_INT_POW10[0]);
```
These tests allowed me discover a **minor typo bug** in the slide version of Andrei's `digits10()` as discussed in the [Introduction](#introduction). (See `test_original_digits10()` in verify_numdigits.cpp)


```
                   0:  1 ==  1 pass
                   1:  1 ==  1 pass
                   9:  1 ==  1 pass
                  10:  2 ==  2 pass
                  99:  2 ==  2 pass
                 100:  3 ==  3 pass
                 999:  3 ==  3 pass
                1000:  4 ==  4 pass
                9999:  4 ==  4 pass
               10000:  6 ==  5 FAIL in original digits10()!
               99999:  5 ==  5 pass
              100000:  5 ==  6 FAIL in original digits10()!
              999999:  6 ==  6 pass
             1000000:  7 ==  7 pass
             9999999:  7 ==  7 pass
            10000000:  8 ==  8 pass
            99999999:  8 ==  8 pass
           100000000:  9 ==  9 pass
           999999999:  9 ==  9 pass
          1000000000: 10 == 10 pass
          9999999999: 10 == 10 pass
         10000000000: 11 == 11 pass
         99999999999: 11 == 11 pass
        100000000000: 12 == 12 pass
        999999999999: 12 == 12 pass
       1000000000000: 13 == 13 pass
       9999999999999: 13 == 13 pass
      10000000000000: 14 == 14 pass
      99999999999999: 14 == 14 pass
     100000000000000: 15 == 15 pass
     999999999999999: 15 == 15 pass
    1000000000000000: 16 == 16 pass
    9999999999999999: 16 == 16 pass
   10000000000000000: 18 == 17 FAIL in original digits10()!
   99999999999999999: 17 == 17 pass
  100000000000000000: 17 == 18 FAIL in original digits10()!
  999999999999999999: 18 == 18 pass
 1000000000000000000: 19 == 19 pass
 9999999999999999999: 19 == 19 pass
10000000000000000000: 20 == 20 pass
```


# Compiling

## Windows

Use MSVC 2022 and open `.\build\numdigits_benchmark.sln`

## macOS

Use the supplied makefile

```bash
make -f Makefile.osx
```

or compile manually:

```bash
g++ -stdc=c++17 -O2 -Wall -Wextra -Iinc src/numdigits_benchmark.cpp -o bin/numdigits_benchmark
g++ -stdc=c++17 -O2 -Wall -Wextra -Iinc src/verify_numdigits.cpp    -o bin/verify_numdigits
```

## Linux

Forthcoming.

# Running and Benchmarking

To run a single test:

```cmd
.\bin\numdigits_benchmark.exe
```

To run a "Best of 5" discarding the best case, worst case, and calculating the average time use a command-line argument `5`.

On Windows:

```cmd
.\bin\numdigits_benchmark.exe -markdown 5
```

or on macOS / Linux:

```bash
./bin/numdigits_benchmark -markdown 5
```

# Benchmark

Included is a tiny (~400 Lines of Code) single header-only mini-benchmark replacement for Google's [benchmark](https://github.com/google/benchmark).

# Further Research

* Use different sample size initalization patterns. (Currently an uniform distribution across uint32_t is used.)
  * [x] random, uniform distribution
  * [ ] increasing:
  * [ ] decreasing
  * [ ] pipe organ
  * [ ] reverse pipe organ
* Benchmarking other implementations.
* Possible to use SIMD to test 4 comparisons at a time?

# Conclusion

1. Don't ASSUME.
2. PROFILE.
3. VERIFY individual tests.
4. COMPARE with other implementations.

Is this THE fastest `NumDigitsBase10()`? Who knows but if we can optimize something as "trivial" as this what _other_ performance gains are we leaving on the table? You will be either happy or depressed at this. :-)

---

This README is Copyleft {C} 2025 Michaelangel007.

Last updated Dec. 23, 2025.
