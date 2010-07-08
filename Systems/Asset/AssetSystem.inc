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

    }//end install()


}//end class

?>
