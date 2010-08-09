<?php
/**
 * LDAP Lib Tests.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program as the file license.txt. If not, see
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>
 *
 * @package    Framework
 * @subpackage Libs
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Libs/LDAP/LDAP.inc';

/**
 * Unit tests for the LDAP::search().
 *
 */
class LibsLDAPSearchUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Set up the testing environment.
     *
     * @return void
     */
    public function setUp()
    {
        parent::setUp();

        if (self::_skip() === TRUE) {
            return;
        }

    }//end setUp()


    /**
     * Skip if something is wrong.
     *
     * @return boolean
     */
    private function _skip()
    {
        if (extension_loaded('ldap') === FALSE) {
            $this->markTestSkipped('The LDAP extension is not available.');
            return TRUE;
        } else if (function_exists('ldap_connect') === FALSE) {
            $this->markTestSkipped('ldap_connect() is not defined.');
            return TRUE;
        } else {
            try {
                $bdn  = 'cn=admin,dc=demo,dc=squiz,dc=net';
                $pass = 'squiz';
                $conn = LDAP::connectToLDAP('localhost', 389, $bdn, $pass);
                $r    = LDAP::disconnect($conn);
                PHPUnit_Framework_Assert::assertEquals(TRUE, $r);
            } catch (Exception $e) {
                $this->markTestSkipped('LDAP directory not setup properly: '.$e->getMessage());
                return TRUE;
            }
        }

        return FALSE;

    }//end _skip()


    /**
     * Test normal search on the LDAP directory tree.
     *
     * @return void
     */
    public function testSearch()
    {
        $bdn  = 'cn=admin,dc=demo,dc=squiz,dc=net';
        $pass = 'squiz';
        $conn = LDAP::connectToLDAP('localhost', 389, $bdn, $pass);

        $start  = 'ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net';
        $filter = '(objectClass=*)';
        $sr     = LDAP::search($conn, $start, $filter);
        $count  = LDAP::getNumEntries($conn, $sr);
        PHPUnit_Framework_Assert::assertEquals(12, $count);

        $entries = LDAP::getEntries($conn, $sr);
        $data    = LDAP::getData($entries);
        $dn      = array();
        foreach ($data as $d) {
            $dn[] = $d['dn'];
        }

        sort($dn);
        $expected = array(
                     'ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     'uid=Manager,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     'uid=aschwarzenegger,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     'uid=blee,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     'uid=jchan,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     'uid=jrabbit,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     'uid=jroberts,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     'uid=mgibson,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     'uid=mmonroe,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     'uid=nkidman,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     'uid=sjohansson,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     'uid=sstallone,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $dn);

    }//end testSearch()


    /**
     * Test single level search, ldap_list().
     *
     * @return void
     */
    public function testSingleLevelSearch()
    {
        $bdn  = 'cn=admin,dc=demo,dc=squiz,dc=net';
        $pass = 'squiz';
        $conn = LDAP::connectToLDAP('localhost', 389, $bdn, $pass);

        // Should only get the 'Staff' group.
        $start      = 'c=au,dc=demo,dc=squiz,dc=net';
        $filter     = '(objectClass=*)';
        $multiLevel = FALSE;
        $sr         = LDAP::search($conn, $start, $filter, $multiLevel);
        $count      = LDAP::getNumEntries($conn, $sr);
        PHPUnit_Framework_Assert::assertEquals(1, $count);

        $entries  = LDAP::getEntries($conn, $sr);
        $data     = LDAP::getData($entries);
        $expected = 'o=Squiz,c=au,dc=demo,dc=squiz,dc=net';
        PHPUnit_Framework_Assert::assertEquals($expected, $data[0]['dn']);

    }//end testSingleLevelSearch()


    /**
     * Test one entry read, ldap_read().
     *
     * @return void
     */
    public function testOneEntryRead()
    {
        $bdn  = 'cn=admin,dc=demo,dc=squiz,dc=net';
        $pass = 'squiz';
        $conn = LDAP::connectToLDAP('localhost', 389, $bdn, $pass);

        // Should only get 'Marilyn Monroe' user.
        $start       = 'uid=mmonroe,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net';
        $filter      = '(objectClass=*)';
        $multiLevel  = FALSE;
        $multiResult = FALSE;
        $sr          = LDAP::search($conn, $start, $filter, $multiLevel, $multiResult);
        $count       = LDAP::getNumEntries($conn, $sr);
        PHPUnit_Framework_Assert::assertEquals(1, $count);

        $entries = LDAP::getEntries($conn, $sr);
        $data    = LDAP::getData($entries);
        PHPUnit_Framework_Assert::assertEquals('Marilyn Monroe', $data[0]['cn']);

    }//end testOneEntryRead()


    /**
     * Test specific set of attributes to fetch, dn is always returned.
     *
     * @return void
     */
    public function testSpecificAttributes()
    {
        $bdn  = 'cn=admin,dc=demo,dc=squiz,dc=net';
        $pass = 'squiz';
        $conn = LDAP::connectToLDAP('localhost', 389, $bdn, $pass);

        $start  = 'ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net';
        $filter = '(objectClass=*)';
        $attrs  = array(
                   'uidnumber',
                   'cn',
                  );
        $sr     = LDAP::search($conn, $start, $filter, TRUE, TRUE, $attrs);
        $count  = LDAP::getNumEntries($conn, $sr);
        PHPUnit_Framework_Assert::assertEquals(12, $count);

        $entries = LDAP::getEntries($conn, $sr);
        $data    = LDAP::getData($entries);
        sort($data);

        $expected = array(
                     0  => array('dn' => 'ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net'),
                     1  => array(
                            'dn'        => 'uid=Manager,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                            'uidnumber' => '1312',
                            'cn'        => 'Manager',
                           ),
                     2  => array(
                            'dn'        => 'uid=aschwarzenegger,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                            'uidnumber' => '1307',
                            'cn'        => 'Arnold Schwarzenegger',
                           ),
                     3  => array(
                            'dn'        => 'uid=blee,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                            'uidnumber' => '1308',
                            'cn'        => 'Bruce Lee',
                           ),
                     4  => array(
                            'dn'        => 'uid=jchan,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                            'uidnumber' => '1309',
                            'cn'        => 'Jackie Chan',
                           ),
                     5  => array(
                            'dn'        => 'uid=jrabbit,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                            'uidnumber' => '1310',
                            'cn'        => 'Jessica Rabbit',
                           ),
                     6  => array(
                            'dn'        => 'uid=jroberts,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                            'uidnumber' => '1311',
                            'cn'        => 'Julia Roberts',
                           ),
                     7  => array(
                            'dn'        => 'uid=mgibson,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                            'uidnumber' => '1313',
                            'cn'        => 'Mel Gibson',
                           ),
                     8  => array(
                            'dn'        => 'uid=mmonroe,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                            'uidnumber' => '1314',
                            'cn'        => 'Marilyn Monroe',
                           ),
                     9  => array(
                            'dn'        => 'uid=nkidman,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                            'uidnumber' => '1315',
                            'cn'        => 'Nicole Kidman',
                           ),
                     10 => array(
                            'dn'        => 'uid=sjohansson,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                            'uidnumber' => '1316',
                            'cn'        => 'Scarlett Johansson',
                           ),
                     11 => array(
                            'dn'        => 'uid=sstallone,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                            'uidnumber' => '1317',
                            'cn'        => 'Sylvester Stallone',
                           ),
                    );

        PHPUnit_Framework_Assert::assertEquals($expected, $data);

    }//end testSpecificAttributes()


    /**
     * Test limit of entries to fetch.
     *
     * @return void
     */
    public function testFetchLimit()
    {
        $bdn  = 'cn=admin,dc=demo,dc=squiz,dc=net';
        $pass = 'squiz';
        $conn = LDAP::connectToLDAP('localhost', 389, $bdn, $pass);

        $start  = 'ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net';
        $filter = '(objectClass=*)';
        $attrs  = array('*');
        $limit  = 4;
        $sr     = LDAP::search($conn, $start, $filter, TRUE, TRUE, $attrs, $limit);
        $count  = LDAP::getNumEntries($conn, $sr);
        PHPUnit_Framework_Assert::assertEquals($limit, $count);

    }//end testFetchLimit()


    /**
     * Test sort by field.
     *
     * @return void
     */
    public function testSort()
    {
        $bdn  = 'cn=admin,dc=demo,dc=squiz,dc=net';
        $pass = 'squiz';
        $conn = LDAP::connectToLDAP('localhost', 389, $bdn, $pass);

        $start  = 'ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net';
        $filter = '(objectClass=*)';
        $attrs  = array('cn');
        $sort   = 'cn';
        $sr     = LDAP::search($conn, $start, $filter, TRUE, TRUE, $attrs, 0, $sort);

        $entries = LDAP::getEntries($conn, $sr);
        $data    = LDAP::getData($entries);
        $dn      = array();
        foreach ($data as $d) {
            $dn[] = $d['dn'];
        }

        // Result sorted by name (cn).
        $expected = array(
                     0  => 'ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     1  => 'uid=aschwarzenegger,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     2  => 'uid=blee,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     3  => 'uid=jchan,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     4  => 'uid=jrabbit,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     5  => 'uid=jroberts,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     6  => 'uid=Manager,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     7  => 'uid=mmonroe,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     8  => 'uid=mgibson,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     9  => 'uid=nkidman,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     10 => 'uid=sjohansson,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                     11 => 'uid=sstallone,ou=Staff,o=Squiz,c=au,dc=demo,dc=squiz,dc=net',
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $dn);

    }//end testSort()


}//end class

?>
