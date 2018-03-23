// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_DETAILS_FELDMAN_HASHSET_BASE_H
#define CDSLIB_INTRUSIVE_DETAILS_FELDMAN_HASHSET_BASE_H

#include <memory.h> // memcmp, memcpy
#include <type_traits>

#include <cds/intrusive/details/base.h>
#include <cds/opt/compare.h>
#include <cds/algo/atomic.h>
#include <cds/algo/split_bitstring.h>
#include <cds/details/marked_ptr.h>
#include <cds/urcu/options.h>

namespace cds { namespace intrusive {

    /// FeldmanHashSet related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace feldman_hashset {
        /// Hash accessor option
        /**
            @copydetails traits::hash_accessor
        */
        template <typename Accessor>
        struct hash_accessor {
            //@cond
            template <typename Base> struct pack: public Base
            {
                typedef Accessor hash_accessor;
            };
            //@endcond
        };

        /// Hash size option
        /**
            @copydetails traits::hash_size
        */
        template <size_t Size>
        struct hash_size {
            //@cond
            template <typename Base> struct pack: public Base
            {
                enum: size_t {
                    hash_size = Size
                };
            };
            //@endcond
        };

        /// Hash splitter option
        /**
            @copydetails traits::hash_splitter
        */
        template <typename Splitter>
        struct hash_splitter {
            //@cond
            template <typename Base> struct pack: public Base
            {
                typedef Splitter hash_splitter;
            };
            //@endcond
        };


        /// \p FeldmanHashSet internal statistics
        template <typename EventCounter = cds::atomicity::event_counter>
        struct stat {
            typedef EventCounter event_counter ; ///< Event counter type

            event_counter   m_nInsertSuccess;   ///< Number of success \p insert() operations
            event_counter   m_nInsertFailed;    ///< Number of failed \p insert() operations
            event_counter   m_nInsertRetry;     ///< Number of attempts to insert new item
            event_counter   m_nUpdateNew;       ///< Number of new item inserted for \p update()
            event_counter   m_nUpdateExisting;  ///< Number of existing item updates
            event_counter   m_nUpdateFailed;    ///< Number of failed \p update() call
            event_counter   m_nUpdateRetry;     ///< Number of attempts to update the item
            event_counter   m_nEraseSuccess;    ///< Number of successful \p erase(), \p unlink(), \p extract() operations
            event_counter   m_nEraseFailed;     ///< Number of failed \p erase(), \p unlink(), \p extract() operations
            event_counter   m_nEraseRetry;      ///< Number of attempts to \p erase() an item
            event_counter   m_nFindSuccess;     ///< Number of successful \p find() and \p get() operations
            event_counter   m_nFindFailed;      ///< Number of failed \p find() and \p get() operations

            event_counter   m_nExpandNodeSuccess; ///< Number of succeeded attempts converting data node to array node
            event_counter   m_nExpandNodeFailed;  ///< Number of failed attempts converting data node to array node
            event_counter   m_nSlotChanged;     ///< Number of array node slot changing by other thread during an operation
            event_counter   m_nSlotConverting;  ///< Number of events when we encounter a slot while it is converting to array node

            event_counter   m_nArrayNodeCount;  ///< Number of array nodes
            event_counter   m_nHeight;          ///< Current height of the tree

            //@cond
            void onInsertSuccess()              { ++m_nInsertSuccess;       }
            void onInsertFailed()               { ++m_nInsertFailed;        }
            void onInsertRetry()                { ++m_nInsertRetry;         }
            void onUpdateNew()                  { ++m_nUpdateNew;           }
            void onUpdateExisting()             { ++m_nUpdateExisting;      }
            void onUpdateFailed()               { ++m_nUpdateFailed;        }
            void onUpdateRetry()                { ++m_nUpdateRetry;         }
            void onEraseSuccess()               { ++m_nEraseSuccess;        }
            void onEraseFailed()                { ++m_nEraseFailed;         }
            void onEraseRetry()                 { ++m_nEraseRetry;          }
            void onFindSuccess()                { ++m_nFindSuccess;         }
            void onFindFailed()                 { ++m_nFindFailed;          }

            void onExpandNodeSuccess()          { ++m_nExpandNodeSuccess;   }
            void onExpandNodeFailed()           { ++m_nExpandNodeFailed;    }
            void onSlotChanged()                { ++m_nSlotChanged;         }
            void onSlotConverting()             { ++m_nSlotConverting;      }
            void onArrayNodeCreated()           { ++m_nArrayNodeCount;      }
            void height( size_t h )             { if (m_nHeight < h ) m_nHeight = h; }
            //@endcond
        };

