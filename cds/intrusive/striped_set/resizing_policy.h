// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_STRIPED_SET_RESIZING_POLICY_H
#define CDSLIB_INTRUSIVE_STRIPED_SET_RESIZING_POLICY_H

#include <cds/opt/options.h>

namespace cds { namespace intrusive { namespace striped_set {

    /// Load factor based resizing policy
    /** @ingroup cds_striped_resizing_policy
        When total item count in a container exceeds
        <tt>container.bucket_count() * LoadFactor</tt>
        then resizing is needed.

        This policy is stateless.

        The <tt>reset()</tt> function is called after the resizing is done.
        The function is intended for resetting internal state of the policy.
    */
    template <size_t LoadFactor>
    struct load_factor_resizing
    {
        /// Main policy operator returns \p true when resizing is needed
        template <typename Container, typename Bucket>
        bool operator ()(
            size_t nSize,                   ///< Current item count of \p container
            Container const& container,     ///< Container
            Bucket const& /*bucket*/        ///< reference to a container's bucket (not used)
        ) const
        {
            return nSize > container.bucket_count() * LoadFactor;
        }

        /// Resets internal state of the policy (does nothing)
        void reset()
        {}
    };

    /// Load factor based resizing policy, stateful specialization
    /** @ingroup cds_striped_resizing_policy
        This specialization allows to specify a load factor at runtime.
    */
    template <>
    struct load_factor_resizing<0>
    {
        ///@cond
        const size_t m_nLoadFactor;
        //@endcond
    public:
        /// Default ctor, load factor is 4
        load_factor_resizing()
            : m_nLoadFactor(4)
        {}

        /// Ctor with explicitly defined \p nLoadFactor
        explicit load_factor_resizing( size_t nLoadFactor )
            : m_nLoadFactor( nLoadFactor )
        {}

        /// Copy ctor
        load_factor_resizing( load_factor_resizing const& src )
            : m_nLoadFactor( src.m_nLoadFactor )
        {}

        /// Move ctor
        load_factor_resizing( load_factor_resizing&& src )
            : m_nLoadFactor( src.m_nLoadFactor )
        {}

        /// Main policy operator returns \p true when resizing is needed
        template <typename Container, typename Bucket>
        bool operator ()(
            size_t nSize,                   ///< Current item count of \p container
            Container const& container,     ///< Container
            Bucket const& /*bucket*/        ///< reference to a container's bucket (not used)
        )
        {
            return nSize > container.bucket_count() * m_nLoadFactor;
        }

        /// Resets internal state of the policy (does nothing)
        void reset()
        {}
    };

    /// Rational load factor resizing policy
    /** @ingroup cds_striped_resizing_policy
        When total item count in a container exceeds
        <tt>container.bucket_count() * Numerator / Denominator</tt>
        then resizing is needed.

        This policy is stateless: \p Numerator and \p Denominator specifies in compile time as template arguments
    */
    template <size_t Numerator, size_t Denominator = 1>
    struct rational_load_factor_resizing
    {
        static_assert( Denominator != 0, "Denominator must not be zero" );

        /// Main policy operator returns \p true when resizing is needed
        template <typename Container, typename Bucket>
        bool operator ()(
            size_t nSize,                   ///< Current item count of \p container
            Container const& container,     ///< Container
            Bucket const& /*bucket*/        ///< reference to a container's bucket (not used)
        ) const
        {
            return nSize * Denominator > container.bucket_count() * Numerator;
        }

        /// Resets internal state of the policy (does nothing)
        void reset()
        {}
    };

    /// Rational load factor resizing policy
    /** @ingroup cds_striped_resizing_policy
        When total item count in a container exceeds
        <tt>container.bucket_count() * Numerator / Denominator</tt>
        then resizing is needed.

        This policy is stateful: \p Numerator and \p Denominator specifies in construction time.
    */
    template <size_t Denominator>
    struct rational_load_factor_resizing<0, Denominator>
    {
        ///@cond
        const size_t m_nNumerator;
        const size_t m_nDenominator;
        //@endcond
    public:
        /// Default ctor, load factor is 1/2
        rational_load_factor_resizing()
            : m_nNumerator(1), m_nDenominator(2)
        {}

