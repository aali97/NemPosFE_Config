// The Art of C++ / taopq
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_PQ_CONNECTION_POOL_h
#define TAO_PQ_CONNECTION_POOL_h

#include <memory>
#include <string>
#include <utility>

#include <internal/pool.h>

#include <connection.h>
#include <result.h>

namespace taopq
{
class connection_pool
        : public internal::pool< taopq::connection >
{
private:
    const std::string connection_info_;

    [[nodiscard]] std::unique_ptr< taopq::connection > v_create() const override;
    [[nodiscard]] bool v_is_valid( taopq::connection& c ) const override;

public:
    [[nodiscard]] static std::shared_ptr< connection_pool > create( const std::string& connection_info );

private:
    // pass-key idiom
    class private_key
    {
        private_key() = default;
        friend std::shared_ptr< connection_pool > connection_pool::create( const std::string& connect_info );
    };

public:
    connection_pool( const private_key&, const std::string& connection_info );

    [[nodiscard]] std::shared_ptr< taopq::connection > connection()
    {
        return this->get();
    }

    template< typename... Ts >
    result execute( Ts&&... ts )
    {
        return this->connection()->direct()->execute( std::forward< Ts >( ts )... );
    }
};

}  // namespace taopq

#endif
