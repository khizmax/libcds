//$$CDS-header$$

#ifndef CDSUNIT_NONCONCURRENT_ITERATOR_SEQUENCE_H
#define CDSUNIT_NONCONCURRENT_ITERATOR_SEQUENCE_H

#include <cds/details/bounded_array.h>
#include <cds/atomic.h>
#include <algorithm>    // std::sort

namespace map { namespace nonconcurrent_iterator {

    class Sequence
    {
    public:
        typedef int        TKey;
        struct TValue {
            TKey        keyControl    ;    // используется для контроля, что данные относятся к ключу
            int            nShuffle    ;    // случайное значение, используемое для сортировки
            TValue *    pOrigItem    ;    // указатель на элемент в массиве; так как список хранит копии,
                                        // то с помощью этого поля организуется доступ к исходному элементу

            cds::atomics::item_counter<cds::membar_release>   nAccess        ;    // счетчик доступа при обходе списка по итераторам
                                                // (Atomic, так как возможен параллеьный доступ)

            TValue()
            {
                ++nConstructCount;
            }
            TValue( const TValue& v )
            {
                memcpy( this, &v, sizeof(*this) );
                ++nConstructCount;
            }
            ~TValue()
            {
                ++nDestructCount;
            }
        };

        struct Data {
            TKey    key;
            TValue    value;
        };

    protected:
        static size_t nConstructCount;
        static size_t nDestructCount;

    public:
        typedef cds::details::BoundedArray<Data>        TDataArray;
        typedef TDataArray::const_iterator              const_iterator;

        TDataArray                                        arrData;

        const_iterator begin() const    { return arrData.begin(); }
        const_iterator end() const      { return arrData.end(); }

    public:
        Sequence( size_t nSize )
            : arrData( nSize )
        {}

        static unsigned int Rand( unsigned int nMax )
        {
            double rnd = double( rand() ) / double( RAND_MAX );
            unsigned int n = (unsigned int) (rnd * nMax);
            return n < nMax ? n : (n-1);
        }

        void generateSequence()
        {
            // Генерируем тестовый массив данных. Массив должен быть перемешан случайным образом, поэтому
            // в качестве значения поля value.nShuffle используем случайное, и сортируем массив
            // по этому случайному значению

            size_t nMax = arrData.capacity();
            for ( size_t i = 0; i < nMax; ++i ) {
                arrData[i].key = (unsigned int) i;
                arrData[i].value.keyControl = (unsigned int) i;
                arrData[i].value.nShuffle = Rand( (unsigned int) nMax );
                arrData[i].value.pOrigItem = &(arrData[i].value);
                arrData[i].value.nAccess.reset( cds::membar_relaxed::order );
            }
        }

        void restoreLinks()
        {
            size_t nMax = arrData.capacity();
            for ( size_t i = 0; i < nMax; ++i )
                arrData[i].value.pOrigItem = &(arrData[i].value);
        }

        static bool sortValue( const Data& p1, const Data&p2 )
        {
            return p1.value.nShuffle < p2.value.nShuffle;
        }

        void makeRandomSortedSequence()
        {
            std::sort( arrData.begin(), arrData.end(), sortValue );
            restoreLinks();
        }

        static bool sortAsc( const Data& p1, const Data&p2 )
        {
            return p1.key < p2.key;
        }

        void makeAscSortedSequence()
        {
            // Сортирует массив в порядке возрастания ключей
            std::sort( arrData.begin(), arrData.end(), sortAsc );
            restoreLinks();
        }

        static bool sortDesc( const Data& p1, const Data&p2 )
        {
            return p2.key < p1.key;
        }

        void makeDescSortedSequence()
        {
            // Сортирует массив в порядке убывания ключей
            std::sort( arrData.begin(), arrData.end(), sortDesc );
            restoreLinks();
        }

        void clearAccess()
        {
            size_t nMax = arrData.capacity();
            for ( size_t i = 0; i < nMax; ++i )
                arrData[i].value.nAccess.reset( cds::membar_relaxed::order );
        }
    };

} } // namespace map::nonconcurrent_iterator

#endif    // #ifndef CDSUNIT_NONCONCURRENT_ITERATOR_SEQUENCE_H
