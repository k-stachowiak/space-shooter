#include <fstream>

#include "logger.h"

logger::logger() {
        std::ofstream out("log", std::ios_base::trunc);
        out.close();
}

void logger::trace(std::string const& msg) {
        m_buffer << msg << std::endl;
}

void logger::flush() {
        std::ofstream out("log", std::ios_base::out | std::ios_base::app);
        if(!out.is_open())
                throw;
        out << m_buffer.str();
        out.close();
        m_buffer.str(std::string());
}

logger& logger::instance() {
        static logger inst;
        return inst;
}

