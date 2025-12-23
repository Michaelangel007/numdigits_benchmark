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

I took the most popular answers, fixed them, wrote a verification utility and benchmark 33 implementations to get actual hard data. Timings show average ns/call and which algorithms are faster compared Andrei's "base" version.

| Algorithm                 |Apple M2           |%Faster |Ryzen 5600X        |%Faster |Threadripper 3960X |%Faster |
|:--------------------------|------------------:|-------:|------------------:|-------:|------------------:|-------:|
| alexandrescu_v1           |~ 9.332 avg ns/call|   0.00%|~ 9.780 avg ns/call|   0.00%|~11.678 avg ns/call|   0.00%|
| alexandrescu_pohoreski_v2 |~ 4.976 avg ns/call|  46.66%|~ 5.430 avg ns/call|  44.47%|~ 6.310 avg ns/call|  45.76%|
| alexandrescu_pohoreski_v3 |~ 1.371 avg ns/call|  85.30%|~ 2.562 avg ns/call|  73.80%|~ 2.895 avg ns/call|  75.11%|
| alink_buggy_v1            |~ 6.403 avg ns/call|  31.35%|~ 4.903 avg ns/call|  49.86%|~ 6.094 avg ns/call|  47.62%|
| alink_buggy_v2            |~ 9.539 avg ns/call|  -2.26%|~ 7.669 avg ns/call|  21.58%|~ 9.446 avg ns/call|  18.81%|
| alink_fixed_v1a           |~ 5.848 avg ns/call|  37.31%|~ 5.847 avg ns/call|  40.20%|~ 6.822 avg ns/call|  41.36%|
| alink_fixed_v1b           |~ 5.300 avg ns/call|  43.19%|~ 5.149 avg ns/call|  47.35%|~ 5.873 avg ns/call|  49.52%|
| alink_fixed_v2a           |~ 7.152 avg ns/call|  23.33%|~ 8.838 avg ns/call|  24.03%|~ 8.838 avg ns/call|  24.03%|
| alink_fixed_v2b           |~ 5.390 avg ns/call|  42.22%|~ 5.963 avg ns/call|  39.02%|~ 6.970 avg ns/call|  40.09%|
| clifford_buggy            |~ 0.813 avg ns/call|  91.28%|~ 5.563 avg ns/call|  43.11%|~ 6.057 avg ns/call|  47.93%|
| clifford_fixed            |~ 3.904 avg ns/call|  58.15%|~ 5.413 avg ns/call|  44.65%|~ 6.001 avg ns/call|  48.42%|
| dumb_sprintf_strlen       |~49.636 avg ns/call|-432.12%|~56.827 avg ns/call|-481.14%|~89.763 avg ns/call|-671.61%|
| gabriel_v1a               |~ 2.625 avg ns/call|  71.86%|~ 5.803 avg ns/call|  40.65%|~ 7.487 avg ns/call|  35.64%|
| gabriel_v1b               |~ 2.825 avg ns/call|  69.71%|~ 3.599 avg ns/call|  63.20%|~ 4.398 avg ns/call|  62.20%|
| gabriel_v1c               |~ 2.550 avg ns/call|  72.67%|~ 3.435 avg ns/call|  64.87%|~ 4.396 avg ns/call|  62.22%|
| if_naive                  |~ 5.852 avg ns/call|  37.26%|~ 6.677 avg ns/call|  31.71%|~ 7.516 avg ns/call|  35.39%|
| microsoft_itoa_strlen     |~10.740 avg ns/call| -15.14%|~28.799 avg ns/call|-194.51%|~51.631 avg ns/call|-343.82%|
| pohoreski_v1a             |~ 1.869 avg ns/call|  79.97%|~ 6.971 avg ns/call|  28.71%|~ 8.458 avg ns/call|  27.30%|
| pohoreski_v1b             |~ 2.293 avg ns/call|  75.42%|~ 4.700 avg ns/call|  51.94%|~ 5.379 avg ns/call|  53.76%|
| pohoreski_v2a             |~ 1.448 avg ns/call|  84.48%|~ 6.727 avg ns/call|  31.20%|~ 8.018 avg ns/call|  31.08%|
| pohoreski_v2b             |~ 2.293 avg ns/call|  75.41%|~ 4.332 avg ns/call|  55.70%|~ 5.056 avg ns/call|  56.54%|
| pohoreski_v3a             |~ 1.765 avg ns/call|  81.08%|~ 2.926 avg ns/call|  70.08%|~ 3.506 avg ns/call|  69.86%|
| pohoreski_v3b             |~ 1.764 avg ns/call|  81.09%|~ 2.940 avg ns/call|  69.94%|~ 3.518 avg ns/call|  69.76%|
| pohoreski_v4a             |~ 1.944 avg ns/call|  79.16%|~ 8.180 avg ns/call|  16.35%|~ 9.756 avg ns/call|  16.14%|
| pohoreski_v4b             |~ 1.765 avg ns/call|  81.08%|~ 5.498 avg ns/call|  43.77%|~ 6.833 avg ns/call|  41.27%|
| ransom                    |~ 4.960 avg ns/call|  46.83%|~ 8.521 avg ns/call|  12.86%|~10.065 avg ns/call|  13.48%|
| reference_int             |~ 4.940 avg ns/call|  47.04%|~ 4.767 avg ns/call|  51.25%|~ 5.484 avg ns/call|  52.86%|
| simple                    |~ 0.813 avg ns/call|  91.28%|~ 7.945 avg ns/call|  18.75%|~ 9.406 avg ns/call|  19.14%|
| thomas                    |~11.763 avg ns/call| -26.11%|~12.819 avg ns/call| -31.10%|~14.860 avg ns/call| -27.74%|
| user42690_buggy           |~10.987 avg ns/call| -17.78%|~12.523 avg ns/call| -28.06%|~14.429 avg ns/call| -24.03%|
| user42690_fixed_a         |~13.408 avg ns/call| -43.74%|~14.846 avg ns/call| -51.82%|~17.043 avg ns/call| -46.50%|
| user42690_fixed_b         |~ 9.358 avg ns/call|  -0.32%|~10.937 avg ns/call| -11.85%|~12.481 avg ns/call|  -7.29%|
| vitali                    |~ 9.358 avg ns/call|  -0.32%|~ 5.185 avg ns/call|  46.97%|~ 6.153 avg ns/call|  47.11%|

