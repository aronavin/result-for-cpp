#ifndef RESULT_H
#define RESULT_H

#include <utility>
#include <stdexcept>
#include <type_traits>


namespace result {

struct none {};
struct error_kind {};

namespace details {

    template <typename T>
    struct make_ok {

        T value;

        explicit make_ok(T val) : value(std::move(val)) {}
    };

    template <>
    struct make_ok<none> {

        make_ok() = default;
    };

    template <typename E>
    struct make_error {

        E value;

        explicit make_error(E val) : value(std::move(val)) {}

        template <typename U, typename std::enable_if<std::is_convertible<U, E>::value, bool>::type = true>
        make_error(U&& val) : value(std::forward<U>(val)) {}
    };
}

template <typename T = none, typename E = error_kind>
class result {

public:

    bool is_ok() const { return m_is_ok; }

    bool is_error() const { return !m_is_ok; }
    
    template <typename U = T, typename std::enable_if<!std::is_same<U, none>::value, bool>::type = true>
    const T& value() const& {

        if (!m_is_ok) {

            throw std::runtime_error("result::value() called on error result - check is_ok() first");
        }

        return m_storage.value;
    }

    template <typename U = T, typename std::enable_if<!std::is_same<U, none>::value, bool>::type = true>
    T value() && {

        if (!m_is_ok) {

            throw std::runtime_error("result::value() called on error result - check is_ok() first");
        }

        return std::move(m_storage.value);
    }

    const E& error() const& {

        if (m_is_ok) {

            throw std::runtime_error("result::error() called on success result - check is_error() first");
        }

        return m_storage.error;
    }

    template <typename U = T, typename std::enable_if<!std::is_same<U, none>::value, bool>::type = true>
    T value_or(const T& fallback) const {
        
        return m_is_ok ? m_storage.value : fallback;
    }

    template <typename U = T, typename std::enable_if<!std::is_same<U, none>::value, bool>::type = true>
    result(details::make_ok<T> ok) : m_is_ok(true) {
        
        new (&m_storage.value) T (std::move(ok.value));
    }

    template <typename U = T, typename std::enable_if<std::is_same<U, none>::value, bool>::type = true>
    result(details::make_ok<T> ok) : m_is_ok(true) {

        new (&m_storage.value) T();
    }


    result(details::make_error<E> err) : m_is_ok(false) {
        
        new (&m_storage.error) E (std::move(err.value));
    }

    template <typename U, typename std::enable_if<std::is_convertible<U, E>::value, bool>::type = true>
    result(details::make_error<U> err) : m_is_ok(false) {

        new (&m_storage.error) E (std::move(err.value));
    }
        
    ~result() {
        
        if (m_is_ok) {

            m_storage.value.~T();
        }
        else {

            m_storage.error.~E();
        }
    }
    
    explicit operator bool() const {
        
        return m_is_ok;
    }

private:

    bool m_is_ok;

    union storage {

        T value;

        E error;

        storage() {}
        ~storage() {}

    } m_storage;

};

template <typename T>
details::make_ok<T> make_ok(T value) {

    return details::make_ok<T>(std::move(value));
}

inline details::make_ok<none> make_ok() {

    return {};
}

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


