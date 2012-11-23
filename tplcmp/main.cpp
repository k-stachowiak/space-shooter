#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

enum class ent_msg_t : uint32_t {
	PING,
	PONG
};

struct ent_msg {
	ent_msg_t type;

	union {
		struct {
			uint32_t param;
		} ping;

		struct {
			uint32_t param;
		} pong;
	} _;
};

/// Forward declaration.
template<class... COMPONENTS>
class entity;

/// The entity speciffic content.
template<>
class entity<> {
	uint32_t _id;
	vector<ent_msg> _messages;
	vector<ent_msg> _bound_msgs;

public:	
	entity(uint32_t id) : _id(id) {}

	uint32_t get_id() const { return _id; }
	vector<ent_msg>& msgs() { return _messages; }
	const vector<ent_msg>& bound_msgs() const { return _bound_msgs; }
	void bind_msgs() { _bound_msgs = _messages; _messages.clear(); }
	bool has_msgs() const { return !_bound_msgs.empty(); }

	// Recursion plugs.
	// ----------------
	void init() { }
	void deinit() { }
	void action() { }
	void handle_msgs() {}
};

/// The intermediate component ingredient.
template<class HEAD, class... TAIL>
struct entity<HEAD, TAIL...> {

	/// The component associated with this.
	HEAD _cmp;

	/// The rest of this entity.
	entity<TAIL...> _tail;

	/// An intermediate constructor, initializing the current
	/// component and passing the id and the rest of the components
	/// further down the components list.
	entity(uint32_t id, const HEAD& cmp, const TAIL&... tail)
	: _cmp(cmp)
	, _tail(id, tail...) {
	}

	// Component speciffic calls.
	// --------------------------
	void init() { _cmp.init(); _tail.init(); }
	void deinit() { _cmp.deinit(); _tail.deinit(); }
	void action() { _cmp.action(_tail.msgs()); _tail.action(); }
	void handle_msgs() { _cmp.handle_msgs(_tail.bound_msgs(), _tail.msgs()); _tail.handle_msgs(); }

	// Entity speciffic proxies.
	// -------------------------
	uint32_t get_id() const { _tail.get_id(); }
	vector<ent_msg>& msgs() { return _tail.msgs(); }
	const vector<ent_msg>& bound_msgs() const { return _tail.bound_msgs(); }
	void bind_msgs() { _tail.bind_msgs(); }
	bool has_msgs() const { return _tail.has_msgs(); }
};

struct a {
	void init() { cout << "Initializing a." << endl; }
	void deinit() { cout << "Deinitializing a." << endl; }

	void action(vector<ent_msg>& out_msgs) {

		cout << "Performing an action on a." << endl;

		ent_msg m;
		m.type = ent_msg_t::PING;
		m._.ping.param = 1;

		out_msgs.push_back(m);
	}

	void handle_msgs(const vector<ent_msg>& in_msgs, vector<ent_msg>& out_msgs) {
	}
};

struct b {
	void init() { cout << "Initializing b." << endl; }
	void deinit() { cout << "Deinitializing b." << endl; }
	void action(vector<ent_msg>& out_msgs) { cout << "Performing an action on b." << endl; }
	void handle_msgs(const vector<ent_msg>& in_msgs, vector<ent_msg>& out_msgs) {

		for(const auto& m : in_msgs) {
			if(m.type == ent_msg_t::PING) {

				cout << "B received PING message." << endl;

				ent_msg m;
				m.type = ent_msg_t::PONG;
				m._.pong.param = 2;

				out_msgs.push_back(m);
			}
		}
	}
};

struct c {
	void init() { cout << "Initializing c." << endl; }
	void deinit() { cout << "Deinitializing c." << endl; }
	void action(vector<ent_msg>& out_msgs) { cout << "Performing an action on c." << endl; }
	void handle_msgs(const vector<ent_msg>& in_msgs, vector<ent_msg>& out_msgs) {
		for(const auto& m : in_msgs) {
			if(m.type == ent_msg_t::PING) {

				cout << "C received PING message." << endl;

				ent_msg m;
				m.type = ent_msg_t::PONG;
				m._.pong.param = 3;

				out_msgs.push_back(m);
			}
		}
	}
};

struct d {
	void init() { cout << "Initializing d." << endl; }
	void deinit() { cout << "Deinitializing d." << endl; }
	void action(vector<ent_msg>& out_msgs) { cout << "Performing an action on d." << endl; }
	void handle_msgs(const vector<ent_msg>& in_msgs, vector<ent_msg>& out_msgs) {
		for(const auto& m : in_msgs) {
			if(m.type == ent_msg_t::PONG) {
				cout << "D received PONG message." << endl;
			}
		}
	}
};

int main() {
	
	entity<a, b, c, d> ent(1, a(), b(), c(), d());

	// Let all the components initialize their external resources and
	// inform each other about themselves.
	ent.init(); 

	// Perform action - may spawn messages internally.
	ent.action();
	ent.bind_msgs();

	// Exhaust the internal messages queue.
	// Note that the message handlers are allowed to spawn
	// further messages. Use this feature with caution.
	while(ent.has_msgs()) {
		ent.handle_msgs();
		ent.bind_msgs();
	}

	// Allow the components to deallocate their extenral resources.
	ent.deinit();

	return 0;
}
