#define _CRT_SECURE_NO_WARNINGS 1
    #include <stdio.h>    // printf()
    #include <assert.h>
    #include <inttypes.h> // PRId64
    #include <algorithm>  // abs()

inline static uint32_t digits10_alexandrescu_v3( uint64_t );
#define digits10_fixed digits10_alexandrescu_v3

#include "bug_fix.h"
#include "numdigits.h"

    const char* STATUS[2] = {
        "FAIL",
        "pass"
    };

// --- int ---

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

// -------------------- int64_t --------------------

    const int64_t TEST_I64_MINMAX[] = {
         -9'223'372'036'854'775'807ll-1, //  -2^63 // SIGH https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1280r2
         -9'223'372'036'854'775'807ll,   //  -2^63 + 1
                                  0ll,   //
         +9'223'372'036'854'775'807ll    //  2^63 - 1
    };
    const size_t NUM_I64_MINMAX = sizeof(TEST_I64_MINMAX) / sizeof(TEST_I64_MINMAX[0]);

    const int64_t TEST_I64_NINES[] = {
    //  -99999999999999999999, //21 overflows int64_t
    //  -18446744073709551616, //21  - 2^64
    //   -9999999999999999999, //20 overflows int64_t
    //   -9223372036854775808, //20  - 2^63
          -999999999999999999, //19
           -99999999999999999, //18
            -9999999999999999, //17
             -999999999999999, //16
              -99999999999999, //15
               -9999999999999, //14
                -999999999999, //13
                 -99999999999, //12
                  -9999999999, //11
              //  -2147483648, //11  - 2^31
                   -999999999, //10
                    -99999999, // 9
                     -9999999, // 8
                      -999999, // 7
                       -99999, // 6
                        -9999, // 5
                         -999, // 4
                          -99, // 3
                           -9, // 2
                           +9, // 1
                          +99, // 2
                         +999, // 3
                        +9999, // 4
                       +99999, // 5
                      +999999, // 6
                     +9999999, // 7
                    +99999999, // 8
                   +999999999  // 9
              //  +2147483647  //10  + 2^31 - 1
                  +9999999999, //10
                 +99999999999, //11
                +999999999999, //12
               +9999999999999, //13
              +99999999999999, //14
             +999999999999999, //15
            +9999999999999999, //16
           +99999999999999999, //17
          +999999999999999999, //18
    //   +9223372036854775808, //19  + 2^63
    //   +9999999999999999999, //19 overflows int64_t
    //  +18446744073709551616, //20  + 2^64
    //  +99999999999999999999, //20 overflows uint64_t

    };
    const int NUM_I64_NINES = sizeof(TEST_I64_NINES) / sizeof(TEST_I64_NINES[0]);

    const int64_t TEST_I64_POW2[] = {
    //  -18446744073709551616LL, // - 2 ^64 overflows int64_t
         -9223372036854775807LL-1,//- 2^63 INT64_MIN
         -4611686018427387904LL, // - 2^62
         -2305843009213693952LL, // - 2^61
         -1152921504606846976LL, // - 2^60
          -576460752303423488LL, // - 2^59
          -288230376151711744LL, // - 2^58
          -144115188075855872LL, // - 2^57
           -72057594037927936LL, // - 2^56
           -36028797018963968LL, // - 2^55
           -18014398509481984LL, // - 2^54
            -9007199254740992LL, // + 2^53
            -4503599627370496LL, // + 2^52
            -2251799813685248LL, // + 2^51
            -1125899906842624LL, // + 2^50
             -562949953421312LL, // + 2^49
             -281474976710656LL, // + 2^48
             -140737488355328LL, // + 2^47
              -70368744177664LL, // + 2^46
              -35184372088832LL, // + 2^45
              -17592186044416LL, // + 2^44
               -8796093022208LL, // + 2^43
               -4398046511104LL, // + 2^42
               -2199023255552LL, // + 2^41
               -1099511627776LL, // + 2^40
                -549755813888LL, // + 2^39
                -274877906944LL, // + 2^38
                -137438953472LL, // + 2^37
                 -68719476736LL, // + 2^36
                 -34359738368LL, // + 2^35
                 -17179869184LL, // + 2^34
                  -8589934592LL, // + 2^33
                  -4294967296LL, // + 2^32
                  -2147483648LL, // - 2^31 INT32_MIN
                  -1073741824LL, // - 2^30
                   -536870912LL, // - 2^29
                   -268435456LL, // - 2^28
                   -134217728LL, // - 2^27
                    -67108864LL, // - 2^26
                    -33554432LL, // - 2^25
                    -16777216LL, // - 2^24
                     -8388608LL, // - 2^23
                     -4194304LL, // - 2^22
                     -2097152LL, // - 2^21
                     -1048576LL, // - 2^20
                      -524288LL, // - 2^19
                      -262144LL, // - 2^18
                      -131072LL, // - 2^17
                       -65536LL, // - 2^16
                       -32768LL, // - 2^15
                       -16384LL, // - 2^14
                        -8192LL, // - 2^13
                        -4096LL, // - 2^12
                        -2048LL, // - 2^11
                        -1024LL, // - 2^10
                         -512LL, // - 2^9
                         -256LL, // - 2^8
                         -128LL, // - 2^7
                          -64LL, // - 2^6
                          -32LL, // - 2^5
                          -16LL, // - 2^4
                           -8LL, // - 2^3
                           -4LL, // - 2^2
                           -2LL, // - 2^1
                           -1LL, // - 2^0
                           +1LL, // + 2^0
                           +2LL, // + 2^1
                           +4LL, // + 2^2
                           +8LL, // + 2^3
                          +16LL, // + 2^4
                          +32LL, // + 2^5
                          +64LL, // + 2^6
                         +128LL, // + 2^7
                         +256LL, // + 2^8
                         +512LL, // + 2^9
                        +1024LL, // + 2^10
                        +2048LL, // + 2^11
                        +4096LL, // + 2^12
                        +8192LL, // + 2^13
                       +16384LL, // + 2^14
                       +32768LL, // + 2^15
                       +65536LL, // + 2^16
                      +131072LL, // + 2^17
                      +262144LL, // + 2^18
                      +524288LL, // + 2^19
                     +1048576LL, // + 2^20
                     +2097152LL, // + 2^21
                     +4194304LL, // + 2^22
                     +8388608LL, // + 2^23
                    +16777216LL, // + 2^24
                    +33554432LL, // + 2^25
                    +67108864LL, // + 2^26
                   +134217728LL, // + 2^27
                   +268435456LL, // + 2^28
                   +536870912LL, // + 2^29
                  +1073741824LL, // + 2^30
                  +2147483648LL, // + 2^31 INT32_MAX+1
                  +4294967296LL, // + 2^32
                  +8589934592LL, // + 2^33
                 +17179869184LL, // + 2^34
                 +34359738368LL, // + 2^35
                 +68719476736LL, // + 2^36
                +137438953472LL, // + 2^37
                +274877906944LL, // + 2^38
                +549755813888LL, // + 2^39
               +1099511627776LL, // + 2^40
               +2199023255552LL, // + 2^41
               +4398046511104LL, // + 2^42
               +8796093022208LL, // + 2^43
              +17592186044416LL, // + 2^44
              +35184372088832LL, // + 2^45
              +70368744177664LL, // + 2^46
             +140737488355328LL, // + 2^47
             +281474976710656LL, // + 2^48
             +562949953421312LL, // + 2^49
            +1125899906842624LL, // + 2^50
            +2251799813685248LL, // + 2^51
            +4503599627370496LL, // + 2^52
            +9007199254740992LL, // + 2^53
           +18014398509481984LL, // + 2^54
           +36028797018963968LL, // + 2^55
           +72057594037927936LL, // + 2^56
          +144115188075855872LL, // + 2^57
          +288230376151711744LL, // + 2^58
          +576460752303423488LL, // + 2^59
         +1152921504606846976LL, // + 2^60
         +2305843009213693952LL, // + 2^61
         +4611686018427387904LL, // + 2^62
    //   +9223372036854775808LL, // + 2^63 overflows int64_t
    //  +18446744073709551616LL, // + 2^64 overflows int64_t
    };
    const int NUM_I64_POW2 = sizeof(TEST_I64_POW2) / sizeof(TEST_I64_POW2[0]);

    const int64_t TEST_I64_POW10[] = {
    //  -18446744073709551616  // [--] - 2^64
    //   -9223372036854775808  // [--] - 2^63
         -1000000000000000000, // [ 0]
          -100000000000000000, // [ 1]
           -10000000000000000, // [ 2]
            -1000000000000000, // [ 3]
             -100000000000000, // [ 4]
              -10000000000000, // [ 5]
               -1000000000000, // [ 6]
                -100000000000, // [ 7]
                 -10000000000, // [ 8]
                  -1000000000, // [ 9] - 2^31 = -2147483648
                   -100000000, // [10]
                    -10000000, // [11]
                     -1000000, // [12]
                      -100000, // [13]
                       -10000, // [14]
                        -1000, // [15]
                         -100, // [16]
                          -10, // [17]
                           -1, // [18]
                           +1, // [19]
                          +10, // [20]
                         +100, // [21]
                        +1000, // [22]
                       +10000, // [23]
                      +100000, // [24]
                     +1000000, // [25]
                    +10000000, // [26]
                   +100000000, // [27]
                  +1000000000, // [28] + 2^31 - 1 = 2147483647
                 +10000000000, // [29]
                +100000000000, // [30]
               +1000000000000, // [31]
              +10000000000000, // [32]
             +100000000000000, // [33]
            +1000000000000000, // [34]
           +10000000000000000, // [35]
          +100000000000000000, // [36]
         +1000000000000000000  // [37]
    //   +9223372036854775807  // [--] + 2^63 - 1
    //  +18446744073709551615  // [--] + 2^64 - 1
    };
    const int NUM_I64_POW10 = sizeof(TEST_I64_POW10) / sizeof(TEST_I64_POW10[0]);

