// TL:DR; int NumDigitsBase10( int x )
// Fastest x86:  numdigits_alexandrescu_pohoreski_v3( int n )
// Fastest M# :  numdigits_simple( int n ) -> numdigits10_uint64()
// Easiest:  numdigits_dumb_sprintf_strlen( int x );
// Simplest: numdigits10_uint32( uint32_t n )

// Reference uint32_t
uint32_t numdigits10_uint32( uint32_t n )
{
    if (n <            10) return  1;
    if (n <           100) return  2;
    if (n <         1'000) return  3;
    if (n <        10'000) return  4;
    if (n <       100'000) return  5;
    if (n <     1'000'000) return  6;
    if (n <    10'000'000) return  7;
    if (n <   100'000'000) return  8;
    if (n < 1'000'000'000) return  9;
    /*else<=4'294'967'295*/return 10; // 2^32 - 1
}

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

uint32_t numdigits10_reverse_uint64( uint64_t n )
{
    if (n >= 10'000'000'000'000'000'000ull) return 20; // 2^64 - 1
    if (n >=  1'000'000'000'000'000'000ull) return 19;
    if (n >=    100'000'000'000'000'000ull) return 18;
    if (n >=     10'000'000'000'000'000ull) return 17;
    if (n >=      1'000'000'000'000'000ull) return 16;
    if (n >=        100'000'000'000'000ull) return 15;
    if (n >=         10'000'000'000'000ull) return 14;
    if (n >=          1'000'000'000'000ull) return 13;
    if (n >=            100'000'000'000ull) return 12;
    if (n >=             10'000'000'000ull) return 11;
    if (n >=              1'000'000'000ull) return 10;
    if (n >=                100'000'000ull) return  9;
    if (n >=                 10'000'000ull) return  8;
    if (n >=                  1'000'000ull) return  7;
    if (n >=                    100'000ull) return  6;
    if (n >=                     10'000ull) return  5;
    if (n >=                      1'000ull) return  4;
    if (n >=                        100ull) return  3;
    if (n >=                         10ull) return  2;
    /*else>=                          0  */ return  1;
}

// Reference int using standard powers of 10 and zero
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

// powers of 10 and repeating 9s
int numdigits10_mixed_int( int n )
{
    if (n <   -999'999'999ll) return 11; // -2'147'483'648 .. -1'000'000'000  -2^31
    if (n <    -99'999'999ll) return 10; //   -999'999'999 ..   -100'000'000
    if (n <     -9'999'999ll) return  9; //    -99'999'999 ..    -10'000'000
    if (n <       -999'999ll) return  8; //     -9'999'999 ..     -1'000'000
    if (n <        -99'999ll) return  7; //       -999'999 ..       -100'000
    if (n <         -9'999ll) return  6; //        -99'999 ..        -10'000
    if (n <           -999ll) return  5; //         -9'999 ..         -1'000
    if (n <            -99ll) return  4; //           -999 ..           -100
    if (n <             -9ll) return  3; //            -99 ..            -10
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

// Reference int32_t
int32_t numdigits10_int32( int32_t n )
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

// Reference int64_t
int32_t numdigits10_int64( int64_t n )
{
    if (n <=-1'000'000'000'000'000'000ll) return 20; //-9'223'372'036'854'775'808 ..-1'000'000'000'000'000'000  -2^63
    if (n <=  -100'000'000'000'000'000ll) return 19; //  -999'999'999'999'999'999 ..  -100'000'000'000'000'000
    if (n <=   -10'000'000'000'000'000ll) return 18; //   -99'999'999'999'999'999 ..   -10'000'000'000'000'000
    if (n <=    -1'000'000'000'000'000ll) return 17; //    -9'999'999'999'999'999 ..    -1'000'000'000'000'000
    if (n <=      -100'000'000'000'000ll) return 16; //      -999'999'999'999'999 ..      -100'000'000'000'000
    if (n <=       -10'000'000'000'000ll) return 15; //       -99'999'999'999'999 ..       -10'000'000'000'000
    if (n <=        -1'000'000'000'000ll) return 14; //        -9'999'999'999'999 ..        -1'000'000'000'000
    if (n <=          -100'000'000'000ll) return 13; //          -999'999'999'999 ..          -100'000'000'000
    if (n <=           -10'000'000'000ll) return 12; //           -99'999'999'999 ..           -10'000'000'000
    if (n <=            -1'000'000'000ll) return 11; //            -9'999'999'999 ..            -1'000'000'000
    if (n <=              -100'000'000ll) return 10; //              -999'999'999 ..              -100'000'000
    if (n <=               -10'000'000ll) return  9; //               -99'999'999 ..               -10'000'000
    if (n <=                -1'000'000ll) return  8; //                -9'999'999 ..                -1'000'000
    if (n <=                  -100'000ll) return  7; //                  -999'999 ..                  -100'000
    if (n <=                   -10'000ll) return  6; //                   -99'999 ..                   -10'000
    if (n <=                    -1'000ll) return  5; //                    -9'999 ..                    -1'000
    if (n <=                      -100ll) return  4; //                      -999 ..                      -100
    if (n <=                       -10ll) return  3; //                       -99 ..                       -10
    if (n <                          0ll) return  2; //                        -9 ..                        -1
    if (n <                         10ll) return  1; //                         0 ..                         9
    if (n <                        100ll) return  2; //                        10 ..                        99
    if (n <                      1'000ll) return  3; //                       100 ..                       999
    if (n <                     10'000ll) return  4; //                     1'000 ..                     9'999
    if (n <                    100'000ll) return  5; //                    10'000 ..                    99'999
    if (n <                  1'000'000ll) return  6; //                   100'000 ..                   999'999
    if (n <                 10'000'000ll) return  7; //                 1'000'000 ..                 9'999'999
    if (n <                100'000'000ll) return  8; //                10'000'000 ..                99'999'999
    if (n <              1'000'000'000ll) return  9; //               100'000'000 ..               999'999'999
    if (n <             10'000'000'000ll) return 10; //             1'000'000'000 ..             9'999'999'999
    if (n <            100'000'000'000ll) return 11; //            10'000'000'000 ..            99'999'999'999
    if (n <          1'000'000'000'000ll) return 12; //           100'000'000'000 ..           999'999'999'999
    if (n <         10'000'000'000'000ll) return 13; //         1'000'000'000'000 ..         9'999'999'999'999
    if (n <        100'000'000'000'000ll) return 14; //        10'000'000'000'000 ..        99'999'999'999'999
    if (n <      1'000'000'000'000'000ll) return 15; //       100'000'000'000'000 ..       999'999'999'999'999
    if (n <     10'000'000'000'000'000ll) return 16; //     1'000'000'000'000'000 ..     9'999'999'999'999'999
    if (n <    100'000'000'000'000'000ll) return 17; //    10'000'000'000'000'000 ..    99'999'999'999'999'999
    if (n <  1'000'000'000'000'000'000ll) return 18; //   100'000'000'000'000'000 ..   999'999'999'999'999'999
    /*else<= 9'223'372'036'854'775'807 */ return 19; // 1'000'000'000'000'000'000 .. 9'223'372'036'854'775'807   2^63 - 1
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// Andrei Alexandrescu
// Three Optimization Tips for C++, Slide 17 of 33
// https://www.slideshare.net/slideshow/three-optimization-tips-for-c-15708507/15708507#24
uint32_t digits10_alexandrescu_v1( uint64_t v ) {
    uint32_t result = 0;
    do
    {
        ++result;
        v /= 10;
    } while( v );
    return result;
}

// NOTE: Andrei never provided an int version so I provided the fastest one.
// I tried a bunch of things to find the fastest version
//    const uint32_t x = (n < 0) ? (uint64_t)-n & 0xFFFFFFFFU : n;
// and this ended up being faster:
//    const int64_t  i = n;
//    const uint64_t x = abs(i);
int numdigits_alexandrescu_v1( int n )
{
    const int64_t  i = n;
    const uint64_t x = abs(i);
    return (n < 0) + digits10_alexandrescu_v1( x );
}

// Three Optimization Tips for C++, Slide 18 of 33
// https://www.slideshare.net/slideshow/three-optimization-tips-for-c-15708507/15708507#25
// Fixed formatting with multi-column alignment for readability
uint32_t digits10_alexandrescu_v2( uint64_t v )
{
    uint32_t result = 1;
    for (;;) {
        if (v <     10) return result + 0;
        if (v <    100) return result + 1;
        if (v <   1000) return result + 2;
        if (v <  10000) return result + 3;
        // Skip ahead by 4 orders of magnitude
        v /= 10000U;
        result += 4;
    }
}

// https://engineering.fb.com/2013/03/15/developer-tools/three-optimization-tips-for-c/
// https://www.slideshare.net/slideshow/three-optimization-tips-for-c-15708507/15708507#34
// /!\ BUG NOTE: There is a bug in the slides version that does NOT exist in the original implementation!
//     digits( 100000 ) returns 5 instead of 6.
//                return 5 + (value <  P05); // BUG
//                return 5 + (value >= P05); // FIXED
//     digits( 10000000000000000 ) also fails due to the recursive call.
uint32_t digits10_alexandrescu_v3( uint64_t v )
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

// NOTE: Andrei never provided an int version so I provided a fast version.
int numdigits_alexandrescu_pohoreski_v2( int n )
{
    const int64_t  i = n;
    const uint64_t x = abs(i);
    return (n < 0) + digits10_alexandrescu_v2( x );
}

// NOTE: Andrei never provided an int version so I provided a fast version.
// This is currently the fastest version to the best of my knowledge (2025)
int numdigits_alexandrescu_pohoreski_v3( int n )
{
    const int64_t  i = n;
    const uint64_t x = abs(i);
    return (n < 0) + digits10_alexandrescu_v3( x );
}

// There is no:
//     int numdigits_alexandrescu_v3( int n )
// as Andrei never provided an int version.
// Instead see:
//     int numdigits_pohoreski_v5( int n )

// https://stackoverflow.com/questions/1696086/whats-the-best-way-to-get-the-length-of-the-decimal-representation-of-an-int-in
// https://stackoverflow.com/a/1698436/1339447
// /!\ BUG NOTE: Fails on
//  * all negative numbers
int numdigits_alink_buggy_v1( int n ) {
    int digits = 1;
    // reduce n to avoid overflow at the s*=10 step.
    // n/=10 was enough but we reuse this to optimize big numbers
    if (n >= 1000000) {
        n /= 1000000;
        digits += 6; // because 1000000 = 10^6
    }
    int s = 10;
    while (s <= n) {
        s *= 10;
        ++digits;
    }
    return digits;
}

// https://stackoverflow.com/questions/1696086/whats-the-best-way-to-get-the-length-of-the-decimal-representation-of-an-int-in
// https://stackoverflow.com/a/1698731/1339447
#include <vector>
#include <limits.h>
// /!\ BUG NOTE: Fails on
//  * ALL negative numbers
//  * 1000000000 .. INT_MAX
// Fixed typo of MAX_INT -> INT_MAX
// Fixed implicit int64 cast
int numdigits_alink_buggy_v2( int n ) {
    struct power10 {
        std::vector<int> data;
        power10() {
            for(int i=10; i < INT_MAX/10; i *= 10) data.push_back(i);
        }
    };

    static const power10 p10;
    return (int)(1 + upper_bound(p10.data.begin(), p10.data.end(), n) - p10.data.begin());
}

// Fixed to handle negative numbers
int numdigits_alink_fixed_v1a( int n )
{
    if (n <= -1'000'000'000ll) return 11; // -2'147'483'648 .. -1'000'000'000  -2^31
    if (n < 0) return (n < 0) + numdigits_alink_buggy_v1( -n );
    else       return           numdigits_alink_buggy_v1(  n );
}

int numdigits_alink_fixed_v1b( int n )
{
    if (n <= -1'000'000'000ll) return 11; // -2'147'483'648 .. -1'000'000'000  -2^31
    const int i = n;
    const int x = abs(i);
    return (n < 0) + numdigits_alink_buggy_v1( x );
}

int numdigits_alink_fixed_v2( int n ) {
    struct power10 {
        std::vector<uint64_t> data;
        power10() {
            uint64_t n = 10;
            for(int i=0; i < 9; i++, n*=10 ) data.push_back(n);
        }
    };

    static const power10 p10;
    const uint64_t found = upper_bound(p10.data.begin(), p10.data.end(), n) - p10.data.begin();
    return (int)(1 + found);
}

// Fixed to handle negative numbers
int numdigits_alink_fixed_v2a( int n )
{
    if (n <= -1'000'000'000ll) return 11; // -2'147'483'648 .. -1'000'000'000  -2^31
    if (n >=  1'000'000'000ll) return 10; //  1'000'000'000 ..  2'147'483'647   2^31 - 1
    if (n < 0) return (n < 0) + numdigits_alink_fixed_v2( -n );
    else       return           numdigits_alink_fixed_v2(  n );
}

int numdigits_alink_fixed_v2b( int n )
{
    if (n <= -1'000'000'000ll) return 11; // -2'147'483'648 .. -1'000'000'000  -2^31
    if (n >=  1'000'000'000ll) return 10; //  1'000'000'000 ..  2'147'483'647   2^31 - 1
    const int i = n;
    const int x = abs(i);
    return (n < 0) + numdigits_alink_fixed_v2( x );
}

// https://stackoverflow.com/questions/1696086/whats-the-best-way-to-get-the-length-of-the-decimal-representation-of-an-int-in
// https://stackoverflow.com/a/1697272/1339447
// /!\ BUG NOTE: FAILS on
//  * -2147483648 (INT_MIN)
//  *           0 (ZERO)
#include <cmath>
#include <cstdlib>
int numdigits_clifford_buggy( int n )
{
    int digits = (int)log10( (double)abs(n) ) + 1 ;
    return n >= 0 ? digits : digits + 1 ;
}

// Fixed edge-case -2147483648
int numdigits_clifford_fixed( int n )
{
    if (!n) return 1;
    if (n <= -1000000000) return 11;
    int digits = (int)log10( (double)abs(n) ) + 1 ;
    return n >= 0 ? digits : digits + 1 ;
}

// Francesco D'Agostino
#include <array>    // std::array<>
#if 0 // C++23
    constexpr auto init_array()
    {
         std::array<uint64_t,19> _values;
         for ( std::size_t ndx = 0; ndx < _values.size(); ndx++ )
            _values[ndx] = std::pow(10, ndx + 1);

         return _values;
    }
    constexpr static const auto Pow10_u64 = init_array();
#else
    const std::array<uint64_t,19> Pow10_u64 =
    {
                            10ull // [ 0]
        ,                  100ull // [ 1]
        ,                 1000ull // [ 2]
        ,                10000ull // [ 3]
        ,               100000ull // [ 4]
        ,              1000000ull // [ 5]
        ,             10000000ull // [ 6]
        ,            100000000ull // [ 7]
        ,           1000000000ull // [ 8]
        ,          10000000000ull // [ 9]
        ,         100000000000ull // [10]
        ,        1000000000000ull // [11]
        ,       10000000000000ull // [12]
        ,      100000000000000ull // [13]
        ,     1000000000000000ull // [14]
        ,    10000000000000000ull // [15]
        ,   100000000000000000ull // [16]
        ,  1000000000000000000ull // [17]
        , 10000000000000000000ull // [18]
    };
#endif

uint32_t digits10_dagostino( uint64_t n )
{
    return
         (n >= Pow10_u64[ 0]) + (n >= Pow10_u64[ 1]) + (n >= Pow10_u64[ 2]) +
         (n >= Pow10_u64[ 3]) + (n >= Pow10_u64[ 4]) + (n >= Pow10_u64[ 5]) +
         (n >= Pow10_u64[ 6]) + (n >= Pow10_u64[ 7]) + (n >= Pow10_u64[ 8]) +
         (n >= Pow10_u64[ 9]) + (n >= Pow10_u64[10]) + (n >= Pow10_u64[11]) +
         (n >= Pow10_u64[12]) + (n >= Pow10_u64[13]) + (n >= Pow10_u64[14]) +
         (n >= Pow10_u64[15]) + (n >= Pow10_u64[16]) + (n >= Pow10_u64[17]) +
         (n >= Pow10_u64[18]);
         ;
}

int numdigits_dagostino_pohoreski( int n )
{
    const int64_t  i = n;
    const uint64_t x = abs(i);
    return (n < 0) + digits10_dagostino( x );
}

// Dumb string version using sprintf() strlen()
int numdigits_dumb_int64( int64_t n )
{
    char buffer[32];               // We can optimize away
    sprintf( buffer, "%lld", n );  //     memset( buffer, 0, sizeof(buffer));
    return (int) strlen( buffer ); // since sprintf() will null terminate for us.
}

int numdigits_dumb_sprintf_strlen( int n )
{
    char buffer[16];               // We can optimize away
    sprintf( buffer, "%d", n );    //     memset( buffer, 0, sizeof(buffer));
    return (int) strlen( buffer ); // since sprintf() will null terminate for us.
}

// https://stackoverflow.com/questions/1696086/whats-the-best-way-to-get-the-length-of-the-decimal-representation-of-an-int-in
// https://stackoverflow.com/a/56485075/1339447
uint32_t digits10_gabriel( uint64_t value )
{
    uint32_t result = 0;
    for (;;)
    {
        result += 1
               + (uint32_t)(value >= 10)
               + (uint32_t)(value >= 100)
               + (uint32_t)(value >= 1000)
               + (uint32_t)(value >= 10000)
               + (uint32_t)(value >= 100000);
        if (value < 1000000) return (int)result;
        value /= 1000000U;
    }
}

// NOTE: Gabriel never provided an int version so I provided the fastest version.
int numdigits_gabriel_v1a( int n )
{
    const uint64_t x = (n < 0) ? (uint64_t)-n & 0xFFFFFFFFU : n;
    return (n < 0) + digits10_gabriel( x );
}

int numdigits_gabriel_v1b( int n )
{
    if (n == INT_MIN)      return 11; // handle edge case: -2'147'483'648
    const int i = n;
    const int x = abs(i);
    return (n < 0) + digits10_gabriel( x );
}

int numdigits_gabriel_v1c( int n )
{
    const int64_t  i = n;
    const uint64_t x = abs(i);
    return (n < 0) + digits10_gabriel( x );
}

// Naive version
int numdigits_if_naive( int n )
{
    assert( sizeof(int) <= sizeof(int32_t) );
    if (n == INT_MIN)      return 11; // handle edge case: -2'147'483'648
    if (n <             0) return  1 + numdigits_if_naive( -n );
    if (n <            10) return  1;
    if (n <           100) return  2;
    if (n <         1'000) return  3;
    if (n <        10'000) return  4;
    if (n <       100'000) return  5;
    if (n <     1'000'000) return  6;
    if (n <    10'000'000) return  7;
    if (n <   100'000'000) return  8;
    if (n < 1'000'000'000) return  9;
    /*else<=2'147'483'647*/return 10; // 1'000'000'000 .. 2'147'483'647
}

// Naive use of log()
int numdigits_log(int n)
{
    double x = (double)n;
    int negative = (n < 0);
    if (negative)
        x = -x;
    double digits
        = (x < 10.) // [0,10)
        ? 0.0       // [0, 0] Handle log(0) = undefined
        : log10(x); // (0,10)
    return negative + (int)(digits) + 1;
}

#include <string>
// Only really for testing of how slow _itoa() is!
int32_t numdigits_microsoft_itoa_strlen( int n )
{
#if _MSC_VER
    char buffer[16];
    assert( sizeof(buffer) >= _MAX_ITOSTR_BASE10_COUNT );
    _itoa( n, buffer, 10 );
    return (int32_t) strlen( buffer );
#else
    std::string buffer = std::to_string( n );
    return (int32_t) buffer.length();
#endif
}

// Pohoreski aka Michaelangel007
uint32_t digits10_pohoreski_v1( uint64_t n )
{
    if (n < P04) {
        if (n < P01) return 1;
        if (n < P02) return 2;
        if (n < P03) return 3;
        /*    <P 04*/return 4;
    }
    if (n < P08) {
        if (n < P05) return 5;
        if (n < P06) return 6;
        if (n < P07) return 7;
        /*    < P08*/return 8;
    }
    if (n < P12) {
        if (n < P09) return  9;
        if (n < P10) return 10;
        if (n < P11) return 11;
        /*    < P12*/return 12;
    }
    return 12 + digits10_pohoreski_v1(n / P12);
}

int numdigits_pohoreski_v1a(int n )
{
    const uint64_t x = (n < 0) ? (uint64_t)-n & 0xFFFFFFFFU : n;
    return (n < 0) + digits10_pohoreski_v1( x );
}

int numdigits_pohoreski_v1b(int n )
{
    const int64_t  i = n;
    const uint64_t x = abs(i);
    return (n < 0) + digits10_pohoreski_v1( x );
}

uint32_t digits10_pohoreski_v2( uint64_t n )
{
    if (n < P08) {
        if (n < P04) {
            if (n < P01) return 1;
            if (n < P02) return 2;
            if (n < P03) return 3;
            /*  n < P04*/return 4;
        }
        if (n < P05) return 5;
        if (n < P06) return 6;
        if (n < P07) return 7;
        /*  n < P08*/return 8;
    }
    if (n < P12) {
        if (n < P09) return  9;
        if (n < P10) return 10;
        if (n < P11) return 11;
        /*  n < P12*/return 12;
    }
    return 12 + digits10_pohoreski_v2(n / P12);
}

int numdigits_pohoreski_v2a( int n )
{
    const uint64_t x = (n < 0) ? (uint64_t)-n & 0xFFFFFFFFU : n;
    return (n < 0) + digits10_pohoreski_v2( x );
}

int numdigits_pohoreski_v2b( int n )
{
    const int64_t  i = n;
    const uint64_t x = abs(i);
    return (n < 0) + digits10_pohoreski_v2( x );
}

uint32_t digits10_pohoreski_v3( uint32_t n )
{
    if (n <            10) return  1;
    if (n <           100) return  2;
    if (n <         1'000) return  3;
    if (n <        10'000) return  4;
    if (n <       100'000) return  5;
    if (n <     1'000'000) return  6;
    if (n <    10'000'000) return  7;
    if (n <   100'000'000) return  8;
    if (n < 1'000'000'000) return  9;
    /*  n <=2'147'483'647*/return 10;
}

// 2nd fastest
#include <assert.h>
int numdigits_pohoreski_v3a( int n )
{
    assert( sizeof(int) >= sizeof(uint32_t) );
    const uint32_t x = (n < 0) ? (uint64_t)-n & 0xFFFFFFFFU : n;
    return (n < 0) +  digits10_pohoreski_v3( x );
}

int numdigits_pohoreski_v3b( int n )
{
    const int64_t  i = n;
    const uint64_t x = abs(i);
    return (n < 0) +  digits10_pohoreski_v3( (uint32_t)x );
}

int numdigits_pohoreski_v4a( int n )
{
    const uint64_t x = (n < 0) ? (uint64_t)-n & 0xFFFFFFFFU: n;
    return (n < 0) + numdigits10_uint64( x );
}

int numdigits_pohoreski_v4b( int n )
{
    const int64_t  i = n;
    const uint64_t x = abs(i);
    return (n < 0) + numdigits10_uint64( x );
}

// https://stackoverflow.com/questions/1696086/whats-the-best-way-to-get-the-length-of-the-decimal-representation-of-an-int-in
// https://stackoverflow.com/a/1732801/1339447
#include <algorithm>
int numdigits_ransom( int n )
{
    if (n < 0) {
        if (n == std::numeric_limits<int32_t>::min())
            return 11;
        return numdigits_ransom(-n) + 1;
    }

    static const uint64_t MaxTable[9] = { 10,100,1000,10000,100000,1000000,10000000,100000000,1000000000 };
    return 1 + (uint32_t)(std::upper_bound(MaxTable, MaxTable+9, n) - MaxTable);
}

// Simple version
int numdigits_simple_int64(int64_t n)
{
    if (n == INT64_MIN) return 20; // handle edge case of negating int min
    if (n < 0) return 1 + numdigits10_uint64( (uint64_t) -n );
    else       return     numdigits10_uint64( (uint64_t)  n );
}

// Simple version
int numdigits_simple_v2_int64(int64_t n)
{
    if (n <=-1'000'000'000'000'000'000ll) return 20; // handle edge case of negating int min
    if (n < 0) return 1 + numdigits10_reverse_uint64( (uint64_t) -n );
    else       return     numdigits10_reverse_uint64( (uint64_t)  n );
}

int numdigits_simple(int n) {
    return numdigits_simple_int64( (int64_t) n );
}

int numdigits_simple_reverse_int64(int64_t n)
{
    if (n == INT64_MIN) return 20; // handle edge case of negating int min
    if (n < 0) return 1 + numdigits10_reverse_uint64( (uint64_t) -n );
    else       return     numdigits10_reverse_uint64( (uint64_t)  n );
}
int numdigits_simple_reverse(int n) {
    return numdigits_simple_reverse_int64( (int64_t) n );
}

// https://stackoverflow.com/questions/1696086/whats-the-best-way-to-get-the-length-of-the-decimal-representation-of-an-int-in
// https://stackoverflow.com/a/1696174/1339447
int numdigits_thomas( int n) {
    int digits = 0;
    if (n <= 0) {
        n = -n;
        ++digits;
    }
    while (n) {
        n /= 10;
        ++digits;
    }
    return digits;
}

// https://stackoverflow.com/questions/1489830/efficient-way-to-determine-number-of-digits-in-an-integer
// https://stackoverflow.com/a/1489861/1339447
// /!\ BUG NOTE: FAILS on
//  * all negative numbers
int numdigits_user42690_buggy( int n )
{
    int digits = 0; do { n /= 10; digits++; } while (n != 0);
    return digits;
}

int numdigits_user42690_fixed_a( int n )
{
    if (n == INT_MIN) return 11; //  -2'147'483'648
    if (n < 0) return 1 + numdigits_user42690_fixed_a( -n );
    int digits = 0; do { n /= 10; digits++; } while (n != 0);
    return digits;
}

int numdigits_user42690_fixed_b( int n )
{
    if (n <= -1'000'000'000) return 11; //  -2'147'483'648 .. -1'000'000'000
    if (n < 0) return 1 + numdigits_user42690_fixed_a( -n );
    int digits = 0; do { n /= 10; digits++; } while (n != 0);
    return digits;
}

// https://stackoverflow.com/questions/1489830/efficient-way-to-determine-number-of-digits-in-an-integer
// https://stackoverflow.com/a/1489873/1339447
#include <climits>
// generic solution
template <class T>
int numdigits_vitali_template(T number)
{
    int digits = 0;
    if (number < 0) digits = 1; // Keep this line if '-' counts as a digit.
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}

// partial specialization optimization for 64-bit numbers
template <>
int numdigits_vitali_template(int64_t x) {
    if (x == INT64_MIN) return 19 + 1;
    if (x < 0) return numdigits_vitali_template(-x) + 1;

    if (x >= 10000000000) {
        if (x >= 100000000000000) {
            if (x >= 10000000000000000) {
                if (x >= 100000000000000000) {
                    if (x >= 1000000000000000000)
                        return 19;
                    return 18;
                }
                return 17;
            }
            if (x >= 1000000000000000)
                return 16;
            return 15;
        }
        if (x >= 1000000000000) {
            if (x >= 10000000000000)
                return 14;
            return 13;
        }
        if (x >= 100000000000)
            return 12;
        return 11;
    }
    if (x >= 100000) {
        if (x >= 10000000) {
            if (x >= 100000000) {
                if (x >= 1000000000)
                    return 10;
                return 9;
            }
            return 8;
        }
        if (x >= 1000000)
            return 7;
        return 6;
    }
    if (x >= 100) {
        if (x >= 1000) {
            if (x >= 10000)
                return 5;
            return 4;
        }
        return 3;
    }
    if (x >= 10)
        return 2;
    return 1;
}

// partial specialization optimization for 32-bit numbers
template<>
int numdigits_vitali_template(int32_t x)
{
    if (x == INT32_MIN) return 10 + 1;
    if (x < 0) return numdigits_vitali_template(-x) + 1;

    if (x >= 10000) {
        if (x >= 10000000) {
            if (x >= 100000000) {
                if (x >= 1000000000)
                    return 10;
                return 9;
            }
            return 8;
        }
        if (x >= 100000) {
            if (x >= 1000000)
                return 7;
            return 6;
        }
        return 5;
    }
    if (x >= 100) {
        if (x >= 1000)
            return 4;
        return 3;
    }
    if (x >= 10)
        return 2;
    return 1;
}

int numdigits_vitali(int n) {
    return numdigits_vitali_template( (int32_t) n );
}
