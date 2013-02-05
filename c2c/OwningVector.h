/* Copyright 2013 Bas van den Berg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OWNING_VECTOR_H
#define OWNING_VECTOR_H

#include <stdlib.h>

template <typename T>
class OwningVector {
public:
    OwningVector(int initial = 32)
        : m_capacity(initial)
        , m_size(0)
    {
        m_data = (T**)malloc(sizeof(T*)*m_capacity);
    }
    OwningVector(OwningVector& rhs)
        : m_capacity(rhs.m_capacity)
        , m_size(rhs.m_size)
        , m_data(rhs.m_data)
    {
        rhs.m_capacity = 0;
        rhs.m_size = 0;
        rhs.m_data = 0;
    }
    ~OwningVector () {
        if (m_data) {
            for (unsigned int i=0; i<m_size; i++) delete m_data[i];
            free(m_data);
        }
    }
    T* operator[] (unsigned int index) {
        assert(index < m_size);
        return m_data[index];
    }
    void push_back(T* s) {
        if (m_size == m_capacity) {
            if (m_capacity == 0) m_capacity = 4;
            else m_capacity *= 2;
            T** data2 = (T**)malloc(sizeof(T*)*m_capacity);
            memcpy(data2, m_data, m_size*sizeof(T*));
            // data = data2
        }
        m_data[m_size] = s;
        m_size++;
    }
    bool empty() const { return m_size == 0; }
    unsigned int size() const { return m_size; }
private:
    unsigned int m_capacity;
    unsigned int m_size;
    T** m_data;
};

#endif

