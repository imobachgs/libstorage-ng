/*
 * Copyright (c) 2017 SUSE LLC
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
 * with this program; if not, contact SUSE LLC.
 *
 * To contact SUSE LLC about this file by physical or electronic mail, you may
 * find current contact information at www.suse.com.
 */


#include "storage/CompoundAction/Formatter/LvmLv.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/Encryption.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Filesystems/Swap.h"


namespace storage
{

    CompoundAction::Formatter::LvmLv::LvmLv(const CompoundAction::Impl* compound_action) :
	CompoundAction::Formatter(compound_action),
	lv(to_lvm_lv(compound_action->get_target_device()))
    {}

    
    Text
    CompoundAction::Formatter::LvmLv::text() const
    {
	if (has_create<storage::BlkFilesystem>() && is_swap(get_created_filesystem()))
	{
	    if (has_create<storage::Encryption>())
		return create_encrypted_with_swap_text();

	    else
		return create_with_swap_text();
	}

	else if (has_create<storage::LvmLv>() && has_create<storage::Encryption>() && has_create<storage::BlkFilesystem>() && has_create<storage::MountPoint>())
	    return create_encrypted_with_fs_and_mount_point_text();

	else if (has_create<storage::LvmLv>() && has_create<storage::Encryption>() && has_create<storage::BlkFilesystem>())
	    return create_encrypted_with_fs_text();

	else if (has_create<storage::LvmLv>() && has_create<storage::Encryption>())
	    return create_encrypted_text();

	else if (has_create<storage::LvmLv>() && has_create<storage::BlkFilesystem>() && has_create<storage::MountPoint>())
	    return create_with_fs_and_mount_point_text();

	else if (has_create<storage::LvmLv>() && has_create<storage::BlkFilesystem>())
	    return create_with_fs_text();

	else if (has_create<storage::LvmLv>())
	    return create_text();

	else if (has_create<storage::Encryption>() && has_create<storage::BlkFilesystem>() && has_create<storage::MountPoint>())
	    return encrypted_with_fs_and_mount_point_text();

	else if (has_create<storage::Encryption>() && has_create<storage::BlkFilesystem>())
	    return encrypted_with_fs_text();

	else if (has_create<storage::Encryption>())
	    return encrypted_text();

	else if (has_create<storage::BlkFilesystem>() && has_create<storage::MountPoint>())
	    return fs_and_mount_point_text();

	else if (has_create<storage::BlkFilesystem>())
	    return fs_text();

	else if (has_create<storage::MountPoint>())
	    return mount_point_text();

	else
	    return default_text();
    }