        /// \p FeldmanHashSet empty internal statistics
        struct empty_stat {
            //@cond
            void onInsertSuccess()              const {}
            void onInsertFailed()               const {}
            void onInsertRetry()                const {}
            void onUpdateNew()                  const {}
            void onUpdateExisting()             const {}
            void onUpdateFailed()               const {}
            void onUpdateRetry()                const {}
            void onEraseSuccess()               const {}
            void onEraseFailed()                const {}
            void onEraseRetry()                 const {}
            void onFindSuccess()                const {}
            void onFindFailed()                 const {}

            void onExpandNodeSuccess()          const {}
            void onExpandNodeFailed()           const {}
            void onSlotChanged()                const {}
            void onSlotConverting()             const {}
            void onArrayNodeCreated()           const {}
            void height(size_t)                 const {}
            //@endcond
        };

        /// \p FeldmanHashSet traits
        struct traits
        {
            /// Mandatory functor to get hash value from data node
            /**
                It is most-important feature of \p FeldmanHashSet.
                That functor must return a reference to fixed-sized hash value of data node.
                The return value of that functor specifies the type of hash value.

                Example:
                \code
                typedef uint8_t hash_type[32]; // 256-bit hash type
                struct foo {
                    hash_type  hash; // 256-bit hash value
                    // ... other fields
                };

                // Hash accessor
                struct foo_hash_accessor {
                    hash_type const& operator()( foo const& d ) const
                    {
                        return d.hash;
                    }
                };
                \endcode
            */
            typedef cds::opt::none hash_accessor;

            /// The size of hash value in bytes
            /**
                By default, the size of hash value is <tt>sizeof( hash_type )</tt>.
                Sometimes it is not correct, for example, for that 6-byte struct \p static_assert will be thrown:
                \code
                struct key_type {
                    uint32_t    key1;
                    uint16_t    subkey;
                };

                static_assert( sizeof( key_type ) == 6, "Key type size mismatch" );
                \endcode
                For that case you can specify \p hash_size explicitly.

                Value \p 0 means <tt>sizeof( hash_type )</tt>.
            */
            static constexpr size_t const hash_size = 0;

            /// Hash splitter
            /**
                This trait specifies hash bit-string splitter algorithm.
                By default, \p cds::algo::number_splitter is used if \p HashType is a number,
                \p cds::algo::split_bitstring otherwise.
            */
            typedef cds::opt::none hash_splitter;

            /// Disposer for removing data nodes
            typedef cds::intrusive::opt::v::empty_disposer disposer;

            /// Hash comparing functor
            /**
                No default functor is provided.
                If the option is not specified, the \p less option is used.
            */
            typedef cds::opt::none compare;

            /// Specifies binary predicate used for hash compare.
            /**
                If \p %less and \p %compare are not specified, \p memcmp() -like @ref bitwise_compare "bit-wise hash comparator" is used
                because the hash value is treated as fixed-sized bit-string.
            */
            typedef cds::opt::none less;

            /// Item counter
            /**
                The item counting is an important part of \p FeldmanHashSet algorithm:
                the \p empty() member function depends on correct item counting.
                Therefore, \p atomicity::empty_item_counter is not allowed as a type of the option.

                Default is \p atomicity::item_counter. To avoid false sharing you can aldo use \p atomicity::cache_friendly_item_counter
            */
            typedef cds::atomicity::item_counter item_counter;

            /// Array node allocator
            /**
                Allocator for array nodes. The allocator is used for creating \p headNode and \p arrayNode when the set grows.
                Default is \ref CDS_DEFAULT_ALLOCATOR
            */
            typedef CDS_DEFAULT_ALLOCATOR node_allocator;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef cds::opt::v::relaxed_ordering memory_model;

            /// Back-off strategy
            typedef cds::backoff::Default back_off;

            /// Internal statistics
            /**
                By default, internal statistics is disabled (\p feldman_hashset::empty_stat).
                Use \p feldman_hashset::stat to enable it.
            */
            typedef empty_stat stat;

            /// RCU deadlock checking policy (only for \ref cds_intrusive_FeldmanHashSet_rcu "RCU-based FeldmanHashSet")
            /**
                List of available policy see \p opt::rcu_check_deadlock
            */
            typedef cds::opt::v::rcu_throw_deadlock rcu_check_deadlock;
        };

