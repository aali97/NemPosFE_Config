// The Art of C++ / taopq
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_PQ_FIELD_h
#define TAO_PQ_FIELD_h

#include <null.h>
#include <result_traits.h>

#include <optional>
#include <type_traits>

namespace taopq
{
class row;

class field
{
private:
    friend class row;

    const row& row_;
    const std::size_t column_;

    field( const row& row, const std::size_t column )
        : row_( row ),
          column_( column )
    {
    }

public:
    [[nodiscard]] std::string name() const;

    [[nodiscard]] bool is_null() const;
    [[nodiscard]] const char* get() const;

    template< typename T >
    [[nodiscard]] std::enable_if_t< result_traits_size< T > != 1, T > as() const
    {
        static_assert( !std::is_same_v< T, T >, "taopq::result_traits<T>::size does not yield exactly one column for T, which is required for field access" );
        __builtin_unreachable();
    }

    template< typename T >
    [[nodiscard]] std::enable_if_t< result_traits_size< T > == 1, T > as() const;  // implemented in row.h

    template< typename T >
    [[nodiscard]] std::optional< T > optional() const
    {
        return as< std::optional< T > >();
    }
};

[[nodiscard]] inline bool operator==( const field& f, const null_t& )
{
    return f.is_null();
}

[[nodiscard]] inline bool operator!=( const field& f, const null_t& )
{
    return !f.is_null();
}

}  // namespace taopq

#endif
