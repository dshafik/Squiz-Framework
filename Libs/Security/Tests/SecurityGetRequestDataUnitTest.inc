<?php
/**
 * Unit Tests for the getRequestData() action.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for the getRequestData() action.
 *
 * @since 4.0.0
 */
class SecurityGetRequestDataUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Sets up this unit test.
     *
     * This bakes out a working system that contains the systems that this
     * unit test requires to be able to run.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