// -------------------- uint64_t --------------------

    const uint64_t TEST_U64_NINES[] = {
                           +9ULL, // 1, [ 0]
                          +99ULL, // 2, [ 1]
                         +999ULL, // 3, [ 2]
                        +9999ULL, // 4, [ 3]
                       +99999ULL, // 5, [ 4]
                      +999999ULL, // 6, [ 5]
                     +9999999ULL, // 7, [ 6]
                    +99999999ULL, // 8, [ 7]
                   +999999999ULL  // 9, [ 8]
                  +9999999999ULL, //10, [ 9] + 2^31 - 1 = +2147483647
                 +99999999999ULL, //11, [10]
                +999999999999ULL, //12, [11]
               +9999999999999ULL, //13, [12]
              +99999999999999ULL, //14, [13]
             +999999999999999ULL, //15, [14]
            +9999999999999999ULL, //16, [15]
           +99999999999999999ULL, //17, [16]
          +999999999999999999ULL, //18, [17]
         +9999999999999999999ULL, //19, [18] + 2^63 =  +9223372036854775808
    //  +18446744073709551616ULL, //20, [--] + 2^64
    //  +99999999999999999999ULL, //20, [--] overflows uint64_t
    };
    const int NUM_U64_NINES = sizeof(TEST_U64_NINES) / sizeof(uint64_t);

    const uint64_t TEST_U64_MINMAX[] = {
                                  0ull, // [0]
        +18'446'744'073'709'551'615ull  // [1] + 2^64 - 1
    };
    const int NUM_U64_MINMAX = sizeof(TEST_U64_MINMAX) / sizeof(uint64_t);

    const uint64_t TEST_U64_POW2[] = {
                           +1ULL, // + 2^0
                           +2ULL, // + 2^1
                           +4ULL, // + 2^2
                           +8ULL, // + 2^3
                          +16ULL, // + 2^4
                          +32ULL, // + 2^5
                          +64ULL, // + 2^6
                         +128ULL, // + 2^7
                         +256ULL, // + 2^8
                         +512ULL, // + 2^9
                        +1024ULL, // + 2^10
                        +2048ULL, // + 2^11
                        +4096ULL, // + 2^12
                        +8192ULL, // + 2^13
                       +16384ULL, // + 2^14
                       +32768ULL, // + 2^15
                       +65536ULL, // + 2^16
                      +131072ULL, // + 2^17
                      +262144ULL, // + 2^18
                      +524288ULL, // + 2^19
                     +1048576ULL, // + 2^20
                     +2097152ULL, // + 2^21
                     +4194304ULL, // + 2^22
                     +8388608ULL, // + 2^23
                    +16777216ULL, // + 2^24
                    +33554432ULL, // + 2^25
                    +67108864ULL, // + 2^26
                   +134217728ULL, // + 2^27
                   +268435456ULL, // + 2^28
                   +536870912ULL, // + 2^29
                  +1073741824ULL, // + 2^30
                  +2147483648ULL, // + 2^31
                  +4294967296ULL, // + 2^32
                  +8589934592ULL, // + 2^33
                 +17179869184ULL, // + 2^34
                 +34359738368ULL, // + 2^35
                 +68719476736ULL, // + 2^36
                +137438953472ULL, // + 2^37
                +274877906944ULL, // + 2^38
                +549755813888ULL, // + 2^39
               +1099511627776ULL, // + 2^40
               +2199023255552ULL, // + 2^41
               +4398046511104ULL, // + 2^42
               +8796093022208ULL, // + 2^43
              +17592186044416ULL, // + 2^44
              +35184372088832ULL, // + 2^45
              +70368744177664ULL, // + 2^46
             +140737488355328ULL, // + 2^47
             +281474976710656ULL, // + 2^48
             +562949953421312ULL, // + 2^49
            +1125899906842624ULL, // + 2^50
            +2251799813685248ULL, // + 2^51
            +4503599627370496ULL, // + 2^52
            +9007199254740992ULL, // + 2^53
           +18014398509481984ULL, // + 2^54
           +36028797018963968ULL, // + 2^55
           +72057594037927936ULL, // + 2^56
          +144115188075855872ULL, // + 2^57
          +288230376151711744ULL, // + 2^58
          +576460752303423488ULL, // + 2^59
         +1152921504606846976ULL, // + 2^60
         +2305843009213693952ULL, // + 2^61
         +4611686018427387904ULL, // + 2^62
         +9223372036854775808ULL  // + 2^63
    //  +18446744073709551615ULL  // + 2^64 - 1 max uint64_t
    //  +99999999999999999999UL,  // overflows uint64_t
    };
    const int NUM_U64_POW2 = sizeof(TEST_U64_POW2) / sizeof(uint64_t);

    const uint64_t TEST_U64_POW10[] = {
                           +1ULL,
                          +10ULL,
                         +100ULL,
                        +1000ULL,
                       +10000ULL,
                      +100000ULL,
                     +1000000ULL,
                    +10000000ULL,
                   +100000000ULL,
                  +1000000000ULL, // + 2^31 - 1 = 2147483647
                 +10000000000ULL,
                +100000000000ULL,
               +1000000000000ULL,
              +10000000000000ULL,
             +100000000000000ULL,
            +1000000000000000ULL,
           +10000000000000000ULL,
          +100000000000000000ULL,
         +1000000000000000000ULL, // + 2^63 - 1 = 9223372036854775807
        +10000000000000000000ULL
    //  +18446744073709551615ULL  // + 2^64 - 1
    };
    const int NUM_U64_POW10 = sizeof(TEST_U64_POW10) / sizeof(uint64_t);

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
typedef int      (*NumDigitsIntFuncPtr)(int     );
typedef int32_t  (*NumDigitsI64FuncPtr)(int64_t );
typedef uint32_t (*NumDigitsU64FuncPtr)(uint64_t);