    Text
    CompoundAction::Formatter::LvmLv::create_encrypted_with_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by logical volume name (e.g. root),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by volume group name (e.g. system)
	Text text = _("Create encrypted LVM logical volume %1$s (%2$s) on volume group %3$s for swap");

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       lv->get_lvm_vg()->get_vg_name().c_str());
    }


    Text
    CompoundAction::Formatter::LvmLv::create_with_swap_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by logical volume name (e.g. root),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by volume group name (e.g. system)
	Text text = _("Create LVM logical volume %1$s (%2$s) on volume group %3$s for swap");

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       lv->get_lvm_vg()->get_vg_name().c_str());
    }


    Text
    CompoundAction::Formatter::LvmLv::create_encrypted_with_fs_and_mount_point_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by logical volume name (e.g. root),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by volume group name (e.g. system),
	// %4$s is replaced by mount point (e.g. /home),
	// %5$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Create encrypted LVM logical volume %1$s (%2$s) on volume group %3$s for %4$s with %5$s");

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       lv->get_lvm_vg()->get_vg_name().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::LvmLv::create_encrypted_with_fs_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by logical volume name (e.g. root),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by volume group name (e.g. system),
	// %4$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Create encrypted LVM logical volume %1$s (%2$s) on volume group %3$s with %4$s");

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       lv->get_lvm_vg()->get_vg_name().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::LvmLv::create_encrypted_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by logical volume name (e.g. root),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by volume group name (e.g. system)
	Text text = _("Create encrypted LVM logical volume %1$s (%2$s) on volume group %3$s");

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       lv->get_lvm_vg()->get_vg_name().c_str());
    }


    Text
    CompoundAction::Formatter::LvmLv::create_with_fs_and_mount_point_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by logical volume name (e.g. root),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by volume group name (e.g. system),
	// %4$s is replaced by mount point (e.g. /home),
	// %5$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Create LVM logical volume %1$s (%2$s) on volume group %3$s for %4$s with %5$s");

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       lv->get_lvm_vg()->get_vg_name().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::LvmLv::create_with_fs_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by logical volume name (e.g. root),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by volume group name (e.g. system),
	// %4$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Create LVM logical volume %1$s (%2$s) on volume group %3$s with %4$s");

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       lv->get_lvm_vg()->get_vg_name().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::LvmLv::create_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by logical volume name (e.g. root),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by volume group name (e.g. system)
	Text text = _("Create LVM logical volume %1$s (%2$s) on volume group %3$s");

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       lv->get_lvm_vg()->get_vg_name().c_str());
    }


    Text
    CompoundAction::Formatter::LvmLv::encrypted_with_fs_and_mount_point_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by logical volume name (e.g. root),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by volume group name (e.g. system),
	// %4$s is replaced by mount point (e.g. /home),
	// %5$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Encrypt LVM logical volume %1$s (%2$s) on volume group %3$s for %4$s with %5$s");

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       lv->get_lvm_vg()->get_vg_name().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::LvmLv::encrypted_with_fs_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by logical volume name (e.g. root),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by volume group name (e.g. system),
	// %4$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Encrypt LVM logical volume %1$s (%2$s) on volume group %3$s with %4$s");

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       lv->get_lvm_vg()->get_vg_name().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::LvmLv::encrypted_text() const
    {
	// TRANSLATORS:
	// %1$s is replaced by logical volume name (e.g. root),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by volume group name (e.g. system)
	Text text = _("Encrypt LVM logical volume %1$s (%2$s) on volume group %3$s");

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       lv->get_lvm_vg()->get_vg_name().c_str());
    }


    Text
    CompoundAction::Formatter::LvmLv::fs_and_mount_point_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by logical volume name (e.g. root),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by volume group name (e.g. system),
	// %4$s is replaced by mount point (e.g. /home),
	// %5$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Format LVM logical volume %1$s (%2$s) on volume group %3$s for %4$s with %5$s");

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       lv->get_lvm_vg()->get_vg_name().c_str(),
		       filesystem->get_mount_point()->get_path().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::LvmLv::fs_text() const
    {
	auto filesystem = get_created_filesystem();

	// TRANSLATORS:
	// %1$s is replaced by logical volume name (e.g. root),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by volume group name (e.g. system),
	// %4$s is replaced by filesystem name (e.g. ext4)
	Text text = _("Format LVM logical volume %1$s (%2$s) on volume group %3$s with %4$s");

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       lv->get_lvm_vg()->get_vg_name().c_str(),
		       filesystem->get_displayname().c_str());
    }


    Text
    CompoundAction::Formatter::LvmLv::mount_point_text() const
    {
	auto mount_point = get_created_mount_point();

	// TRANSLATORS:
	// %1$s is replaced by logical volume name (e.g. root),
	// %2$s is replaced by size (e.g. 2GiB),
	// %3$s is replaced by volume group name (e.g. system),
	// %4$s is replaced by mount point (e.g. /home)
	Text text = _("Mount LVM logical volume %1$s (%2$s) on volume group %3$s at %4$s");

	return sformat(text, 
		       lv->get_name().c_str(), 
		       lv->get_size_string().c_str(),
		       lv->get_lvm_vg()->get_vg_name().c_str(),
		       mount_point->get_path().c_str());
    }

}

