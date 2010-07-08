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
     * Get the type of an asset.
     *
     * If you pass in a single assetid, this function will return
     * a single asset type as a string. If you pass in an array of
     * assetids, this function will return an array in the form
     * [assetid] => [typeid].
     *
     * @param float|array $assetid Assetid to get the type of.
     *
     * @since  4.0.0
     * @return string|array
     */
    public static function getTypeOfAsset($assetid)
    {
        $query = Asset::getTypeOfAssetQuery($assetid);
        if (is_array($assetid) === TRUE) {
            $result = DAL::getAssoc($query);
            $types  = array();
            foreach ($result as $row) {
                $types[$row['assetid']] = $row['typeid'];
            }

            return $types;
        } else {
            $type = DAL::getOne($query);
            return $type;
        }

    }//end getTypeOfAsset()


    /**
     * Get getTypeOfAsset query object.
     *
     * @param float|array $assetid Assetid to get the type of.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getTypeOfAssetQuery($assetid)
    {
        $query = DAL::getDALQuery('Asset', 'getTypeOfAsset');
        return $query;

    }//end getTypeOfAssetQuery()


    /**
     * Get a list of assetids of a given type.
     *
     * If you supply an array of typeids, this function will still
     * return a single array of assetids. If you need to know
     * which assets are of each type, you must call this function
     * multiple times.
     *
     * @param string|array $typeid The type of assets to get.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getAssetsByType($typeid)
    {
        $assets = DAL::getAll(Asset::getAssetsByTypeQuery($typeid));
        return $assets;

    }//end getAssetsByType()


    /**
     * Get getAssetsByType query object.
     *
     * @params string|array $typeid The type of assets to get.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getAssetsByTypeQuery($typeid)
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


    /**
     * Creates a new asset type.
     *
     * @param string $typeid     The unique identifier of the type code to
     *                           install.
     * @param string $typeName   The readable name of the new type.
     * @param string $category   The category this asset type belongs to.
     * @param string $parentType The type the new asset inherits from.
     * @param string $isCustom   Set to TRUE if this is a custom asset type.
     *
     * @since  4.0.0
     * @return void
     * @throws 
     */
    public static function createType(
        $typeid,
        $name,
        $category,
        $parentType,
        $isCustom=FALSE
    ) {
        try {
            DAL::beginTransaction();

            // Add this asset type to asset_type table.
            DAL::executeQuery('Asset', 'addAssetType');

            // Add level 0.
            self::_addAssetTypeTree($typeid, $typeid, 0);

            // For each parent type add a new row to asset_type_tree.
            if ($typeid !== $parentType) {
                self::_addAssetTypeTree($typeid, $parentType, 1);
            }

            // Get parent type's ancestors.
            $ancestors = Asset::getParentTypes($parentType);
            foreach ($ancestors as $typeInfo) {
                // Add it to asset_type_tree.
                self::_addAssetTypeTree(
                    $typeid,
                    $typeInfo['typeid'],
                    ($typeInfo['lvl'] + 1)
                );
            }

            DAL::commit();
        } catch (DALException $e) {
            DAL::rollBack();
            throw new ChannelActionException('DB Error: '.$e->getMessage());
        }

    }//end createAssetType()


    /**
     * Adds a single row to asset_type_tree.
     *
     * @param string  $typeid       Id of the new asset type.
     * @param string  $parentTypeid Id of the parent type.
     * @param integer $lvl          Number of levels between new type and
     *                              this parent.
     *
     * @since  4.0.0
     * @return void
     * @throws ChannelActionException If there is a database error.
     */
    private static function _addAssetTypeTree($typeid, $parentTypeid, $lvl)
    {
        Channels::createEventBasket();
        Channels::addToBasket('typeid', $typeid);
        Channels::addToBasket('parentTypeid', $parentTypeid);
        Channels::addToBasket('lvl', $lvl);
        DAL::executeQuery('Asset', 'addAssetTypeTree');
        Channels::removeEventBasket();

    }//end _addAssetTypeTree()


    /**
     * Returns TRUE if asset is of specified type or child of specified type.
     *
     * For example: if asset is a page asset and $type is 'file' then method
     * will return FALSE, if asset is 'image' and $type is file then method will
     * return TRUE.
     *
     * @param integer $assetid The id of the asset to check.
     * @param string  $typeid  The type id.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function isTypeOf($assetid, $typeid)
    {
        $typeCode = Asset::getTypes($assetid);
        if ($typeCode === $typeid) {
            return TRUE;
        }

        $typeAncestors = Asset::getTypeAncestors($typeCode);
        foreach ($typeAncestors as $typeInfo) {
            if ($typeInfo['typeid'] === $typeid) {
                return TRUE;
            }
        }

        return FALSE;

    }//end isTypeOf()


    /**
     * Check if the passed asset type exists in the asset_type tree.
     *
     * Returns TRUE if the type is already installed.
     *
     * @param string $typeid Typeid to check.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function typeExists($typeid)
    {
        $ret = DAL::executeAssoc('Asset', 'typeExists');
        if (empty($ret) === TRUE) {
            return FALSE;
        }

        return TRUE;

    }//end typeExists()


    /**
     * Returns the asset type information.
     *
     * @param string $typeid Asset typeid to check.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getTypeInfo($typeid)
    {
        $result = DAL::executeAssoc('Asset', 'getAssetTypeInfo');
        $result = array_pop($result);
        return $result;

    }//end getAssetTypeInfo()


    /**
     * Returns the list of parent asset types.
     *
     * @param string $typeid Asset typeid to check.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getParentTypes($typeid)
    {
        $result = DAL::executeAssoc('Asset', 'getParentTypes');
        return $result;

    }//end getTypeAncestors()


    /**
     * Returns the list of child asset types.
     *
     * @param string  $typeid Asset typeid to check.
     * @param integer $lvl    Level restriction to filter.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getChildTypes($typeid, $lvl=NULL)
    {
        $q = DAL::getDALQuery('Asset', 'getChildTypes', '');
        echo $q."\n";
        
        $children = DAL::executeAssoc('Asset', 'getChildTypes');
        return $children;

    }//end getChildTypes()


    /**
     * Returns given asset type's icon path.
     *
     * If typeid not given then returns base icons web dir path.
     *
     * @param string  $typeid          Id of the asset type.
     * @param boolean $defaultToParent If TRUE and given type does not have an
     *                                 icon then first parent that has an icon
     *                                 is returned.
     * @param string  $type            Type of icon (small, medium or large).
     * @param integer $assetid         Optional assetid can be passed. In fact
     *                                 this assetid intended to be used in other
     *                                 plugs only rather than real action.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getTypeIconPath(
        $typeid='',
        $defaultToParent=TRUE,
        $type='small',
        $assetid=NULL
    ) {
        $webPath  = Init::WEB_DIR;
        $webPath .= str_replace(
            dirname(dirname(dirname(__FILE__))),
            '',
            dirname(__FILE__)
        );
        $webPath .= '/icons/';

        $suffix = '';
        if ($type !== NULL) {
            $suffix = '_'.$type;
        }

        $fileName = $typeid.$suffix.'.png';

        if ($typeid !== '') {
            $webPath .= $fileName;
            if ($defaultToParent === TRUE && file_exists($webPath) === FALSE) {
                // File does not exist, return parent type's path.
                $ancestors = Asset::getParentTypes($typeid);
                $ancestors = array_reverse($ancestors);
                foreach ($ancestors as $parent) {
                    $parentPath = Asset::getTypeIconPath(
                        $parent['typeid'],
                        FALSE,
                        $type
                    );
                    if (file_exists($parentPath) === TRUE) {
                        $webPath = $parentPath;
                    }
                }
            }
        }

        return $webPath;

    }//end getTypeIconPath()


    /**
     * Returns given asset type's icon URL.
     *
     * If typeid not given then returns base icons URL.
     *
     * @param string  $typeid          Id of the asset type.
     * @param boolean $defaultToParent If TRUE and given type does not have an
     *                                 icon then first parent that has an icon
     *                                 is returned.
     * @param string  $type            Type of icon (small, medium or large).
     * @param integer $assetid         Optional assetid can be passed. In fact
     *                                 this assetid intended to be used in other
     *                                 plugs only rather than real action.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getTypeIconUrl(
        $typeid='',
        $defaultToParent=TRUE,
        $type='small',
        $assetid=NULL
    ) {
        $baseUrl  = Init::getWebURL();
        $iconPath = Asset::getTypeIconPath(
            $typeid,
            $defaultToParent,
            $type,
            $assetid
        );
        $iconPath = str_replace(Init::WEB_DIR, '', $iconPath);
        $baseUrl .= $iconPath;

        return $baseUrl;

    }//end getTypeIconUrl()


}//end class

?>
