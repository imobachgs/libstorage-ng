#ifndef STORAGE_ACTIONGRAPH_IMPL_H
#define STORAGE_ACTIONGRAPH_IMPL_H


#include <list>
#include <map>
#include <deque>
#include <set>
#include <boost/noncopyable.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "storage/Devices/Device.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Actiongraph.h"


namespace storage
{
    using std::string;
    using std::vector;
    using std::list;
    using std::map;
    using std::deque;
    using std::set;


    class Storage;
    class CommitCallbacks;


    namespace Action
    {
	class Base;
    }


    class Actiongraph::Impl : private boost::noncopyable
    {
    public:

	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
				      std::shared_ptr<Action::Base>> graph_t;

	typedef graph_t::vertex_descriptor vertex_descriptor;
	typedef graph_t::edge_descriptor edge_descriptor;

	typedef graph_t::vertex_iterator vertex_iterator;
	typedef graph_t::edge_iterator edge_iterator;

	typedef graph_t::vertices_size_type vertices_size_type;

	Impl(const Storage& storage, const Devicegraph* lhs, const Devicegraph* rhs);

	const Storage& get_storage() const { return storage; }

	const Devicegraph* get_devicegraph(Side side) const { return side == LHS ? lhs : rhs; }

	bool empty() const;

	vertex_descriptor add_vertex(Action::Base* action);

	edge_descriptor add_edge(vertex_descriptor a, vertex_descriptor b);

	void add_chain(vector<Action::Base*>& actions);

	vector<vertex_descriptor> huhu(sid_t sid, bool first, bool last) const; // TODO better concept

	boost::iterator_range<vertex_iterator> vertices() const;

	Action::Base* get_vertex(vertex_descriptor v) { return graph[v].get(); }
	const Action::Base* get_vertex(vertex_descriptor v) const { return graph[v].get(); }

	void print_graph() const;
	void write_graphviz(const string& filename, bool details = false) const;

	vector<const Action::Base*> get_commit_actions() const;
	void commit(const CommitCallbacks* commit_callbacks) const;

	graph_t graph;		// TODO private?

	// special actions, TODO make private and provide interface
	vertex_iterator mount_root_filesystem;

    private:

	void get_actions();
	void add_dependencies();
	void set_special_actions();
	void get_order();

	const Storage& storage;

	const Devicegraph* lhs;
	const Devicegraph* rhs;

	typedef deque<vertex_descriptor> Order;

	Order order;

    };

}

#endif