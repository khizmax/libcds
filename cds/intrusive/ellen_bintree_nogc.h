#ifndef CDSLIB_INTRUSIVE_ELLEN_BINTREE_NOGC_H
#define CDSLIB_INTRUSIVE_ELLEN_BINTREE_NOGC_H
#include <cds/intrusive/details/ellen_bintree_base.h>
#include <cds/gc/nogc.h>
#include <cds/opt/compare.h>
namespace cds
{
namespace intrusive
{
template <typename Key,
          typename T,
          class Traits>
class EllenBinTree<gc::nogc, Key, T, Traits>
{
  public:
    typedef gc::nogc gc;   ///< Garbage collector
    typedef Key key_type;  ///< type of a key to be stored in internal nodes; key is a part of \p value_type
    typedef T value_type;  ///< type of value stored in the binary tree
    typedef Traits traits; ///< Traits template parameter

    typedef typename traits::hook hook;         ///< hook type
    typedef typename hook::node_type node_type; ///< node type
    typedef typename traits::disposer disposer; ///< leaf node disposer
    typedef typename traits::back_off back_off; ///< back-off strategy
  protected:
    typedef ellen_bintree::base_node<gc> tree_node;
    typedef node_type leaf_node;
    typedef ellen_bintree::node_types<gc, key_type, typename leaf_node::tag> node_factory;
    typedef typename node_factory::internal_node_type internal_node;
    typedef ellen_bintree::update_desc<leaf_node, internal_node> update_desc;
    typedef typename update_desc::update_ptr update_ptr;

  public:
#ifdef CDS_DOXYGEN_INVOKED
    typedef implementation_defined key_comparator;                                   ///< key compare functor based on \p Traits::compare and \p Traits::less
    typedef typename get_node_traits<value_type, node_type, hook>::type node_traits; ///< Node traits
#else
    typedef typename opt::details::make_comparator<value_type, traits>::type key_comparator;
    struct node_traits : public get_node_traits<value_type, node_type, hook>::type
    {
        static internal_node const &to_internal_node(tree_node const &n)
        {
            assert(n.is_internal());
            return static_cast<internal_node const &>(n);
        }

        static leaf_node const &to_leaf_node(tree_node const &n)
        {
            assert(n.is_leaf());
            return static_cast<leaf_node const &>(n);
        }
    };
#endif

    typedef typename traits::node_allocator node_allocator;
    typedef typename traits::item_counter item_counter;
    typedef typename traits::memory_model memory_model;
    typedef typename traits::stat stat; ///< internal statistics type
    typedef typename traits::key_extractor key_extractor;
    typedef typename traits::update_desc_allocator update_desc_allocator;

  protected:
    typedef ellen_bintree::details::compare<key_type, value_type, key_comparator, node_traits> node_compare;

    typedef cds::details::Allocator<internal_node, node_allocator> cxx_node_allocator;
    typedef cds::details::Allocator<update_desc, update_desc_allocator> cxx_update_desc_allocator;
    struct search_result
    {
        internal_node *pParent;
        leaf_node *pLeaf;
        bool bRightLeaf;
        update_ptr updParent;

        search_result()
            : pParent(nullptr), pLeaf(nullptr), bRightLeaf(false)
        {
        }
    };

  protected:
    //@cond
    internal_node m_Root; ///< Tree root node (key= Infinite2)
    leaf_node m_LeafInf1; ///< Infinite leaf 1 (key= Infinite1)
    leaf_node m_LeafInf2; ///< Infinite leaf 2 (key= Infinite2)

    item_counter m_ItemCounter;
    mutable stat m_Stat; ///< internal statistics

  protected:
    static void free_leaf_node(void *p)
    {
        disposer()(reinterpret_cast<value_type *>(p));
    }

    static void free_internal_node(void *pNode)
    {
        cxx_node_allocator().Delete(reinterpret_cast<internal_node *>(pNode));
    }

    struct internal_node_deleter
    {
        void operator()(internal_node *p) const
        {
            cxx_node_allocator().Delete(p);
        }
    };

