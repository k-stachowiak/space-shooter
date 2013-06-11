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

#ifndef HIGH_SCORE_H
#define HIGH_SCORE_H

#include <string>
#include <vector>

class high_score {
public:
        struct entry {
                std::string name;
                unsigned score;
        };

private:
        std::string m_filename;
        std::vector<entry> m_entries;

        struct entry_comparer {
                bool operator()(entry const& lhs, entry const& rhs) const {
                        return lhs.score < rhs.score;
                }
        };

public:
        high_score(std::string filename);
        ~high_score();

        std::vector<entry> const& get_entries() const;
        void try_adding_entry(entry const& e);
        bool can_add_entry(entry const& e) const;
};

#endif
