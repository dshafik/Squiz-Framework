<?php
/**
 * Unit Tests for the keyword replacement %prev_page_href%.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for the keyword replacement %prev_page_href%.
 *
 * @since 4.0.0
 */
class PrevPageHrefKeywordReplacementUnitTest extends AbstractMySourceUnitTest
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
     * Test correct value is returned for %prev_page_href%.
     *
     * @since  4.0.0
     * @return void
     */
    public function testPrevPageHrefKeywordReplacement()
    {
        $arrayofassets = array(
                          25,
                          26,
                         );

        // No assets.
        $expected = '';
        $result   = ListingEngine::getAssetList(
            1,
            '%prev_page_href%',
            array(),
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // All assets on one page.
        $result = ListingEngine::getAssetList(
            1,
            '%prev_page_href%',
            $arrayofassets,
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // One asset per page, href not on the first page.
        $result = ListingEngine::getAssetList(
            1,
            '%prev_page_href%',
            $arrayofassets,
            '',
            array(),
            array(),
            1
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Href on the second page.
        // @codingStandardsIgnoreStart
        // Ignore coding standards because we have to manipulate
        // the super globals for testing.
        $_REQUEST['1_p'] = 2;
        // @codingStandardsIgnoreEnd

        $expected = '?1_p=1';
        $result   = ListingEngine::getAssetList(
            1,
            '%prev_page_href%',
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

    }//end testPrevPageHrefKeywordReplacement()


}//end class

?>
