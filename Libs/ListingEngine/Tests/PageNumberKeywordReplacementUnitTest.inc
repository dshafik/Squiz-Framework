<?php
/**
 * Unit Tests for the %page_number% keyword replacement.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for the %page_number% keyword replacement.
 *
 * @since 4.0.0
 */
class PageNumberKeywordReplacementUnitTest extends AbstractMySourceUnitTest
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
        include_once 'Libs/ListingEngine/ListingEngine.inc';

    }//end setUp()


    /**
     * Test correct page number is returned for %page_number%.
     *
     * @since  4.0.0
     * @return void
     */
    public function testPageNumberKeywordReplacement()
    {
        $arrayofassets = array(
                          25,
                          26,
                          27,
                          28,
                          29,
                          30,
                         );

        // No assets.
        $expected = '1';
        $result   = ListingEngine::getAssetList(
            1,
            '%page_number%',
            array(),
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // All assets on one page.
        $expected = '1';
        $result   = ListingEngine::getAssetList(
            1,
            '%page_number%',
            array(),
            '',
            array(),
            array(),
            1
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Page number for the first page in the list.
        $expected = '1';
        $result   = ListingEngine::getAssetList(
            1,
            '%page_number%',
            $arrayofassets,
            '',
            array(),
            array(),
            1
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Page number for the second page in the list.
        // @codingStandardsIgnoreStart
        // Ignore coding standards because we have to manipulate
        // the super globals for testing.
        $_REQUEST['1_p'] = 2;
        // @codingStandardsIgnoreEnd
        $expected = '2';
        $result   = ListingEngine::getAssetList(
            1,
            '%page_number%',
            $arrayofassets,
            '',
            array(),
            array(),
            1
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // @codingStandardsIgnoreStart
        // Ignore coding standards because we have to manipulate
        // the super globals for testing.
        unset($_REQUEST['1_p']);
        // @codingStandardsIgnoreEnd

    }//end testPageNumberKeywordReplacement()


}//end class

?>