    typedef std::unique_ptr<internal_node, internal_node_deleter> unique_internal_node_ptr;

    internal_node *alloc_internal_node() const
    {
        m_Stat.onInternalNodeCreated();
        internal_node *pNode = cxx_node_allocator().New();
        return pNode;
    }
    update_desc *alloc_update_desc() const
    {
        m_Stat.onUpdateDescCreated();
        return cxx_update_desc_allocator().New();
    }

    static void free_update_desc(void *pDesc)
    {
        cxx_update_desc_allocator().Delete(reinterpret_cast<update_desc *>(pDesc));
    }

  public:
    EllenBinTree()
    {
        make_empty_tree();
    }

    ~EllenBinTree()
    {
        clear();
    }

    /// Inserts new node
    /**
                The function inserts \p val in the tree if it does not contain
                an item with key equal to \p val.

                Returns \p true if \p val is placed into the tree, \p false otherwise.
            */
    bool insert(value_type &val)
    {
        return insert(val, [](value_type &) {});
    }

    /// Inserts new node
    /**
                This function is intended for derived non-intrusive containers.

                The function allows to split creating of new item into two part:
                - create item with key only
                - insert new item into the tree
                - if inserting is success, calls  \p f functor to initialize value-field of \p val.

                The functor signature is:
                \code
                    void func( value_type& val );
                \endcode
                where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
                \p val no any other changes could be made on this tree's item by concurrent threads.
                The user-defined functor is called only if the inserting is success.
            */
    template <typename Func>
    bool insert(value_type &val, Func f)
    {
        search_result res;
        back_off bkoff;
        unique_internal_node_ptr pNewInternal;

        while (true)
        {
            if (search(res, val, node_compare()))
            {
                m_Stat.onInsertFailed();
                return false; // uniq value
            }

            if (res.updParent.bits() == update_desc::Clean)
            {
                if (!pNewInternal.get())
                {
                    pNewInternal.reset(alloc_internal_node());
                }

                if (try_insert(val, pNewInternal.get(), res))
                {
                    f(val);
                    pNewInternal.release();
                    break;
                }
            }
            bkoff();
            m_Stat.onInsertRetry();
        }
        ++m_ItemCounter;
        m_Stat.onInsertSuccess();
        return true;
    }

    /// Updates the node
    /**
                The operation performs inserting or changing data with lock-free manner.

                If the item \p val is not found in the set, then \p val is inserted into the set
                iff \p bAllowInsert is \p true.
                Otherwise, the functor \p func is called with item found.
                The functor \p func signature is:
                \code
                    void func( bool bNew, value_type& item, value_type& val );
                \endcode
                with arguments:
                - \p bNew - \p true if the item has been inserted, \p false otherwise
                - \p item - item of the set
                - \p val - argument \p val passed into the \p %update() function
                If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
                refer to the same thing.

                The functor can change non-key fields of the \p item; however, \p func must guarantee
                that during changing no any other modifications could be made on this item by concurrent threads.

                Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
                i.e. the node has been inserted or updated,
                \p second is \p true if new item has been added or \p false if the item with \p key
                already exists.

                @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
            */
    template <typename Func>
    std::pair<bool, bool> update(value_type &val, Func func, bool bAllowInsert = true)
    {

        unique_internal_node_ptr pNewInternal;
        search_result res;
        back_off bkoff;

        for (;;)
        {
            if (search(res, val, node_compare()))
            {
                func(false, *node_traits::to_value_ptr(res.pLeaf), val);
                m_Stat.onUpdateExist();
                return std::make_pair(true, false);
            }

            if (res.updParent.bits() == update_desc::Clean)
            {
                if (!bAllowInsert)
                    return std::make_pair(false, false);

                if (!pNewInternal.get())
                    pNewInternal.reset(alloc_internal_node());

                if (try_insert(val, pNewInternal.get(), res))
                {
                    func(true, val, val);
                    pNewInternal.release(); // internal node has been linked into the tree and should not be deleted
                    break;
                }
            }
            bkoff();
            m_Stat.onUpdateRetry();
        }

        ++m_ItemCounter;
        m_Stat.onUpdateNew();
        return std::make_pair(true, true);
    }