static void validate_table_int( const IntKeyVal*aNumbers, int nNumbers, const char *pDescription, NumDigitsIntFuncPtr pFunc );
static void validate_table_i64( const int64_t  *aNumbers, int nNumbers, const char *pDescription, NumDigitsI64FuncPtr pFunc );
static void validate_table_u64( const uint64_t *aNumbers, int nNumbers, const char *pDescription, NumDigitsU64FuncPtr pFunc );

// ========================================

// Three Optimization Tips for C++, Slide 27 of 33
// http://www.slideshare.net/andreialexandrescu1/three-optimization-tips-for-c-15708507
void test_original_digits10()
{
    uint64_t n = 0, x, prev;

    printf( "----------------------------------------\n\n" );

    while (true) {
        uint32_t actual = digits10_buggy( n );
        uint32_t expect = digits10_fixed( n );
        bool     pass   = (actual == expect);
        printf( "%20llu: %2u == %2u %s", n, actual, expect, STATUS[pass] );
        if (!pass) printf( " in original digits10()!" );
        printf( "\n" );
        if (!n) { n = 1; continue; }
        else {
            prev = n;
            n *= 10;
            if (n < prev) break; // overflow
        }
        x = n - 1;
        actual = digits10_fixed    ( x );
        expect = numdigits10_uint64( x );
        pass   = (actual == expect);
        printf( "%20llu: %2u == %2u %s\n", x, actual, expect, STATUS[pass] );
    }
    printf( "\n" );
}

