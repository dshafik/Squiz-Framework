<?php
/**
 * Actions for the Asset System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Asset
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * AssetActions.
 *
 * Actions for the Asset system. This system stores all standard assets.
 * It contains actions for deleting, creating.
 *
 * @since 4.0.0
 */
class AssetActions
{


    /**
     * Asset System Constructor.
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
     * Create a new asset.
     *
     * The new asset is assigned a unique asset ID, and it is added to the the
     * database. The new type must be supplied, and must be installed in Asset
     * system. It also creates a data directory for the asset.
     *
     * Returns the asset ID of the newly created asset.
     *
     * @param string $typeid The typeid of the new asset.
     *
     * @since  4.0.0
     * @return integer
     * @throws ChannelException If there is a DB error.
     */
    public static function create($typeid)
    {
        try {
            DAL::beginTransaction();
            Channels::includeSystem('Asset');
            $assetid = DAL::seqNextVal('seq_assetid');
            Channels::addToBasket('assetid', $assetid);
            DAL::executeQuery('Asset', 'addAsset');

            // Prepare the asset's data directory.
            include_once 'Libs/FileSystem/FileSystem.inc';
            $assetDataPath = Asset::getDataPath($assetid);
            FileSystem::mkdir($assetDataPath, 0755, TRUE);
            DAL::commit();
        } catch (Exception $e) {
            FileSystem::clearDirectory($assetDataPath, TRUE);
            DAL::rollBack();
            throw new ChannelException('DB Error: '.$e->getMessage());
        }//end try

        return (int) $assetid;

    }//end create()


    /**
     * Delete an asset from this Asset System.
     *
     * Removes the database entry created in the create() method.
     * All info relating to this asset in other systems and database tables
     * should have a foreign keys to the asset table, with an ON DELETE CASCADE
     * action set.
     *
     * Returns deleted assetid on success, FALSE if nothing was deleted.
     *
     * @param integer $assetid The identifier of the asset to delete.
     *
     * @since  4.0.0
     * @return boolean
     * @throws ChannelException If there is a DB error.
     */
    public static function delete($assetid)
    {
        $ret = DAL::executeQuery('Asset', 'deleteAsset');
        return $ret;

    }//end delete()


    /**
     * Returns TRUE if the specified asset exists.
     *
     * @param integer $assetid The identifier of the asset to check.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function exists($assetid)
    {
        $id = (int) DAL::executeOne('Asset', 'assetExists');
        if ($id === (int) $assetid) {
            return TRUE;
        }

        return FALSE;

    }//end exists()


    /**
     * Returns the number of assets of specified asset type(s).
     *
     * It can take a single typeid in string or an array of typeids.
     * When a single typeid is quried, it returns an integer number of asset
     * types. If an array is passed in, it returns 'typeid' => number pairs.
     *
     * @param string|array|NULL $typeids Type ID(s) of asset to count.
     *
     * @since  4.0.0
     * @return integer|array
     */
    public static function count($typeids=NULL)
    {
        $result = array();
        $query  = DAL::getDALQuery('Asset', 'getAssetCount', '');
        $result = DAL::getAssoc($query);
        if ($typeids === NULL) {
            // No typeid is specified. Return an integer value of
            // sum of all assets
            $result = DAL::getOne($query);
            return $result;
        } else {
            // Return 'typeid' => sum(*) array structure for each type.
            $newResult = array();
            foreach ($result as $typeArr) {
                $newResult[$typeArr['typeid']] = $typeArr['assetcount'];
            }

            return $newResult;
        }

    }//end count()


    /**
     * Returns the typeid(s) of asset(s).
     *
     * When a single assetid is quried, one typeid will be returned. If an
     * array of assetids are passed in, it returns an array of assetid => typeid
     * pairs.
     *
     * @param float|array $assetids Single or array of assetids to get types.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getTypes($assetids)
    {
        $result = array();
        $query  = DAL::getDALQuery('Asset', 'getTypes', '');
        $assets = DAL::getAssoc($query);
        if (is_array($assetids) === TRUE) {
            foreach ($assets as $asset) {
                $result[$asset['assetid']] = $asset['typeid'];
            }
        } else {
            $asset  = array_pop($assets);
            $result = $asset['typeid'];
        }

        return $result;

    }//end getTypes()


    /**
     * Returns the assetid(s) of type(s).
     *
     * When a single typeid is quried, one assetid will be returned. If an
     * array of typeids are passed in, it returns an array of assetid => typeid
     * pairs.
     *
     * @param string|array $typeids Typeid(s) to get the assets.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getAssets($typeids)
    {
        $result = array();
        $query  = DAL::getDALQuery('Asset', 'getAssets', '');
        $assets = DAL::getAssoc($query);
        if (is_array($typeids) === TRUE) {
            foreach ($assets as $asset) {
                $result[$asset['assetid']] = $asset['typeid'];
            }
        } else {
            foreach ($assets as $asset) {
                $result[] = $asset['assetid'];
            }
        }

        return $result;

    }//end getAssets()


    /**
     * Returns getTypeQuery Query object.
     *
     * @params float $assetid Assetid to get the typeid.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getTypeQuery($assetid)
    {
        $query = DAL::getTypeQuery('Asset', 'getTypeQuery', '');
        return $query;

    }//end getTypeQuery()


    /**
     * Returns getAssetsQuery Query object.
     *
     * @params string|array $typeids Typeids to get the assets.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getAssetsQuery($typeids)
    {
        $query = DAL::getDALQuery('Asset', 'getAssetsQuery', '');
        return $query;

    }//end getAssetsQuery()


    /**
     * Returns the path to the specified asset's data dir.
     *
     * @param integer $assetid ID of an asset.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getDataPath($assetid)
    {
        include_once 'Libs/FileSystem/FileSystem.inc';
        $result = FileSystem::getHashDir($assetid, 32000);
        $path   = BaseSystem::getDataDir('Asset').'/'.$result.'/'.$assetid;

        return $path;

    }//end getDataPath()


}//end class

?>
