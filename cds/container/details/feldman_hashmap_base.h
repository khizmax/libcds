// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_FELDMAN_HASHMAP_BASE_H
#define CDSLIB_CONTAINER_DETAILS_FELDMAN_HASHMAP_BASE_H

#include <cds/intrusive/details/feldman_hashset_base.h>
#include <cds/container/details/base.h>
#include <cds/opt/hash.h>

namespace cds { namespace container {
    /// \p FeldmanHashMap related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace feldman_hashmap {
        /// \p FeldmanHashMap internal statistics, see cds::intrusive::feldman_hashset::stat
        template <typename EventCounter = cds::atomicity::event_counter>
        using stat = cds::intrusive::feldman_hashset::stat< EventCounter >;

        /// \p FeldmanHashMap empty internal statistics
        typedef cds::intrusive::feldman_hashset::empty_stat empty_stat;

        /// Bit-wise memcmp-based comparator for hash value \p T
        template <typename T>
        using bitwise_compare = cds::intrusive::feldman_hashset::bitwise_compare< T >;

        /// \p FeldmanHashMap level statistics
        typedef cds::intrusive::feldman_hashset::level_statistics level_statistics;

        /// Key size option
        /**
            @copydetails cds::container::feldman_hashmap::traits::hash_size
        */
        template <size_t Size>
        using hash_size = cds::intrusive::feldman_hashset::hash_size< Size >;

        /// Hash splitter option
        /**
            @copydetails cds::container::feldman_hashmap::traits::hash_splitter
        */
        template <typename Splitter>
        using hash_splitter = cds::intrusive::feldman_hashset::hash_splitter< Splitter >;


        /// \p FeldmanHashMap traits
        struct traits
        {
            /// Hash functor, default is \p opt::none
            /**
                \p FeldmanHashMap may use any hash functor converting a key to
                fixed-sized bit-string, for example, <a href="https://en.wikipedia.org/wiki/Secure_Hash_Algorithm">SHA1, SHA2</a>,
                <a href="https://en.wikipedia.org/wiki/MurmurHash">MurmurHash</a>,
                <a href="https://en.wikipedia.org/wiki/CityHash">CityHash</a>
                or its successor <a href="https://code.google.com/p/farmhash/">FarmHash</a>.

                If you use a fixed-sized key you can use it directly instead of a hash.
                In such case \p %traits::hash should be specified as \p opt::none.
                However, if you want to use the hash values or if your key type is not fixed-sized
                you must specify a proper hash functor in your traits.
                For example:
                fixed-sized key - IP4 address map
                @code
                    // Key - IP address
                    struct ip4_address {
                        uint8_t ip[4];
                    };

                    // IP compare
                    struct ip4_cmp {
                        int operator()( ip4_address const& lhs, ip4_address const& rhs ) const
                        {
                            return memcmp( &lhs, &rhs, sizeof(lhs));
                        }
                    };

                    // Value - statistics for the IP address
                    struct statistics {
                        // ...
                    };

                    // Traits
                    // Key type (ip4_addr) is fixed-sized so we may use the map without any hash functor
                    struct ip4_map_traits: public cds::container::multilevl_hashmap::traits
                    {
                        typedef ip4_cmp  compare;
                    };

                    // IP4 address - statistics map
                    typedef cds::container::FeldmanHashMap< cds::gc::HP, ip4_address, statistics, ip4_map_traits > ip4_map;
                @endcode

                variable-size key requires a hash functor: URL map
                @code
                    // Value - statistics for the URL
                    struct statistics {
                        // ...
                    };

                    // Traits
                    // Key type (std::string) is variable-sized so we must provide a hash functor in our traits
                    // We do not specify any comparing predicate (less or compare) so <tt> std::less<std::string> </tt> will be used by default
                    struct url_map_traits: public cds::container::multilevl_hashmap::traits
                    {
                        typedef std::hash<std::string> hash;
                    };

                    // URL statistics map
                    typedef cds::container::FeldmanHashMap< cds::gc::HP, std::string, statistics, url_map_traits > url_map;
                @endcode
            */
            typedef opt::none hash;

