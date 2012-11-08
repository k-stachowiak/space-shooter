#include <algorithm>
using namespace std;

#include "../debug.h"

#include "collision_space.h"

void collision_space::register_shape(collision_class cc, shared_ptr<shape> shp, function<void(const coll_report&)> cb) {
	_collision_entries.push_back(coll_entry(cc, shp, cb));	
}

void collision_space::check_collisions() {
	for(auto a = begin(_collision_entries); a != end(_collision_entries); ++a) {
		for(auto b = (a + 1); b != end(_collision_entries); ++b) {
			const shape& shp_a = *a->shp;
			const shape& shp_b = *b->shp;

			if(shp_a.collide_with(shp_b)) {

				coll_report report;
				report.class_a = a->cc;
				report.shape_a = a->shp;

				report.class_b = b->cc;
				report.shape_b = b->shp;

				a->callback(report);
				b->callback(report);
			}
		}
	}
}

void collision_space::remove_entry_by_shape_id(uint32_t id) {
	auto new_end = remove_if(
		begin(_collision_entries), 
		end(_collision_entries),
		[id](const coll_entry& ce) { return ce.shp->get_id() == id; });
	_collision_entries.erase(new_end, end(_collision_entries));
}
