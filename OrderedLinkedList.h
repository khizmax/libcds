#ifndef ORDEREDLINKEDLIST_H_INCLUDED
#define ORDEREDLINKEDLIST_H_INCLUDED
#include "LinkedListType.h"
#include <iostream>
#include <cassert>
#include <type_traits>
#include <windows.h>
using namespace std;
using namespace boost;

 template <class Type>
 class orderedLinkedList:public LinkedListType<Type>

     {

        public:
        Type insert (const Type&);
        Type deleteNode(const Type&);
        nodeType<Type>*temp;
         private:
         int compare (int one, int two);
         bool try_insert(nodeType<Type>* prev, nodeType<Type>* next, nodeType<Type>* node);
         bool try_delete(nodeType<Type>* prev, nodeType<Type>* next, const Type& item);

     };


 template <class Type>
   int orderedLinkedList<Type>::compare (int one, int two)

       {
            if (one>two) return +1;
            if (one==two) return 0;
            if (two>one) return -1;
             return 0;
       }


//help метод для метода insert
 template <class Type>
 bool orderedLinkedList<Type>::try_insert(nodeType<Type>* prev, nodeType<Type>* next, nodeType<Type>* node)

 {

 nodeType<Type>* tmp;

 for (;;)

     {

         if (node->info<first->info)

              {
                  tmp=first;
                  first=node;
                  first->link.store(next, memory_order_relaxed);

                   if (first->markedAtomic.compare_exchange_strong(first->marked, 0, memory_order_release))

                     if (first->link.compare_exchange_strong(next, tmp, memory_order_release))// если cas успешна, запись вставлена
                         {
                              count++;

                              return true;
                         }


              } else {
                        // пытаемся вставить узел cas перацией
                        node->link.store(next, memory_order_relaxed);
                        //подмена marked поля

                         if (node->markedAtomic.compare_exchange_strong(node->marked, 0, memory_order_release))

                          if (prev->link.compare_exchange_strong(next, node, memory_order_release))// если cas успешна, запись вставлена

                              {

                                     count++;

                                     return true;


                              }

                                // в другом случае, что - то поменялось,
                                // и мы должны найти новое место для вставки
                                // обновляем предыдущий и следующий указатели

                                for (;;)

                                   {
                                         // сравниваем значения
                                         int cmp = compare(node->info,next->info);

                                         if (cmp > 0)

                                             {

                                                   // если cas успешна, запись вставлена

                                                   // new 'next' ключ узла меньше вставляемого ключа
                                                   // запоминаем как предыдущий узел и пересравниваем значения
                                                    prev = next;
                                                    next = prev->link.load(memory_order_consume);

                                                   if (next == 0)
                                                      break;
                                                   continue;

                                             }

                                              else if (cmp == 0)

                                                 {
                                                      // такой ключ уже есть в списке

                                                      return false;


                                                 }
                                                   else /* (cmp < 0) */

                                                      {
                          // new 'next' ключ узла больше чем ключ вставляемого элемента
                         // делаем cas еще раз
                                                          break;

                                                      }


                                   }


                     }

     }

 if (node->info>last->info)

      {

         tmp=last;
         last=node;
         last->link.store(next, memory_order_relaxed);
         if (last->markedAtomic.compare_exchange_strong(last->marked, 0, memory_order_release))
          if (last->link.compare_exchange_strong(next, tmp, memory_order_release))
            // если cas успешна, запись вставлена
             {


                 count++;

                 return true;

             }

       }

  }
//help функция для delete метода
template <class Type>
bool orderedLinkedList<Type>::try_delete(nodeType<Type>* prev, nodeType<Type>* next,const Type& item)

 {
     nodeType<Type> *current,*trailCurrent;
     bool found;
     for (;;)

        {
                   //проходим по списку
            LinkedListIterator<int> it=begin();
            current=it.current;
            found=false;
            while(current !=NULL && !found)
                if (current->info>=item)
                found=true;
                else

                       {

                           trailCurrent = current;
                           ++it;
                           current=it.current;
                           current->markedAtomic.compare_exchange_strong(current->marked, 1, memory_order_release);


                       }
                       //если прошли все узлы списка и не нашли нужный нам ,то выводим сообщение о том что узел с тайим ключом не найден
                  if (current ==NULL)

                      {

                        cout<< "The item isn't in List."<<endl;
                        return false;
                        break;


                      }

                  //если все таки нащли элемент по ключу
                  else if (current->info==item)

                    {
                      //удаляем первый элемент списка
                         if (first==current)

                             {

                              current->markedAtomic.compare_exchange_strong(current->marked, 1, memory_order_release);


                             }
               //удаляем элемент из другого места
                 else

                  {

                       trailCurrent->link= it.current->link;
                       if (current==last)
                       last=trailCurrent;
                       current->markedAtomic.compare_exchange_strong(current->marked, 1, memory_order_release);


                 }
                 //physical deletion of node
                 if (current->marked==1)

                               {

                                     current->link.compare_exchange_strong(current, nullptr, memory_order_release);
                                     count--;

                               }
                return true;
                break;

                    }



         }

 }





//lock free вставка нового элемента в список
 template <class Type>
Type orderedLinkedList<Type>::insert (const Type& item)

 {

     nodeType<Type>  *newNode;
     newNode=new nodeType<Type>;
     newNode->info=item;
     newNode->link =NULL;
     if(try_insert( first, last, newNode))
     return 1;
     else return 0;

 }


//lock free удаление  элемента из списка
 template <class Type>
 Type orderedLinkedList<Type>::deleteNode(const Type& item)

     {

         if(try_delete( first, last, item))
         return 1;
         else return 0;

     }

#endif // ORDEREDLINKEDLIST_H_INCLUDED
