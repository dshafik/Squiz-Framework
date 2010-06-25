<?php
/**
 * LDAP Lib Tests.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage UnitTest
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Libs/LDAP/LDAP.inc';

/**
 * Unit tests for the LDAP::connectToLDAP().
 *
 * @since 4.0.0
 */
class LibsLDAPConnectToLDAPUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Set up the testing environment.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
                $this->markTestSkipped('LDAP directory not setup properly.');
                return TRUE;
            }
        }

        return FALSE;

    }//end _skip()


    /**
     * Test invalid hostname.
     *
     * @since  4.0.0
     * @return void
     */
    public function testInvalidHostname()
    {
        try {
            $conn = LDAP::connectToLDAP('');
        } catch (Exception $e) {
            $error = 'ldap_bind(): Unable to bind to server: Can\'t contact LDAP server';
            PHPUnit_Framework_Assert::assertEquals($error, $e->getMessage());
            return;
        }

        PHPUnit_Framework_Assert::fail('Expecting LDAP error');

    }//end testInvalidHostname()


    /**
     * Test invalid port.
     *
     * @since  4.0.0
     * @return void
     */
    public function testInvalidPort()
    {
        try {
            $conn = LDAP::connectToLDAP('localhost', 666);
        } catch (Exception $e) {
            $error = 'ldap_bind(): Unable to bind to server: Can\'t contact LDAP server';
            PHPUnit_Framework_Assert::assertEquals($error, $e->getMessage());
            return;
        }

        PHPUnit_Framework_Assert::fail('Expecting LDAP error');

    }//end testInvalidPort()


    /**
     * Test bind with invalid DN.
     *
     * @since  4.0.0
     * @return void
     */
    public function testInvalidDn()
    {
        try {
            $bdn  = '**@@##rong$$==&&';
            $conn = LDAP::connectToLDAP('localhost', 389, $bdn);
        } catch (Exception $e) {
            $error = 'ldap_bind(): Unable to bind to server: Invalid DN syntax';
            PHPUnit_Framework_Assert::assertEquals($error, $e->getMessage());
            return;
        }

        PHPUnit_Framework_Assert::fail('Expecting LDAP error');

    }//end testInvalidDn()


    /**
     * Test bind without password.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoPassword()
    {
        try {
            $bdn  = 'cn=admin,dc=demo,dc=squiz,dc=net';
            $conn = LDAP::connectToLDAP('localhost', 389, $bdn);
        } catch (Exception $e) {
            $error = 'ldap_bind(): Unable to bind to server: Server is unwilling to perform';
            PHPUnit_Framework_Assert::assertEquals($error, $e->getMessage());
            return;
        }

        PHPUnit_Framework_Assert::fail('Expecting LDAP error');

    }//end testNoPassword()


    /**
     * Test bind with wrong password.
     *
     * @since  4.0.0
     * @return void
     */
    public function testWrongPassword()
    {
        try {
            $bdn  = 'cn=admin,dc=demo,dc=squiz,dc=net';
            $pass = 'xxx';
            $conn = LDAP::connectToLDAP('localhost', 389, $bdn, $pass);
        } catch (Exception $e) {
            $error = 'ldap_bind(): Unable to bind to server: Invalid credentials';
            PHPUnit_Framework_Assert::assertEquals($error, $e->getMessage());
            return;
        }

        PHPUnit_Framework_Assert::fail('Expecting LDAP error');

    }//end testWrongPassword()


    /**
     * Test a very valid connection.
     *
     * @since  4.0.0
     * @return void
     */
    public function testConnect()
    {
        try {
            $bdn  = 'cn=admin,dc=demo,dc=squiz,dc=net';
            $pass = 'squiz';
            $conn = LDAP::connectToLDAP('localhost', 389, $bdn, $pass);
            $r    = LDAP::disconnect($conn);
            PHPUnit_Framework_Assert::assertEquals(TRUE, $r);
        } catch (Exception $e) {
            PHPUnit_Framework_Assert::fail('Not expecting LDAP error');
        }

    }//end testConnect()


}//end class

?>
