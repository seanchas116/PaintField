#ifndef JSON_SPIRIT_WRITER
#define JSON_SPIRIT_WRITER

//          Copyright John W. Wilkinson 2007 - 2013
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.06

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "json_spirit_value.h"
#include "json_spirit_writer_options.h"
#include "json_spirit_writer_template.h"
#include <iostream>

namespace json_spirit
{
    // these functions to convert JSON Values to text

#ifdef JSON_SPIRIT_VALUE_ENABLED
    inline void write( const Value& value, std::ostream& os, unsigned int options )
    {
        write_stream( value, os, options );
    }
    inline std::string write( const Value& value, unsigned int options )
    {
        return write_string( value, options );
    }

    inline void write_formatted( const Value& value, std::ostream& os )
    {
        write_stream( value, os, pretty_print );
    }

    inline std::string write_formatted( const Value& value )
    {
        return write_string( value, pretty_print );
    }
#endif

#ifdef JSON_SPIRIT_MVALUE_ENABLED
    inline void write( const mValue& value, std::ostream& os, unsigned int options )
    {
        write_stream( value, os, options );
    }

    inline std::string write( const mValue& value, unsigned int options )
    {
        return write_string( value, options );
    }

    inline void write_formatted( const mValue& value, std::ostream& os )
    {
        write_stream( value, os, pretty_print );
    }

    inline std::string write_formatted( const mValue& value )
    {
        return write_string( value, pretty_print );
    }
#endif

#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
    inline void write( const wValue& value, std::wostream& os, unsigned int options )
    {
        write_stream( value, os, options );
    }

    inline std::wstring write( const wValue& value, unsigned int options )
    {
        return write_string( value, options );
    }

    inline void write_formatted( const wValue& value, std::wostream& os )
    {
        write_stream( value, os, pretty_print );
    }

    inline std::wstring write_formatted( const wValue& value )
    {
        return write_string( value, pretty_print );
    }
#endif

#if defined( JSON_SPIRIT_WMVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
    inline void write_formatted( const wmValue& value, std::wostream& os )
    {
        write_stream( value, os, pretty_print );
    }

    inline std::wstring write_formatted( const wmValue& value )
    {
        return write_string( value, pretty_print );
    }

    inline void write( const wmValue& value, std::wostream& os, unsigned int options )
    {
        write_stream( value, os, options );
    }

    inline std::wstring write( const wmValue& value, unsigned int options )
    {
        return write_string( value, options );
    }
#endif
}

#endif
