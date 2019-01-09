// The Art of C++ / taopq
// Copyright (c) 2016-2018 Daniel Frey

#include <internal/demangle.h>

#ifndef WIN32
#include <cstdlib>
#include <cxxabi.h>
#include <memory>
#endif

namespace taopq
{
namespace internal
{
std::string demangle( const char* const symbol )
{
#ifdef WIN32
    return symbol;
#else
    const std::unique_ptr< char, decltype( &std::free ) > demangled( abi::__cxa_demangle( symbol, nullptr, nullptr, nullptr ), &std::free );
    return demangled ? demangled.get() : symbol;
#endif
}

std::string demangle( const std::type_info& type_info )
{
    return demangle( type_info.name() );
}

}  // namespace internal

}  // namespace taopq