**NOTES:**

 * Accurate to roughly +/- 1.0 ns/call.
 * 5 runs, discard best case, worst case, average remaining 3 timings.

Sorted by performance from best to worst:

## Windows 10, AMD Threadripper 3960X

|Rank| Algorithm                  |Threadripper 3960X |%Faster |
|---:|:---------------------------|------------------:|-------:|
|  1 | alexandrescu_pohoreski_v3  |~ 2.895 avg ns/call|  75.11%|
|  2 | pohoreski_v3a              |~ 3.506 avg ns/call|  69.86%|
|  3 | pohoreski_v3b              |~ 3.518 avg ns/call|  69.76%|
|  4 | gabriel_v1c                |~ 4.396 avg ns/call|  62.22%|
|  5 | gabriel_v1b                |~ 4.398 avg ns/call|  62.20%|
|  6 | pohoreski_v2b              |~ 5.056 avg ns/call|  56.54%|
|  7 | pohoreski_v1b              |~ 5.379 avg ns/call|  53.76%|
|  8 | reference_int              |~ 5.484 avg ns/call|  52.86%|
|  9 | alink_fixed_v1b            |~ 5.873 avg ns/call|  49.52%|
| 10 | clifford_fixed             |~ 6.001 avg ns/call|  48.42%|
| 11 | clifford_buggy             |~ 6.057 avg ns/call|  47.93%|
| 12 | alink_buggy_v1             |~ 6.094 avg ns/call|  47.62%|
| 13 | vitali                     |~ 6.153 avg ns/call|  47.11%|
| 14 | alexandrescu_pohoreski_v2  |~ 6.310 avg ns/call|  45.76%|
| 15 | alink_fixed_v1a            |~ 6.822 avg ns/call|  41.36%|
| 16 | pohoreski_v4b              |~ 6.833 avg ns/call|  41.27%|
| 17 | alink_fixed_v2b            |~ 6.970 avg ns/call|  40.09%|
| 18 | gabriel_v1a                |~ 7.487 avg ns/call|  35.64%|
| 19 | if_naive                   |~ 7.516 avg ns/call|  35.39%|
| 20 | pohoreski_v2a              |~ 8.018 avg ns/call|  31.08%|
| 21 | pohoreski_v1a              |~ 8.458 avg ns/call|  27.30%|
| 22 | alink_fixed_v2a            |~ 8.838 avg ns/call|  24.03%|
| 23 | simple                     |~ 9.406 avg ns/call|  19.14%|
| 24 | alink_buggy_v2             |~ 9.446 avg ns/call|  18.81%|
| 25 | pohoreski_v4a              |~ 9.756 avg ns/call|  16.14%|
| 26 | ransom                     |~10.065 avg ns/call|  13.48%|
| 27 | alexandrescu_v1            |~11.678 avg ns/call|   0.00%|
| 28 | user42690_fixed_b          |~12.481 avg ns/call|  -7.29%|
| 29 | user42690_buggy            |~14.429 avg ns/call| -24.03%|
| 30 | thomas                     |~14.860 avg ns/call| -27.74%|
| 31 | user42690_fixed_a          |~17.043 avg ns/call| -46.50%|
| 32 | microsoft_itoa_strlen      |~51.631 avg ns/call|-343.82%|
| 33 | dumb_sprintf_strlen        |~89.763 avg ns/call|-671.61%|