            /// The size of hash value in bytes
            /**
                By default, the size of hash value is <tt>sizeof( hash_type )</tt>
                where \p hash_type is type of \p hash() result or <tt>sizeof( key )</tt> if you use fixed-sized key.

                Sometimes that size is wrong, for example, for that 6-byte key:
                \code
                struct key_type {
                    uint32_t    key;
                    uint16_t    subkey;
                };

                static_assert( sizeof( key_type ) == 6, "Key type size mismatch" );
                \endcode
                Here <tt>sizeof( key_type ) == 8</tt> so \p static_assert will be thrown.

                For that case you can specify \p hash_size explicitly.

                Value \p 0 means auto-calculated <tt>sizeof( key_type )</tt>.
            */
            static constexpr size_t const hash_size = 0;

            /// Hash splitter
            /**
                @copydetails cds::intrusive::feldman_hashset::traits::hash_splitter
            */
            typedef cds::opt::none hash_splitter;

            /// Hash comparing functor
            /**
                @copydetails cds::intrusive::feldman_hashset::traits::compare
            */
            typedef cds::opt::none compare;

            /// Specifies binary predicate used for hash compare.
            /**
                @copydetails cds::intrusive::feldman_hashset::traits::less
            */
            typedef cds::opt::none less;

            /// Item counter
            /**
                @copydetails cds::intrusive::feldman_hashset::traits::item_counter
            */
            typedef cds::atomicity::item_counter item_counter;

            /// Item allocator
            /**
                Default is \ref CDS_DEFAULT_ALLOCATOR
            */
            typedef CDS_DEFAULT_ALLOCATOR allocator;

            /// Array node allocator
            /**
                @copydetails cds::intrusive::feldman_hashset::traits::node_allocator
            */
            typedef CDS_DEFAULT_ALLOCATOR node_allocator;

            /// C++ memory ordering model
            /**
                @copydetails cds::intrusive::feldman_hashset::traits::memory_model
            */
            typedef cds::opt::v::relaxed_ordering memory_model;

            /// Back-off strategy
            typedef cds::backoff::Default back_off;

            /// Internal statistics
            /**
                @copydetails cds::intrusive::feldman_hashset::traits::stat
            */
            typedef empty_stat stat;

            /// RCU deadlock checking policy (only for \ref cds_container_FeldmanHashMap_rcu "RCU-based FeldmanHashMap")
            /**
                @copydetails cds::intrusive::feldman_hashset::traits::rcu_check_deadlock
            */
            typedef cds::opt::v::rcu_throw_deadlock rcu_check_deadlock;
        };

        /// Metafunction converting option list to \p feldman_hashmap::traits
        /**
            Supported \p Options are:
            - \p opt::hash - a hash functor, default is \p std::hash
                @copydetails traits::hash
            - \p feldman_hashmap::hash_size - the size of hash value in bytes.
                @copydetails traits::hash_size
            - \p opt::allocator - item allocator
                @copydetails traits::allocator
            - \p opt::node_allocator - array node allocator.
                @copydetails traits::node_allocator
            - \p opt::compare - hash comparison functor. No default functor is provided.
                If the option is not specified, the \p opt::less is used.
            - \p opt::less - specifies binary predicate used for hash comparison.
                @copydetails cds::container::feldman_hashmap::traits::less
            - \p opt::back_off - back-off strategy used. If the option is not specified, the \p cds::backoff::Default is used.
            - \p opt::item_counter - the type of item counting feature.
                @copydetails cds::container::feldman_hashmap::traits::item_counter
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            - \p opt::stat - internal statistics. By default, it is disabled (\p feldman_hashmap::empty_stat).
                To enable it use \p feldman_hashmap::stat
            - \p opt::rcu_check_deadlock - a deadlock checking policy for \ref cds_intrusive_FeldmanHashSet_rcu "RCU-based FeldmanHashSet"
                Default is \p opt::v::rcu_throw_deadlock
        */
        template <typename... Options>
        struct make_traits
        {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                ,Options...
            >::type   type;
#   endif
        };
    } // namespace feldman_hashmap

