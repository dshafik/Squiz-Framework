<?php
/**
 * Actions for the Connection System.
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
 * @subpackage Connection
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * ConnectionActions.
 *
 * Actions for the Connection system. This system stores all standard assets.
 * It contains actions for deleting, creating, cloning and morphing local
 * assets.
 */
class ConnectionActions
{

    /**
     * Character to separate connection id and remote id.
     *
     * @var string
     */
    private static $_separator = '-';


    /**
     * Connection System Constructor.
     *
     * All methods should be called statically, and therefore this class should
     * never need to be instantiated.
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Returns shadow ID or FALSE if not found.
     *
     * @param string $connection The connection asset ID.
     * @param string $remoteid   The remote resource ID.
     *
     * @return mixed
     */
    public static function getLocalid($connection, $remoteid)
    {
        $parts = Connection::splitShadowid(html_entity_decode(urldecode($remoteid)));
        $query = DAL::getDALQuery('Connection', 'getLocalid');
        $res   = DAL::getOne($query);

        if (empty($res) === TRUE) {
            return FALSE;
        }

        return $res;

    }//end getLocalid()


    /**
     * Splits the specified remoteid in to connectionid and remoteid.
     *
     * Returns FALSE if specified remoteid is not valid.
     *
     * @param string $remoteid Remote asset ID.
     *
     * @return mixed
     */
    public static function splitShadowid($remoteid)
    {
        $parts = array();
        $pos   = strpos($remoteid, self::$_separator);

        if ($pos === FALSE) {
            return FALSE;
        }

        $parts['connection'] = substr($remoteid, 0, $pos);
        $parts['remoteid']   = substr($remoteid, ($pos + 1));

        return $parts;

    }//end splitShadowid()


    /**
     * Creates a local asset from given connection and remoteid.
     *
     * Returns the local asset ID.
     *
     * @param string $connection The connection asset ID.
     * @param string $remoteid   The remote resource ID.
     *
     * @return string
     * @throws ChannelException If connection or remoteid is invalid.
     */
    public static function createAsset($connection, $remoteid)
    {
        Channels::includeSystem('Asset');

        // Asset may already exist.
        $assetid = Connection::getLocalid($connection, $remoteid);
        if ($assetid !== FALSE) {
            return $assetid;
        }

        // Get the asset type for this connection.
        if (Asset::isAssetid($connection) === FALSE) {
            throw new ChannelException(_('Specified connection is not a valid asset id'));
        }

        $connectionType = Asset::getTypeOfAsset($connection);

        // Include Connection asset type.
        Asset::includeAsset($connectionType);

        $method = array(
                   ucfirst($connectionType).'AssetType',
                   'getRemoteAssetType',
                  );

        if (is_callable($method) === FALSE) {
            throw new ChannelException(_('Could not get remote asset type'));
        }

        // Get the remote type.
        $assetType = call_user_func($method, $connection, $remoteid);

        try {
            DAL::beginTransaction();
            $assetid = DAL::seqNextVal('seq_shadowid');
            Channels::addToBasket('assetid', $assetid);
            Channels::addToBasket('typeid', $assetType);
            DAL::executeQuery('Asset', 'addAsset');

            Channels::addToBasket('localid', $assetid);
            Channels::addToBasket('connectionType', $connectionType);
            Channels::addToBasket('assetType', $assetType);
            DAL::executeQuery('Connection', 'addConnectionAsset');

            DAL::commit();
        } catch (Exception $e) {
            DAL::rollback();
            throw new ChannelException($e->getMessage());
        }

        return $assetid;

    }//end createAsset()


    /**
     * Converts specified shadowid to a valid local asset id.
     *
     * If shadowid is not valid then it will be returned without any modifications.
     *
     * @param string $shadowid A shadowid. Shadowids must be in following format:
     *                         123-remoteid where 123 is the connection asset ID and
     *                         remoteid is the remote resource ID (e.g. file path, LDAP user).
     *
     * @return string
     */
    public static function convertToAssetid($shadowid)
    {
        $parts = Connection::splitShadowid($shadowid);
        if ($parts === FALSE) {
            // Not a valid shadowid so don't modify.
            return $shadowid;
        }

        // Create the shadow asset.
        $assetid = Connection::createAsset($parts['connection'], $parts['remoteid']);
        return $assetid;

    }//end convertToAssetid()


    /**
     * Creates the child assetid if its a connection assetid.
     *
     * This is a modifier plug for Linking::createLink method. Note that it checks
     * if parent asset exists before modifying the database.
     *
     * @param string $parentid The parent asset to link to.
     * @param string $childid  The child asset to link.
     *
     * @return string
     * @throws ChannelException If the parent asset does not exist.
     */
    public static function createChildAsset($parentid, $childid)
    {
        // Check if parent and child is valid.
        Channels::includeSystem('Asset');
        if (Asset::exists($parentid) === FALSE) {
            $msg = sprintf(_('Parent asset (%s) does not exist'), $parentid);
            throw new ChannelException($msg);
        }

        $childid = Connection::convertToAssetid($childid);

        return $childid;

    }//end createChildAsset()


}//end class

?>