    /// Checks whether the set contains \p key
    /**
                The function searches the item with key equal to \p key
                and returns \p true if it is found, and \p false otherwise.
            */
    template <typename Q>
    bool contains(Q const &key) const
    {
        search_result res;
        if (search(res, key, node_compare()))
        {
            m_Stat.onFindSuccess();
            return true;
        }
        m_Stat.onFindFailed();
        return false;
    }

    //@cond
    template <typename Q>
    CDS_DEPRECATED("deprecated, use contains()")
    bool find(Q const &key)
    {
        return contains(key);
    }
    //@endcond

    /// Checks whether the set contains \p key using \p pred predicate for searching
    /**
                The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
                \p Less functor has the interface like \p std::less.
                \p Less must imply the same element order as the comparator used for building the set.
            */
    template <typename Q, typename Less>
    bool contains(Q const &key, Less pred) const
    {
        typedef ellen_bintree::details::compare<
            key_type,
            value_type,
            opt::details::make_comparator_from_less<Less>,
            node_traits>
            compare_functor;

        search_result res;
        if (search(res, key, compare_functor()))
        {
            m_Stat.onFindSuccess();
            return true;
        }
        m_Stat.onFindFailed();
        return false;
    }
    //@cond
    template <typename Q, typename Less>
    CDS_DEPRECATED("deprecated, use contains()")
    bool find_with(Q const &key, Less pred) const
    {
        return contains(key, pred);
    }
    //@endcond

    /// Finds the key \p key
    /** @anchor cds_intrusive_EllenBinTree_find_func
                The function searches the item with key equal to \p key and calls the functor \p f for item found.
                The interface of \p Func functor is:
                \code
                struct functor {
                    void operator()( value_type& item, Q& key );
                };
                \endcode
                where \p item is the item found, \p key is the <tt>find</tt> function argument.

                The functor can change non-key fields of \p item. Note that the functor is only guarantee
                that \p item cannot be disposed during functor is executing.
                The functor does not serialize simultaneous access to the tree \p item. If such access is
                possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

                The function returns \p true if \p key is found, \p false otherwise.
            */
    template <typename Q, typename Func>
    bool find(Q &key, Func f) const
    {
        return find_(key, f);
    }
    //@cond
    template <typename Q, typename Func>
    bool find(Q const &key, Func f) const
    {
        return find_(key, f);
    }
    //@endcond
    /// Finds the key \p key with comparing functor \p pred
    /**
                The function is an analog of \ref cds_intrusive_EllenBinTree_find_func "find(Q&, Func)"
                but \p pred is used for key comparison.
                \p Less functor has the interface like \p std::less and should meet \ref cds_intrusive_EllenBinTree_less
                "Predicate requirements".
                \p pred must imply the same element order as the comparator used for building the tree.
            */
    //@cond
    template <typename Q, typename Less, typename Func>
    bool find_with(Q const &key, Less pred, Func f) const
    {
        return find_with_(key, pred, f);
    }
    //@endcond

    /// Checks if the tree is empty
    bool empty() const
    {
        return m_Root.m_pLeft.load(memory_model::memory_order_relaxed)->is_leaf();
    }

    /// Clears the tree
    void clear()
    {
        while (true)
        {
            internal_node *pParent = nullptr;
            internal_node *pGrandParent = nullptr;
            tree_node *pLeaf = const_cast<internal_node *>(&m_Root);

            // Get leftmost leaf
            while (pLeaf->is_internal())
            {
                pGrandParent = pParent;
                pParent = static_cast<internal_node *>(pLeaf);
                pLeaf = pParent->m_pLeft.load(memory_model::memory_order_relaxed);
            }

            if (pLeaf->infinite_key())
            {
                m_ItemCounter.reset();
                return;
            }

            // Remove leftmost leaf and its parent node
            assert(pGrandParent);
            assert(pParent);
            assert(pLeaf->is_leaf());

            pGrandParent->m_pLeft.store(pParent->m_pRight.load(memory_model::memory_order_relaxed), memory_model::memory_order_relaxed);
            free_leaf_node(node_traits::to_value_ptr(static_cast<leaf_node *>(pLeaf)));
            free_internal_node(pParent);
            m_ItemCounter--;
        }
    }

