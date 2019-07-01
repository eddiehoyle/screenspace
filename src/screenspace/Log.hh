// Copyright 2019 Edward Hoyle
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TECHNIC_LOG_HH
#define TECHNIC_LOG_HH

#include <mutex>
#include <ostream>

namespace technic {

enum class Level {
    Debug,
    Info,
    Warning,
    Error,
};

class Stream;
class NullStream;

class Log {
public:
    Log( std::ostream* stream, Level level );
private:
    std::mutex m_mutex;
    std::ostream* m_stream;
    Level m_level;

    friend Stream;
    friend NullStream;
};

class Stream {
public:
    explicit Stream( Log& log, const std::string& prefix  );
    ~Stream();

    template< typename T >
    Stream& operator<<( T value );

private:
    Stream( Stream& ) = delete;
    Stream& operator=( Stream& ) = delete;
    Stream& operator=( Stream&& ) = delete;

    Log& m_log;
};

template< typename T >
Stream& Stream::operator<<( T value ) {
    *( m_log.m_stream ) << value;
    return *this;
}

class NullStream {
public:
    explicit NullStream( Log& log, const std::string& prefix );
    ~NullStream() = default;

    template< typename T >
    NullStream& operator<<( T value );

private:
    NullStream( NullStream& ) = delete;
    NullStream& operator=( Stream& ) = delete;
    NullStream& operator=( NullStream&& ) = delete;

    Log& m_log;
};

template< typename T >
NullStream& NullStream::operator<<( T value ) {
    return *this;
}

Log& debug_log();
Log& info_log();
Log& warn_log();
Log& error_log();

} // namespace technic

#define TNC_INITIALISE_LOG() \
    ::technic::debug_log();     \
    ::technic::info_log();      \
    ::technic::warn_log();      \
    ::technic::error_log();

#ifdef TNC_LOGGING_ENABLED
#define TNC_DEBUG ::technic::Stream( ::technic::debug_log(), "DBG" ) << "[" << __FUNCTION__ << "]: "
#define TNC_INFO ::technic::Stream( ::technic::info_log(), "NFO" ) << "[" << __FUNCTION__ << "]: "
#define TNC_WARN ::technic::Stream( ::technic::warn_log(), "WRN" ) << "[" << __FUNCTION__ << "]: "
#define TNC_ERROR ::technic::Stream( ::technic::error_log(), "ERR" ) << "[" << __FUNCTION__ << "]: "
#else
#define TNC_DEBUG ::technic::NullStream( ::technic::debug_log(), "DBG" )
#define TNC_INFO ::technic::NullStream( ::technic::info_log(), "NFO" )
#define TNC_WARN ::technic::NullStream( ::technic::warn_log(), "WRN" )
#define TNC_ERROR ::technic::NullStream( ::technic::error_log(), "ERR" )
#endif

#endif // TECHNIC_LOG_HH