void test_int( NumDigitsIntFuncPtr pFunc )
{
    validate_table_int( TEST_INT_MINMAX, NUM_INT_MINMAX, "min,0,max"   , pFunc );
    validate_table_int( TEST_INT_NINES , NUM_INT_NINES , "nines"       , pFunc );
    validate_table_int( TEST_INT_POW2  , NUM_INT_POW2  , "powers of 2" , pFunc );
    validate_table_int( TEST_INT_POW10 , NUM_INT_POW10 , "powers of 10", pFunc );
}

void test_i64( NumDigitsI64FuncPtr pFunc )
{
    validate_table_i64( TEST_I64_MINMAX, NUM_I64_MINMAX, "min,0,max"   , pFunc );
    validate_table_i64( TEST_I64_NINES , NUM_I64_NINES , "nines"       , pFunc );
    validate_table_i64( TEST_I64_POW10 , NUM_I64_POW10 , "powers of 10", pFunc );
    validate_table_i64( TEST_I64_POW2  , NUM_I64_POW2  , "powers of 2" , pFunc );
}

void test_u64( NumDigitsU64FuncPtr pFunc )
{
    validate_table_u64( TEST_U64_MINMAX, NUM_U64_MINMAX, "min,0,max"   , pFunc );
    validate_table_u64( TEST_U64_NINES , NUM_U64_NINES , "nines"       , pFunc );
    validate_table_u64( TEST_U64_POW2  , NUM_U64_POW2  , "powers of 2" , pFunc );
    validate_table_u64( TEST_U64_POW10 , NUM_U64_POW10 , "powers of 10", pFunc );
}

