/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include <iostream>

#include "storage/Devices/MsdosImpl.h"
#include "storage/Devices/PartitionableImpl.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Msdos>::classname = "Msdos";


    Msdos::Impl::Impl(const xmlNode* node)
	: PartitionTable::Impl(node), minimal_mbr_gap(default_minimal_mbr_gap)
    {
	getChildValue(node, "minimal-mbr-gap", minimal_mbr_gap);
    }


    void
    Msdos::Impl::save(xmlNode* node) const
    {
	PartitionTable::Impl::save(node);

	setChildValueIf(node, "minimal-mbr-gap", minimal_mbr_gap, minimal_mbr_gap !=
			default_minimal_mbr_gap);
    }


    void
    Msdos::Impl::delete_partition(Partition* partition)
    {
	PartitionType old_type = partition->get_type();
	unsigned int old_number = partition->get_number();

	PartitionTable::Impl::delete_partition(partition);

	// After deleting a logical partition the numbers of partitions with
	// higher numbers are shifted.
	if (old_type == PartitionType::LOGICAL)
	{
	    vector<Partition*> partitions = get_partitions();
            for (Partition* tmp : partitions)
	    {
		unsigned int number = tmp->get_number();
		if (number > old_number)
		    tmp->get_impl().set_number(number - 1);
	    }
	}
    }


    void
    Msdos::Impl::set_minimal_mbr_gap(unsigned long minimal_mbr_gap)
    {
	Impl::minimal_mbr_gap = minimal_mbr_gap;
    }


    Region
    Msdos::Impl::get_usable_region() const
    {
	Region device_region = get_partitionable()->get_region();

	// Reserve one sector or minimal-mbr-gap (per default 1 MiB) for the
	// MBR and the MBR gap, see
	// https://en.wikipedia.org/wiki/BIOS_boot_partition. Normally the
	// space for the MBR gap is unused anyway due to partition alignment
	// but for disks with an alignment offset it can be required to
	// explicitely reserve it.

	unsigned long long first_usable_sector = max(1ULL, device_region.to_blocks(minimal_mbr_gap));
	unsigned long long last_usable_sector = UINT32_MAX - 1;
	Region usable_region(first_usable_sector, last_usable_sector - first_usable_sector + 1,
				   device_region.get_block_size());

	return device_region.intersection(usable_region);
    }


    bool
    Msdos::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!PartitionTable::Impl::equal(rhs))
	    return false;

	return minimal_mbr_gap == rhs.minimal_mbr_gap;
    }


    void
    Msdos::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	PartitionTable::Impl::log_diff(log, rhs);

	storage::log_diff(log, "minimal-mbr-gap", minimal_mbr_gap, rhs.minimal_mbr_gap);
    }


    void
    Msdos::Impl::print(std::ostream& out) const
    {
	PartitionTable::Impl::print(out);

	if (minimal_mbr_gap != default_minimal_mbr_gap)
	    out << " minimal_mbr_gap:" << minimal_mbr_gap;
    }


    bool
    Msdos::Impl::is_partition_id_supported(unsigned int id) const
    {
	return id > 0 && id <= 255;
    }


    unsigned int
    Msdos::Impl::max_primary() const
    {
	return min(4U, get_partitionable()->get_range());
    }


    unsigned int
    Msdos::Impl::max_logical() const
    {
	return min(Partitionable::Impl::default_range, get_partitionable()->get_range());
    }


    bool
    Msdos::Impl::has_extended() const
    {
	vector<const Partition*> partitions = PartitionTable::Impl::get_partitions();
	return any_of(partitions.begin(), partitions.end(), [](const Partition* partition) {
	    return partition->get_type() == PartitionType::EXTENDED;
	});
    }


    unsigned int
    Msdos::Impl::num_logical() const
    {
	vector<const Partition*> partitions = get_partitions();
	return count_if(partitions.begin(), partitions.end(), [](const Partition* partition) {
	    return partition->get_type() == PartitionType::LOGICAL;
	});
    }


    const Partition*
    Msdos::Impl::get_extended() const
    {
	vector<const Partition*> partitions = PartitionTable::Impl::get_partitions();
	for (const Partition* partition : partitions)
	{
	    if (partition->get_type() == PartitionType::EXTENDED)
		return partition;
	}

	ST_THROW(Exception("has no extended partition"));
    }


    vector<Partition*>
    Msdos::Impl::get_partitions()
    {
	vector<Partition*> partitions = PartitionTable::Impl::get_partitions();

	for (Partition* partition : partitions)
	{
	    if (partition->get_type() == PartitionType::EXTENDED)
	    {
		vector<Partition*> logicals = get_logical_partitions(partition);
		partitions.insert(partitions.end(), logicals.begin(), logicals.end());
		break;
	    }
	}

	return partitions;
    }


    vector<const Partition*>
    Msdos::Impl::get_partitions() const
    {
	vector<const Partition*> partitions = PartitionTable::Impl::get_partitions();

	for (const Partition* partition : partitions)
	{
	    if (partition->get_type() == PartitionType::EXTENDED)
	    {
		vector<const Partition*> logicals = get_logical_partitions(partition);
		partitions.insert(partitions.end(), logicals.begin(), logicals.end());
		break;
	    }
	}

	return partitions;
    }


    vector<Partition*>
    Msdos::Impl::get_logical_partitions(Partition* partition)
    {
	if (partition->get_type() != PartitionType::EXTENDED)
	    ST_THROW(Exception("function called on wrong partition"));

	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();

	return devicegraph.filter_devices_of_type<Partition>
	    (devicegraph.children(partition->get_impl().get_vertex()), compare_by_number);
    }


    vector<const Partition*>
    Msdos::Impl::get_logical_partitions(const Partition* partition) const
    {
	if (partition->get_type() != PartitionType::EXTENDED)
	    ST_THROW(Exception("function called on wrong partition"));

	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();

	return devicegraph.filter_devices_of_type<const Partition>
	    (devicegraph.children(partition->get_impl().get_vertex()), compare_by_number);
    }


    Text
    Msdos::Impl::do_create_text(Tense tense) const
    {
	const Partitionable* partitionable = get_partitionable();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Create MSDOS partition table on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Creating MSDOS partition table on %1$s"));

	return sformat(text, partitionable->get_displayname().c_str());
    }


    void
    Msdos::Impl::do_create()
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTEDBIN " --script " + quote(partitionable->get_name()) + " mklabel msdos";
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create msdos failed"));

	SystemCmd(UDEVADMBIN_SETTLE);
    }


    Text
    Msdos::Impl::do_delete_text(Tense tense) const
    {
	const Partitionable* partitionable = get_partitionable();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Delete MSDOS partition table on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Deleting MSDOS partition table on %1$s"));

	return sformat(text, partitionable->get_displayname().c_str());
    }

}