    //@cond
    // Forward declaration
    template < class GC, typename Key, typename T, class Traits = feldman_hashmap::traits >
    class FeldmanHashMap;
    //@endcond

    //@cond
    namespace details {

        template <typename Key, typename Value, typename Hash>
        struct hash_selector
        {
            typedef Key key_type;
            typedef Value mapped_type;
            typedef Hash hasher;

            typedef typename std::decay<
                typename std::remove_reference<
                decltype(hasher()(std::declval<key_type>()))
                >::type
            >::type hash_type;

            struct node_type
            {
                std::pair< key_type const, mapped_type> m_Value;
                hash_type const m_hash;

                node_type() = delete;
                node_type(node_type const&) = delete;

                template <typename Q>
                node_type(hasher& h, Q const& key)
                    : m_Value( std::move( std::make_pair( key_type( key ), mapped_type())))
                    , m_hash( h( m_Value.first ))
                {}

                template <typename Q, typename U >
                node_type(hasher& h, Q const& key, U const& val)
                    : m_Value( std::move( std::make_pair( key_type( key ), mapped_type(val))))
                    , m_hash( h( m_Value.first ))
                {}

                template <typename Q, typename... Args>
                node_type(hasher& h, Q&& key, Args&&... args)
                    : m_Value( std::move(std::make_pair( key_type( std::forward<Q>(key)), std::move( mapped_type(std::forward<Args>(args)...)))))
                    , m_hash( h( m_Value.first ))
                {}
            };

            struct hash_accessor
            {
                hash_type const& operator()(node_type const& node) const
                {
                    return node.m_hash;
                }
            };
        };

        template <typename Key, typename Value>
        struct hash_selector<Key, Value, opt::none>
        {
            typedef Key key_type;
            typedef Value mapped_type;

            struct hasher {
                key_type const& operator()(key_type const& k) const
                {
                    return k;
                }
            };
            typedef key_type hash_type;

            struct node_type
            {
                std::pair< key_type const, mapped_type> m_Value;

                node_type() = delete;
                node_type(node_type const&) = delete;

                template <typename Q, typename... Args>
                node_type( hasher /*h*/, Q&& key, Args&&... args )
                    : m_Value( std::make_pair( key_type( std::forward<Q>( key )), mapped_type( std::forward<Args>(args)...)))
                {}
            };

            struct hash_accessor
            {
                hash_type const& operator()(node_type const& node) const
                {
                    return node.m_Value.first;
                }
            };
        };

        template <typename GC, typename Key, typename T, typename Traits>
        struct make_feldman_hashmap
        {
            typedef GC      gc;
            typedef Key     key_type;
            typedef T       mapped_type;
            typedef Traits  original_traits;


            typedef hash_selector< key_type, mapped_type, typename original_traits::hash > select;
            typedef typename select::hasher    hasher;
            typedef typename select::hash_type hash_type;
            typedef typename select::node_type node_type;

            typedef cds::details::Allocator< node_type, typename original_traits::allocator > cxx_node_allocator;

            struct node_disposer
            {
                void operator()( node_type * p ) const
                {
                    cxx_node_allocator().Delete( p );
                }
            };

            struct intrusive_traits: public original_traits
            {
                typedef typename select::hash_accessor hash_accessor;
                typedef node_disposer disposer;
            };

            // Metafunction result
            typedef cds::intrusive::FeldmanHashSet< GC, node_type, intrusive_traits > type;
        };
    } // namespace details
    //@endcond

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_DETAILS_FELDMAN_HASHMAP_BASE_H
