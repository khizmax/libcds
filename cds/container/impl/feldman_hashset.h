// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_IMPL_FELDMAN_HASHSET_H
#define CDSLIB_CONTAINER_IMPL_FELDMAN_HASHSET_H

#include <cds/intrusive/impl/feldman_hashset.h>
#include <cds/container/details/feldman_hashset_base.h>

namespace cds { namespace container {

    /// Hash set based on multi-level array
    /** @ingroup cds_nonintrusive_set
        @anchor cds_container_FeldmanHashSet_hp

        Source:
        - [2013] Steven Feldman, Pierre LaBorde, Damian Dechev "Concurrent Multi-level Arrays:
                 Wait-free Extensible Hash Maps"

        [From the paper] The hardest problem encountered while developing a parallel hash map is how to perform
        a global resize, the process of redistributing the elements in a hash map that occurs when adding new
        buckets. The negative impact of blocking synchronization is multiplied during a global resize, because all
        threads will be forced to wait on the thread that is performing the involved process of resizing the hash map
        and redistributing the elements. \p %FeldmanHashSet implementation avoids global resizes through new array
        allocation. By allowing concurrent expansion this structure is free from the overhead of an explicit resize,
        which facilitates concurrent operations.

        The presented design includes dynamic hashing, the use of sub-arrays within the hash map data structure;
        which, in combination with <b>perfect hashing</b>, means that each element has a unique final, as well as current, position.
        It is important to note that the perfect hash function required by our hash map is trivial to realize as
        any hash function that permutes the bits of the key is suitable. This is possible because of our approach
        to the hash function; we require that it produces hash values that are equal in size to that of the key.
        We know that if we expand the hash map a fixed number of times there can be no collision as duplicate keys
        are not provided for in the standard semantics of a hash map.

        \p %FeldmanHashSet is a multi-level array which has an internal structure similar to a tree:
        @image html feldman_hashset.png
        The multi-level array differs from a tree in that each position on the tree could hold an array of nodes or a single node.
        A position that holds a single node is a \p dataNode which holds the hash value of a key and the value that is associated
        with that key; it is a simple struct holding two variables. A \p dataNode in the multi-level array could be marked.
        A \p markedDataNode refers to a pointer to a \p dataNode that has been bitmarked at the least significant bit (LSB)
        of the pointer to the node. This signifies that this \p dataNode is contended. An expansion must occur at this node;
        any thread that sees this \p markedDataNode will try to replace it with an \p arrayNode; which is a position that holds
        an array of nodes. The pointer to an \p arrayNode is differentiated from that of a pointer to a \p dataNode by a bitmark
        on the second-least significant bit.

        \p %FeldmanHashSet multi-level array is similar to a tree in that we keep a pointer to the root, which is a memory array
        called \p head. The length of the \p head memory array is unique, whereas every other \p arrayNode has a uniform length;
        a normal \p arrayNode has a fixed power-of-two length equal to the binary logarithm of a variable called \p arrayLength.
        The maximum depth of the tree, \p maxDepth, is the maximum number of pointers that must be followed to reach any node.
        We define \p currentDepth as the number of memory arrays that we need to traverse to reach the \p arrayNode on which
        we need to operate; this is initially one, because of \p head.

        That approach to the structure of the hash set uses an extensible hashing scheme; <b> the hash value is treated as a bit
        string</b> and rehash incrementally.

        @note Two important things you should keep in mind when you're using \p %FeldmanHashSet:
        - all keys must be fixed-size. It means that you cannot use \p std::string as a key for \p %FeldmanHashSet.
          Instead, for the strings you should use well-known hashing algorithms like <a href="https://en.wikipedia.org/wiki/Secure_Hash_Algorithm">SHA1, SHA2</a>,
          <a href="https://en.wikipedia.org/wiki/MurmurHash">MurmurHash</a>, <a href="https://en.wikipedia.org/wiki/CityHash">CityHash</a>
          or its successor <a href="https://code.google.com/p/farmhash/">FarmHash</a> and so on, which
          converts variable-length strings to fixed-length bit-strings, and use that hash as a key in \p %FeldmanHashSet.
        - \p %FeldmanHashSet uses a perfect hashing. It means that if two different keys, for example, of type \p std::string,
          have identical hash then you cannot insert both that keys in the set. \p %FeldmanHashSet does not maintain the key,
          it maintains its fixed-size hash value.

        The set supports @ref cds_container_FeldmanHashSet_iterators "bidirectional thread-safe iterators".

        Template parameters:
        - \p GC - safe memory reclamation schema. Can be \p gc::HP, \p gc::DHP or one of \ref cds_urcu_type "RCU type"
        - \p T - a value type to be stored in the set
        - \p Traits - type traits, the structure based on \p feldman_hashset::traits or result of \p feldman_hashset::make_traits metafunction.
            \p Traits is the mandatory argument because it has one mandatory type - an @ref feldman_hashset::traits::hash_accessor "accessor"
            to hash value of \p T. The set algorithm does not calculate that hash value.

        There are several specializations of \p %FeldmanHashSet for each \p GC. You should include:
        - <tt><cds/container/feldman_hashset_hp.h></tt> for \p gc::HP garbage collector
        - <tt><cds/container/feldman_hashset_dhp.h></tt> for \p gc::DHP garbage collector
        - <tt><cds/container/feldman_hashset_rcu.h></tt> for \ref cds_intrusive_FeldmanHashSet_rcu "RCU type". RCU specialization
            has a slightly different interface.
    */
    template <
        class GC
        , typename T
#ifdef CDS_DOXYGEN_INVOKED
        , class Traits = feldman_hashset::traits
#else
        , class Traits
#endif
    >
    class FeldmanHashSet
#ifdef CDS_DOXYGEN_INVOKED
        : protected cds::intrusive::FeldmanHashSet< GC, T, Traits >
#else
        : protected cds::container::details::make_feldman_hashset< GC, T, Traits >::type
#endif
    {
        //@cond
        typedef cds::container::details::make_feldman_hashset< GC, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond

    public:
        typedef GC      gc;         ///< Garbage collector
        typedef T       value_type; ///< type of value stored in the set
        typedef Traits  traits;     ///< Traits template parameter, see \p feldman_hashset::traits

        typedef typename base_class::hash_accessor hash_accessor; ///< Hash accessor functor
        typedef typename base_class::hash_type hash_type; ///< Hash type deduced from \p hash_accessor return type
        typedef typename base_class::hash_comparator hash_comparator; ///< hash compare functor based on \p opt::compare and \p opt::less option setter

        typedef typename traits::item_counter   item_counter;   ///< Item counter type
        typedef typename traits::allocator      allocator;      ///< Element allocator
        typedef typename traits::node_allocator node_allocator; ///< Array node allocator
        typedef typename traits::memory_model   memory_model;   ///< Memory model
        typedef typename traits::back_off       back_off;       ///< Backoff strategy
        typedef typename traits::stat           stat;           ///< Internal statistics type

        typedef typename gc::template guarded_ptr< value_type > guarded_ptr; ///< Guarded pointer

        /// Count of hazard pointers required
        static constexpr size_t const c_nHazardPtrCount = base_class::c_nHazardPtrCount;

        /// The size of \p hash_type in bytes, see \p feldman_hashset::traits::hash_size for explanation
        static constexpr size_t const c_hash_size = base_class::c_hash_size;

        /// Level statistics
        typedef feldman_hashset::level_statistics level_statistics;

    protected:
        //@cond
        typedef typename maker::cxx_node_allocator cxx_node_allocator;
        typedef std::unique_ptr< value_type, typename maker::node_disposer > scoped_node_ptr;
        //@endcond

    public:
    ///@name Thread-safe iterators
    ///@{
        /// Bidirectional iterator
        /** @anchor cds_container_FeldmanHashSet_iterators
            The set supports thread-safe iterators: you may iterate over the set in multi-threaded environment.
            It is guaranteed that the iterators will remain valid even if another thread deletes the node the iterator points to:
            Hazard Pointer embedded into the iterator object protects the node from physical reclamation.

            @note Since the iterator object contains hazard pointer that is a thread-local resource,
            the iterator should not be passed to another thread.

            Each iterator object supports the following interface:
            - dereference operators:
                @code
                value_type [const] * operator ->() noexcept
                value_type [const] & operator *() noexcept
                @endcode
            - pre-increment and pre-decrement. Post-operators is not supported
            - equality operators <tt>==</tt> and <tt>!=</tt>.
                Iterators are equal iff they point to the same cell of the same array node.
                Note that for two iterators \p it1 and \p it2, the conditon <tt> it1 == it2 </tt>
                does not entail <tt> &(*it1) == &(*it2) </tt>
            - helper member function \p release() that clears internal hazard pointer.
                After \p release() the iterator points to \p nullptr but it still remain valid: further iterating is possible.

            During iteration you may safely erase any item from the set;
            @ref erase_at() function call doesn't invalidate any iterator.
            If some iterator points to the item to be erased, that item is not deleted immediately
            but only after that iterator will be advanced forward or backward.

            @note It is possible the item can be iterated more that once, for example, if an iterator points to the item
            in array node that is being splitted.
        */
        typedef typename base_class::iterator               iterator;
        typedef typename base_class::const_iterator         const_iterator; ///< @ref cds_container_FeldmanHashSet_iterators "bidirectional const iterator" type
        typedef typename base_class::reverse_iterator       reverse_iterator;       ///< @ref cds_container_FeldmanHashSet_iterators "bidirectional reverse iterator" type
        typedef typename base_class::const_reverse_iterator const_reverse_iterator; ///< @ref cds_container_FeldmanHashSet_iterators "bidirectional reverse const iterator" type

        /// Returns an iterator to the beginning of the set
        iterator begin()
        {
            return base_class::begin();
        }

        /// Returns an const iterator to the beginning of the set
        const_iterator begin() const
        {
            return base_class::begin();
        }

        /// Returns an const iterator to the beginning of the set
        const_iterator cbegin()
        {
            return base_class::cbegin();
        }

        /// Returns an iterator to the element following the last element of the set. This element acts as a placeholder; attempting to access it results in undefined behavior.
        iterator end()
        {
            return base_class::end();
        }

        /// Returns a const iterator to the element following the last element of the set. This element acts as a placeholder; attempting to access it results in undefined behavior.
        const_iterator end() const
        {
            return base_class::end();
        }

        /// Returns a const iterator to the element following the last element of the set. This element acts as a placeholder; attempting to access it results in undefined behavior.
        const_iterator cend()
        {
            return base_class::cend();
        }

        /// Returns a reverse iterator to the first element of the reversed set
        reverse_iterator rbegin()
        {
            return base_class::rbegin();
        }

        /// Returns a const reverse iterator to the first element of the reversed set
        const_reverse_iterator rbegin() const
        {
            return base_class::rbegin();
        }

        /// Returns a const reverse iterator to the first element of the reversed set
        const_reverse_iterator crbegin()
        {
            return base_class::crbegin();
        }

        /// Returns a reverse iterator to the element following the last element of the reversed set
        /**
            It corresponds to the element preceding the first element of the non-reversed container.
            This element acts as a placeholder, attempting to access it results in undefined behavior.
        */
        reverse_iterator rend()
        {
            return base_class::rend();
        }

        /// Returns a const reverse iterator to the element following the last element of the reversed set
        /**
            It corresponds to the element preceding the first element of the non-reversed container.
            This element acts as a placeholder, attempting to access it results in undefined behavior.
        */
        const_reverse_iterator rend() const
        {
            return base_class::rend();
        }

        /// Returns a const reverse iterator to the element following the last element of the reversed set
        /**
            It corresponds to the element preceding the first element of the non-reversed container.
            This element acts as a placeholder, attempting to access it results in undefined behavior.
        */
        const_reverse_iterator crend()
        {
            return base_class::crend();
        }
    ///@}

    public:
        /// Creates empty set
        /**
            @param head_bits - 2<sup>head_bits</sup> specifies the size of head array, minimum is 4.
            @param array_bits - 2<sup>array_bits</sup> specifies the size of array node, minimum is 2.

            Equation for \p head_bits and \p array_bits:
            \code
            sizeof(hash_type) * 8 == head_bits + N * array_bits
            \endcode
            where \p N is multi-level array depth.
        */
        FeldmanHashSet( size_t head_bits = 8, size_t array_bits = 4 )
            : base_class( head_bits, array_bits )
        {}

        /// Destructs the set and frees all data
        ~FeldmanHashSet()
        {}

        /// Inserts new element
        /**
            The function creates an element with copy of \p val value and then inserts it into the set.

            The type \p Q should contain as minimum the complete hash for the element.
            The object of \ref value_type should be constructible from a value of type \p Q.
            In trivial case, \p Q is equal to \ref value_type.

            Returns \p true if \p val is inserted into the set, \p false otherwise.
        */
        template <typename Q>
        bool insert( Q const& val )
        {
            scoped_node_ptr sp( cxx_node_allocator().New( val ));
            if ( base_class::insert( *sp )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Inserts new element
        /**
            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls \p f functor to initialize value-fields of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
            \p val no any other changes could be made on this set's item by concurrent threads.
            The user-defined functor is called only if the inserting is success.
        */
        template <typename Q, typename Func>
        bool insert( Q const& val, Func f )
        {
            scoped_node_ptr sp( cxx_node_allocator().New( val ));
            if ( base_class::insert( *sp, f )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Updates the element
        /**
            The operation performs inserting or replacing with lock-free manner.

            If the \p val key not found in the set, then the new item created from \p val
            will be inserted into the set iff \p bInsert is \p true.
            Otherwise, if \p val is found, it is replaced with new item created from \p val
            and previous item is disposed.
            In both cases \p func functor is called.

            The functor \p Func signature:
            \code
                struct my_functor {
                    void operator()( value_type& cur, value_type * prev );
                };
            \endcode
            where:
            - \p cur - current element
            - \p prev - pointer to previous element with such hash. \p prev is \p nullptr
                 if \p cur was just inserted.

            The functor may change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successful,
            i.e. the item has been inserted or updated,
            \p second is \p true if the new item has been added or \p false if the item with key equal to \p val
            already exists.
        */
        template <typename Q, typename Func>
        std::pair<bool, bool> update( Q const& val, Func func, bool bInsert = true )
        {
            scoped_node_ptr sp( cxx_node_allocator().New( val ));
            std::pair<bool, bool> bRes = base_class::do_update( *sp, func, bInsert );
            if ( bRes.first )
                sp.release();
            return bRes;
        }

        /// Inserts data of type \p value_type created in-place from <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_node_ptr sp( cxx_node_allocator().MoveNew( std::forward<Args>(args)... ));
            if ( base_class::insert( *sp )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Deletes the item from the set
        /**
            The function searches \p hash in the set,
            deletes the item found, and returns \p true.
            If that item is not found the function returns \p false.
        */
        bool erase( hash_type const& hash )
        {
            return base_class::erase( hash );
        }

        /// Deletes the item from the set
        /**
            The function searches \p hash in the set,
            call \p f functor with item found, and deltes the element from the set.

            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode

            If \p hash is not found the function returns \p false.
        */
        template <typename Func>
        bool erase( hash_type const& hash, Func f )
        {
            return base_class::erase( hash, f );
        }

        /// Deletes the item pointed by iterator \p iter
        /**
            Returns \p true if the operation is successful, \p false otherwise.

            The function does not invalidate the iterator, it remains valid and can be used for further traversing.
        */
        bool erase_at( iterator const& iter )
        {
            return base_class::erase_at( iter );
        }
        //@cond
        bool erase_at( reverse_iterator const& iter )
        {
            return base_class::erase_at( iter );
        }
        //@endcond

        /// Extracts the item with specified \p hash
        /**
            The function searches \p hash in the set,
            unlinks it from the set, and returns a guarded pointer to the item extracted.
            If \p hash is not found the function returns an empty guarded pointer.

            The item returned is reclaimed by garbage collector \p GC
            when returned \ref guarded_ptr object to be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::container::FeldmanHashSet< your_template_args > my_set;
            my_set theSet;
            // ...
            {
                my_set::guarded_ptr gp( theSet.extract( 5 ));
                if ( gp ) {
                    // Deal with gp
                    // ...
                }
                // Destructor of gp releases internal HP guard
            }
            \endcode
        */
        guarded_ptr extract( hash_type const& hash )
        {
            return base_class::extract( hash );
        }

        /// Finds an item by it's \p hash
        /**
            The function searches the item by \p hash and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode
            where \p item is the item found.

            The functor may change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during the functor is executing.
            The functor does not serialize simultaneous access to the set's \p item. If such access is
            possible you must provide your own synchronization schema on item level to prevent unsafe item modifications.

            The function returns \p true if \p hash is found, \p false otherwise.
        */
        template <typename Func>
        bool find( hash_type const& hash, Func f )
        {
            return base_class::find( hash, f );
        }

        /// Checks whether the set contains \p hash
        /**
            The function searches the item by its \p hash
            and returns \p true if it is found, or \p false otherwise.
        */
        bool contains( hash_type const& hash )
        {
            return base_class::contains( hash );
        }

        /// Finds an item by it's \p hash and returns the item found
        /**
            The function searches the item by its \p hash
            and returns the guarded pointer to the item found.
            If \p hash is not found the function returns an empty \p guarded_ptr.

            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::container::FeldmanHashSet< your_template_params >  my_set;
            my_set theSet;
            // ...
            {
                my_set::guarded_ptr gp( theSet.get( 5 ));
                if ( theSet.get( 5 )) {
                    // Deal with gp
                    //...
                }
                // Destructor of guarded_ptr releases internal HP guard
            }
            \endcode
        */
        guarded_ptr get( hash_type const& hash )
        {
            return base_class::get( hash );
        }

        /// Clears the set (non-atomic)
        /**
            The function unlink all data node from the set.
            The function is not atomic but is thread-safe.
            After \p %clear() the set may not be empty because another threads may insert items.
        */
        void clear()
        {
            base_class::clear();
        }

        /// Checks if the set is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the set is empty.
            Thus, the correct item counting feature is an important part of the set implementation.
        */
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns item count in the set
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }

        /// Returns the size of head node
        size_t head_size() const
        {
            return base_class::head_size();
        }

        /// Returns the size of the array node
        size_t array_node_size() const
        {
            return base_class::array_node_size();
        }

        /// Collects tree level statistics into \p stat
        /**
            The function traverses the set and collects statistics for each level of the tree
            into \p feldman_hashset::level_statistics struct. The element of \p stat[i]
            represents statistics for level \p i, level 0 is head array.
            The function is thread-safe and may be called in multi-threaded environment.

            Result can be useful for estimating efficiency of hash functor you use.
        */
        void get_level_statistics(std::vector< feldman_hashset::level_statistics>& stat) const
        {
            base_class::get_level_statistics(stat);
        }
    };

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_IMPL_FELDMAN_HASHSET_H
