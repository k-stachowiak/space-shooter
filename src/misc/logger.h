#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <sstream>

namespace logger {

        class logger {
                std::stringstream m_buffer;
                logger();
        public:
                void trace(std::string const& msg);
                void flush();
                static logger& instance();
        };

}

#endif
