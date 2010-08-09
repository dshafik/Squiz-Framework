<?php
/**
 * Asset System Class.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program as the file license.txt. If not, see
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>
 *
 * @package    Framework
 * @subpackage Asset
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * AssetSystem.
 *
 * Installs all local asset types for this system.*
 *
 */
class AssetSystem extends BaseSystem
{


    /**
     * Constructor.
     *
     */
    public function __construct()
    {

    }//end __construct()


    /**
     * Register some metadata related log type.
     *
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
