
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <iostream>

#include "storage/EtcCrypttab.h"

using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE( parse_and_format )
{
    // This needs to be formatted exactly like the expected output

    string_vec input = {
        /** 00 **/ "cr_home  UUID=4711-0815",
        /** 01 **/ "cr_data  /dev/sdb2       none    nofail,timeout=20s",
        /** 02 **/ "cr_db1   /dev/sdb3       s3cr3t  timeout=20s",
        /** 03 **/ "cr_db2   /dev/sdb4       s4cr4t"
    };

    EtcCrypttab crypttab;
    crypttab.parse( input );


    //
    // Check formatting
    //

    string_vec output = crypttab.format_lines();

    BOOST_CHECK_EQUAL( crypttab.get_entry_count(), input.size() );

    for ( int i=0; i < crypttab.get_entry_count(); ++i )
        BOOST_CHECK_EQUAL( output[i], input[i] );


    //
    // Check all fields
    //

    int i=0;
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_crypt_device(), "cr_home" );
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_crypt_device(), "cr_data" );
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_crypt_device(), "cr_db1"  );
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_crypt_device(), "cr_db2"  );

    i=0;
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_block_device(), "UUID=4711-0815" );
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_block_device(), "/dev/sdb2" );
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_block_device(), "/dev/sdb3" );
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_block_device(), "/dev/sdb4" );

    i=0;
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_password(), ""       );
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_password(), ""       );
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_password(), "s3cr3t" );
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_password(), "s4cr4t" );

    i=0;
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_crypt_opts().empty(), true  );
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_crypt_opts().empty(), false );
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_crypt_opts().empty(), false );
    BOOST_CHECK_EQUAL( crypttab.get_entry( i++ )->get_crypt_opts().empty(), true  );


    //
    // Check crypt options
    //

    CryptOpts opts = crypttab.get_entry( 1 )->get_crypt_opts(); // cr_data
    BOOST_CHECK_EQUAL( opts.size(), 2 );
    BOOST_CHECK_EQUAL( opts.get_opt( 0 ), "nofail"      );
    BOOST_CHECK_EQUAL( opts.get_opt( 1 ), "timeout=20s" );
    BOOST_CHECK_EQUAL( opts.contains( "nofail"      ), true  );
    BOOST_CHECK_EQUAL( opts.contains( "timeout=20s" ), true  );
    BOOST_CHECK_EQUAL( opts.contains( "timeout"     ), false );
    BOOST_CHECK_EQUAL( opts.contains( "wrglbrmpf"   ), false );

    opts = crypttab.get_entry( 2 )->get_crypt_opts(); // cr_db1
    BOOST_CHECK_EQUAL( opts.size(), 1 );
    BOOST_CHECK_EQUAL( opts.contains( "timeout=20s" ), true  );
}


BOOST_AUTO_TEST_CASE( create_new )
{
    EtcCrypttab crypttab;
    crypttab.read( "/tmp/wrglbrmpf/crypttab" );

    BOOST_CHECK_EQUAL( crypttab.get_entry_count(), 0 );

    CrypttabEntry * entry = new CrypttabEntry();
    entry->set_crypt_device( "cr_data" );
    entry->set_block_device( "/dev/sda1" );
    crypttab.add( entry );

    string filename = "./test-crypttab";
    crypttab.write( filename );

    EtcCrypttab crypttab2;
    crypttab2.read( filename );

    BOOST_CHECK_EQUAL( crypttab2.get_entry_count(), 1 );
    
    BOOST_CHECK_EQUAL( crypttab2.get_entry( 0 )->get_crypt_device(), "cr_data" );
    BOOST_CHECK_EQUAL( crypttab2.get_entry( 0 )->get_block_device(), "/dev/sda1" );
    BOOST_CHECK_EQUAL( crypttab2.get_entry( 0 )->get_password(), ""  );
    BOOST_CHECK_EQUAL( crypttab2.get_entry( 0 )->get_crypt_opts().empty(), true  );

    remove( filename.c_str() );
}
