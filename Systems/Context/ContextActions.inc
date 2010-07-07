<?php
/**
 * Actions for the Context System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Context
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * ContextActions.
 *
 * @since 4.0.0
 */
class ContextActions
{

    /**
     * Maximum context levels.
     *
     * @var   integer
     * @since 4.0.0
     */
    private static $_maxContextLevels = 4;

    /**
     * Master context string.
     *
     * @var   string
     * @since 4.0.0
     */
    private static $_masterContext = '00000000';


    /**
     * System Constructor.
     *
     * All methods should be called statically, and therefore this class should
     * never need to be instantiated.
     *
     * @since 4.0.0
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Sets the current context.
     *
     * Returns the old context value.
     *
     * @param string  $context    The new context value.
     * @param boolean $persistent If TRUE then a session var will be set so the
     *                            context is not reset.
     *
     * @since  4.0.0
     * @return string
     */
    public static function setCurrentContext($context, $persistent=FALSE)
    {
        return self::$_masterContext;

    }//end setCurrentContext()


    /**
     * Returns the current context value.
     *
     * @param boolean $persistent If TRUE then a session var will be set so the
     *                            context is not reset.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getCurrentContext($persistent=FALSE)
    {
        return self::$_masterContext;

    }//end getCurrentContext()


    /**
     * Returns array of assetids for the given assetid(s) in context.
     *
     * @param mixed $assetids The assetid or assetids.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getAssetids($assetids)
    {
        return $assetids;

    }//end getAssetids()


    /**
     * Given an assetid it returns its id for the current context.
     *
     * Note: If assetid already have a context then it will be returned with
     * that context.
     *
     * @param mixed   $assetid  The id of an asset.
     * @param string  $context  The context to use.
     * @param boolean $noCreate If TRUE then context asset will not be created
     *                          if it does not exist.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getAssetid($assetid, $context=NULL, $noCreate=FALSE)
    {

        return $assetid;

    }//end getAssetid()


    /**
     * Returns the context from specified assetid.
     *
     * @param string $assetid ID of an asset.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getContextFromAssetid($assetid)
    {
        return self::$_masterContext;

    }//end getContextFromAssetid()


    /**
     * Returns the masterid for specifid assetid.
     *
     * @param string $assetid ID of an asset.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getMasterid($assetid)
    {
        return $assetid;

    }//end getMasterid()


    /**
     * Returns the 'lineage' of the specified context.
     *
     * @param string $context The context to explode.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getContexts($context=NULL)
    {
        return self::$_masterContext;

    }//end getContexts()


    /**
     * Makes a given specified context a valid context string.
     *
     * For example, given '010001' will return '01000100'.
     *
     * @param string $context A context string.
     *
     * @since  4.0.0
     * @return string
     */
    public static function makeValidContextString($context)
    {
        $context = str_pad(
            $context,
            (self::$_maxContextLevels * 2),
            '0',
            STR_PAD_RIGHT
        );

        return $context;

    }//end makeValidContextString()


    /**
     * Returns TRUE if the context is the master context.
     *
     * @param string $context If specified then this context will be checked.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function isMasterContext($context=NULL)
    {
        return TRUE;

    }//end isMasterContext()


    /**
     * Returns the master context string.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getMasterContext()
    {
        return self::$_masterContext;

    }//end getMasterContext()


}//end class

?>
