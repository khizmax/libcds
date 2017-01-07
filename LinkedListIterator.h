#ifndef LINKEDLISTITERATOR_H_INCLUDED
#define LINKEDLISTITERATOR_H_INCLUDED

#include <iostream>
//����� �������� ��� ������, ����� �� ���������  �������� ��������, ������� ����� ������ �� �������
template <class Type>
struct nodeType
{
    Type info;
    nodeType<Type> *link;
};
template <class Type>
class LinkedListIterator
{
public:
  LinkedListIterator();
  LinkedListIterator(nodeType<Type>*) ;
  Type operator*() ;
  LinkedListIterator<Type> operator++();
  bool operator == (const LinkedListIterator<Type>&) const;
   bool operator != (const LinkedListIterator<Type>&) const;
private:
    nodeType<Type> *current;
};
template <class Type>
LinkedListIterator<Type>::LinkedListIterator()
{
    current=NULL;
}
template <class Type>
LinkedListIterator<Type>::LinkedListIterator(nodeType<Type> *ptr)
{
    current=ptr;
}
template <class Type>
Type LinkedListIterator<Type>::operator *()
{
    return current->info;
}

template <class Type>
LinkedListIterator<Type> LinkedListIterator<Type>::operator++()
{current=current->link;
    return *this;
}
template <class Type>
bool LinkedListIterator<Type>::operator ==(const LinkedListIterator<Type>& other) const
{
    return (current==other.current);
}
template <class Type>
bool LinkedListIterator<Type>::operator !=(const LinkedListIterator<Type>& other) const
{
    return (current!=other.current);
}
#endif // LINKEDLISTITERATOR_H_INCLUDED



