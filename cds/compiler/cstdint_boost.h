//$$CDS-header$$

#ifndef __CDS_COMPILER_CSTDINT_BOOST_H
#define __CDS_COMPILER_CSTDINT_BOOST_H
//@cond

#include <boost/cstdint.hpp>

namespace cds {
    using boost::int_least8_t;
    using boost::uint_least8_t;
    using boost::int_least16_t;
    using boost::uint_least16_t;
    using boost::int_least32_t;
    using boost::uint_least32_t;
    using boost::int_least64_t;
    using boost::uint_least64_t;
    using boost::int_fast8_t;
    using boost::uint_fast8_t;
    using boost::int_fast16_t;
    using boost::uint_fast16_t;
    using boost::int_fast32_t;
    using boost::uint_fast32_t;
    using boost::int_fast64_t;
    using boost::uint_fast64_t;

    using boost::intmax_t;
    using boost::uintmax_t;

    using boost::int8_t;
    using boost::uint8_t;
    using boost::int16_t;
    using boost::uint16_t;
    using boost::int32_t;
    using boost::uint32_t;
    using boost::int64_t;
    using boost::uint64_t;

    using ::intptr_t;
    using ::uintptr_t;
} // namespace cds

//@endcond
#endif // #ifndef __CDS_COMPILER_CSTDINT_BOOST_H
