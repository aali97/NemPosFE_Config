// The Art of C++ / taopq
// Copyright (c) 2016-2018 Daniel Frey

#include <connection_pool.h>

namespace taopq
{
connection_pool::connection_pool( const connection_pool::private_key&, const std::string& connection_info )
    : connection_info_( connection_info )
{
}

std::unique_ptr< connection > connection_pool::v_create() const
{
    return std::make_unique< taopq::connection >( connection::private_key(), connection_info_ );
}

bool connection_pool::v_is_valid( taopq::connection& c ) const
{
    return c.is_open();
}

std::shared_ptr< connection_pool > connection_pool::create( const std::string& connection_info )
{
    return std::make_shared< connection_pool >( connection_pool::private_key(), connection_info );
}

}  // namespace taopq