// ========================================
void validate_table_int( const IntKeyVal *aNumbers, int nNumbers, const char *pDescription, NumDigitsIntFuncPtr pFunc )
{
    bool status = true;

    // We have a few choices to use as our reference version to compare against:
    //   numdigits_dumb_int64( (int64_t)n );
    //   numdigits_dumb_sprintf_strlen
    //   numdigits_if_naive
    //   numdigits10_i32
    // But couldn't/shouldn't trust any of those
    // so we embed the expected answer along with the test number.
    for (int iNumber = 0; iNumber < nNumbers; iNumber++ ) {
        int      n = aNumbers[ iNumber ].number;
        int actual = pFunc( n );
        int expect = aNumbers[ iNumber ].expect;
        if (actual != expect) {
            status = false;
            printf( "(%11d) Actual %2d != Expect %2d %s test [%2d]\n", n, actual, expect, STATUS[status], iNumber );
        }
    }
    printf( "%s %s\n", STATUS[status], pDescription );
}

void validate_table_i64(const int64_t* aNumbers, int nNumbers, const char* pDescription, NumDigitsI64FuncPtr pFunc)
{
    bool status = true;

    for (int iNumber = 0; iNumber < nNumbers; iNumber++ ) {
        int64_t   n = aNumbers[ iNumber ];
        int32_t actual = pFunc            ( n );
        int32_t expect = numdigits10_int64( n );
        if (actual != expect) {
            status = false;
            printf( "(%11" PRId64 ") %2d != %2d %s test [%2d]\n", n, actual, expect, STATUS[status], iNumber );
        }
    }
    printf( "%s %s\n", STATUS[status], pDescription );
}