        /// Ctor with explicitly defined \p nLoadFactor
        rational_load_factor_resizing( size_t nNumerator, size_t nDenominator )
            : m_nNumerator( nNumerator ), m_nDenominator( nDenominator )
        {}

        /// Copy ctor
        rational_load_factor_resizing( rational_load_factor_resizing const& src )
            : m_nNumerator( src.m_nNumerator ), m_nDenominator( src.m_nDenominator )
        {}

        /// Move ctor
        rational_load_factor_resizing( rational_load_factor_resizing&& src )
            : m_nNumerator( src.m_nNumerator ), m_nDenominator( src.m_nDenominator )
        {}

        /// Main policy operator returns \p true when resizing is needed
        template <typename Container, typename Bucket>
        bool operator ()(
            size_t nSize,                   ///< Current item count of \p container
            Container const& container,     ///< Container
            Bucket const& /*bucket*/        ///< reference to a container's bucket (not used)
        )
        {
            return nSize * m_nDenominator > container.bucket_count() * m_nNumerator;
        }

        /// Resets internal state of the policy (does nothing)
        void reset()
        {}
    };

    /// Single bucket threshold resizing policy
    /** @ingroup cds_striped_resizing_policy
        If any single bucket size exceeds the global \p Threshold then resizing is needed.

        This policy is stateless.
    */
    template <size_t Threshold>
    struct single_bucket_size_threshold
    {
        /// Main policy operator returns \p true when resizing is needed
        template <typename Container, typename Bucket>
        bool operator ()(
            size_t /*nSize*/,                   ///< Current item count of \p container (not used)
            Container const& /*container*/,     ///< Container (not used)
            Bucket const& bucket                ///< reference to a container's bucket
            ) const
        {
            return bucket.size() > Threshold;
        }

        /// Resets internal state of the policy (does nothing)
        void reset()
        {}
    };


    /// Single bucket threshold resizing policy, stateful specialization
    /** @ingroup cds_striped_resizing_policy
        This specialization allows to specify and modify a threshold at runtime.
    */
    template <>
    struct single_bucket_size_threshold<0>
    {
        size_t  m_nThreshold    ;   ///< The bucket size threshold

        /// Default ctor, the threshold is 4
        single_bucket_size_threshold()
            : m_nThreshold(4)
        {}

        /// Ctor with explicitly defined \p nThreshold
        explicit single_bucket_size_threshold( size_t nThreshold )
            : m_nThreshold( nThreshold )
        {}

        /// Copy ctor
        single_bucket_size_threshold( single_bucket_size_threshold const& src )
            : m_nThreshold( src.m_nThreshold )
        {}

        /// Move ctor
        single_bucket_size_threshold( single_bucket_size_threshold&& src )
            : m_nThreshold( src.m_nThreshold )
        {}

        /// Main policy operator returns \p true when resizing is needed
        template <typename Container, typename Bucket>
        bool operator ()(
            size_t /*nSize*/,                   ///< Current item count of \p container (not used)
            Container const& /*container*/,     ///< Container (not used)
            Bucket const& bucket                ///< reference to a container's bucket
            ) const
        {
            return bucket.size() > m_nThreshold;
        }

        /// Resets internal state of the policy (does nothing)
        void reset()
        {}
    };

    /// Dummy resizing policy
    /** @ingroup cds_striped_resizing_policy
        This policy is dummy and always returns \p false that means no resizing is needed.

        This policy is stateless.
    */
    struct no_resizing
    {
        /// Main policy operator always returns \p false
        template <typename Container, typename Bucket>
        bool operator ()(
            size_t /*nSize*/,                   ///< Current item count of \p container (not used)
            Container const& /*container*/,     ///< Container (not used)
            Bucket const& /*bucket*/            ///< reference to a container's bucket (not used)
        ) const
        {
            return false;
        }

        /// Resets internal state of the policy (does nothing)
        void reset()
        {}
    };

}}} // namespace cds::intrusive::striped_set

#endif // #define CDSLIB_INTRUSIVE_STRIPED_SET_RESIZING_POLICY_H
