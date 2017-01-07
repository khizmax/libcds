#ifndef ORDEREDLINKEDLIST_H_INCLUDED
#define ORDEREDLINKEDLIST_H_INCLUDED
#include "LinkedListType.h"
//классс дл€ отсортированного Lock Free списка. ¬ нем объ€вл€ем и описываем методы, дл€ неблокирующих операций
//вставки, поиска и удалени€ элемента из списка.
using namespace std;
template <class Type>
class orderedLinkedList:public LinkedListType<Type>
{
public:
    volatile bool CAS(nodeType<Type> **node1_add,nodeType<Type>* node1,nodeType<Type>* node2);

   volatile bool search(const Type&) const;
   volatile bool insert (const Type&);
    volatile bool insertFirst(const Type&);
    volatile  bool insertLast(const Type&);
     volatile bool deleteNode(const Type&);
};
 template <class Type>
 volatile bool orderedLinkedList<Type>::CAS (nodeType<Type>** node1_add,nodeType<Type>* node1,nodeType<Type>* node2)
 {if (*node1_add==node1)

 {*node1_add=node2;
 return true;
 }
 else
 {

     return false;
 }
 }


template <class Type>
 volatile bool orderedLinkedList<Type>::search (const Type& item) const
 {bool found=false;
 nodeType<Type> *current;
     current=first;
     while(current !=NULL &&!found)
        if (current -> info>=item)
        found = true;
     else
        current = current->link;
     if (found)

        found = (current->info == item);
          if (CAS(&(current), current,current->link))
        return true;
else
    return false;
 }

 template <class Type>
volatile bool orderedLinkedList<Type>::insert (const Type& item)
 {
     nodeType<Type> *current,*trailCurrent, *newNode;
     bool found;
     newNode = new nodeType<Type>;

   newNode->info=item;
   newNode->link =NULL;
   if (first ==NULL)
   {
       first=newNode;
       last=newNode;
       count++;
       if (CAS(&(first->link),first->link,last->link))
        return true;
   }
   else{
    current=first;
    found=false;
    while(current !=NULL && !found)
        if (current->info>=item)


        found=true;

    else{
        trailCurrent = current;
        current = current->link;

    }
    if (current==first)
    {
        newNode->link=first;
        first=newNode;
        count++;
          if (CAS(&(first->link),first->link,newNode->link))
        return true;
    }
    else
        {
            trailCurrent->link=newNode;
    newNode->link=current;
    if (current==NULL)
        last=newNode;
    count++;
           if (CAS(&( last), last,newNode))
        return true;
 return true;

    }
   }




 }
 template <class Type>
volatile bool orderedLinkedList<Type>::insertFirst(const Type& item)
 {

     if (insert(item))
      return true;
     else
        return false;
 }
 template <class Type>
 volatile bool orderedLinkedList<Type>::insertLast(const Type& item)
 {
     if (insert(item))
      return true;
     else
        return false;
 }
 template <class Type>
 volatile bool orderedLinkedList<Type>::deleteNode(const Type& item)
 {
       nodeType<Type> *current,*trailCurrent;
       bool found;
       if (first==NULL)
        cout<<"List is empty."<<endl;
       else
       {
           current=first;
           found=false;
           while(current !=NULL && !found)
            if (current->info>=item)
            found=true;
           else
           {
               trailCurrent = current;
               current=current->link;
           }
           if (current ==NULL)
            cout<< "The item isn't in List."<<endl;
           else if (current->info==item)
           {

               if (first==current)
               {

                   first=first->link;
                   if (first==NULL)
                    last=NULL;
                   delete current;

               }
               else
               {
                   trailCurrent->link=current->link;
                   if (current==last)
                    last=trailCurrent;
                   delete current;
               }
if (CAS(&( current->link), current->link,current))
        return true;
               count--;
               return true;
           }
           else
            cout<<"The item isn't in the list"<<endl;
           }
       }

#endif // ORDEREDLINKEDLIST_H_INCLUDED
