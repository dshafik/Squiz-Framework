<?php
/**
 * Actions for the Backup System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Backup
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * Backup System Actions Class.
 *
 * @since 4.0.0
 */
class BackupActions
{


    /**
     * Returns the contents of the widget (or in this case, template).
     *
     * @param array  $settings The list of settings for this widget.
     * @param string $tplFile  The template file to use.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getTemplateContent(array $settings=array(), $tplFile=NULL)
    {
        $content = '';
        if ($tplFile !== NULL) {
            $content = file_get_contents($tplFile);
        }

        return $content;

    }//end getTemplateContent()


    /**
     * Prepare for baking.
     *
     * @param DOMNode $htmlTpl The HTML content of the widget.
     *
     * @since  4.0.0
     * @return DOMNode
     */
    public static function prepareBake(DOMNode $htmlTpl)
    {
        return $htmlTpl;

    }//end prepareBake()


}//end class

?>
