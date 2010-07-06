<?php
/**
 * Basic session handling system.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Session
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * Basic session handling system.
 *
 * @since 4.0.0
 */
class SessionSystem extends BaseSystem
{


    /**
     * Constructor.
     *
     * @since 4.0.0
     */
    public function __construct()
    {
        $this->setSystemOption('exclusive', TRUE);

    }//end __construct()


}//end class

?>
