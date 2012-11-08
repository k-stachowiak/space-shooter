#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>

#include <functional>
#include <vector>
#include <memory>
using namespace std;

#include "../components/component.h"

class entity {

	uint32_t _id;
	vector<shared_ptr<component>> _components;

	void exhaust_msg_queue(const vector<ent_msg>& messages);

public:
	// <for std::vector storage>
	entity();
	entity(const entity& copy);
	entity& operator=(const entity& rhs);
	// </for std::vector storage>

	entity(uint32_t id, const vector<shared_ptr<component>>& components);

	uint32_t get_id() const;

	void initialize_components();
	void deinitialize_components(game_model& gm);
	void action(const config& cfg, game_model& gs);

	void visit_components(function<void(component const&)> f) const;
};

#endif
