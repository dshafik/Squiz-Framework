<?php
/**
 * Actions for the Context System.
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
 * @subpackage Context
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * ContextActions.
 */
class ContextActions
{

    /**
     * Maximum context levels.
     *
     * @var integer
     */
    private static $_maxContextLevels = 4;

    /**
     * Master context string.
     *
     * @var string
     */
    private static $_masterContext = '00000000';


    /**
     * System Constructor.
     *
     * All methods should be called statically, and therefore this class should
     * never need to be instantiated.
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
     * @return boolean
     */
    public static function isMasterContext($context=NULL)
    {
        return TRUE;

    }//end isMasterContext()


    /**
     * Returns the master context string.
     *
     * @return string
     */
    public static function getMasterContext()
    {
        return self::$_masterContext;

    }//end getMasterContext()


    /**
     * Replaces specified context with another one.
     *
     * Other systems can hookin to this channel to do the extra processing.
     *
     * @param string $oldContext The context to get values from.
     * @param string $newContext The context to override.
     * @param string $assetid    Asset id to use.
     *
     * @return void
     */
    public static function replaceContext($oldContext, $newContext, $assetid=NULL)
    {

    }//end replaceContext()


    /**
     * Removes the context data for the specified context and asset.
     *
     * Other systems can hookin to this channel to do the extra processing.
     *
     * @param string $context The context to remove.
     * @param string $assetid Asset id to use.
     *
     * @return void
     */
    public static function removeContextData($context, $assetid=NULL)
    {

    }//end removeContextData()


    /**
     * Systems should hookin to this method to remove context data.
     *
     * @param string $context Context string that could be used in LIKE queries.
     *                        E.g. _____01__ .
     *
     * @return void
     */
    public static function removeAllContextData($context)
    {

    }//end removeAllContextData()


}//end class

?>
