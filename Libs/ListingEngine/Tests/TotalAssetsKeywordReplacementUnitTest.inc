<?php
/**
 * Unit Tests for the %total_assets% keyword replacement.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for the %total_assets% keyword replacement.
 *
 * @since 4.0.0
 */
class TotalAssetsKeywordReplacementUnitTest extends AbstractMySourceUnitTest
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
     * Test correct value is returned for %total_assets%.
     *
     * @since  4.0.0
     * @return void
     */
    public function testTotalAssetsKeywordReplacement()
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
        $expected = '0';
        $result   = ListingEngine::getAssetList(
            1,
            '%total_assets%',
            array(),
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

        $expected = '6';
        $result   = ListingEngine::getAssetList(
            1,
            '%total_assets%',
            $arrayofassets,
            ''
        );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);
        ListingEngine::clearListing(1);

    }//end testTotalAssetsKeywordReplacement()


}//end class

?>
