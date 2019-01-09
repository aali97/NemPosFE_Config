// The Art of C++ / taopq
// Copyright (c) 2016-2018 Daniel Frey

#include <transaction.h>

#include <stdexcept>

#include <connection.h>
#include <internal/printf.h>

namespace taopq
{
namespace
{
class transaction_base
        : public transaction
{
private:
    const std::shared_ptr< transaction > previous_;

protected:
    explicit transaction_base( const std::shared_ptr< taopq::connection >& connection )
        : transaction( connection ),
          previous_( current_transaction()->shared_from_this() )
    {
        current_transaction() = this;
    }

    ~transaction_base()
    {
        if( connection_ ) {
            current_transaction() = previous_.get();  // LCOV_EXCL_LINE
        }
    }

    [[nodiscard]] bool v_is_direct() const
    {
        return false;
    }

    void v_reset() noexcept
    {
        current_transaction() = previous_.get();
        connection_.reset();
    }
};

class top_level_transaction final
        : public transaction_base
{
public:
    explicit top_level_transaction( const std::shared_ptr< taopq::connection >& connection )
        : transaction_base( connection )
    {
        execute( "START TRANSACTION" );
    }

    ~top_level_transaction()
    {
        if( connection_ && connection_->is_open() ) {
            try {
                rollback();
            }
            // LCOV_EXCL_START
            catch( const std::exception& ) {
                // TAO_LOG( WARNING, "unable to rollback transaction, swallowing exception: " + std::string( e.what() ) );
            }
            catch( ... ) {
                // TAO_LOG( WARNING, "unable to rollback transaction, swallowing unknown exception" );
            }
            // LCOV_EXCL_STOP
        }
    }

private:
    void v_commit()
    {
        execute( "COMMIT TRANSACTION" );
    }

    void v_rollback()
    {
        execute( "ROLLBACK TRANSACTION" );
    }
};

class nested_transaction final
        : public transaction_base
{
public:
    explicit nested_transaction( const std::shared_ptr< connection >& connection )
        : transaction_base( connection )
    {
        execute( internal::printf( "SAVEPOINT \"TAOPQ_%p\"", static_cast< void* >( this ) ) );
    }

    ~nested_transaction()
    {
        if( connection_ && connection_->is_open() ) {
            try {
                rollback();
            }
            // LCOV_EXCL_START
            catch( const std::exception& ) {
                // TODO: Add more information about exception when available
                // TAO_LOG( WARNING, "unable to rollback transaction, swallowing exception: " + std::string( e.what() ) );
            }
            catch( ... ) {
                // TAO_LOG( WARNING, "unable to rollback transaction, swallowing unknown exception" );
            }
            // LCOV_EXCL_STOP
        }
    }

private:
    void v_commit()
    {
        execute( internal::printf( "RELEASE SAVEPOINT \"TAOPQ_%p\"", static_cast< void* >( this ) ) );
    }

    void v_rollback()
    {
        execute( internal::printf( "ROLLBACK TO \"TAOPQ_%p\"", static_cast< void* >( this ) ) );
    }
};
}  // namespace

transaction::transaction( const std::shared_ptr< taopq::connection >& connection )
    : connection_( connection )
{
}

transaction::~transaction()
{
}

transaction*& transaction::current_transaction() const noexcept
{
    return connection_->current_transaction_;
}

void transaction::check_current_transaction() const
{
    if( !connection_ || this != current_transaction() ) {
        throw std::logic_error( "transaction order error" );
    }
}

result transaction::execute_params( const std::string& statement, const int n_params, const char* const param_values[] )
{
    check_current_transaction();
    return connection_->execute_params( statement, n_params, param_values );
}

void transaction::commit()
{
    check_current_transaction();
    try {
        v_commit();
    }
    // LCOV_EXCL_START
    catch( ... ) {
        v_reset();
        throw;
    }
    // LCOV_EXCL_STOP
    v_reset();
}

void transaction::rollback()
{
    check_current_transaction();
    try {
        v_rollback();
    }
    // LCOV_EXCL_START
    catch( ... ) {
        v_reset();
        throw;
    }
    // LCOV_EXCL_STOP
    v_reset();
}

std::shared_ptr< transaction > transaction::subtransaction()
{
    check_current_transaction();
    if( v_is_direct() ) {
        return std::make_shared< top_level_transaction >( connection_ );
    }
    else {
        return std::make_shared< nested_transaction >( connection_ );
    }
}

}  // namespace taopq