## Windows 10, AMD Ryzen 5600X

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
|  1 | simple                     |~ 0.813 avg ns/call|  91.28%|
|  2 | clifford_buggy             |~ 0.813 avg ns/call|  91.28%|
|  3 | alexandrescu_pohoreski_v3  |~ 1.371 avg ns/call|  85.30%|
|  4 | pohoreski_v2a              |~ 1.448 avg ns/call|  84.48%|
|  5 | pohoreski_v3b              |~ 1.764 avg ns/call|  81.09%|
|  6 | pohoreski_v4b              |~ 1.765 avg ns/call|  81.08%|
|  7 | pohoreski_v3a              |~ 1.765 avg ns/call|  81.08%|
|  8 | pohoreski_v1a              |~ 1.869 avg ns/call|  79.97%|
|  9 | pohoreski_v4a              |~ 1.944 avg ns/call|  79.16%|
| 10 | pohoreski_v2b              |~ 2.293 avg ns/call|  75.41%|
| 11 | pohoreski_v1b              |~ 2.293 avg ns/call|  75.42%|
| 12 | gabriel_v1c                |~ 2.550 avg ns/call|  72.67%|
| 13 | gabriel_v1a                |~ 2.625 avg ns/call|  71.86%|
| 14 | gabriel_v1b                |~ 2.825 avg ns/call|  69.71%|
| 15 | clifford_fixed             |~ 3.904 avg ns/call|  58.15%|
| 16 | reference_int              |~ 4.940 avg ns/call|  47.04%|
| 17 | ransom                     |~ 4.960 avg ns/call|  46.83%|
| 18 | alexandrescu_pohoreski_v2  |~ 4.976 avg ns/call|  46.66%|
| 19 | vitali                     |~ 5.108 avg ns/call|  45.24%|
| 20 | alink_fixed_v1b            |~ 5.300 avg ns/call|  43.19%|
| 21 | alink_fixed_v2b            |~ 5.390 avg ns/call|  42.22%|
| 22 | alink_fixed_v1a            |~ 5.848 avg ns/call|  37.31%|
| 23 | if_naive                   |~ 5.852 avg ns/call|  37.26%|
| 24 | alink_buggy_v1             |~ 6.403 avg ns/call|  31.35%|
| 25 | alink_fixed_v2a            |~ 7.152 avg ns/call|  23.33%|
| 26 | alexandrescu_v1            |~ 9.332 avg ns/call|   0.00%|
| 27 | user42690_fixed_b          |~ 9.358 avg ns/call|  -0.32%|
| 28 | alink_buggy_v2             |~ 9.539 avg ns/call|  -2.26%|
| 29 | microsoft_itoa_strlen      |~10.740 avg ns/call| -15.14%|
| 30 | user42690_buggy            |~10.987 avg ns/call| -17.78%|
| 31 | thomas                     |~11.763 avg ns/call| -26.11%|
| 32 | user42690_fixed_a          |~13.408 avg ns/call| -43.74%|
| 33 | dumb_sprintf_strlen        |~49.636 avg ns/call|-432.12%|

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

On macOS the simple version beats everything  using the reference uin64_t.
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
.\bin\numdigits_benchmark.exe 5
```

or on macOS / Linux:

```bash
./bin/numdigits_benchmark 5
```

# Further Research

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

Last updated Dec. 22, 2025.
