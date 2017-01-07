#ifndef LINKEDLISTTYPE_H_INCLUDED
#define LINKEDLISTTYPE_H_INCLUDED
#include "LinkedListIterator.h"
#include <iostream>

#include <cassert>
//абстрактный класс для списка как сортированного так и не сортированного, так как сортированный список делался
//из не сортированного. Так же в классе описываются конструктор и деструктор списка, а отсортированный список просто наследует этот класс.
using namespace std;
template <class Type>
class LinkedListType
{
public:
    const LinkedListType<Type>& operator=(const LinkedListType<Type>&);
    void initializeList();
    bool isEmptyList() const;
    void print() const;
    int length() const;
    void destroyList();
    Type front() const;
    Type back() const;

    virtual volatile bool search (const Type&) const=0;
    virtual volatile bool insertFirst (const Type&)=0;
    virtual volatile bool insertLast (const Type&) =0;
    virtual volatile bool deleteNode (const Type&)=0;
    LinkedListIterator<Type> begin();
     LinkedListIterator<Type> end();
     LinkedListType();
      LinkedListType(const LinkedListType<Type>&);
    ~LinkedListType();
protected:
    int count;
    nodeType<Type> *first;
     nodeType<Type> *last;
private:
    void copyList(const LinkedListType<Type>&);
      void copyNode(const nodeType<Type>&);
};


template <class Type>
bool LinkedListType<Type>::isEmptyList() const
{
    return(first==NULL);
}
template <class Type>
LinkedListType<Type>::LinkedListType()
{
    first=NULL;
    last=NULL;
    count=0;
}
template <class Type>
void LinkedListType<Type>::destroyList()
{
    nodeType<Type> *temp;
    while(first!=NULL)
    {
        temp=first;
        first=first->link;
        delete temp;
    }
    last=NULL;
    count=0;
}
template<class Type>
void LinkedListType<Type>::initializeList()
{
    destroyList();
}
template<class Type>
void LinkedListType<Type>::print() const
{
    nodeType<Type> *current;
    current=first;
    while(current!=NULL)
    {
        cout<<current->info<<" ";
        current = current->link;
    }
}
template <class Type>
int LinkedListType<Type>::length() const
{
   return count;
}
template <class Type>
Type LinkedListType<Type>::front() const
{assert(first !=NULL);

return first->info;
}
template <class Type>
Type LinkedListType<Type>::back() const
{assert(last !=NULL);

return last->info;
}
template <class Type>
LinkedListIterator<Type> LinkedListType<Type>::begin()
{
    LinkedListIterator<Type> temp(first);
    return temp;
}
template <class Type>
LinkedListIterator<Type> LinkedListType<Type>::end()
{
    LinkedListIterator<Type> temp(NULL)
    return temp;
}
template <class Type>
void LinkedListType<Type>::copyList(const LinkedListType<Type>&other)
{
    nodeType<Type> *newNode,*current;
    if (first !=NULL)
        destroyList();
     if (other.first ==NULL)
       {
          first = NULL;
          last= NULL;
          count = 0;
       }
    else {
        current=other.first;
        count = other.count;
        first = new nodeType<Type>;
        first->info = current->info;
        first->link = NULL;
        current = current->link;
        while(current !=NULL)
        {
            newNode = new nodeType<Type>;
            newNode->info = current->info;
            newNode->link = NULL;
            last->link = newNode;
            last=newNode;
            current = current->link;
        }
    }
}
template <class Type>
void LinkedListType<Type>::copyNode(const nodeType<Type>&other)
{
   nodeType<Type> *current;
    current->info = other->link;
    current = other->link;
}
template <class Type>

LinkedListType<Type>::~LinkedListType()
{
    destroyList();

}

template <class Type>

LinkedListType<Type>::LinkedListType(const LinkedListType<Type>& other)
{
   first = NULL;
   copyList(other);

}
template <class Type>

const LinkedListType<Type>& LinkedListType<Type>::operator =(const LinkedListType<Type>&other)
{
    if (this !=&other)
    {
        copyList(other);
    }
    return *this;

}

#endif // LINKEDLISTTYPE_H_INCLUDED

