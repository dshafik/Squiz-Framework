<?php
/**
 * Unit Tests for the getRequestData() action.
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

/**
 * Unit Tests for the getRequestData() action.
 *
 */
class SecurityGetRequestDataUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Sets up this unit test.
     *
     * This bakes out a working system that contains the systems that this
     * unit test requires to be able to run.
     *
     * @return void
     */
    protected function setUp()
    {
        parent::setUp();
        include_once 'Libs/Security/Security.inc';

    }//end setUp()


    /**
     * Test each data type returns the correct result.
     *
     * Tests that the correct results are returned for the POST, GET,
     * REQUEST and user array.
     *
     * @return void
     */
    public function testDataTypes()
    {
        // @codingStandardsIgnoreStart
        // Ignore coding standards because we have to manipulate
        // the super globals for testing.
        $_POST['username']    = 'admin';
        $_GET['username']     = 'editor';
        $_REQUEST['username'] = 'public';
        // @codingStandardsIgnoreEnd

        $result = Security::getRequestData('username', NULL, FALSE, 'post');
        PHPUnit_Framework_Assert::assertEquals('admin', $result);

        $result = Security::getRequestData('username', NULL, FALSE, 'get');
        PHPUnit_Framework_Assert::assertEquals('editor', $result);

        $result = Security::getRequestData('username');
        PHPUnit_Framework_Assert::assertEquals('public', $result);

        // User array.
        $array = array(
                  'first item',
                  'second item',
                 );

        $result = Security::getRequestData(1, NULL, FALSE, $array);
        PHPUnit_Framework_Assert::assertEquals('second item', $result);

        $result = Security::getRequestData(1, NULL, FALSE, 'value');
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

        $result = Security::getRequestData(1, 'test', FALSE, 'value');
        PHPUnit_Framework_Assert::assertEquals('test', $result);

    }//end testDataTypes()


    /**
     * Test the HtmlEntities value.
     *
     * Tests that the HtmlEntities is performed on the result before
     * it is returned.
     *
     * @return void
     */
    public function testHtmlEntities()
    {
        // @codingStandardsIgnoreStart
        // Ignore coding standards because we have to manipulate
        // the super globals for testing.
        $_POST['username'] = '<admin <user';
        $_GET['username']  = '>editor';
        // @codingStandardsIgnoreEnd

        $result = Security::getRequestData('username', NULL, FALSE, 'post');
        PHPUnit_Framework_Assert::assertEquals('&lt;admin &lt;user', $result);

        $result = Security::getRequestData('username', NULL, FALSE, 'get');
        PHPUnit_Framework_Assert::assertEquals('&gt;editor', $result);

    }//end testHtmlEntities()


    /**
     * Test Json decoding of the array value.
     *
     * Tests to ensure that Json decoding is only performed on the result
     * when it is required.
     *
     * @return void
     */
    public function testJsonDecode()
    {
        $array = array(
                  'one',
                  'two',
                 );

        $jsonEncoded = json_encode($array);

        // @codingStandardsIgnoreStart
        // Ignore coding standards because we have to manipulate
        // the super globals for testing.
        $_REQUEST['json'] = $jsonEncoded;
        // @codingStandardsIgnoreEnd

        $expected = htmlentities($jsonEncoded);
        $result   = Security::getRequestData('json', NULL, FALSE);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $result = Security::getRequestData('json', NULL, TRUE);
        PHPUnit_Framework_Assert::assertEquals($array, $result);

    }//end testJsonDecode()


}//end class

?>
