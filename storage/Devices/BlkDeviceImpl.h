#ifndef BLK_DEVICE_IMPL_H
#define BLK_DEVICE_IMPL_H


#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage_bgl
{

    using namespace std;


    // abstract class

    class BlkDevice::Impl : public Device::Impl
    {
    public:

	const string& get_name() const { return name; }
	void set_name(const string& name);

	unsigned long long get_size_k() const { return size_k; }
	void set_size_k(unsigned long long size_k);

    protected:

	Impl(const string& name)
	    : Device::Impl(), name(name), size_k(0) {}

	Impl(const xmlNode* node);

	void save(xmlNode* node) const override;

    private:

	string name;
	unsigned long long size_k;

	// major and minor
	// udev_id and udev_path

    };

}

#endif