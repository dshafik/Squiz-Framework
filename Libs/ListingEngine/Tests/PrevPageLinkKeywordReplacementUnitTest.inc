<?php
/**
 * Unit Tests for the %prev_page_link% keyword replacement.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for getAssetList.
 *
 * @since 4.0.0
 */
class PrevPageLinkKeywordReplacementUnitTest extends AbstractMySourceUnitTest
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
     * Test correct value is returned for %prev_page_link%.
     *
     * @since  4.0.0
     * @return void
     */
    public function testPrevPageLinkKeywordReplacement()
    {
        $arrayofassets = array(
                          25,
                          26,
                          27,
                          28,
                         );

        // No assets.
        $expected = '';
        $result   = ListingEngine::getAssetList(
            1,
            '%prev_page_link%',
            array(),
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // All assets on one page.
        $result = ListingEngine::getAssetList(
            1,
            '%prev_page_link%',
            $arrayofassets,
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // One asset per page, link not on the first page.
        $result = ListingEngine::getAssetList(
            1,
            '%prev_page_link%',
            $arrayofassets,
            '',
            array(),
            array(),
            1
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Link on the second page.
        self::_setRequest(2);
        $expected = '<a href="?1_p=1"></a>';
        $result   = ListingEngine::getAssetList(
            1,
            '%prev_page_link%',
            $arrayofassets,
            '',
            array(),
            array(),
            1
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        self::_unsetRequest();

    }//end testPrevPageLinkKeywordReplacement()


    /**
     * Test %prev_page_link% with a prev link text value.
     *
     * @since  4.0.0
     * @return void
     */
    public function testLinkWithPrevLinkText()
    {
        $arrayofassets = array(
                          25,
                          26,
                         );

        // When all assets are on one page it should show the text only.
        $expected = 'Prev';
        $result   = ListingEngine::getAssetList(
            1,
            '%prev_page_link%',
            $arrayofassets,
            '',
            array(),
            array(),
            '',
            array('prevLinkText' => 'Prev')
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // One asset per page, first page should have text without link.
        $result = ListingEngine::getAssetList(
            1,
            '%prev_page_link%',
            $arrayofassets,
            '',
            array(),
            array(),
            1,
            array('prevLinkText' => 'Prev')
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Last page in the list should have the text with a link.
        self::_setRequest(2);
        $expected = '<a href="?1_p=1">Prev</a>';
        $result   = ListingEngine::getAssetList(
            1,
            '%prev_page_link%',
            $arrayofassets,
            '',
            array(),
            array(),
            1,
            array('prevLinkText' => 'Prev')
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        self::_unsetRequest();

    }//end testLinkWithPrevLinkText()


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
