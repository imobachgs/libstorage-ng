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


#include "storage/Utils/AlignmentImpl.h"


namespace storage
{

    AlignError::AlignError()
	: Exception("region cannot be aligned")
    {
    }


    Alignment::Alignment(const Topology& topology, AlignType align_type)
	: impl(new Impl(topology, align_type))
    {
    }


    Alignment::Alignment(const Alignment& alignment)
	: impl(new Impl(alignment.get_impl()))
    {
    }


    Alignment::~Alignment()
    {
    }


    Alignment&
    Alignment::operator=(const Alignment& alignment)
    {
	*impl = alignment.get_impl();
	return *this;
    }


    Alignment::Impl&
    Alignment::get_impl()
    {
        return *impl;
    }


    const Alignment::Impl&
    Alignment::get_impl() const
    {
        return *impl;
    }


    bool
    Alignment::can_be_aligned(const Region& region, AlignPolicy align_policy) const
    {
	return get_impl().can_be_aligned(region, align_policy);
    }


    Region
    Alignment::align(const Region& region, AlignPolicy align_policy) const
    {
	return get_impl().align(region, align_policy);
    }


    std::ostream&
    operator<<(std::ostream& s, const Alignment& alignment)
    {
	return operator<<(s, alignment.get_impl());
    }

}