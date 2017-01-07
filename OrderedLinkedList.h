#ifndef ORDEREDLINKEDLIST_H_INCLUDED
#define ORDEREDLINKEDLIST_H_INCLUDED
#include "LinkedListType.h"
#include <type_traits>
#include <windows.h>

using namespace std;
using namespace boost;
template <class Type>
class orderedLinkedList:public LinkedListType<Type>
{
public:

  int compare (int one, int two);

bool try_insert(nodeType<Type>* prev, nodeType<Type>* next, nodeType<Type>* node);

bool try_delete(nodeType<Type>* prev, nodeType<Type>* next,nodeType<Type>* node, const Type& item);
 nodeType<Type>* search(nodeType<Type>**left_node,const Type&) ;
     bool find(const Type&) ;
  bool insert (const Type&);

      bool deleteNode(const Type&);

};

template <class Type>
   int orderedLinkedList<Type>::compare (int one, int two) {

            if (one>two) return +1;
            if (one==two) return 0;
            if (two>one) return -1;
return 0;
        }
//help метод для метода insert
template <class Type>
bool orderedLinkedList<Type>::try_insert(nodeType<Type>* prev, nodeType<Type>* next, nodeType<Type>* node)
{


    for (;;)
    {





        // пытаемся вставить узел cas перацией
        node->link.store(next, memory_order_relaxed);




        if (prev->link.compare_exchange_strong(next, node, memory_order_release))
            // если cas успешна, запись вставлена
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
         }   }

}



}
//help функция для delete метода
template <class Type>
bool orderedLinkedList<Type>::try_delete(nodeType<Type>* prev, nodeType<Type>* next,nodeType<Type>* node,const Type& item)
{      nodeType<Type> *current,*trailCurrent;
       bool found;
for (;;)
{
//проходим по списку
           current=first;

           found=false;
           while(current !=NULL && !found)
            if (current->info>=item)



            found=true;


           else
           {

               trailCurrent = current;
                 current =  current->link.load(memory_order_consume);


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


                // first= first->link.load(memory_order_consume);


first->link.compare_exchange_strong(first, nullptr, memory_order_release);
count--;



               }
               //удаляем другие элементы
               else
               {

                    trailCurrent->link= current->link.load(memory_order_consume);

                   if (current==last)
                    last=trailCurrent;

                  trailCurrent->link.compare_exchange_strong(current, nullptr, memory_order_release);
                   count--;


               }


               return true;
break;
           }



}
}





//lock free вставка нового элемента в список
 template <class Type>
bool orderedLinkedList<Type>::insert (const Type& item)
 {
     nodeType<Type>  *newNode;


 newNode=new nodeType<Type>;

   newNode->info=item;
   newNode->link =NULL;

  if(  try_insert( first, last, newNode))
return true;
else return false;

   }


//lock free удаление  элемента из списка
 template <class Type>
 bool orderedLinkedList<Type>::deleteNode(const Type& item)
 {
 nodeType<Type>  *node;
if(  try_delete( first, last,node, item))
return true;
else return false;

       }

#endif // ORDEREDLINKEDLIST_H_INCLUDED