void validate_table_u64( const uint64_t* aNumbers, int nNumbers, const char* pDescription, NumDigitsU64FuncPtr pFunc )
{
    bool status = true;

    for (int iNumber = 0; iNumber < nNumbers; iNumber++ ) {
        uint64_t   n = aNumbers[ iNumber ];
        int32_t actual = pFunc            (  n );
        int32_t expect = numdigits10_uint64( n );
        if (actual != expect) {
            status = false;
            printf( "(%11" PRId64 ") %2d != %2d %s test [%2d]\n", n, actual, expect, STATUS[status], iNumber );
        }
    }
    printf( "%s %s\n", STATUS[status], pDescription );
}

// ========================================
void verify_int()
{
    struct FuncDescInt
    {
        NumDigitsIntFuncPtr pFunc;
        const char         *pName;
    };

    FuncDescInt aFuncs[] = {
          { numdigits_alexandrescu_v1          , "alexandrescu_v1"           }
        , { numdigits_alexandrescu_pohoreski_v2, "alexandrescu_pohoreski_v2" }
        , { numdigits_alexandrescu_pohoreski_v3, "alexandrescu_pohoreski_v3" }
        , { numdigits_alink_buggy_v1           , "alink_buggy_v1"            }
        , { numdigits_alink_buggy_v2           , "alink_buggy_v2"            }
        , { numdigits_alink_fixed_v1a          , "alink_fixed_v1a"           }
        , { numdigits_alink_fixed_v1b          , "alink_fixed_v1b"           }
        , { numdigits_alink_fixed_v2a          , "alink_fixed_v2a"           }
        , { numdigits_alink_fixed_v2b          , "alink_fixed_v2b"           }
        , { numdigits_clifford_buggy           , "clifford_buggy"            }
        , { numdigits_clifford_fixed           , "clifford_fixed"            }
        , { numdigits_dumb_sprintf_strlen      , "dumb_sprintf_strlen"       }
        , { numdigits_gabriel_v1a              , "gabriel_v1a"               }
        , { numdigits_gabriel_v1b              , "gabriel_v1b"               }
        , { numdigits_gabriel_v1c              , "gabriel_v1c"               }
        , { numdigits_if_naive                 , "if_naive"                  }
        , { numdigits_log                      , "log"                       }
        , { numdigits_microsoft_itoa_strlen    , "microsoft_itoa_strlen"     }
        , { numdigits_pohoreski_v1a            , "pohoreski_v1a"             }
        , { numdigits_pohoreski_v1b            , "pohoreski_v1b"             }
        , { numdigits_pohoreski_v2a            , "pohoreski_v2a"             }
        , { numdigits_pohoreski_v2b            , "pohoreski_v2b"             }
        , { numdigits_pohoreski_v3a            , "pohoreski_v3a"             }
        , { numdigits_pohoreski_v3b            , "pohoreski_v3b"             }
        , { numdigits_pohoreski_v4a            , "pohoreski_v4a"             }
        , { numdigits_pohoreski_v4b            , "pohoreski_v4b"             }
        , { numdigits_ransom                   , "ransom"                    }
        , { numdigits10_int32                  , "reference_int32"           }
        , { numdigits10_int                    , "reference_pow10_int"       }
        , { numdigits10_mixed_int              , "reference_mixed_int"       }
        , { numdigits_simple                   , "simple"                    }
        , { numdigits_simple_reverse           , "simple_reverse"            }
        , { numdigits_thomas                   , "thomas"                    }
        , { numdigits_user42690_buggy          , "user42690_buggy"           }
        , { numdigits_user42690_fixed_a        , "user42690_fixed_a"         }
        , { numdigits_user42690_fixed_b        , "user42690_fixed_b"         }
        , { numdigits_vitali                   , "vitali"                    }
    };
    const int nFuncs = sizeof(aFuncs) / sizeof(aFuncs[0]);

    printf( "----------------------------------------\n\n" );
    for (int iFunc = 0; iFunc < nFuncs; iFunc++) {
        printf( "\n========== %s (int) ==========\n", aFuncs[ iFunc ].pName );
        test_int( aFuncs[ iFunc ].pFunc );
    }
    printf( "\n" );
}

