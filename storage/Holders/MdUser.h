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


#ifndef STORAGE_MD_USER_H
#define STORAGE_MD_USER_H


#include "storage/Holders/User.h"


namespace storage
{

    class MdUser : public User
    {
    public:

	static MdUser* create(Devicegraph* devicegraph, const Device* source, const Device* target);
	static MdUser* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual MdUser* clone() const override;

	bool is_spare() const;
	void set_spare(bool spare);

	bool is_faulty() const;
	void set_faulty(bool faulty);

	unsigned int get_sort_key() const;

	/**
	 * When creating a MD RAID the device list passed to the mdadm command
	 * is sorted according to the sort-key. For use-cases see
	 * https://fate.suse.com/313521/.
	 *
	 * The raid devices and spare devices are sorted
	 * independently. Sorting of devices with the same sort-key is
	 * undefined. Any value is allowed but 0 should mean
	 * unknown/unspecified.
	 */
	void set_sort_key(unsigned int sort_key);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	MdUser(Impl* impl);

    };


    bool is_md_user(const Holder* holder);

    /**
     * Converts pointer to Holder to pointer to MdUser.
     *
     * @return Pointer to MdUser.
     * @throw HolderHasWrongType, NullPointerException
     */
    MdUser* to_md_user(Holder* holder);

    /**
     * @copydoc to_md_user(Holder*)
     */
    const MdUser* to_md_user(const Holder* holder);

}

#endif
