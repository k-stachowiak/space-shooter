/* Copyright (C) 2013 Krzysztof Stachowiak */

/*
 * This file is part of space-shooter.
 *
 * space-shooter is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * space-shooter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with space-shooter; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "config.h"
#include "high_score.h"

high_score::high_score(std::string filename) : m_filename(filename) {
        std::ifstream in(m_filename.c_str());
        for(unsigned i = 0; i < cfg::real("high_score_entries_count"); ++i) {
                entry e;
				char line[128] = { 0 };

                in.getline(line, 128, '#');
                e.name = line;

                in.getline(line, 128, '\n');
                {
                        std::stringstream ss(line);
                        ss >> e.score;
                }

                m_entries.push_back(e);
        }
        in.close();

        std::sort(begin(m_entries), end(m_entries), entry_comparer());
        std::reverse(begin(m_entries), end(m_entries));
}

high_score::~high_score() {
        std::ofstream out(m_filename.c_str(), std::ios_base::trunc);
        for(auto const& e : m_entries) {
                out << e.name << '#' << e.score << std::endl;
        }
        out.close();
} 

std::vector<high_score::entry> const& high_score::get_entries() const {
        return m_entries;
}

void high_score::try_adding_entry(entry const& e) {
        if(e.score < m_entries.back().score)
                return;

        m_entries.back() = e;
        std::sort(begin(m_entries), end(m_entries), entry_comparer());
        std::reverse(begin(m_entries), end(m_entries));
}

bool high_score::can_add_score(unsigned score) const {
        return score > m_entries.back().score;
}

