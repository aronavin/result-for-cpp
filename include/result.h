#ifndef RESULT_H
#define RESULT_H

#include <cassert>
#include <utility>
#include <type_traits>


namespace result {

namespace details {

    template <typename E>
    struct make_error {

        E value;

        make_error(E val) : value(std::move(val)) {}

        template <typename U, typename std::enable_if<std::is_convertible<U, E>::value, bool>::type = true>
        make_error(U&& val) : value(std::forward<U>(val)) {}
    };
}

template <typename T, typename E>
class result {

public:

    bool has_value() const { return m_has_value; }

    bool is_error() const { return !m_has_value; }
    
    const T& value() const& {

        assert(m_has_value && "result::value() called on error result");

        return m_storage.value;
    }

    T value() && {

        assert(m_has_value && "result::value() called on error result");

        return std::move(m_storage.value);
    }

    const E& error() const& {

        assert(!m_has_value && "result::error() called on success result");

        return m_storage.error;
    }

    T value_or(const T& fallback) const {
        
        return m_has_value ? m_storage.value : fallback;
    }

    T value_or(T&& fallback) && {
        
        return m_has_value ? std::move(m_storage.value) : std::move(fallback);
    }

    result(T ok) : m_has_value(true) {
        
        new (&m_storage.value) T (std::move(ok));
    }
    
    template <typename U, typename = typename std::enable_if<std::is_convertible<U, E>::value>::type>
    result(details::make_error<U> err) : m_has_value(false) {
        
        new (&m_storage.error) E(std::move(err.value));
    }

    result(const result& other) : m_has_value(other.m_has_value) {
        
        if (m_has_value) {
            
            new (&m_storage.value) T(other.m_storage.value);
        
        }
        else {
            
            new (&m_storage.error) E(other.m_storage.error);
        }
    }
    
    result(result&& other) : m_has_value(other.m_has_value) {
            
            if (m_has_value) {
                
                new (&m_storage.value) T(std::move(other.m_storage.value));
            }
            else {
                
                new (&m_storage.error) E(std::move(other.m_storage.error));
            }
    }
    
    result& operator=(const result& other) {
        
        if (this != &other) {
            
            if (m_has_value) {
                
                m_storage.value.~T();
            }
            else {

                m_storage.error.~E();
            }

            m_has_value = other.m_has_value;

            if (m_has_value) {

                new (&m_storage.value) T(other.m_storage.value);
            }
            else {

                new (&m_storage.error) E(other.m_storage.error);
            }
        }

        return *this;
    }

    result& operator=(result&& other) {
        
        if (this != &other) {

            if (m_has_value) {
                
                m_storage.value.~T();
            }
            else {
                
                m_storage.error.~E();
            }
            
            m_has_value = other.m_has_value;
            
            if (m_has_value) {
                
                new (&m_storage.value) T(std::move(other.m_storage.value));
            }
            else {
                
                new (&m_storage.error) E(std::move(other.m_storage.error));
            }
        }
        
        return *this;
    }
    
    ~result() {
        
        if (m_has_value) {

            m_storage.value.~T();
        }
        else {

            m_storage.error.~E();
        }
    }
    
    explicit operator bool() const {
        
        return m_has_value;
    }
    
    constexpr T& operator*() & {
        
        return m_storage.value;
    }
    
    constexpr T&& operator*() && {
        
        return std::move(m_storage.value);
    }

    constexpr T* operator->() {
        
        return &m_storage.value; 
    }

    constexpr bool operator==(const result& other) const {

        if ((this->has_value != other.has_value) || !has_value) {

            return false;
        }

        return (other.m_storage.value == this->m_storage.value);
    }

    constexpr bool operator!=(const result& other) const {

        return !(other == *this);
    }

private:

    bool m_has_value;

    union storage {

        T value;

        E error;

        storage() {}
        ~storage() {}

    } m_storage;

};


template <typename E>
class result <void, E> {

public:

    bool has_value() const { return m_has_value; }

    bool is_error() const { return !m_has_value; }

    const E& error() const& {

        assert(!m_has_value && "result::error() called on success result");

        return m_storage.error;
    }

    result() : m_has_value(true) {}
    
    template <typename U, typename = typename std::enable_if<std::is_convertible<U, E>::value>::type>
    result(details::make_error<U> err) : m_has_value(false) {
        
        new (&m_storage.error) E(std::move(err.value));
    }
    
    result(const result& other) : m_has_value(other.m_has_value) {
        
        if (!m_has_value) {
            
            new (&m_storage.error) E(other.m_storage.error);
        }
    }
    
    result(result&& other) : m_has_value(other.m_has_value) {
        
        if (!m_has_value) {
            
            new (&m_storage.error) E(std::move(other.m_storage.error));
        }
    }
    
    result& operator=(const result& other) {
        
        if (this != &other) {
            
            if (!m_has_value) {
                
                m_storage.error.~E();
            }
            m_has_value = other.m_has_value;
            
            if (!m_has_value) {
                
                new (&m_storage.error) E(other.m_storage.error);
            }
        }

        return *this;
    }
    
    result& operator=(result&& other) {
        
        if (this != &other) {
            
            if (!m_has_value) {
                
                m_storage.error.~E();
            }
            m_has_value = other.m_has_value;
            
            if (!m_has_value) {
                
                new (&m_storage.error) E(std::move(other.m_storage.error));
            }
        }

        return *this;
    }

    ~result() {
        
        if (!m_has_value) {
            
            m_storage.error.~E();
        }
    }
    
    explicit operator bool() const {
        
        return m_has_value;
    }

private:

    bool m_has_value;

    union storage {

        E error;

        storage() {}
        ~storage() {}

    } m_storage;
};


template <typename E>
details::make_error<E> make_error(E value) {

    return details::make_error<E>(std::move(value));
}

template <typename E, typename U, typename std::enable_if<std::is_convertible<U, E>::value, bool>::type = true>
details::make_error<E> make_error(U&& value) {

    return details::make_error<E>(std::forward<U>(value));
}

}


#endif // RESULT_H


