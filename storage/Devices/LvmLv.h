/*
 * Copyright (c) [2016-2017] SUSE LLC
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


#ifndef STORAGE_LVM_LV_H
#define STORAGE_LVM_LV_H


#include "storage/Devices/BlkDevice.h"
#include "storage/Devicegraph.h"


namespace storage
{

    class LvmVg;


    /**
     * LVM logical volume types, see lvs(8).
     */
    enum class LvType
    {
	UNKNOWN, NORMAL, THIN_POOL, THIN, RAID
    };


    class LvmLvNotFoundByLvName : public DeviceNotFound
    {
    public:

	LvmLvNotFoundByLvName(const std::string& lv_name);
    };


    /**
     * A Logical Volume of the Logical Volume Manager (LVM).
     */
    class LvmLv : public BlkDevice
    {

    public:

	static LvmLv* create(Devicegraph* devicegraph, const std::string& vg_name,
			     const std::string& lv_name, LvType lv_type);
	static LvmLv* load(Devicegraph* devicegraph, const xmlNode* node);

	static std::vector<LvmLv*> get_all(Devicegraph* devicegraph);
	static std::vector<const LvmLv*> get_all(const Devicegraph* devicegraph);

	/**
	 * Get logical volume name. This is different from get_name().
	 */
	const std::string& get_lv_name() const;
	void set_lv_name(const std::string& lv_name);

	LvType get_lv_type() const;

	unsigned int get_stripes() const;

	/**
	 * Set the number of stripes. The size of the LV must be a multiple of
	 * the number of stripes and the stripe size. Thin LV cannot be
	 * striped.
	 */
	void set_stripes(unsigned int stripes);

	unsigned long long get_stripe_size() const;
	void set_stripe_size(unsigned long long stripe_size);

	unsigned long long get_chunk_size() const;

	/**
	 * Set the chunk size. Only thin pools can have a chunk size.
	 */
	void set_chunk_size(unsigned long long chunk_size);

	/**
	 * Return volume group this logical volume belongs to.
	 */
	const LvmVg* get_lvm_vg() const;

	/**
	 * Create a logical volume with name lv_name and type lv_type in the
	 * thin pool. Only supported lv_type is THIN.
	 *
	 * @throw Exception
	 */
	LvmLv* create_lvm_lv(const std::string& lv_name, LvType lv_type, unsigned long long size);

	/**
	 * @throw Exception
	 */
	LvmLv* get_lvm_lv(const std::string& lv_name);

	std::vector<LvmLv*> get_lvm_lvs();
	std::vector<const LvmLv*> get_lvm_lvs() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual LvmLv* clone() const override;

    protected:

	LvmLv(Impl* impl);

    };


    bool is_lvm_lv(const Device* device);

    /**
     * Converts pointer to Device to pointer to LvmLv.
     *
     * @return Pointer to LvmLv.
     * @throw DeviceHasWrongType, NullPointerException
     */
    LvmLv* to_lvm_lv(Device* device);

    /**
     * @copydoc to_lvm_lv(Device*)
     */
    const LvmLv* to_lvm_lv(const Device* device);

}

#endif
