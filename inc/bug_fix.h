const uint64_t P01 = 10;
const uint64_t P02 = 100;
const uint64_t P03 = 1000;
const uint64_t P04 = 10000;
const uint64_t P05 = 100000;
const uint64_t P06 = 1000000;
const uint64_t P07 = 10000000;
const uint64_t P08 = 100000000;
const uint64_t P09 = 1000000000;
const uint64_t P10 = 10000000000;
const uint64_t P11 = 100000000000;
const uint64_t P12 = 1000000000000;

// Andrei Alexandrescu
// Three Optimization Tips for C++, Slide 27 of 33
// http://www.slideshare.net/andreialexandrescu1/three-optimization-tips-for-c-15708507
// https://engineering.fb.com/2013/03/15/developer-tools/three-optimization-tips-for-c/
// /!\ NOTE: The slide version has a bug but the original Facebook post does not!
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

int digits10_alexandrescu_pohoreski( int n ) {
    const int64_t  i = n;
    const uint64_t x = abs(i);
    return (n < 0) + digits10_fixed( x );
}


