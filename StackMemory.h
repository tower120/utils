#pragma once

#include <array>
#include <functional>

#include "PureFunction.h"

/*

StackMemory<maxSize> memory;
StackMemory<maxSize, BaseClass> memory;


memory.emplace<ConcreteClass>(arg1, arg2);

BaseClass& base         = memory.get();
ConcreteClass& concrete = memory.get<ConcreteClass>();
*/

namespace utils{
        
template<bool enable, class dummy = void>
class StackMemoryTrivialCopyHelper;

class StackMemoryTrivialCopyHelperBase {
protected:
        using Size = short;
};

template<class dummy>
class StackMemoryTrivialCopyHelper<false, dummy> : public StackMemoryTrivialCopyHelperBase {
protected:
        constexpr const Size size() const {
                return -1;
        }
        void size(Size s) {}
};


template<class dummy>
class StackMemoryTrivialCopyHelper<true, dummy> : public StackMemoryTrivialCopyHelperBase {
        Size m_size = -1;	// -1 = non trivial
protected:
        Size size() const {
                return m_size;
        }
        void size(Size s) {
                m_size = s;
        }
};

// can't have BaseClass widthout ctr_dstr_op, because C++ does not allow virtual copy constructor
template<int maxSize
        , class BaseClass = void
        , bool use_trivial_copy_optimisation = std::is_trivially_copyable<std::decay_t<BaseClass>>::value
>
class StackMemory : public StackMemoryTrivialCopyHelper< use_trivial_copy_optimisation >
{
        using Memory = std::array<char, maxSize>;
        Memory memory;

        enum class Type {Destructor, CopyCtr, AssigmentCtr, MoveCtr, AssigmentMoveCtr };

        PureFunction<void, Type, void*, const void*> ctr_dstr_op;

        bool constructed = false;


        ///		Triviality
        ///	------------------------------
        bool m_is_trivially_destructable;

        //short size = -1;	// -1 = non trivial
        constexpr const bool is_trivially_copyable() const {
                return size() != -1;
        }
        ///	------------------------------

        void default_copy(const StackMemory& other) {
                m_is_trivially_destructable = other.m_is_trivially_destructable;
                size(other.size());
                ctr_dstr_op					= other.ctr_dstr_op;
        }

        void copy(const StackMemory& other) {
                std::copy(other.memory.begin(), other.memory.begin() + other.size(), memory.begin());
        }

        void destroy() {
                if (m_is_trivially_destructable) {
                        return;
                }
                ctr_dstr_op(Type::Destructor, memory.data(), nullptr);
        }

        using GuardType = void;
        template<class T>
        void checkBaseClass() const {
                static_assert( std::is_same<BaseClass, GuardType>::value
                                        || std::is_base_of<BaseClass, T>::value
                        , "StackMemory object must derive BaseClass");
        }
        template<class T>
        void checkGet() const {
                static_assert( !std::is_same<BaseClass, GuardType>::value
                                        || !std::is_same<BaseClass, T>::value
                        , "StackMemory get<T>() must have type parameter");
        }

public:
        void* raw() {
                return memory.data();
        }

        template<class T, class ...Args>
        void emplace(Args&&...args) {
                static_assert( sizeof(T)<=maxSize, "StackMemory insufficient capacity");
                checkBaseClass<T>();

                if (constructed) {
                        destroy();
                }
                constructed = true;

                void* ptr = memory.data();
                new (ptr) T(std::forward<Args>(args)...);

                constexpr const bool trivially_copyable		= std::is_trivially_copyable	<std::decay_t<T>>::value;
                constexpr const bool trivially_destructable = std::is_trivially_destructible<std::decay_t<T>>::value;

                m_is_trivially_destructable = trivially_destructable;

                if (trivially_copyable) {
                        // no need for ctr_dstr_op
                        size(sizeof(T));
                        return;
                }
                size(-1);

                ctr_dstr_op = [](Type type, void* m_self, const void* m_other) {
                        T* self = static_cast<T*>(m_self);
                        const T* other = static_cast<const T*>(m_other);

                        switch (type) {
                                case Type::Destructor:
                                        self->~T();
                                        break;
                                case Type::CopyCtr:
                                        new (self) T(*other);
                                        break;
                                case Type::MoveCtr:
                                        new (self) T(std::move(*other));
                                        break;
                                case Type::AssigmentCtr:
                                        self->operator=(*other);
                                        break;
                                case Type::AssigmentMoveCtr:
                                        self->operator=(std::move(*other));
                                        break;
                        }
                };
        }

        template<class T = BaseClass>
        T& get() {
                checkGet<T>();
                checkBaseClass<T>();

                void* ptr = memory.data();
                return *static_cast<T*>(ptr);
        }
        template<class T = BaseClass>
        const T& get() const{
                checkGet<T>();
                checkBaseClass<T>();

                const void* ptr = memory.data();
                return *static_cast<const T*>(ptr);
        }


        StackMemory() {}
        StackMemory(const StackMemory& other) {
                if (!other.constructed) {
                        //constructed = false;	// already
                        return;
                }

                if (other.is_trivially_copyable()) {
                        copy(other);
                } else {
                        other.ctr_dstr_op(Type::CopyCtr, static_cast<void*>(memory.data()), static_cast<const void*>(other.memory.data()));
                }

                default_copy(other);
                constructed = true;
        }
        StackMemory(StackMemory&& other) {
                if (!other.constructed) {
                        //constructed = false;	// already
                        return;
                }

                if (other.is_trivially_copyable()) {
                        copy(other);
                } else {
                        other.ctr_dstr_op(Type::MoveCtr, static_cast<void*>(memory.data()), static_cast<const void*>(other.memory.data()));
                }

                default_copy(other);
                constructed = true;
        }

        StackMemory& operator=(const StackMemory& other) {
                if (!other.constructed) {
                        free();
                        return *this;
                }

                if (other.is_trivially_copyable()) {
                        copy(other);
                } else {
                        other.ctr_dstr_op(Type::AssigmentCtr, memory.data(), other.memory.data());
                }

                default_copy(other);
                constructed = true;

                return *this;
        }
        StackMemory& operator=(StackMemory&& other) {
                if (!other.constructed) {
                        free();
                        return *this;
                }

                if (other.is_trivially_copyable()) {
                        copy(other);
                } else {
                        other.ctr_dstr_op(Type::AssigmentMoveCtr, memory.data(), other.memory.data());
                }

                default_copy(other);
                constructed = true;

                return *this;
        }

        bool empty() const {
                return !constructed;
        }

        operator bool() const {
                return constructed;
        }

        void free() {
                if (!constructed) {
                        return;
                }
                constructed = false;
                destroy();
        }

        ~StackMemory() {
                if (!constructed) {
                        return;
                }
                destroy();
        }
};
}