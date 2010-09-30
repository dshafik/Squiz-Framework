<?php
/**
 * Folder Asset Type.
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
 * @package    CMS
 * @subpackage CMSAssets
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

Channels::includeSystem('Asset');
Asset::includeAsset('asset');

/**
 * Folder Asset Type.
 */
class FolderAssetType extends AssetAssetType
{


    /**
     * Returns the contents of a folder asset.
     *
     * @param integer $assetid The ID of the folder asset.
     *
     * @return string
     */
    public static function getAssetContent($assetid)
    {
        return '';

    }//end getAssetContent()


}//end class

?>
