/*
 * Copyright (c) 2016 SUSE LLC
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#ifndef STORAGE_LVM_VG_IMPL_H
#define STORAGE_LVM_VG_IMPL_H


#include "storage/Utils/Region.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<LvmVg> { static const char* classname; };


    class LvmVg::Impl : public Device::Impl
    {
    public:

	Impl(const string& vg_name)
	    : Device::Impl(), vg_name(vg_name), uuid(), region() {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<LvmVg>::classname; }

	virtual string get_displayname() const override { return get_vg_name(); }

	virtual void probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo) override;

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	const Region& get_region() const { return region; }

	unsigned long long get_size() const;

	const string& get_vg_name() const { return vg_name; }
	void set_vg_name(const string& vg_name);

	const string& get_uuid() const { return uuid; }
	void set_uuid(const string& uuid) { Impl::uuid = uuid; }

	vector<LvmPv*> get_lvm_pvs();
	vector<const LvmPv*> get_lvm_pvs() const;

	vector<LvmLv*> get_lvm_lvs();
	vector<const LvmLv*> get_lvm_lvs() const;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual Text do_create_text(Tense tense) const override;

    private:

	string vg_name;
	string uuid;

	/**
	 * Holds extent-size and extent-count.
	 */
	Region region;
    };


    bool compare_by_vg_name(const LvmVg* lhs, const LvmVg* rhs);

}

#endif
