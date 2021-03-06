#!/usr/bin/python

# requirements: md raid /dev/md0 with partitions sdb[1-5]


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdb5 = Partition.find_by_name(staging, "/dev/sdb5")

md = Md.find_by_name(staging, "/dev/md0")

md.remove_device(sdb5)

print staging

commit(storage)

