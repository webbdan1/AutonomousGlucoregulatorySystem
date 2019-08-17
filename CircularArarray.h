/******************************************************************************
** FILE: CircularArray.h
**
** ABSTRACT:
** Custom template class for a circular array data
** structure used to store a limited amount of
** application data (bg, insulin entries).
**
**
** DOCUMENTS:
**
**
** AUTHOR:
** Daniel Webb
**
** CREATION DATE:
** 06/24/2019
**
** NOTES:
**
******************************************************************************/

#ifndef CIRCULARARRAY_H
#define CIRCULARARRAY_H

#include <iostream>
#include <vector>
using std::vector;

template <class Type> class CircularArray
{
protected:
    int m_capacity = 288;
    int m_size = 0;
    vector<Type> m_data;
    int m_headIndex = 0;
    int m_tailIndex = 0;

public:
    CircularArray() = default;
    virtual ~CircularArray();
    CircularArray(CircularArray& array);

    int getSize() const;
    int getCapacity() const;
    void setCapacity(const int& cap);
    int getHeadIndex() const;
    void setHeadIndex(const int& headIndex);
    int getTailIndex() const;
    void setTailIndex(const int& tailIndex);
    void enqueue(const Type& t);
    Type dequeue();
    Type getFirstValue() const;
    Type getLastValue() const;
    bool isFull() const;
    bool isEmpty() const;
    vector<Type> getNValues(int n);
    void print();
};

template <class Type>
CircularArray<Type>::~CircularArray()
{
    for(auto i : m_data){
        delete i;
    }
}

template <class Type>
int CircularArray<Type>::getSize() const
{
    return m_size;
}

template <class Type>
int CircularArray<Type>::getCapacity() const
{
    return m_capacity;
}

template <class Type>
void CircularArray<Type>::setCapacity(const int& cap)
{
    m_capacity = cap;
}

template <class Type>
int CircularArray<Type>::getHeadIndex() const
{
    return m_headIndex;
}

template <class Type>
void CircularArray<Type>::setHeadIndex(const int& headIndex)
{
    m_headIndex = headIndex;
}

template <class Type>
int CircularArray<Type>::getTailIndex() const
{
    return m_tailIndex;
}

template <class Type>
void CircularArray<Type>::setTailIndex(const int& tailIndex)
{
    m_tailIndex = tailIndex;
}

template <class Type>
void CircularArray<Type>::enqueue(const Type& t)
{
    if(m_size==m_capacity){
        this->dequeue();
        m_size -= 1;
    }
    m_tailIndex = (m_headIndex + m_size) % m_capacity;
    m_data.insert(m_data.begin()+m_tailIndex, t);
    m_size+=1;
    m_tailIndex+=1;
}

template <class Type>
Type CircularArray<Type>::dequeue()
{
    if(!m_size){
        return nullptr;
    }
    Type item = m_data.at(m_headIndex);
    m_data.erase(m_data.begin()+m_headIndex, m_data.begin()+m_headIndex+1);
    m_headIndex = (m_headIndex + 1) % m_capacity;
    m_size-=1;
    return item;
}

template <class Type>
Type CircularArray<Type>::getFirstValue() const
{
    return m_data.at(m_headIndex);
}

template <class Type>
Type CircularArray<Type>::getLastValue() const
{
    if(m_size>1){
        return m_data.at(m_tailIndex-1);
    }
    else{
        return m_data.at(m_headIndex);
    }

}

template <class Type>
bool CircularArray<Type>::isFull() const
{
    return m_size==m_capacity;
}

template <class Type>
bool CircularArray<Type>::isEmpty() const
{
    return !m_size;
}

template <class Type>
void CircularArray<Type>::print()
{
    for(int i = 0; i< m_data.size();i++){
        std::cout << "Entry " << i << ": " ;
        m_data[i]->print();
    }
}

template <class Type>
vector<Type> CircularArray<Type>::getNValues(int n)
{
    vector<Type> data;
    for(int i = 0; i< n; i++){
        data.push_back(m_data[i]);
    }
    return data;
}

#endif // CIRCULARARRAY_H