        /// Metafunction converting option list to \p feldman_hashset::traits
        /**
            Supported \p Options are:
            - \p feldman_hashset::hash_accessor - mandatory option, hash accessor functor.
                @copydetails traits::hash_accessor
            - \p feldman_hashset::hash_size - the size of hash value in bytes.
                @copydetails traits::hash_size
            - \p feldman_hashset::hash_splitter - a hash splitter algorithm
                @copydetails traits::hash_splitter
            - \p opt::node_allocator - array node allocator.
                @copydetails traits::node_allocator
            - \p opt::compare - hash comparison functor. No default functor is provided.
                If the option is not specified, the \p opt::less is used.
            - \p opt::less - specifies binary predicate used for hash comparison.
                If the option is not specified, \p memcmp() -like bit-wise hash comparator is used
                because the hash value is treated as fixed-sized bit-string.
            - \p opt::back_off - back-off strategy used. If the option is not specified, the \p cds::backoff::Default is used.
            - \p opt::disposer - the functor used for disposing removed data node. Default is \p opt::v::empty_disposer. Due the nature
                of GC schema the disposer may be called asynchronously.
            - \p opt::item_counter - the type of item counting feature.
                 The item counting is an important part of \p FeldmanHashSet algorithm:
                 the \p empty() member function depends on correct item counting.
                 Therefore, \p atomicity::empty_item_counter is not allowed as a type of the option.
                 Default is \p atomicity::item_counter. To avoid false sharing you can use or \p atomicity::cache_friendly_item_counter
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            - \p opt::stat - internal statistics. By default, it is disabled (\p feldman_hashset::empty_stat).
                To enable it use \p feldman_hashset::stat
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

        /// Bit-wise memcmp-based comparator for hash value \p T
        template <typename T>
        struct bitwise_compare
        {
            /// Compares \p lhs and \p rhs
            /**
                Returns:
                - <tt> < 0</tt> if <tt>lhs < rhs</tt>
                - <tt>0</tt> if <tt>lhs == rhs</tt>
                - <tt> > 0</tt> if <tt>lhs > rhs</tt>
            */
            int operator()( T const& lhs, T const& rhs ) const
            {
                return memcmp( &lhs, &rhs, sizeof(T));
            }
        };

        /// One-level statistics, see \p FeldmanHashSet::get_level_statistics
        struct level_statistics
        {
            size_t array_node_count;    ///< Count of array node at the level
            size_t node_capacity;       ///< Array capacity

            size_t data_cell_count;     ///< The number of data cells in all array node at this level
            size_t array_cell_count;    ///< The number of array cells in all array node at this level
            size_t empty_cell_count;    ///< The number of empty cells in all array node at this level

            //@cond
            level_statistics()
                : array_node_count(0)
                , data_cell_count(0)
                , array_cell_count(0)
                , empty_cell_count(0)
            {}
            //@endcond
        };

        //@cond
        namespace details {
            template <typename HashType, size_t HashSize >
            using hash_splitter = cds::algo::split_bitstring< HashType, HashSize >;

            struct metrics {
                size_t  head_node_size;     // power-of-two
                size_t  head_node_size_log; // log2( head_node_size )
                size_t  array_node_size;    // power-of-two
                size_t  array_node_size_log;// log2( array_node_size )

                static metrics make(size_t head_bits, size_t array_bits, size_t hash_size )
                {
                    size_t const hash_bits = hash_size * 8;

                    if (array_bits < 2)
                        array_bits = 2;
                    if (head_bits < 4)
                        head_bits = 4;
                    if (head_bits > hash_bits)
                        head_bits = hash_bits;
                    if ((hash_bits - head_bits) % array_bits != 0)
                        head_bits += (hash_bits - head_bits) % array_bits;

                    assert((hash_bits - head_bits) % array_bits == 0);

                    metrics m;
                    m.head_node_size_log = head_bits;
                    m.head_node_size = size_t(1) << head_bits;
                    m.array_node_size_log = array_bits;
                    m.array_node_size = size_t(1) << array_bits;
                    return m;
                }
            };

        } // namespace details
        //@endcond

        //@cond
        template <typename T, typename Traits>
        class multilevel_array
        {
        public:
            typedef T value_type;
            typedef Traits traits;
            typedef typename Traits::node_allocator node_allocator;
            typedef typename traits::memory_model   memory_model;
            typedef typename traits::back_off       back_off;       ///< Backoff strategy
            typedef typename traits::stat           stat;           ///< Internal statistics type

            typedef typename traits::hash_accessor hash_accessor;
            static_assert(!std::is_same< hash_accessor, cds::opt::none >::value, "hash_accessor functor must be specified");

