<?php
/**
 * Unit tests for the type formats.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit tests for the type formats.
 *
 * @since 4.0.0
 */
class TypeFormatUnitTest extends AbstractDataCleanUnitTest
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
     * Create assets for the tests.
     *
     * @since  4.0.0
     * @return array The assary of asset ids.
     */
    private function _createTestAssets()
    {
        $arrayofassets = array();

        try {
            // Create parent asset.
            Channels::includeSystem('Asset');
            $siteQuery = Asset::getAssetsByType('site');
            $sites     = DAL::getAll($siteQuery);
            $siteid    = $sites[0]['assetid'];

            // Create child assets.
            $parentAsset      = array('parentid' => $siteid);
            $arrayofassets[0] = Asset::create('page', $parentAsset);
            $arrayofassets[1] = Asset::create('file', $parentAsset);
            $arrayofassets[2] = Asset::create('page', $parentAsset);
            $arrayofassets[3] = Asset::create('folder', $parentAsset);

        } catch (Exception $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

        return $arrayofassets;

    }//end _createTestAssets()


    /**
     * Test the type formats for the listing engine.
     *
     * @since  4.0.0
     * @return void
     */
    public function testTypeFormats()
    {
        $arrayofassets = self::_createTestAssets();

        // All assets on one page no type formats.
        $expected  = '%attribute~name:'.$arrayofassets[0].'%';
        $expected .= '%attribute~name:'.$arrayofassets[1].'%';
        $expected .= '%attribute~name:'.$arrayofassets[2].'%';
        $expected .= '%attribute~name:'.$arrayofassets[3].'%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%'
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Formating for standard page.
        $typeFormats = array('page' => 'Page %attribute~name%');

        $expected  = 'Page %attribute~name:'.$arrayofassets[0].'%';
        $expected .= '%attribute~name:'.$arrayofassets[1].'%';
        $expected .= 'Page %attribute~name:'.$arrayofassets[2].'%';
        $expected .= '%attribute~name:'.$arrayofassets[3].'%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            $typeFormats
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Formating for standard page and file.
        $typeFormats = array(
                        'page' => 'Page %attribute~name%',
                        'file' => 'File %attribute~name%',
                       );

        $expected  = 'Page %attribute~name:'.$arrayofassets[0].'%';
        $expected .= 'File %attribute~name:'.$arrayofassets[1].'%';
        $expected .= 'Page %attribute~name:'.$arrayofassets[2].'%';
        $expected .= '%attribute~name:'.$arrayofassets[3].'%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            $typeFormats
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

    }//end testTypeFormats()


    /**
     * Test the type and position formats for the listing engine.
     *
     * @since  4.0.0
     * @return void
     */
    public function testTypeAndPositionFormats()
    {
        $arrayofassets = self::_createTestAssets();

        // All assets on one page no formats.
        $expected  = '%attribute~name:'.$arrayofassets[0].'%';
        $expected .= '%attribute~name:'.$arrayofassets[1].'%';
        $expected .= '%attribute~name:'.$arrayofassets[2].'%';
        $expected .= '%attribute~name:'.$arrayofassets[3].'%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%'
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Formating for standard page.
        $typeFormats = array('page' => 'Page %attribute~name%');

        $expected  = 'Page %attribute~name:'.$arrayofassets[0].'%';
        $expected .= '%attribute~name:'.$arrayofassets[1].'%';
        $expected .= 'Page %attribute~name:'.$arrayofassets[2].'%';
        $expected .= '%attribute~name:'.$arrayofassets[3].'%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            $typeFormats
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        // Formating the second position in the list and
        // the third position on the page along with type format.
        $positionFormats = array(
                            '0:2' => 'ListPos2 %attribute~name%',
                            '1:3' => 'PagePos3 %attribute~name%',
                           );

        $expected  = 'Page %attribute~name:'.$arrayofassets[0].'%';
        $expected .= 'ListPos2 %attribute~name:'.$arrayofassets[1].'%';
        $expected .= 'PagePos3 %attribute~name:'.$arrayofassets[2].'%';
        $expected .= '%attribute~name:'.$arrayofassets[3].'%';
        $result    = ListingEngine::getAssetList(
            1,
            '%asset_listing%',
            $arrayofassets,
            '%attribute~name%',
            $typeFormats,
            $positionFormats
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

    }//end testTypeAndPositionFormats()


}//end class

?>
