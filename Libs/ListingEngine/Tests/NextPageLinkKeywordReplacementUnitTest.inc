<?php
/**
 * Unit Tests for the %next_page_link% keyword replacement.
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
class NextPageLinkKeywordReplacementUnitTest extends AbstractMySourceUnitTest
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
     * Test correct value is returned for %next_page_link%.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNextPageLinkKeywordReplacement()
    {
        $arrayofassets = array(
                          25,
                          26,
                         );

        // No assets.
        $expected = '';
        $result   = ListingEngine::getAssetList(
            1,
            '%next_page_link%',
            array(),
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // All assets on one page.
        $result = ListingEngine::getAssetList(
            1,
            '%next_page_link%',
            $arrayofassets,
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // One asset per page, link on the first page.
        $expected = '<a href="?1_p=2"></a>';
        $result   = ListingEngine::getAssetList(
            1,
            '%next_page_link%',
            $arrayofassets,
            '',
            array(),
            array(),
            1
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Last page in the list should not have the link.
        self::_setRequest(2);
        $expected = '';
        $result   = ListingEngine::getAssetList(
            1,
            '%next_page_link%',
            $arrayofassets,
            '',
            array(),
            array(),
            1
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        self::_unsetRequest();

    }//end testNextPageLinkKeywordReplacement()


    /**
     * Test %next_page_link% with a next link text value.
     *
     * @since  4.0.0
     * @return void
     */
    public function testLinkWithNextLinkText()
    {
        $arrayofassets = array(
                          25,
                          26,
                         );

        // When all assets are on one page it should show the text only.
        $expected = 'Next';
        $result   = ListingEngine::getAssetList(
            1,
            '%next_page_link%',
            $arrayofassets,
            '',
            array(),
            array(),
            '',
            array('nextLinkText' => 'Next')
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // One asset per page, link on the first page.
        $expected = '<a href="?1_p=2">Next</a>';
        $result   = ListingEngine::getAssetList(
            1,
            '%next_page_link%',
            $arrayofassets,
            '',
            array(),
            array(),
            1,
            array('nextLinkText' => 'Next')
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Last page in the list should have the text without a link.
        self::_setRequest(2);
        $expected = 'Next';
        $result   = ListingEngine::getAssetList(
            1,
            '%next_page_link%',
            $arrayofassets,
            '',
            array(),
            array(),
            1,
            array('nextLinkText' => 'Next')
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        self::_unsetRequest();

    }//end testLinkWithNextLinkText()


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
