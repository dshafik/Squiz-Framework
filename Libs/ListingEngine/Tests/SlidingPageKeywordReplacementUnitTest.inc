<?php
/**
 * Unit Tests for the keyword replacement %sliding_page_list%.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for the keyword replacement %sliding_page_list%.
 *
 * @since 4.0.0
 */
class SlidingPageKeywordReplacementUnitTest extends AbstractMySourceUnitTest
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
     * Test correct value is returned for %sliding_page_list%.
     *
     * @since  4.0.0
     * @return void
     */
    public function testSlidingPageListKeywordReplacement()
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
        $expected = '1';
        $result   = ListingEngine::getAssetList(
            1,
            '%sliding_page_list%',
            array(),
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // All assets on one page.
        $result = ListingEngine::getAssetList(
            1,
            '%sliding_page_list%',
            $arrayofassets,
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Two assets per page.
        $expected  = '1 ';
        $expected .= '<a href="?1_p=2">2</a> ';
        $expected .= '<a href="?1_p=3">3</a> ';
        $expected .= '<a href="?1_p=4">4</a> ';
        $expected .= '<a href="?1_p=5">5</a>';
        $result    = ListingEngine::getAssetList(
            1,
            '%sliding_page_list%',
            $arrayofassets,
            '',
            array(),
            array(),
            2
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Set REQUEST to 5 so that we are veiwing the last page of the list.
        // It will not slide.
        self::_setRequest(5);

        $expected  = '<a href="?1_p=1">1</a> ';
        $expected .= '<a href="?1_p=2">2</a> ';
        $expected .= '<a href="?1_p=3">3</a> ';
        $expected .= '<a href="?1_p=4">4</a> ';
        $expected .= '5';
        $result    = ListingEngine::getAssetList(
            1,
            '%sliding_page_list%',
            $arrayofassets,
            '',
            array(),
            array(),
            2
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Set REQUEST to 1 so that we are veiwing the last page of the list.
        self::_setRequest(1);

        // One asset per page.
        $expected  = '1 ';
        $expected .= '<a href="?1_p=2">2</a> ';
        $expected .= '<a href="?1_p=3">3</a> ';
        $expected .= '<a href="?1_p=4">4</a> ';
        $expected .= '<a href="?1_p=5">5</a>';
        $result    = ListingEngine::getAssetList(
            1,
            '%sliding_page_list%',
            $arrayofassets,
            '',
            array(),
            array(),
            1
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Set REQUEST to 5 so that we are veiwing the last page of the list.
        // It will slide.
        self::_setRequest(5);

        $expected  = '<a href="?1_p=3">3</a> ';
        $expected .= '<a href="?1_p=4">4</a> ';
        $expected .= '5 ';
        $expected .= '<a href="?1_p=6">6</a> ';
        $expected .= '<a href="?1_p=7">7</a>';
        $result    = ListingEngine::getAssetList(
            1,
            '%sliding_page_list%',
            $arrayofassets,
            '',
            array(),
            array(),
            1
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        self::_unsetRequest();

    }//end testSlidingPageListKeywordReplacement()


    /**
     * Test value %sliding_page_list% when setting the number of pages to show.
     *
     * @since  4.0.0
     * @return void
     */
    public function testSlidingPageListWithNumOfPagesKeywordReplacement()
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
        $expected = '1';
        $result   = ListingEngine::getAssetList(
            1,
            '%sliding_page_list-3%',
            array(),
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // All assets on one page so it should show 1.
        $expected = '1';
        $result   = ListingEngine::getAssetList(
            1,
            '%sliding_page_list-3%',
            $arrayofassets,
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Setting the list to show 1 page.
        // It should use the default of 5.
        $expected  = '1 ';
        $expected .= '<a href="?1_p=2">2</a> ';
        $expected .= '<a href="?1_p=3">3</a> ';
        $expected .= '<a href="?1_p=4">4</a> ';
        $expected .= '<a href="?1_p=5">5</a>';
        $result    = ListingEngine::getAssetList(
            1,
            '%sliding_page_list-1%',
            $arrayofassets,
            '',
            array(),
            array(),
            2
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Setting the list to show A page.
        // It should use the default of 5.
        $expected  = '1 ';
        $expected .= '<a href="?1_p=2">2</a> ';
        $expected .= '<a href="?1_p=3">3</a> ';
        $expected .= '<a href="?1_p=4">4</a> ';
        $expected .= '<a href="?1_p=5">5</a>';
        $result    = ListingEngine::getAssetList(
            1,
            '%sliding_page_list-A%',
            $arrayofassets,
            '',
            array(),
            array(),
            2
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Setting the list to show -2 pages.
        // It should use the default of 5.
        $expected  = '1 ';
        $expected .= '<a href="?1_p=2">2</a> ';
        $expected .= '<a href="?1_p=3">3</a> ';
        $expected .= '<a href="?1_p=4">4</a> ';
        $expected .= '<a href="?1_p=5">5</a>';
        $result    = ListingEngine::getAssetList(
            1,
            '%sliding_page_list--2%',
            $arrayofassets,
            '',
            array(),
            array(),
            2
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Five assets per page so total pages is two.
        // Setting the list to show six pages.
        $expected = '1 <a href="?1_p=2">2</a>';
        $result   = ListingEngine::getAssetList(
            1,
            '%sliding_page_list-6%',
            $arrayofassets,
            '',
            array(),
            array(),
            5
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Set REQUEST to 2 so that we are veiwing the last page of the list.
        // It will not slide.
        self::_setRequest(2);

        $expected = '<a href="?1_p=1">1</a> 2';
        $result   = ListingEngine::getAssetList(
            1,
            '%sliding_page_list-3%',
            $arrayofassets,
            '',
            array(),
            array(),
            5
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Set REQUEST to 1 so that we are veiwing the last page of the list.
        self::_setRequest(1);

        // Two assets per page, setting it to 2 pages in the list.
        $expected = '1 <a href="?1_p=2">2</a>';
        $result   = ListingEngine::getAssetList(
            1,
            '%sliding_page_list-2%',
            $arrayofassets,
            '',
            array(),
            array(),
            2
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Set REQUEST to 2 so that we are veiwing the second page of the list.
        // It will slide.
        self::_setRequest(2);

        $expected = '2 <a href="?1_p=3">3</a>';
        $result   = ListingEngine::getAssetList(
            1,
            '%sliding_page_list-2%',
            $arrayofassets,
            '',
            array(),
            array(),
            2
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Set REQUEST to 1 so that we are veiwing the last page of the list.
        self::_setRequest(1);

        // Two assets per page, setting it to 3 pages in the list.
        $expected = '1 <a href="?1_p=2">2</a> <a href="?1_p=3">3</a>';
        $result   = ListingEngine::getAssetList(
            1,
            '%sliding_page_list-3%',
            $arrayofassets,
            '',
            array(),
            array(),
            2
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Set REQUEST to 2 so that we are veiwing the second page of the list.
        // It will slide.
        self::_setRequest(2);

        $expected = '<a href="?1_p=1">1</a> 2 <a href="?1_p=3">3</a>';
        $result   = ListingEngine::getAssetList(
            1,
            '%sliding_page_list-3%',
            $arrayofassets,
            '',
            array(),
            array(),
            2
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Set REQUEST to 3 so that we are veiwing the third page of the list.
        // It will slide.
        self::_setRequest(3);

        $expected = '<a href="?1_p=2">2</a> 3 <a href="?1_p=4">4</a>';
        $result   = ListingEngine::getAssetList(
            1,
            '%sliding_page_list-3%',
            $arrayofassets,
            '',
            array(),
            array(),
            2
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Set REQUEST to 5 so that we are veiwing the last page of the list.
        // It will slide.
        self::_setRequest(5);

        $expected = '<a href="?1_p=3">3</a> <a href="?1_p=4">4</a> 5';
        $result   = ListingEngine::getAssetList(
            1,
            '%sliding_page_list-3%',
            $arrayofassets,
            '',
            array(),
            array(),
            2
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Set REQUEST to 1 so that we are veiwing the first page of the list.
        self::_setRequest(1);

        // One assets per page, setting it to 10 pages in the list.
        $expected  = '1 ';
        $expected .= '<a href="?1_p=2">2</a> ';
        $expected .= '<a href="?1_p=3">3</a> ';
        $expected .= '<a href="?1_p=4">4</a> ';
        $expected .= '<a href="?1_p=5">5</a> ';
        $expected .= '<a href="?1_p=6">6</a> ';
        $expected .= '<a href="?1_p=7">7</a> ';
        $expected .= '<a href="?1_p=8">8</a> ';
        $expected .= '<a href="?1_p=9">9</a> ';
        $expected .= '<a href="?1_p=10">10</a>';
        $result    = ListingEngine::getAssetList(
            1,
            '%sliding_page_list-10%',
            $arrayofassets,
            '',
            array(),
            array(),
            1
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        self::_setRequest(4);
        $arrayofassets = array(
                          25,
                          26,
                          27,
                          28,
                         );

        // Testing this case as it was showing 4 5 instead of 3 4.
        $expected = '<a href="?1_p=3">3</a> 4';
        $result   = ListingEngine::getAssetList(
            1,
            '%sliding_page_list-2%',
            $arrayofassets,
            '',
            array(),
            array(),
            1
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        self::_unsetRequest();

    }//end testSlidingPageListWithNumOfPagesKeywordReplacement()


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
