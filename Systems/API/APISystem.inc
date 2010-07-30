<?php
/**
 * API System Class.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage API
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * AuthenticationSystem.
 *
 * @since 4.0.0
 */
class APISystem extends BaseSystem
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
     * Install Any additional assets for the Project system.
     *
     * If the global project doesn't exist (this is a clean system) then it is
     * created.
     *
     * @since  4.0.0
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
            $result  = array();
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