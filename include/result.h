#ifndef RESULT_H
#define RESULT_H

#include <cassert>
#include <utility>
#include <type_traits>
#include <functional>


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


template <typename E>
details::make_error<E> make_error(E value) {

    return details::make_error<E>(std::move(value));
}

template <typename E, typename U, typename std::enable_if<std::is_convertible<U, E>::value, bool>::type = true>
details::make_error<E> make_error(U&& value) {

    return details::make_error<E>(std::forward<U>(value));
}

template <typename T, typename E>
class result {

public:

    constexpr bool s_value() const { return m_has_value; }

    constexpr bool is_error() const { return !m_has_value; }
    
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
    
    template <typename Callable>
    constexpr auto transform(Callable&& callback) const -> result<decltype(callback(std::declval<T>())), E> {
        
        using N = decltype(callback(std::declval<T>()));
        
        if (m_has_value) {
            
            return result<N, E>(callback(m_storage.value));
        }
        
        return result<N, E>(make_error(m_storage.error));
    }

    template <typename Callable>
    constexpr auto transform_error(Callable&& callback) const -> result<T, decltype(callback(std::declval<E>()))> {

        using N = decltype(callback(std::declval<E>()));
        
        if (m_has_value) {
            
            return result<T, N>(m_storage.value);
        }
        
        return result<T, N>(make_error(callback(m_storage.error)));
    }
    
    template <typename Callable>
    constexpr auto and_then(Callable&& callback) const -> decltype(callback(std::declval<T>())) {
    
        if (m_has_value) {
        
            return callback(m_storage.value);
        }
        
        return decltype(callback(std::declval<T>()))(make_error(m_storage.error));
    }
    
    template <typename Callable>
    constexpr auto or_else(Callable&& callback) const -> decltype(callback(std::declval<E>())) {

        if (!m_has_value) {

            return callback(m_storage.error);
        }

        return *this;
    }

    result(T ok) : m_has_value(true) {
        
        new (&m_storage.value) T (std::move(ok));
    }

    template <typename U, typename = typename std::enable_if<std::is_convertible<U, T>::value>::type>
    result(U&& ok) : m_has_value(true) {
        
        new (&m_storage.value) T(std::forward<U>(ok));
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

        if ((this->has_value != other.has_value) || !this->m_has_value) {

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

    constexpr bool has_value() const { return m_has_value; }

    constexpr bool is_error() const { return !m_has_value; }

    const E& error() const& {

        assert(!m_has_value && "result::error() called on success result");

        return m_storage.error;
    }
    
    template <typename Callable>
    constexpr auto and_then(Callable&& callback) const -> decltype(callback()) {
    
        if (m_has_value) {
        
            return callback();
        }
        
        return decltype(callback())(make_error(m_storage.error));
    }
    
    template <typename Callable>
    constexpr auto or_else(Callable&& callback) const -> decltype(callback(std::declval<E>())) {

        if (!m_has_value) {

            return callback(m_storage.error);
        }

        return *this;
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

}


#endif // RESULT_H


