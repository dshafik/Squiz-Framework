<?php
/**
 * Unit Tests for getAssetList.
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
class GetAssetListUnitTest extends AbstractMySourceUnitTest
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
     * Test that the correct content is returned for a listing.
     *
     * @since  4.0.0
     * @return void
     */
    public function testCorrectContentIsReturnedForListing()
    {
        $arrayofassets = array(
                          25,
                          26,
                         );

        $expected = '%attribute~name:25%%attribute~name:26%';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%'
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Test two keyword replacements in the default format.
        $expected  = '%attribute~name:25% %attribute~page_content:25%';
        $expected .= '%attribute~name:26% %attribute~page_content:26%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name% %attribute~page_content%'
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Test using normal content in the default format.
        $expected  = 'Name: %attribute~name:25%';
        $expected .= 'Name: %attribute~name:26%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            'Name: %attribute~name%'
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Test using normal content in the page contents.
        $expected = 'Asset Listing %attribute~name:25%%attribute~name:26%';
        $result   = ListingEngine::getAssetList(
            1,
            'Asset Listing %asset_listing%',
            $arrayofassets,
            '%attribute~name%'
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Test html tags are left within the default format.
        $expected  = '<a href="%lookup~url:25%">%attribute~name:25%</a>';
        $expected .= '<a href="%lookup~url:26%">%attribute~name:26%</a>';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '<a href="%lookup~url%">%attribute~name%</a>'
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

    }//end testCorrectContentIsReturnedForListing()


    /**
     * Test invalid and empty values.
     *
     * Test that the correct content is returned when passing in invalid or
     * empty values.
     *
     * @since  4.0.0
     * @return void
     */
    public function testInvalidAndEmptyValues()
    {
        $arrayofassets = array(
                          25,
                          26,
                         );

        // Test passing through an empty array of assets.
        $expected = '';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            array(),
            '%attribute~name%'
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Test passing through no content for the page contents.
        $result = ListingEngine::getAssetList(
            1,
            NULL,
            $arrayofassets,
            '%attribute~name%'
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Test passing through no content for the page contents.
        $result = ListingEngine::getAssetList(
            1,
            NULL,
            $arrayofassets,
            '%attribute~name%'
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Test passing through no content for the default format.
        $result = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            NULL
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Test passing through empty array for type formats.
        $result = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            NULL,
            array()
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Test passing through empty array for position formats.
        $result = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            NULL,
            array(),
            array()
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Test passing through empty string for assets per page.
        $result = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            NULL,
            array(),
            array(),
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Test passing through an empty array for settings.
        $result = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            NULL,
            array(),
            array(),
            '',
            array()
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

    }//end testInvalidAndEmptyValues()


    /**
     * Test correct content is returned when setting number of assets per page.
     *
     * @since  4.0.0
     * @return void
     */
    public function testAssetsPerPage()
    {
        $arrayofassets = array(
                          25,
                          26,
                          27,
                          28,
                          29,
                          30,
                         );

        // Test without setting number of assets per page.
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

        // Set number of assets per page to 3.
        $expected = '%attribute~name:25%%attribute~name:26%%attribute~name:27%';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            array(),
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Set number of assets per page to 5.
        $expected  = '%attribute~name:25%%attribute~name:26%';
        $expected .= '%attribute~name:27%%attribute~name:28%';
        $expected .= '%attribute~name:29%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            array(),
            5
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

    }//end testAssetsPerPage()


    /**
     * Test trying to view a page number that does not exist in the list.
     *
     * If you try to view a page that does not exist in the list, it should
     * show you the last page in the list.
     *
     * @since  4.0.0
     * @return void
     */
    public function testViewingPageThatDoesNotExist()
    {
        $arrayofassets = array(
                          25,
                          26,
                          27,
                          28,
                          29,
                          30,
                         );

        // Test without setting number of assets per page.
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

        // Set number of assets per page to 3.
        $expected = '%attribute~name:25%%attribute~name:26%%attribute~name:27%';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            array(),
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // View a page that does not exist. Should view page the 2nd page.
        // @codingStandardsIgnoreStart
        // Ignore coding standards because we have to manipulate
        // the super globals for testing.
        $_REQUEST['1_p'] = 3;
        // @codingStandardsIgnoreEnd

        $expected = '%attribute~name:28%%attribute~name:29%%attribute~name:30%';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            array(),
            array(),
            3
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // @codingStandardsIgnoreStart
        // Ignore coding standards because we have to manipulate
        // the super globals for testing.
        unset($_REQUEST['1_p']);
        // @codingStandardsIgnoreEnd

    }//end testViewingPageThatDoesNotExist()


    /**
     * Test that the default format is cached for a list.
     *
     * @since  4.0.0
     * @return void
     */
    public function testDefaultFormatIsCached()
    {
        $arrayofassets = array(
                          25,
                          26,
                         );

        $expected = '%attribute~name:25%%attribute~name:26%';
        $result   = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%'
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        // Pass in 1 and check it returns the same result.
        $result = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            array(),
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        // Pass in 1 and change the page content and check that the content
        // is not cached.
        $expected = 'Asset Listing %attribute~name:25%%attribute~name:26%';
        $result   = ListingEngine::getAssetList(
            1,
            'Asset Listing %asset_listing%',
            array(),
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

    }//end testDefaultFormatIsCached()


}//end class

?>
