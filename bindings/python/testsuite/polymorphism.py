#!/usr/bin/python

import unittest
import storage


class TestPolymorphism(unittest.TestCase):

    def test_polymorphism(self):

        environment = storage.Environment(True, storage.ProbeMode_NONE, storage.TargetMode_DIRECT)
        s = storage.Storage(environment)

        devicegraph = storage.Devicegraph(s)
        sda = storage.Disk.create(devicegraph, "/dev/sda")
        gpt = sda.create_partition_table(storage.PtType_GPT)

        self.assertEqual(sda.get_sid(), 42)
        self.assertEqual(gpt.get_sid(), 43)

        tmp1 = devicegraph.find_device(42)
        self.assertTrue(storage.is_disk(tmp1))
        self.assertTrue(storage.to_disk(tmp1))
        self.assertFalse(storage.is_partition_table(tmp1))
        self.assertRaises(storage.DeviceHasWrongType, lambda: storage.to_partition_table(tmp1))
        self.assertRaises(storage.Exception, lambda: storage.to_partition_table(tmp1))
        self.assertEqual(storage.downcast(tmp1).__class__, storage.Disk)

        tmp2 = devicegraph.find_device(43)
        self.assertTrue(storage.is_partition_table(tmp2))
        self.assertTrue(storage.to_partition_table(tmp2))
        self.assertFalse(storage.is_disk(tmp2))
        self.assertRaises(storage.DeviceHasWrongType, lambda: storage.to_disk(tmp2))
        self.assertRaises(storage.Exception, lambda: storage.to_disk(tmp2))
        self.assertEqual(storage.downcast(tmp2).__class__, storage.Gpt)

        tmp3 = devicegraph.find_holder(42, 43)
        self.assertTrue(storage.is_user(tmp3))
        self.assertTrue(storage.to_user(tmp3))
        self.assertFalse(storage.is_filesystem_user(tmp3))
        self.assertRaises(storage.HolderHasWrongType, lambda: storage.to_filesystem_user(tmp3))
        self.assertRaises(storage.Exception, lambda: storage.to_filesystem_user(tmp3))
        self.assertEqual(storage.downcast(tmp3).__class__, storage.User)


if __name__ == '__main__':
    unittest.main()
