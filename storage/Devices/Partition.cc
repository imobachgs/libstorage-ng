

#include "storage/Devices/PartitionImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage_bgl
{

    using namespace std;


    Partition*
    Partition::create(Devicegraph* devicegraph, const string& name)
    {
	Partition* ret = new Partition(new Partition::Impl(name));
	ret->Device::create(devicegraph);
	return ret;
    }


    Partition*
    Partition::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Partition* ret = new Partition(new Partition::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Partition::Partition(Impl* impl)
	: BlkDevice(impl)
    {
    }


    Partition*
    Partition::clone() const
    {
	return new Partition(get_impl().clone());
    }


    Partition::Impl&
    Partition::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Partition::Impl&
    Partition::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    unsigned int
    Partition::get_number() const
    {
	string::size_type pos = get_name().find_last_not_of("0123456789");
	return atoi(get_name().substr(pos + 1).c_str());
    }

}