    /// Returns item count in the tree
    /**
                Only leaf nodes containing user data are counted.

                The value returned depends on item counter type provided by \p Traits template parameter.
                If it is \p atomicity::empty_item_counter this function always returns 0.
                The function is not suitable for checking the tree emptiness, use \p empty()
                member function for this purpose.
            */
    size_t size() const
    {
        return m_ItemCounter;
    }

    /// Returns const reference to internal statistics
    stat const &statistics() const
    {
        return m_Stat;
    }

    /// Checks internal consistency (not atomic, not thread-safe)
    /**
                The debugging function to check internal consistency of the tree.
            */
    bool check_consistency() const
    {
        return check_consistency(&m_Root);
    }

  protected:
    //@cond

    bool check_consistency(internal_node const *pRoot) const
    {
        tree_node *pLeft = pRoot->m_pLeft.load(atomics::memory_order_relaxed);
        tree_node *pRight = pRoot->m_pRight.load(atomics::memory_order_relaxed);
        assert(pLeft);
        assert(pRight);

        if (node_compare()(*pLeft, *pRoot) < 0 && node_compare()(*pRoot, *pRight) <= 0 && node_compare()(*pLeft, *pRight) < 0)
        {
            bool bRet = true;
            if (pLeft->is_internal())
                bRet = check_consistency(static_cast<internal_node *>(pLeft));
            assert(bRet);

            if (bRet && pRight->is_internal())
                bRet = bRet && check_consistency(static_cast<internal_node *>(pRight));
            assert(bRet);

            return bRet;
        }
        return false;
    }

    template <typename KeyValue, typename Compare>
    bool search(search_result &res, KeyValue const &key, Compare cmp) const
    {
        internal_node *pParent;
        tree_node *pLeaf;
        update_ptr updParent;

        bool bRightLeaf;
        bool bRightParent = false;

        int nCmp = 0;

        pParent = nullptr;
        pLeaf = const_cast<internal_node *>(&m_Root);
        updParent = nullptr;
        bRightLeaf = false;
        while (pLeaf->is_internal())
        {
            pParent = static_cast<internal_node *>(pLeaf);
            bRightParent = bRightLeaf;
            updParent = pParent->m_pUpdate.load(memory_model::memory_order_acquire);
            nCmp = cmp(key, *pParent);
            bRightLeaf = nCmp >= 0;
            pLeaf = pParent->get_child(nCmp >= 0, memory_model::memory_order_acquire);
        };
        assert(pLeaf->is_leaf());

        leaf_node *keyValue = static_cast<leaf_node *>(pLeaf);
        nCmp = cmp(key, *static_cast<leaf_node *>(pLeaf));

        res.pParent = pParent;
        res.pLeaf = static_cast<leaf_node *>(pLeaf);
        res.updParent = updParent;
        res.bRightLeaf = bRightLeaf;
        return nCmp == 0;
    }

    void help_insert(update_desc *pOp)
    {
        tree_node *pLeaf = static_cast<tree_node *>(pOp->iInfo.pLeaf);
        if (pOp->iInfo.bRightLeaf)
        {
            pOp->iInfo.pParent->m_pRight.compare_exchange_strong(pLeaf, static_cast<tree_node *>(pOp->iInfo.pNew),
                                                                 memory_model::memory_order_release, atomics::memory_order_relaxed);
        }
        else
        {
            pOp->iInfo.pParent->m_pLeft.compare_exchange_strong(pLeaf, static_cast<tree_node *>(pOp->iInfo.pNew),
                                                                memory_model::memory_order_release, atomics::memory_order_relaxed);
        }

        // Unflag parent
        update_ptr cur(pOp, update_desc::IFlag);
        CDS_VERIFY(pOp->iInfo.pParent->m_pUpdate.compare_exchange_strong(cur, pOp->iInfo.pParent->null_update_desc(),
                                                                         memory_model::memory_order_release, atomics::memory_order_relaxed));
    }

