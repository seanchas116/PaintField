#ifndef JSON_SPIRIT_READER
#define JSON_SPIRIT_READER

//          Copyright John W. Wilkinson 2007 - 2011
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.04

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "json_spirit_value.h"
#include "json_spirit_error_position.h"
#include <iostream>
#include "json_spirit_reader_template.h"

namespace json_spirit
{
    // functions to reads a JSON values

    bool read( const std::string& s, Value& value );
    bool read( std::istream& is,     Value& value );
    bool read( std::string::const_iterator& begin, std::string::const_iterator end, Value& value );

    void read_or_throw( const std::string& s, Value& value );  
    void read_or_throw( std::istream& is,     Value& value );
    void read_or_throw( std::string::const_iterator& begin, std::string::const_iterator end, Value& value );

#ifndef BOOST_NO_STD_WSTRING

    bool read( const std::wstring& s, wValue& value );
    bool read( std::wistream&  is,    wValue& value );
    bool read( std::wstring::const_iterator& begin, std::wstring::const_iterator end, wValue& value );    

    void read_or_throw( const std::wstring& s, wValue& value );
    void read_or_throw( std::wistream& is,     wValue& value );
    void read_or_throw( std::wstring::const_iterator& begin, std::wstring::const_iterator end, wValue& value );

#endif

    bool read( const std::string& s, mValue& value );
    bool read( std::istream& is,     mValue& value );
    bool read( std::string::const_iterator& begin, std::string::const_iterator end, mValue& value );

    void read_or_throw( const std::string& s, mValue& value );  
    void read_or_throw( std::istream& is,     mValue& value );
    void read_or_throw( std::string::const_iterator& begin, std::string::const_iterator end, mValue& value );

#ifndef BOOST_NO_STD_WSTRING

    bool read( const std::wstring& s, wmValue& value );
    bool read( std::wistream& is,     wmValue& value );
    bool read( std::wstring::const_iterator& begin, std::wstring::const_iterator end, wmValue& value );    

    void read_or_throw( const std::wstring& s, wmValue& value );
    void read_or_throw( std::wistream& is,     wmValue& value );
    void read_or_throw( std::wstring::const_iterator& begin, std::wstring::const_iterator end, wmValue& value );

#endif
	
	inline bool read( const std::string& s, Value& value )
	{
	    return read_string( s, value );
	}
	
	inline void read_or_throw( const std::string& s, Value& value )
	{
	    read_string_or_throw( s, value );
	}
	
	inline bool read( std::istream& is, Value& value )
	{
	    return read_stream( is, value );
	}
	
	inline void read_or_throw( std::istream& is, Value& value )
	{
	    read_stream_or_throw( is, value );
	}
	
	inline bool read( std::string::const_iterator& begin, std::string::const_iterator end, Value& value )
	{
	    return read_range( begin, end, value );
	}
	
	inline void read_or_throw( std::string::const_iterator& begin, std::string::const_iterator end, Value& value )
	{
	    begin = read_range_or_throw( begin, end, value );
	}
	
	#ifndef BOOST_NO_STD_WSTRING
	
	inline bool read( const std::wstring& s, wValue& value )
	{
	    return read_string( s, value );
	}
	
	inline void read_or_throw( const std::wstring& s, wValue& value )
	{
	    read_string_or_throw( s, value );
	}
	
	inline bool read( std::wistream& is, wValue& value )
	{
	    return read_stream( is, value );
	}
	
	inline void read_or_throw( std::wistream& is, wValue& value )
	{
	    read_stream_or_throw( is, value );
	}
	
	inline bool read( std::wstring::const_iterator& begin, std::wstring::const_iterator end, wValue& value )
	{
	    return read_range( begin, end, value );
	}
	
	inline void read_or_throw( std::wstring::const_iterator& begin, std::wstring::const_iterator end, wValue& value )
	{
	    begin = read_range_or_throw( begin, end, value );
	}
	
	#endif
	
	inline bool read( const std::string& s, mValue& value )
	{
	    return read_string( s, value );
	}
	
	inline void read_or_throw( const std::string& s, mValue& value )
	{
	    read_string_or_throw( s, value );
	}
	
	inline bool read( std::istream& is, mValue& value )
	{
	    return read_stream( is, value );
	}
	
	inline void read_or_throw( std::istream& is, mValue& value )
	{
	    read_stream_or_throw( is, value );
	}
	
	inline bool read( std::string::const_iterator& begin, std::string::const_iterator end, mValue& value )
	{
	    return read_range( begin, end, value );
	}
	
	inline void read_or_throw( std::string::const_iterator& begin, std::string::const_iterator end, mValue& value )
	{
	    begin = read_range_or_throw( begin, end, value );
	}
	
	#ifndef BOOST_NO_STD_WSTRING
	
	inline bool read( const std::wstring& s, wmValue& value )
	{
	    return read_string( s, value );
	}
	
	inline void read_or_throw( const std::wstring& s, wmValue& value )
	{
	    read_string_or_throw( s, value );
	}
	
	inline bool read( std::wistream& is, wmValue& value )
	{
	    return read_stream( is, value );
	}
	
	inline void read_or_throw( std::wistream& is, wmValue& value )
	{
	    read_stream_or_throw( is, value );
	}
	
	inline bool read( std::wstring::const_iterator& begin, std::wstring::const_iterator end, wmValue& value )
	{
	    return read_range( begin, end, value );
	}
	
	inline void read_or_throw( std::wstring::const_iterator& begin, std::wstring::const_iterator end, wmValue& value )
	{
	    begin = read_range_or_throw( begin, end, value );
	}
	
	#endif

}




#endif