            /// Hash type deduced from \p hash_accessor return type
            typedef typename std::decay<
                typename std::remove_reference<
                    decltype(hash_accessor()(std::declval<value_type>()))
                >::type
            >::type hash_type;
            static_assert(!std::is_pointer<hash_type>::value, "hash_accessor should return a reference to hash value");

            typedef typename cds::opt::details::make_comparator_from<
                hash_type,
                traits,
                feldman_hashset::bitwise_compare< hash_type >
            >::type hash_comparator;

            /// The size of hash_type in bytes, see \p traits::hash_size for explanation
            static constexpr size_t const c_hash_size = traits::hash_size == 0 ? sizeof( hash_type ) : static_cast<size_t>( traits::hash_size );

            typedef typename std::conditional<
                std::is_same< typename traits::hash_splitter, cds::opt::none >::value,
                typename cds::algo::select_splitter< hash_type, c_hash_size >::type,
                typename traits::hash_splitter
            >::type hash_splitter;

            enum node_flags {
                flag_array_converting = 1,   ///< the cell is converting from data node to an array node
                flag_array_node = 2          ///< the cell is a pointer to an array node
            };

        protected:

            typedef cds::details::marked_ptr< value_type, 3 > node_ptr;
            typedef atomics::atomic< node_ptr > atomic_node_ptr;

            struct array_node {
                array_node * const  pParent;    ///< parent array node
                size_t const        idxParent;  ///< index in parent array node
                atomic_node_ptr     nodes[1];   ///< node array

                array_node(array_node * parent, size_t idx)
                    : pParent(parent)
                    , idxParent(idx)
                {}

                array_node() = delete;
                array_node(array_node const&) = delete;
                array_node(array_node&&) = delete;
            };

            typedef cds::details::Allocator< array_node, node_allocator > cxx_array_node_allocator;

            struct traverse_data {
                hash_splitter splitter;
                array_node * pArr;
                typename hash_splitter::uint_type nSlot;
                size_t nHeight;

                traverse_data( hash_type const& hash, multilevel_array& arr )
                    : splitter( hash )
                {
                    reset( arr );
                }

                void reset( multilevel_array& arr )
                {
                    splitter.reset();
                    pArr = arr.head();
                    nSlot = splitter.cut( static_cast<unsigned>( arr.metrics().head_node_size_log ));
                    assert( static_cast<size_t>( nSlot ) < arr.metrics().head_node_size );
                    nHeight = 1;
                }
            };

        protected:
            feldman_hashset::details::metrics const m_Metrics;
            array_node *      m_Head;
            mutable stat      m_Stat;

        public:
            multilevel_array(size_t head_bits, size_t array_bits )
                : m_Metrics(feldman_hashset::details::metrics::make( head_bits, array_bits, c_hash_size ))
                , m_Head( alloc_head_node())
            {
                assert( hash_splitter::is_correct( static_cast<unsigned>( metrics().head_node_size_log )));
                assert( hash_splitter::is_correct( static_cast<unsigned>( metrics().array_node_size_log )));
            }

            ~multilevel_array()
            {
                destroy_tree();
                free_array_node( m_Head, head_size());
            }

            node_ptr traverse(traverse_data& pos)
            {
                back_off bkoff;
                while (true) {
                    node_ptr slot = pos.pArr->nodes[pos.nSlot].load(memory_model::memory_order_acquire);
                    if ( slot.bits() == flag_array_node ) {
                        // array node, go down the tree
                        assert(slot.ptr() != nullptr);
                        assert( !pos.splitter.eos());
                        pos.nSlot = pos.splitter.cut( static_cast<unsigned>( metrics().array_node_size_log ));
                        assert( static_cast<size_t>( pos.nSlot ) < metrics().array_node_size );
                        pos.pArr = to_array(slot.ptr());
                        ++pos.nHeight;
                    }
                    else if (slot.bits() == flag_array_converting) {
                        // the slot is converting to array node right now
                        bkoff();
                        stats().onSlotConverting();
                    }
                    else {
                        // data node
                        assert(slot.bits() == 0);
                        return slot;
                    }
                } // while
            }

            size_t head_size() const
            {
                return m_Metrics.head_node_size;
            }

            size_t array_node_size() const
            {
                return m_Metrics.array_node_size;
            }

            void get_level_statistics(std::vector< feldman_hashset::level_statistics>& stat) const
            {
                stat.clear();
                gather_level_statistics(stat, 0, m_Head, head_size());
            }

        protected:
            array_node * head() const
            {
                return m_Head;
            }

            stat& stats() const
            {
                return m_Stat;
            }

            feldman_hashset::details::metrics const& metrics() const
            {
                return m_Metrics;
            }

