<?php
/**
 * API System Class.
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
 * @subpackage API
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * AuthenticationSystem.
 */
class APISystem extends BaseSystem
{


    /**
     * Constructor.
     */
    public function __construct()
    {

    }//end __construct()


    /**
     * Install Any additional assets for the Project system.
     *
     * If the global project doesn't exist (this is a clean system) then it is
     * created.
     *
     * @return void
     */
    public function install()
    {
        parent::install();

        Channels::includeSystem('API');
        $ovenDir = Init::ROOT_DIR.'/Channels/Oven';
        if (class_exists('Systems') === FALSE) {
            include_once $ovenDir.'/systems.inc';
        }

        foreach (Systems::$systems as $systemName => $enabled) {
            if ($enabled !== TRUE
                || file_exists($ovenDir.'/'.$systemName.'/'.$systemName.'.inc') === FALSE
            ) {
                continue;
            }

            Channels::includeSystem($systemName);
            $refl    = new ReflectionClass($systemName);
            $methods = $refl->getMethods();
            foreach ($methods as $method) {
                if ($method->isPublic() === TRUE) {
                    $docComment = $method->getDocComment();
                    $options    = API::getAPIOptionsFromComment($docComment);
                    if ($options !== FALSE) {
                        $actionName = $method->getName();
                        $actionType = $options['type'];
                        API::addAPI($systemName, $actionName, $actionType, $options);
                    }
                }
            }

            unset($refl);
            unset($methods);
        }//end foreach

    }//end install()


}//end class

?>