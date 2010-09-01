<?php
/**
 * Actions for the GUI AssetPicker Widget.
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
 * @subpackage GUI
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * GUI AssetPicker Widget Actions Class.
 */
class GUIAssetPickerActions
{

    /**
     * Root node of the asset picker.
     *
     * @var string
     */
    protected static $rootNode = NULL;

    /**
     * Content of the asset picker.
     *
     * @var string
     * @internal
     */
    protected static $content = '';


    /**
     * Prints widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return array
     */
    public static function paint(array $settings)
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        // Print the contents of the AssetPicker.
        echo $settings['content'];

        return $settings;

    }//end paint()


    /**
     * Returns the child assets for specified parent asset.
     *
     * @param string $rootNode The parent assetid.
     *
     * @return array
     */
    public static function getAssets($rootNode=NULL)
    {
        include_once 'Libs/Security/Security.inc';
        $jsSettings = Security::getRequestData('templateOptions', NULL, TRUE);
        if ($jsSettings !== NULL) {
            $rootNode = Util::getArrayIndex($jsSettings, 'rootNode', $rootNode);
        }

        Channels::includeSystem('Linking');
        $children = DAL::getAssoc(Linking::getImmediateChildren($rootNode, FALSE), 0);

        $data = array();

        Channels::includeSystem('Attribute');
        foreach ($children as $child) {
            $name = Attribute::getAttributeValue($child, 'first_name').' '.Attribute::getAttributeValue($child, 'last_name');
            $item = array(
                     'id'          => $child,
                     'title'       => $name,
                     'itemContent' => '[icon] '.$name,
                    );

            // Get number of kids.
            $childKids          = DAL::getAssoc(Linking::getImmediateChildren($child, FALSE), 0);
            $item['childCount'] = count($childKids);

            $data[] = $item;
        }

        return $data;

    }//end getAssets()


}//end class

?>
