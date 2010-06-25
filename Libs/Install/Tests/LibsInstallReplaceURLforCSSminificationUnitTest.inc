<?php
/**
 * Install Lib Tests.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage UnitTest
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Libs/Install/Install.inc';

/**
 * Unit tests for Install::replaceURLforCSSminification().
 *
 * @since 4.0.0
 */
class LibsInstallReplaceURLforCSSminificationUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Tests that addAssetPath stores the appropriate path.
     *
     * @since  4.0.0
     * @return void
     */
    public function testFileNameOnlyURLReference()
    {
        $sample  = '.testClassName1 .testClassName2 {background: transparent url(icon.png);}'."\n";
        $sample .= '.testClassName2 {background: transparent url(SomeWidgetType/icon.png);}';

        include_once 'Libs/Install/Install.inc';
        $urlReplaced = Install::replaceURLforCSSminification($sample);

        $expected = '.testClassName1 .testClassName2 {background: transparent url(Skins/defaultSkin/icon.png);}
.testClassName2 {background: transparent url(Skins/defaultSkin/SomeWidgetType/icon.png);}';

        PHPUnit_Framework_Assert::assertEquals($urlReplaced, $expected);

    }//end testFileNameOnlyURLReference()


}//end class

?>