            void destroy_tree()
            {
                // The function is not thread-safe. For use in dtor only
                // Destroy all array nodes
                destroy_array_nodes(m_Head, head_size());
            }

            void destroy_array_nodes(array_node * pArr, size_t nSize)
            {
                for (atomic_node_ptr * p = pArr->nodes, *pLast = p + nSize; p != pLast; ++p) {
                    node_ptr slot = p->load(memory_model::memory_order_relaxed);
                    if (slot.bits() == flag_array_node) {
                        destroy_array_nodes( to_array(slot.ptr()), array_node_size());
                        free_array_node( to_array( slot.ptr()), array_node_size());
                        p->store(node_ptr(), memory_model::memory_order_relaxed);
                    }
                }
            }

            static array_node * alloc_array_node(size_t nSize, array_node * pParent, size_t idxParent)
            {
                array_node * pNode = cxx_array_node_allocator().NewBlock(sizeof(array_node) + sizeof(atomic_node_ptr) * (nSize - 1), pParent, idxParent);
                new (pNode->nodes) atomic_node_ptr[nSize];
                return pNode;
            }

            array_node * alloc_head_node() const
            {
                return alloc_array_node(head_size(), nullptr, 0);
            }

            array_node * alloc_array_node(array_node * pParent, size_t idxParent) const
            {
                return alloc_array_node(array_node_size(), pParent, idxParent);
            }

            static void free_array_node( array_node * parr, size_t /*nSize*/ )
            {
                cxx_array_node_allocator().Delete( parr, 1 );
            }

            union converter {
                value_type * pData;
                array_node * pArr;

                converter(value_type * p)
                    : pData(p)
                {}

                converter(array_node * p)
                    : pArr(p)
                {}
            };

            static array_node * to_array(value_type * p)
            {
                return converter(p).pArr;
            }
            static value_type * to_node(array_node * p)
            {
                return converter(p).pData;
            }

            void gather_level_statistics(std::vector<feldman_hashset::level_statistics>& stat, size_t nLevel, array_node * pArr, size_t nSize) const
            {
                if (stat.size() <= nLevel) {
                    stat.resize(nLevel + 1);
                    stat[nLevel].node_capacity = nSize;
                }

                ++stat[nLevel].array_node_count;
                for (atomic_node_ptr * p = pArr->nodes, *pLast = p + nSize; p != pLast; ++p) {
                    node_ptr slot = p->load(memory_model::memory_order_relaxed);
                    if (slot.bits()) {
                        ++stat[nLevel].array_cell_count;
                        if (slot.bits() == flag_array_node)
                            gather_level_statistics(stat, nLevel + 1, to_array(slot.ptr()), array_node_size());
                    }
                    else if (slot.ptr())
                        ++stat[nLevel].data_cell_count;
                    else
                        ++stat[nLevel].empty_cell_count;
                }
            }

            bool expand_slot( traverse_data& pos, node_ptr current)
            {
                assert( !pos.splitter.eos());
                return expand_slot( pos.pArr, pos.nSlot, current, pos.splitter.bit_offset());
            }

        private:
            bool expand_slot(array_node * pParent, size_t idxParent, node_ptr current, size_t nOffset)
            {
                assert(current.bits() == 0);
                assert(current.ptr());

                array_node * pArr = alloc_array_node(pParent, idxParent);

                node_ptr cur(current.ptr());
                atomic_node_ptr& slot = pParent->nodes[idxParent];
                if (!slot.compare_exchange_strong(cur, cur | flag_array_converting, memory_model::memory_order_release, atomics::memory_order_relaxed))
                {
                    stats().onExpandNodeFailed();
                    free_array_node( pArr, array_node_size());
                    return false;
                }

                typename hash_splitter::uint_type idx = hash_splitter( hash_accessor()(*current.ptr()), nOffset ).cut(
                    static_cast<unsigned>( m_Metrics.array_node_size_log ));
                pArr->nodes[idx].store(current, memory_model::memory_order_release);

                cur = cur | flag_array_converting;
                CDS_VERIFY(
                    slot.compare_exchange_strong(cur, node_ptr(to_node(pArr), flag_array_node), memory_model::memory_order_release, atomics::memory_order_relaxed)
                    );

                stats().onExpandNodeSuccess();
                stats().onArrayNodeCreated();
                return true;
            }
        };
        //@endcond
    } // namespace feldman_hashset

    //@cond
    // Forward declaration
    template < class GC, typename T, class Traits = feldman_hashset::traits >
    class FeldmanHashSet;
    //@endcond

}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_DETAILS_FELDMAN_HASHSET_BASE_H