void verify_i64()
{
    struct FuncDescI64
    {
        NumDigitsI64FuncPtr pFunc;
        const char         *pName;
    };

    FuncDescI64 aFuncs[] = {
          { numdigits10_int64                 , "numdigits10_int64"    }
        , { numdigits_dumb_int64              , "numdigits_dumb_int64" }
        , { numdigits_simple_int64            , "simple_int64"         }
        , { numdigits_vitali_template<int64_t>, "vitali_int64"         }
    };
    const int nFuncs = sizeof(aFuncs) / sizeof(aFuncs[0]);

    printf( "----------------------------------------\n\n" );
    for (int iFunc = 0; iFunc < nFuncs; iFunc++) {
        printf( "\n========== %s (i64) ==========\n", aFuncs[ iFunc ].pName );
        test_i64( aFuncs[ iFunc ].pFunc );
    }
    printf( "\n" );
}

void verify_u64()
{
    struct FuncDescU64
    {
        NumDigitsU64FuncPtr pFunc;
        const char         *pName;
    };

    FuncDescU64 aFuncs[] = {
          { numdigits10_uint64        , "numdigits10_uint64"         }
        , { numdigits10_reverse_uint64, "numdigits10_reverse_uint64" }
        , { digits10_alexandrescu_v2  , "digits10_alexandrescu_v2"   }
        , { digits10_alexandrescu_v3  , "digits10_alexandrescu_v3"   }
        , { digits10_gabriel          , "digits10_gabriel"           }
        , { digits10_pohoreski_v1     , "digits10_pohoreski_v1"      }
        , { digits10_pohoreski_v2     , "digits10_pohoreski_v2"      }
    };
    const int nFuncs = sizeof(aFuncs) / sizeof(aFuncs[0]);

    printf( "----------------------------------------\n\n" );
    for (int iFunc = 0; iFunc < nFuncs; iFunc++) {
        printf( "\n========== %s (u64) ==========\n", aFuncs[ iFunc ].pName );
        test_u64( aFuncs[ iFunc ].pFunc );
    }
    printf( "\n" );
}

// ========================================
int main()
{
    printf( "Verify numdigits()\n" );
    printf( "    sizeof( int     ) = %zu bytes\n", sizeof( int     ) );
    printf( "    sizeof( int32   ) = %zu bytes\n", sizeof( int32_t ) );
    printf( "    sizeof(long long) = %zu bytes\n", sizeof(long long) );
    printf( "    sizeof( int64_t ) = %zu bytes\n", sizeof( int64_t ) );
    printf( "    sizeof(uint64_t ) = %zu bytes\n", sizeof(uint64_t ) );

    verify_int();
    verify_i64();
    verify_u64();
    test_original_digits10();

    printf( "\nDone.\n" );
    return 0;
}
