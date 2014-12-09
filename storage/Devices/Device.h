#ifndef DEVICE_H
#define DEVICE_H


#include <stdint.h>
#include <libxml/tree.h>
#include <memory>
#include <string>
#include <vector>


namespace storage_bgl
{

    using namespace std;


    class Devicegraph;


    // The storage id (sid) is used to identify devices.  The sid is copied
    // when copying the device graph.  When adding a device it gets a unique
    // sid (across all device graphs).  By using the storage id instead of a
    // device name we can easily identify devices where the device name
    // changed, e.g. renumbered logical partitions or renamed logical volumes.
    // Also some devices do not have a intrinsic device name, e.g. btrfs.  We
    // could even have objects for unused space, e.g. space between partitions
    // or unallocated space of a volume group.

    typedef uint32_t sid_t;


    // The Device class does not have a device name since some device types do
    // not have a intrinsic device name, e.g. btrfs.  Instead most devices are
    // derived from BlkDevice which has a device name, major and minor number,
    // udev id and udev path.


    // abstract class

    class Device
    {

    public:

	virtual ~Device();

	sid_t get_sid() const;

	virtual string get_displayname() const = 0;

	virtual void check() const;

	size_t num_children() const;
	size_t num_parents() const;

	// TODO check if we can somehow return a iterator. getting rid of the
	// ptr would also allow to use references instead of pointer in the
	// interface.
	vector<const Device*> get_children() const;
	vector<const Device*> get_parents() const;
	vector<const Device*> get_siblings(bool itself) const;
	vector<const Device*> get_descendants(bool itself) const;
	vector<const Device*> get_ancestors(bool itself) const;
	vector<const Device*> get_leafs(bool itself) const;
	vector<const Device*> get_roots(bool itself) const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual const char* get_classname() const = 0;

	virtual Device* clone() const = 0;

	void save(xmlNode* node) const;

    protected:

	Device(Impl* impl);

	void create(Devicegraph* devicegraph);
	void load(Devicegraph* devicegraph);

    private:

	void add_to_devicegraph(Devicegraph* devicegraph);

	shared_ptr<Impl> impl;

    };

}

#endif