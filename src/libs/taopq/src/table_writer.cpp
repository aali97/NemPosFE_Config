// The Art of C++ / taopq
// Copyright (c) 2016-2018 Daniel Frey

#include <table_writer.h>

#include <libpq-fe.h>

#include <connection.h>
#include <result.h>
#include <transaction.h>

namespace taopq
{
table_writer::table_writer( const std::shared_ptr< transaction >& transaction, const std::string& statement )
    : transaction_( transaction )
{
    result( ::PQexecParams( transaction->connection_->pgconn_.get(), statement.c_str(), 0, nullptr, nullptr, nullptr, nullptr, 0 ), result::mode_t::expect_copy_in );
}

table_writer::~table_writer()
{
    if( transaction_ ) {
        ::PQputCopyEnd( transaction_->connection_->pgconn_.get(), "cancelled in dtor" );
    }
}

void table_writer::insert( const std::string& data )
{
    const int r = ::PQputCopyData( transaction_->connection_->pgconn_.get(), data.data(), static_cast< int >( data.size() ) );
    if( r != 1 ) {
        throw std::runtime_error( "::PQputCopyData() failed: " + transaction_->connection_->error_message() );
    }
}

std::size_t table_writer::finish()
{
    const auto connection = transaction_->connection_;
    const int r = ::PQputCopyEnd( connection->pgconn_.get(), nullptr );
    if( r != 1 ) {
        throw std::runtime_error( "::PQputCopyEnd() failed: " + connection->error_message() );
    }
    transaction_.reset();
    return result( ::PQgetResult( connection->pgconn_.get() ) ).rows_affected();
}

}  // namespace taopq