    bool try_insert(value_type &val, internal_node *pNewInternal, search_result &res)
    {
        assert(res.updParent.bits() == update_desc::Clean);
        assert(res.pLeaf->is_leaf());

        if (static_cast<leaf_node *>(res.pParent->get_child(res.bRightLeaf, memory_model::memory_order_relaxed)) == res.pLeaf)
        {
            leaf_node *pNewLeaf = node_traits::to_node_ptr(val);

            int nCmp = node_compare()(val, *res.pLeaf);
            if (nCmp < 0)
            {
                if (res.pLeaf->infinite_key())
                {
                    pNewInternal->infinite_key(1);
                }
                else
                {
                    pNewInternal->infinite_key(0);
                    key_extractor()(pNewInternal->m_Key, *node_traits::to_value_ptr(res.pLeaf));
                }
                pNewInternal->m_pLeft.store(static_cast<tree_node *>(pNewLeaf), memory_model::memory_order_relaxed);
                pNewInternal->m_pRight.store(static_cast<tree_node *>(res.pLeaf), memory_model::memory_order_relaxed);
            }
            else
            {
                pNewInternal->infinite_key(0);
                key_extractor()(pNewInternal->m_Key, val);
                pNewInternal->m_pLeft.store(static_cast<tree_node *>(res.pLeaf), memory_model::memory_order_relaxed);
                pNewInternal->m_pRight.store(static_cast<tree_node *>(pNewLeaf), memory_model::memory_order_relaxed);
            }
            update_desc *pOp = alloc_update_desc();

            pOp->iInfo.pParent = res.pParent;
            pOp->iInfo.pNew = pNewInternal;
            pOp->iInfo.pLeaf = res.pLeaf;
            pOp->iInfo.bRightLeaf = res.bRightLeaf;

            update_ptr updCur(res.updParent.ptr());

            if (res.pParent->m_pUpdate.compare_exchange_strong(updCur, update_ptr(pOp, update_desc::IFlag),
                                                               memory_model::memory_order_acq_rel, atomics::memory_order_acquire))
            {

                // do insert
                help_insert(pOp);
                return true;
            }
            else
            {
                free_update_desc(pOp);
            }
        }
        return false;
    }

    template <typename Q, typename Less, typename Func>
    bool find_with(Q &key, Less pred, Func f) const
    {
        return find_with_(key, pred, f);
    }

    template <typename Q, typename Less, typename Func>
    bool find_with_(Q &val, Less pred, Func f) const
    {
        typedef ellen_bintree::details::compare<
            key_type,
            value_type,
            opt::details::make_comparator_from_less<Less>,
            node_traits>
            compare_functor;

        search_result res;
        if (search(res, val, compare_functor()))
        {
            assert(res.pLeaf);
            f(*node_traits::to_value_ptr(res.pLeaf), val);
            m_Stat.onFindSuccess();
            return true;
        }
        m_Stat.onFindFailed();
        return false;
    }

    template <typename Q, typename Func>
    bool find_(Q &val, Func f) const
    {
        search_result res;
        if (search(res, val, node_compare()))
        {
            f(*node_traits::to_value_ptr(res.pLeaf), val);
            m_Stat.onFindSuccess();
            return true;
        }
        m_Stat.onFindFailed();
        return false;
    }

    void make_empty_tree()
    {
        m_Root.infinite_key(2);
        m_LeafInf1.infinite_key(1);
        m_LeafInf2.infinite_key(2);
        m_Root.m_pLeft.store(&m_LeafInf1, memory_model::memory_order_relaxed);
        m_Root.m_pRight.store(&m_LeafInf2, memory_model::memory_order_release);
    }
};
} // namespace intrusive
} // namespace cds
#endif
