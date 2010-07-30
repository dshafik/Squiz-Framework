<?php
/**
 * GUI System Class.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage GUI
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * GUI System Class.
 *
 * @since 4.0.0
 */
class GUISystem extends BaseSystem
{


    /**
     * Install this system.
     *
     * @since  4.0.0
     * @return void
     */
    public function install()
    {
        parent::install();

        // Bake all templates in the system.
        Channels::includeSystem('GUI');
        GUI::bakeTemplates();

    }//end install()


}//end class

?>