<?php
/**
 * Unit Tests for the keyword replacement %odd_even%.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for the keyword replacement %odd_even%.
 *
 * @since 4.0.0
 */
class OddEvenKeywordReplacementUnitTest extends AbstractMySourceUnitTest
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
     * Test correct value is returned for %odd_even%.
     *
     * @since  4.0.0
     * @return void
     */
    public function testOddEvenKeywordReplacement()
    {
        $arrayofassets = array(
                          25,
                          26,
                          27,
                          28,
                          29,
                          30,
                          31,
                          32,
                          33,
                          34,
                         );

        // No assets.
        $expected = '';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            array(),
            '%odd_even% '
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // All assets on one page.
        $expected = 'odd even odd even odd even odd even odd even ';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%odd_even% '
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // One asset per page, position of the asset on the first page.
        $expected = 'odd';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%odd_even%',
            array(),
            array(),
            1
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Three assets per page, position of the assets on the first page.
        $expected = 'odd even odd ';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%odd_even% ',
            array(),
            array(),
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Three assets per page, position of the assets on the second page.
        self::_setRequest(2);
        $expected = 'odd even odd ';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%odd_even% ',
            array(),
            array(),
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Four assets per page, position of the assets on the first page.
        self::_setRequest(1);
        $expected = 'odd even odd even ';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%odd_even% ',
            array(),
            array(),
            4
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Four assets per page, position of the assets on the second page.
        self::_setRequest(2);
        $expected = 'odd even odd even ';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%odd_even% ',
            array(),
            array(),
            4
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        self::_unsetRequest();

    }//end testOddEvenKeywordReplacement()


    /**
     * Set REQUEST to @pageNumber so that we are veiwing a different page.
     *
     * @param integer $pagenumber The page that you want to view in the list.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _setRequest($pagenumber)
    {
        // @codingStandardsIgnoreStart
        // Ignore coding standards because we have to manipulate
        // the super globals for testing.
        $_REQUEST['1_p'] = $pagenumber;
        // @codingStandardsIgnoreEnd

    }//end _setRequest()


    /**
     * Reset REQUEST after testing.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _unsetRequest()
    {
        // @codingStandardsIgnoreStart
        // Ignore coding standards because we have to manipulate
        // the super globals for testing.
        unset($_REQUEST['1_p']);
        // @codingStandardsIgnoreEnd

    }//end _unsetRequest()


}//end class

?>
