<?php
/**
 * Unit Tests for the endsWith function.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit tests for the endsWith function.
 *
 * @since 4.0.0
 */
class StringEndsWithUnitTest extends AbstractMySourceUnitTest
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
        include_once 'Libs/String/String.inc';

    }//end setUp()


    /**
     * Test endsWith.
     *
     * @since  4.0.0
     * @return void
     */
    public function testEndsWith()
    {
        $result = String::endsWith('Hello World', 'World');
        PHPUnit_Framework_Assert::assertTrue($result);

        $result = String::endsWith('Hello World', ' World');
        PHPUnit_Framework_Assert::assertTrue($result);

        $result = String::endsWith('Hello World', 'Hello World');
        PHPUnit_Framework_Assert::assertTrue($result);

        $result = String::endsWith('Hello World', 'World.');
        PHPUnit_Framework_Assert::assertFalse($result);

        $result = String::endsWith('Hello World', 'Test Hello World');
        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testEndsWith()


    /**
     * Test invalid values.
     *
     * @since  4.0.0
     * @return void
     */
    public function testWithInvalidValues()
    {
        $result = String::endsWith(NULL, 'World');
        PHPUnit_Framework_Assert::assertFalse($result);

        $result = String::endsWith('Hello World', NULL);
        PHPUnit_Framework_Assert::assertFalse($result);

        $result = String::endsWith(NULL, NULL);
        PHPUnit_Framework_Assert::assertFalse($result);

        $result = String::endsWith(1, 'World.');
        PHPUnit_Framework_Assert::assertFalse($result);

        $result = String::endsWith('Hello World', 1);
        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testWithInvalidValues()


}//end class

?>
