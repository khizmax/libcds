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
//help ����� ��� ������ insert
template <class Type>
bool orderedLinkedList<Type>::try_insert(nodeType<Type>* prev, nodeType<Type>* next, nodeType<Type>* node)
{


    for (;;)
    {





        // �������� �������� ���� cas ��������
        node->link.store(next, memory_order_relaxed);




        if (prev->link.compare_exchange_strong(next, node, memory_order_release))
            // ���� cas �������, ������ ���������
             {
        count++;


            return true;
        }
        // � ������ ������, ��� - �� ����������,
        // � �� ������ ����� ����� ����� ��� �������
        // ��������� ���������� � ��������� ���������
        for (;;)
        {
            // ���������� ��������
            int cmp = compare(node->info,next->info);
            if (cmp > 0)
            {

                // new 'next' ���� ���� ������ ������������ �����
                // ���������� ��� ���������� ���� � �������������� ��������
             prev = next;
         next = prev->link.load(memory_order_consume);
                if (next == 0)
                    break;
                continue;
            }
            else if (cmp == 0)
            {
                // ����� ���� ��� ���� � ������
                return false;
            }
            else /* (cmp < 0) */
            {
                // new 'next' ���� ���� ������ ��� ���� ������������ ��������
                // ������ cas ��� ���
                break;
         }   }

}



}
//help ������� ��� delete ������
template <class Type>
bool orderedLinkedList<Type>::try_delete(nodeType<Type>* prev, nodeType<Type>* next,nodeType<Type>* node,const Type& item)
{      nodeType<Type> *current,*trailCurrent;
       bool found;
for (;;)
{
//�������� �� ������
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
           //���� ������ ��� ���� ������ � �� ����� ������ ��� ,�� ������� ��������� � ��� ��� ���� � ����� ������ �� ������
           if (current ==NULL)
           {


            cout<< "The item isn't in List."<<endl;
                        return false;
break;
           }
           //���� ��� ���� ����� ������� �� �����
           else if (current->info==item)
           {
//������� ������ ������� ������
               if (first==current)
               {


                // first= first->link.load(memory_order_consume);


first->link.compare_exchange_strong(first, nullptr, memory_order_release);
count--;



               }
               //������� ������ ��������
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





//lock free ������� ������ �������� � ������
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


//lock free ��������  �������� �� ������
 template <class Type>
 bool orderedLinkedList<Type>::deleteNode(const Type& item)
 {
 nodeType<Type>  *node;
if(  try_delete( first, last,node, item))
return true;
else return false;

       }

#endif // ORDEREDLINKEDLIST_H_INCLUDED
