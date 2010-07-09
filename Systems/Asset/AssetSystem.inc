<?php
/**
 * Asset System Class.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Asset
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * AssetSystem.
 *
 * Installs all local asset types for this system.*
 *
 * @since 4.0.0
 */
class AssetSystem extends BaseSystem
{


    /**
     * Constructor.
     *
     * @since 4.0.0
     */
    public function __construct()
    {

    }//end __construct()


    /**
     * Register some metadata related log type.
     *
     * @since  4.0.0
     * @return void
     */
    public function install()
    {
        parent::install();

        include_once 'Libs/FileSystem/FileSystem.inc';

        // Fild all .xml files in AssetTypes dirs.
        $xmlFiles  = array();
        $assetDirs = $this->getAddons('AssetTypes', TRUE);
        foreach ($assetDirs as $system => $addonPath) {
            $systemXml = FileSystem::listDirectory($addonPath, array('.xml'));
            $xmlFiles  = array_merge($xmlFiles, $systemXml);
        }

        // Loop through all the asset.xml files and install asset type.
        $assetTypes = array();
        foreach ($xmlFiles as $xmlFile) {
            if (strtolower(basename($xmlFile)) === 'asset.xml') {
                $currentInfo = array();
                $doc = new DomDocument();
                $doc->load($xmlFile);
                $assetNode = $doc->getElementsByTagName('asset')->item(0);
                foreach ($assetNode->childNodes as $child) {
                    if ($child->nodeType !== 1) {
                        continue;
                    }

                    if ($child->childNodes->length === 1) {
                        $currentInfo[$child->tagName] = $child->nodeValue;
                    }

                    $currentInfo['system'] = basename(dirname(dirname(dirname($xmlFile))));
                }

                if (empty($currentInfo) === FALSE) {
                    $currentInfo['path'] = $xmlFile;
                    $assetTypes[] = $currentInfo;
                }
            }//end if
        }//end foreach

        include_once 'Libs/Install/Install.inc';
        $assetTypes = Install::orderAssets($assetTypes);

        // Install asset types.
        Channels::includeSystem('Asset');
        include_once 'Libs/Util/Util.inc';
        foreach ($assetTypes as $typeInfo) {
            $typeid     = Util::getArrayIndex($typeInfo, 'type_code', '');
            $typeName   = Util::getArrayIndex($typeInfo, 'name', '');
            $parentType = Util::getArrayIndex($typeInfo, 'parent_type', '');
            $category   = Util::getArrayIndex($typeInfo, 'category', '');
            $system     = Util::getArrayIndex($typeInfo, 'system', '');
            Asset::createType(
                $typeid,
                $typeName,
                $category,
                $system,
                $parentType
            );
        }//end foreach

    }//end install()


}//end class

?>
