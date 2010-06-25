<?php
/**
 * Unit Tests for position formats.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for position formats.
 *
 * @since 4.0.0
 */
class PositionFormatUnitTest extends AbstractMySourceUnitTest
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
     * Test the In List position formats for the listing engine.
     *
     * @since  4.0.0
     * @return void
     */
    public function testInListPositionFormats()
    {
        $arrayofassets = array(
                          25,
                          26,
                          27,
                          28,
                          29,
                          30,
                         );

        // All assets on one page no formats.
        $expected  = '%attribute~name:25%%attribute~name:26%';
        $expected .= '%attribute~name:27%%attribute~name:28%';
        $expected .= '%attribute~name:29%%attribute~name:30%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%'
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Formating the first position in the list.
        $positionFormats = array('0:1' => 'ListPos1 %attribute~name%');

        $expected  = 'ListPos1 %attribute~name:25%%attribute~name:26%';
        $expected .= '%attribute~name:27%%attribute~name:28%';
        $expected .= '%attribute~name:29%%attribute~name:30%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Formating the first position in the list.
        // Assets split over two pages.
        $expected  = 'ListPos1 %attribute~name:25%';
        $expected .= '%attribute~name:26%%attribute~name:27%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats,
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // View the second page in the list. Position format should not be used.
        self::_setRequest(2);
        $expected = '%attribute~name:28%%attribute~name:29%%attribute~name:30%';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats,
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // View the first page again.
        self::_setRequest(1);

        // Formating the first and third positions in the list.
        // Assets split over two pages.
        $positionFormats = array(
                            '0:1' => 'ListPos1 %attribute~name%',
                            '0:3' => 'ListPos3 %attribute~name%',
                           );

        $expected  = 'ListPos1 %attribute~name:25%';
        $expected .= '%attribute~name:26%';
        $expected .= 'ListPos3 %attribute~name:27%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats,
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // View the 2nd page in the list. Position formats should not be used.
        self::_setRequest(2);
        $expected = '%attribute~name:28%%attribute~name:29%%attribute~name:30%';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats,
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        self::_unsetRequest();

        // Formating a position that does not exist.
        $positionFormats = array('0:10' => 'ListPos10 %attribute~name%');

        $expected  = '%attribute~name:25%%attribute~name:26%';
        $expected .= '%attribute~name:27%%attribute~name:28%';
        $expected .= '%attribute~name:29%%attribute~name:30%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

    }//end testInListPositionFormats()


    /**
     * Test the On Page position formats for the listing engine.
     *
     * @since  4.0.0
     * @return void
     */
    public function testOnPagePositionFormats()
    {
        $arrayofassets = array(
                          25,
                          26,
                          27,
                          28,
                          29,
                          30,
                         );

        // All assets on one page no formats.
        $expected  = '%attribute~name:25%%attribute~name:26%';
        $expected .= '%attribute~name:27%%attribute~name:28%';
        $expected .= '%attribute~name:29%%attribute~name:30%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%'
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Formating the first position on the list.
        $positionFormats = array('1:1' => 'PagePos1 %attribute~name%');

        $expected  = 'PagePos1 %attribute~name:25%%attribute~name:26%';
        $expected .= '%attribute~name:27%%attribute~name:28%';
        $expected .= '%attribute~name:29%%attribute~name:30%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Formating the first position on the page.
        // Assets split over two pages.
        $expected  = 'PagePos1 %attribute~name:25%';
        $expected .= '%attribute~name:26%%attribute~name:27%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats,
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // View the second page in the list. Position format should be used.
        self::_setRequest(2);
        $expected  = 'PagePos1 %attribute~name:28%';
        $expected .= '%attribute~name:29%%attribute~name:30%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats,
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // View the first page again.
        self::_setRequest(1);

        // Formating the first and third positions on the list.
        // Assets split over two pages.
        $positionFormats = array(
                            '1:1' => 'PagePos1 %attribute~name%',
                            '1:3' => 'PagePos3 %attribute~name%',
                           );

        $expected  = 'PagePos1 %attribute~name:25%';
        $expected .= '%attribute~name:26%';
        $expected .= 'PagePos3 %attribute~name:27%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats,
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // View the 2nd page in the list. Position formats should be used.
        self::_setRequest(2);
        $expected  = 'PagePos1 %attribute~name:28%';
        $expected .= '%attribute~name:29%';
        $expected .= 'PagePos3 %attribute~name:30%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats,
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        self::_unsetRequest();

        // Formating a position that does not exist.
        $positionFormats = array('1:10' => 'ListPos10 %attribute~name%');

        $expected  = '%attribute~name:25%%attribute~name:26%';
        $expected .= '%attribute~name:27%%attribute~name:28%';
        $expected .= '%attribute~name:29%%attribute~name:30%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

    }//end testOnPagePositionFormats()


    /**
     * Test both In List and On Page position formats.
     *
     * @since  4.0.0
     * @return void
     */
    public function testInListAndOnPagePositionFormats()
    {
        $arrayofassets = array(
                          25,
                          26,
                          27,
                          28,
                          29,
                          30,
                         );

        // All assets on one page no formats.
        $expected  = '%attribute~name:25%%attribute~name:26%';
        $expected .= '%attribute~name:27%%attribute~name:28%';
        $expected .= '%attribute~name:29%%attribute~name:30%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%'
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Formating the first position in the list and
        // the third position on the page.
        $positionFormats = array(
                            '0:1' => 'ListPos1 %attribute~name%',
                            '1:3' => 'PagePos3 %attribute~name%',
                           );

        $expected  = 'ListPos1 %attribute~name:25%';
        $expected .= '%attribute~name:26%';
        $expected .= 'PagePos3 %attribute~name:27%';
        $expected .= '%attribute~name:28%%attribute~name:29%';
        $expected .= '%attribute~name:30%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Formating the first position in the list and
        // the third position on the page.
        // Assets split over two pages.
        $expected  = 'ListPos1 %attribute~name:25%';
        $expected .= '%attribute~name:26%';
        $expected .= 'PagePos3 %attribute~name:27%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats,
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // View the second page in the list. Page format should be used.
        self::_setRequest(2);
        $expected  = '%attribute~name:28%%attribute~name:29%';
        $expected .= 'PagePos3 %attribute~name:30%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            $positionFormats,
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        self::_unsetRequest();

    }//end testInListAndOnPagePositionFormats()


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
