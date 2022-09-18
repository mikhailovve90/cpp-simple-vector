#pragma once
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include "array_ptr.h"
#include <stdexcept>

#include <iostream>

class TransCapacity {
private:
    size_t capacity_ = 0;
public:
    TransCapacity(size_t t): capacity_(t){};

    size_t GetCapacity(){
      return capacity_;
    };

};

TransCapacity Reserve(size_t capacity_to_reserve) {
    return TransCapacity(capacity_to_reserve);
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    SimpleVector(SimpleVector&& other) {
       this->swap(other);
    }

    SimpleVector& operator=(SimpleVector&& other) {
        this->swap(other);
        return *this;
    }

    SimpleVector(TransCapacity t_c){
       ArrayPtr<Type> res(t_c.GetCapacity());
       items_.swap(res);
       capacity_ = t_c.GetCapacity();
       size_ = 0;
    }

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        ArrayPtr<Type> res(size);
        items_.swap(res);
        size_ = size;
        capacity_ = size;
        for(size_t i = 0; i < size; ++i){
         items_[i] = std::move(Type());
        }
         //SimpleVector(size, Type{}) не работает
         // std::move(items_.Get(), items_.Get() + size, Type{}); не работает
         //SimpleVector(size, std::move(Type{})); //-не работает
         /*const Type& value = Type{};
         SimpleVector(size, value);*/ //не работает
        //std::fill(items_.Get(), items_.Get() + size, Type{}); не работает
        //std::fill(this->begin(), this->end(), Type{}); не работает
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) {
        ArrayPtr<Type> res(size);
        items_.swap(res);
        capacity_ = size;
        size_ = size;
        std::fill(this->begin(), this->end(), value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        ArrayPtr<Type> res(init.size());

        for(size_t t = 0; t < init.size(); ++t)
          res[t] = std::data(init)[t];

        size_ = init.size();
        capacity_ = init.size();
        items_.swap(res);
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
       return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_ && "requested index is out of range");
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_ && "requested index is out of range");
        const Type &res = items_[index];
        return res;
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if(index >= size_) {
          throw std::out_of_range("index not avaliable");
        }
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if(index >= size_) {
          throw std::out_of_range("index not avaliable");
        }

        const Type &ref_to_item = items_[index];
        return ref_to_item;
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if(new_size > capacity_) {
          SimpleVector<Type> new_vec(new_size);
          for(size_t t = 0; t < this->size_; ++t){
            new_vec[t] = std::move(items_[t]);
          }
          items_.swap(new_vec.items_);
          capacity_ = new_size;
        }
        else if(new_size > size_){
          SimpleVector<Type> new_ptr(new_size);
          for(size_t t = 0; t < this->size_; ++t){
            new_ptr[t] = std::move(items_[t]);
          }
          items_.swap(new_ptr.items_);
        }
        size_ = new_size;
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return this->begin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return this->end();
    }
    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    SimpleVector(const SimpleVector& other) {
        SimpleVector<Type> lst1(other.size_);
        std::copy(other.begin(), other.end(), lst1.begin());
        this->swap(lst1);
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if(this != &rhs){
          SimpleVector<Type> lst1(rhs);
          this->swap(lst1);
        }
        return *this;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if(size_ < capacity_){
          items_[size_] = item;
          ++size_;
        } else {
          size_t cur_size = size_;
          capacity_ = capacity_ == 0 ? 1 : capacity_ * 2;
          this->Resize(capacity_);
          size_ = cur_size;
          items_[size_] = item;
          ++size_;
        }
    }


        // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(Type&& item) {
        if(size_ < capacity_){
          items_[size_] = std::move(item);
          ++size_;
        } else {
          size_t cur_size = size_;
          capacity_ = capacity_ == 0 ? 1 : capacity_ * 2;
          this->Resize(capacity_);
          size_ = cur_size;
          items_[size_] = std::move(item);
          ++size_;
        }
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
      Iterator s = const_cast<Iterator>(pos);
      assert((s >= this->begin() && s <= this->end()) && "Iterator out of range");
      size_t dist = std::distance(this->begin(), s);
      size_t cur_size = size_;
      if(!(size_ < capacity_)){
        capacity_ = capacity_ == 0 ? 1 : capacity_;
        Resize(capacity_ = capacity_ == 0 ? 1 : capacity_ * 2);
        size_ =cur_size + 1;
        std::copy_backward(this->begin() + dist, this->end(), this->end()+1);
        items_[dist] = value;
        return &items_[dist];
      } else {
        std::copy_backward(this->begin() + dist, this->end(), this->end()+1);
        items_[dist] = value;
        size_ = cur_size + 1;
        return &items_[dist];
      }
    }

        // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, Type&& value) {
      Iterator s = const_cast<Iterator>(pos);
      assert((s >= this->begin() && s <= this->end()) && "Iterator out of range");
      size_t dist = std::distance(this->begin(), s);
      size_t cur_size = size_;
      if(!(size_ < capacity_)){
        capacity_ = capacity_ == 0 ? 1 : capacity_;
        Resize(capacity_ = capacity_ == 0 ? 1 : capacity_ * 2);
        size_ =cur_size + 1;
        std::move_backward(this->begin() + dist, this->end(), this->end()+1);
        items_[dist] = std::move(value);
        return &items_[dist];
      } else {
        std::move_backward(this->begin() + dist, this->end(), this->end()+1);
        items_[dist] = std::move(value);
        size_ = cur_size + 1;
        return &items_[dist];
      }
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert(this->IsEmpty() && "Simple vector already empty");
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        Iterator s = const_cast<Iterator>(pos);
        assert((s >= this->begin() && s <= this->end()) && "Iterator out of range");
        assert(this->IsEmpty() && "Simple vector already empty");
        size_t dist = std::distance(this->begin(), s);
        SimpleVector<Type> to_vector(size_ - 1);
        std::move(this->begin(), s , to_vector.begin());
        std::move_backward(s + 1, this->end(), to_vector.end());
        items_.swap(to_vector.items_);
        --size_;
        return &items_[dist];
    }

    void Reserve(size_t new_capacity){
      if(new_capacity > capacity_){
        ArrayPtr<Type> res(new_capacity);
        for(size_t i = 0; i < size_; ++i){
          res[i] = items_[i];
        }
        capacity_ = new_capacity;
        items_.swap(res);
      }
    }

private:
  ArrayPtr<Type> items_;
  size_t size_ = 0;
  size_t capacity_ = 0;
  Type temp{};
};

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(),lhs.end(),rhs.begin(),rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
