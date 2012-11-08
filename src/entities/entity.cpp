#include "entity.h"

using namespace std::placeholders;

void entity::exhaust_msg_queue(const vector<ent_msg>& messages) {

	// Store the messages that need to be processed.
	vector<ent_msg> all_msgs(messages);
	while(!all_msgs.empty()) {

		// Gather messages raised by the message handling code.
		vector<ent_msg> new_msgs;

		// Make all the components process all the messages.
		for(auto& c : _components)
			for(const auto& msg : all_msgs)
				c->receive_message(msg, new_msgs);

		// Replace previous messages with the ones gathered
		// while processing them.
		all_msgs = new_msgs;
	}
}

entity::entity()
	: _id(0) {
}

entity::entity(const entity& copy)
	: _id(copy._id)
	, _components(copy._components) {
}

entity& entity::operator=(const entity& rhs) {
	_id = rhs._id;
	_components = rhs._components;
	return *this;
}

entity::entity(uint32_t id, const vector<shared_ptr<component>>& components)
	: _id(id)
	, _components(components) {
}

uint32_t entity::get_id() const {
	return _id;
}

void entity::initialize_components() {

	vector<ent_msg> all_msgs;
	for(auto& c : _components) {
		c->initialize(all_msgs);
	}
	exhaust_msg_queue(all_msgs);
}

void entity::deinitialize_components(game_model& gm) {
	for(auto& c : _components) {
		c->deinitialize(gm);
	}
}

void entity::action(const config& cfg, game_model& gm) {
	vector<ent_msg> all_msgs;
	for(auto& c : _components) {
		c->action(cfg, gm, all_msgs);
	}
	exhaust_msg_queue(all_msgs);
}

void entity::visit_components(function<void(component const&)> f) const {
	for(auto const& c : _components)
		f(*c);
}
