<?php
/**
 * Actions for the Linking System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Linking
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/Linking/LinkingActions.inc';

/**
 * Linking Actions Class.
 *
 * @since 4.0.0
 */
class LinkingActions
{

    /**
     * Stores parent link info during context merging.
     *
     * TODO: Should use gatherer.
     *
     * @var   array
     * @since 4.0.0
     */
    private static $_oldContextParents = array();


    /**
     * Batches the create link operations.
     *
     * @param array   $parentids The parentids to create the link under.
     * @param integer $childid   The id of the child to link.
     * @param integer $sortOrder The sort order at which to add the child.
     *
     * @since  4.0.0
     * @return void
     */
    public static function batchCreateLink(
        array $parentids,
        $childid,
        $sortOrder
    ) {
        foreach ($parentids as $parentid) {
            Linking::createLink($parentid, $childid, $sortOrder);
        }

    }//end batchCreateLink()


    /**
     * Activates a deleted link.
     *
     * Most likely will only be used by a Trash system to recover deleted links.
     *
     * @param integer $parentid  The parentid to create the link under.
     * @param integer $childid   The id of the child to link.
     * @param integer $sortOrder The sort order at which to add the child.
     * @param boolean $showMenu  Option to show the link in menu/lineage.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function activateLink(
        $parentid,
        $childid,
        $sortOrder=0,
        $showMenu=TRUE
    ) {
        return Linking::createLink($parentid, $childid, $sortOrder, $showMenu, TRUE);

    }//end activateLink()


    /**
     * Create a link between specified parent and child.
     *
     * @param integer $parentid  The parentid to create the link under.
     * @param integer $childid   The id of the child to link.
     * @param integer $sortOrder The sort order at which to add the child.
     * @param boolean $showMenu  Option to show the link in menu/lineage.
     * @param boolean $activate  Actvates specified link if it was deleted, if
     *                           link does not exist then it will be created.
     *
     * @since  4.0.0
     * @return void
     * @throws Exception        If you try to link an asset to the same parent.
     * @throws ChannelException If create link fails.
     */
    public static function createLink(
        $parentid,
        $childid,
        $sortOrder=0,
        $showMenu=TRUE,
        $activate=FALSE
    ) {
        if ($parentid === NULL) {
            return;
        }

        self::_createLink($parentid, $childid, $sortOrder, $showMenu, $activate);

    }//end createLink()


    /**
     * Create a link between specified parent and child.
     *
     * Used commonly in both of createLink() and createLinkOnly() actions.
     *
     * @param integer $parentid  The parentid to create the link under.
     * @param integer $childid   The id of the child to link.
     * @param integer $sortOrder The sort order at which to add the child.
     * @param boolean $showMenu  Option to show the link in menu/lineage.
     * @param boolean $activate  Activates specified link if it was deleted, if
     *                           link does not exist then it will be created.
     *
     * @since  4.0.0
     * @return boolean
     * @throws ChannelException If you try to link an asset to the same parent.
     * @throws ChannelException If create link fails.
     */
    private static function _createLink(
        $parentid,
        $childid,
        $sortOrder=0,
        $showMenu=TRUE,
        $activate=FALSE
    ) {
        if ($parentid === $childid) {
            throw new ChannelException('Cannot link asset to itself');
        }

        if ($sortOrder < 0) {
            $sortOrder = 0;
        }

        Channels::includeSystem('Context');

        $parentid = Context::getAssetid($parentid);
        $childid  = Context::getMasterid($childid);

        Channels::createEventBasket();
        Channels::addToBasket('parentid', $parentid);
        Channels::addToBasket('childid', $childid);
        Channels::addToBasket('sortOrder', $sortOrder);
        Channels::addToBasket('showMenu', $showMenu);

        $userData = self::_getUserData();
        Channels::addToBasket('userid', $userData['userid']);
        Channels::addToBasket('lastUpdated', $userData['time']);

        try {
            // Try to activate previously "deleted" link.
            // If updated zero entry, we need to create the actual link.
            DAL::beginTransaction();
            $status    = 4;
            $activated = DAL::executeQuery('Linking', 'activateLink');

            if ($activated === 0) {
                $status    = 0;
                $childrenQ = Linking::getChildren($childid);
                $children  = array_keys(DAL::getGrouped($childrenQ));

                if (in_array($parentid, $children) === TRUE) {
                    throw new ChannelException('already exists');
                }

                $status = 2;
                try {
                    DAL::executeQuery('Linking', 'createLink');
                } catch (Exception $e) {
                    throw new ChannelException($e->getMessage());
                }
            }

            // Add a message to the logs for the child asset.
            self::_setCreateLinkLogMessage($parentid, $childid);

            DAL::commit();
        } catch (Exception $e) {
            DAL::rollBack();
            switch ($status) {
                case 0:
                    $msg  = 'Creating a link failed since the selected parent ';
                    $msg .= '[#'.$parentid.'] is already a child of asset ';
                    $msg .= '[#'.$childid.']';
                break;

                case 1:
                    $msg  = 'Creating a link failed while updating the sort ';
                    $msg .= 'order to '.$sortOrder.".\n";
                    $msg .= $e->getMessage();
                break;

                case 2:
                    $msg  = 'Creating a link failed while running createLink ';
                    $msg .= "query.\n".$e->getMessage();
                break;

                case 3:
                    $msg  = "Creating a link failed while logging.\n";
                    $msg .= $e->getMessage();
                break;

                case 4:
                    $msg  = "Activating link failed.\n";
                    $msg .= $e->getMessage();
                break;

                default:
                    // None.
                break;
            }//end switch

            throw new ChannelException($msg);
        }//end try

        Channels::removeEventBasket();
        return TRUE;

    }//end _createLink()


    /**
     * Retrieves current userid and also current timestamp.
     *
     * @since  4.0.0
     * @return array
     */
    private static function _getUserData()
    {
        Channels::includeSystem('User');
        $userid = User::getCurrentUserid();
        if ($userid === 0) {
            $userid = NULL;
        }

        include_once 'Libs/String/String.inc';
        $ret = array(
                'userid' => $userid,
                'time'   => String::tsIso8601(time()),
               );

        return $ret;

    }//end _getUserData()


    /**
     * Sets the log message for the createLink action.
     *
     * @param integer $parentid Asset id of the parent.
     * @param integer $childid  Asset id of the child.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _setCreateLinkLogMessage($parentid, $childid)
    {
        if ($parentid !== 0 && Channels::systemExists('Log') === TRUE) {
            Channels::includeSystem('Log');
            Channels::includeSystem('Attribute');

            $name = Attribute::getAttributeValue($parentid, 'name');

            $msg  = 'Asset linked under <strong>';
            $msg .= $name.' (#'.$parentid.')</strong>';

            Log::addAssetLog($childid, 'linking.create', $msg);
        }

    }//end _setCreateLinkLogMessage()


    /**
     * Creates a link for a newly created asset.
     *
     * This action is used to be plugged into Asset::create event. It is not
     * required any other event to plug into this event.
     *
     * @param integer $parentid  The parentid to create the link under.
     * @param integer $childid   The id of the child to link.
     * @param integer $sortOrder The sort order at which to add the child.
     * @param boolean $showMenu  Option to show the link in menu/lineage.
     *
     * @since  4.0.0
     * @return void
     */
    public static function createLinkOnly(
        $parentid,
        $childid,
        $sortOrder=0,
        $showMenu=TRUE
    ) {
        if ($parentid === NULL) {
            return;
        }

        $retVal = self::_createLink($parentid, $childid, $sortOrder, $showMenu);

        return $retVal;

    }//end createLinkOnly()


    /**
     * Creates a link for a newly created asset used only during asset creation process.
     *
     * @param integer $parentid The parentid to create the link under.
     * @param integer $childid  The id of the child to link.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function createLinkOnlyPlug($parentid, $childid)
    {
        Channels::includeSystem('Context');
        $prevContext = Context::setCurrentContext('');
        $retVal      = Linking::createLinkOnly($parentid, $childid);
        Context::setCurrentContext($prevContext);

        return $retVal;

    }//end createLinkOnlyPlug()


    /**
     * Updates the sort order between specified parent and child link.
     *
     * @param integer $parentid  The parentid to create the link under.
     * @param integer $childid   The id of the child to link.
     * @param integer $sortOrder The sort order at which to add the child.
     * @param boolean $showMenu  Option to show the link in menu/lineage.
     *
     * @since  4.0.0
     * @return void
     */
    public static function updateLink(
        $parentid,
        $childid,
        $sortOrder,
        $showMenu
    ) {
        Linking::updateSortOrders($parentid, $childid, $parentid, $sortOrder);
        $userData = self::_getUserData();
        Channels::addToBasket('userid', $userData['userid']);
        Channels::addToBasket('lastUpdated', $userData['time']);

        DAL::executeQuery('Linking', 'updateLink');

    }//end updateLink()


    /**
     * Update link sort orders.
     *
     * If sortOrder is less than or equal to 0 then it will be added as the last
     * child.
     *
     * @param integer $parentid    The parentid to create the link under.
     * @param integer $childid     The id of the child to link.
     * @param integer $newParentid The id of the new parent asset.
     * @param integer $sortOrder   The sort order at which to add the child.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function updateSortOrders(
        $parentid,
        $childid,
        $newParentid,
        $sortOrder
    ) {
        if ($parentid === NULL && $newParentid === NULL) {
            // It should not do anything as both parents are NULL.
            return FALSE;
        }

        Channels::includeSystem('Context');
        $parentid = Context::getAssetid($parentid);
        Channels::modifyBasket('parentid', $parentid);
        $query  = DAL::getQuery('Linking', 'updateSortOrder');
        $retVal = DAL::getOne($query);
        return $retVal;

    }//end updateSortOrders()


    /**
     * Retruns TRUE if the specified asset has only 1 link remianing.
     *
     * This is a switcher plug for deleteLink. If asset has only 1 link remaining and
     * Linking::deleteLink is called then this method will switch the channel to
     * Linking::trashAsset.
     *
     * @param integer $assetid The asset ID to check.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function isLastLink($assetid)
    {
        $parents = DAL::getAssoc(Linking::getParents($assetid, 1, 1), 0);
        if (count($parents) === 1) {
            return TRUE;
        }

        return FALSE;

    }//end isLastLink()


    /**
     * Deletes a link between parent and child.
     *
     * IMPORTANT: If asset needs to be trashed (i.e. all parent links are being
     * deleted) then use Linking::trashAsset().
     *
     * If last link is being deleted then asset will be trashed.
     *
     * @param integer $parentid ID of the parent.
     * @param integer $childid  ID of the child.
     *
     * @since  4.0.0
     * @return object
     * @throws ChannelException If you try to delete a link that doesn't exist.
     */
    public static function deleteLink($parentid, $childid)
    {
        try {
            DAL::beginTransaction();
            Channels::includeSystem('Context');
            $context = Context::getCurrentContext();
            Channels::addToBasket('context', NULL);
            if (Context::isMasterContext($context) !== TRUE) {
                $parentid = Context::getAssetid($parentid);
                $childid  = Context::getAssetid($childid);
                Channels::addToBasket('parentid', $parentid, TRUE);
                Channels::addToBasket('context', $context, TRUE);
                Channels::addToBasket('sortOrder', 0, TRUE);
                Channels::addToBasket('active', 0, TRUE);
                $userData = self::_getUserData();
                Channels::addToBasket('userid', $userData['userid']);
                Channels::addToBasket('lastUpdated', $userData['time']);
            }

            // If this is the last link then use Linking::trashAsset().
            $baseQuery = DAL::getDALQuery(
                'Linking',
                'deleteLink',
                'executeQuery'
            );

            $updateResult = FALSE;
            if (Context::isMasterContext($context) !== TRUE) {
                $query        = DAL::getQuery('Linking', 'turnOffLink');
                $updateResult = (bool) DAL::executeQuery($query);
            }

            if ($updateResult === FALSE) {
                $query  = DAL::getQuery('Linking', 'deleteLink');
                $result = (bool) DAL::executeQuery($query);
            } else {
                $result = TRUE;
            }

            if (Channels::systemExists('Log') === TRUE) {
                Channels::includeSystem('Log');
                Channels::includeSystem('Attribute');
                $name = Attribute::getAttributeValue($parentid, 'name');
                $msg  = 'Asset unlinked from under <strong>'.$name.' (#';
                $msg .= $parentid.')</strong>';
                Log::addAssetLog($childid, 'linking.delete', $msg);
            }

            DAL::commit();
        } catch (DALException $e) {
            DAL::rollBack();
            throw new ChannelException($e);
        }//end try

        return $result;

    }//end deleteLink()


    /**
     * Disabled links to all parents for a particular child asset.
     *
     * If an asset needs to be trashed then this method must be used.
     *
     * If Trash system is installed then asset will appear in the trash.
     * This method sets the active flag to false on the asset_link table and
     * does not delete the entries. So that restore to old parents will work.
     *
     * @param integer $childid ID of the asset to move to trash.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function trashAsset($childid)
    {
        $userData = self::_getUserData();
        Channels::addToBasket('userid', $userData['userid']);
        Channels::addToBasket('lastUpdated', $userData['time']);

        Channels::includeSystem('Context');
        $context = Context::getCurrentContext();
        Channels::addToBasket('context', NULL);
        $result = FALSE;

        if (Context::isMasterContext($context) !== TRUE) {
            // Not the master context so all links need to be marked as removed.
            $parents = DAL::getAssoc(Linking::getParents($childid, 1, 1));
            foreach ($parents as $parentid) {
                $parentid = Context::getAssetid($parentid);
                Channels::addToBasket('parentid', $parentid, TRUE);
                Channels::addToBasket('context', $context, TRUE);
                Channels::addToBasket('sortOrder', 0, TRUE);
                Channels::addToBasket('active', FALSE, TRUE);

                // First try to turn off exisiting db entry.
                $query        = DAL::getQuery('Linking', 'turnOffLink');
                $updateResult = (bool) DAL::executeQuery($query);

                if ($updateResult === FALSE) {
                    // Failed to update so insert a new row with active flag = FALSE.
                    $query  = DAL::getQuery('Linking', 'deleteLink');
                    $result = DAL::executeQuery($query);
                }
            }
        } else {
            $result = DAL::executeQuery('Linking', 'deleteParentLinks');
        }//end if

        if (Channels::systemExists('Log') === TRUE) {
            Channels::includeSystem('Log');
            $msg = 'Asset moved to <strong>the trash</strong>';
            Log::addAssetLog($childid, 'linking.delete', $msg);
        }

        return $result;

    }//end trashAsset()


    /**
     * Returns query object to update show_menu option for the link.
     *
     * @param integer $parentid Parent assetid.
     * @param integer $childid  Child assetid.
     * @param boolean $showMenu True link is displayed on the menu and lineage.
     *
     * @since  4.0.0
     * @return object
     */
    public static function updateLinkShowMenu($parentid, $childid, $showMenu)
    {
        Channels::includeSystem('Context');
        $parentid = Context::getAssetid($parentid);
        Channels::modifyBasket('parentid', $parentid);

        // Also create childid asset in the current context.
        Context::getAssetid($childid);

        $userData = self::_getUserData();
        Channels::addToBasket('userid', $userData['userid']);
        Channels::addToBasket('lastUpdated', $userData['time']);

        $result = (bool) DAL::executeQuery('Linking', 'updateLinkShowMenu');

        if ($result === FALSE
            && Context::isMasterContext() === FALSE
        ) {
            // Failed to update row and not master context. May need to insert a new row to DB.
            $result = Linking::createLinkOnly($parentid, $childid, DAL::getOne(Linking::getSortOrder($parentid, $childid)), $showMenu);
        }

        return $result;

    }//end updateLinkShowMenu()


    /**
     * Returns query to get list of links with the given show_menu option.
     *
     * @param integer      $parentid Parent assetid.
     * @param array|object $children Child assetids.
     * @param boolean      $showMenu True link is displayed on the menu and
     *                               lineage.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getShowMenuLinksOnly(
        $parentid,
        $children,
        $showMenu=TRUE
    ) {
        Channels::includeSystem('Context');
        $parentid = Context::getAssetid($parentid, NULL, TRUE);
        Channels::modifyBasket('parentid', $parentid);

        $baseQuery = DAL::getDALQuery(
            'Linking',
            'getShowMenuLinksOnly',
            'getAssoc'
        );
        return $baseQuery;

    }//end getShowMenuLinksOnly()


    /**
     * Returns query object to get show_menu option for the link.
     *
     * @param integer $parentid Parent assetid.
     * @param integer $childid  Child assetid.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function getLinkShowMenu($parentid, $childid)
    {
        Channels::includeSystem('Context');
        $parentid = Context::getAssetid($parentid, NULL, TRUE);
        Channels::modifyBasket('parentid', $parentid);

        $baseQuery = DAL::getDALQuery(
            'Linking',
            'getLinkShowMenu',
            'getOne'
        );

        $showMenu = DAL::getOne($baseQuery);
        return $showMenu;

    }//end getLinkShowMenu()


    /**
     * Returns getImmediateParents Query object.
     *
     * @param string  $childid     The asset ID of the asset to link to.
     * @param integer $notParentid If specified then all immediate parents
     *                             except the specified parent will be
     *                             returned.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getImmediateParents($childid, $notParentid=NULL)
    {
        Channels::includeSystem('Context');
        $childid = Context::getAssetid($childid, NULL, TRUE);
        Channels::modifyBasket('childid', $childid);

        $baseQuery = DAL::getDALQuery(
            'Linking',
            'getImmediateParents',
            'executeQuery'
        );
        return $baseQuery;

    }//end getImmediateParents()


    /**
     * Returns getImmediateChildren Query object.
     *
     * @param string       $parentid      The ID of the parent asset.
     * @param boolean      $withSortOrder If TRUE, the result query returns
     *                                    sort_order column together.
     * @param array|object $types         Optional list of asset types to get.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getImmediateChildren(
        $parentid,
        $withSortOrder=FALSE,
        $types=array()
    ) {
        Channels::includeSystem('Context');
        // Get the context assetid, SQL:getDirectChildren() method requres the
        // context in assetid.
        $parentid = Context::getAssetid($parentid, NULL, TRUE);
        Channels::modifyBasket('parentid', $parentid);

        $result = DAL::getDALQuery(
            'Linking',
            'getImmediateChildren',
            'executeQuery'
        );
        if (empty($types) === FALSE) {
            Channels::includeSystem('AssetType');
            $result = AssetType::filterAssetsByTypes($result, $types);
        }

        return $result;

    }//end getImmediateChildren()


    /**
     * Slightly different query from getImmediateChildren.
     *
     * The caller can pass multiple parent ids and the result query selects
     * both of parent and child ids.
     *
     * @param array|object $parentids     List of parent ids.
     * @param boolean      $withSortOrder If TRUE, the result query returns
     *                                    sort_order column together.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getLevelImmediateChildren(
        $parentids,
        $withSortOrder=FALSE
    ) {
        $result = DAL::getDALQuery(
            'Linking',
            'getLevelImmediateChildren',
            'executeQuery'
        );
        return $result;

    }//end getLevelImmediateChildren()


    /**
     * Returns getSortOrder Query object.
     *
     * @param string $parentid The asset ID of the asset to link the new
     *                         asset underneath.
     * @param string $childid  The asset ID of the asset to link to.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getSortOrder($parentid, $childid)
    {
        Channels::includeSystem('Context');
        $parentid = Context::getAssetid($parentid, NULL, TRUE);
        Channels::modifyBasket('parentid', $parentid);

        $baseQuery = DAL::getDALQuery(
            'Linking',
            'getSortOrder',
            'getOne'
        );
        return $baseQuery;

    }//end getSortOrder()


    /**
     * Returns the child ids with their sort orders for a particular parent.
     *
     * @param string $parentid The ID of the parent assets.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getChildSortOrders($parentid)
    {
        Channels::includeSystem('Context');
        $parentid = Context::getAssetid($parentid, NULL, TRUE);
        Channels::modifyBasket('parentid', $parentid);

        $baseQuery = DAL::getDALQuery(
            'Linking',
            'getChildSortOrders',
            'getAssoc'
        );
        return $baseQuery;

    }//end getChildSortOrders()


    /**
     * Check if the link is already exists between two assets.
     *
     * @param string $parentid The Asset Id of the parent asset.
     * @param string $childid  The Asset id of the child asset.
     *
     * @since  4.0.0
     * @return boolean
     * @throws ChannelException When query failed.
     * @access public
     */
    public static function linkExists($parentid, $childid)
    {
        Channels::includeSystem('Context');
        $parentid = Context::getAssetids($parentid);
        Channels::modifyBasket('parentid', $parentid);

        $baseQuery = DAL::getDALQuery('Linking', 'linkExists', 'getAssoc');

        try {
            $linkCheck = DAL::getAssoc($baseQuery);
        } catch (ChannelException $e) {
            throw new ChannelException($e->getMessage());
        }//end try

        if (empty($linkCheck) === TRUE) {
            return FALSE;
        } else {
            return TRUE;
        }//end if

    }//end linkExists()


    /**
     * Return assetids which are not in trash from a list of assetids.
     *
     * @param string $assetids The list of assetids to filter.
     *
     * @since  4.0.0
     * @return boolean
     * @throws ChannelException When query failed.
     * @access public
     */
    public static function removeAssetInTrashFromListQuery($assetids)
    {
        $baseQuery = DAL::getDALQuery('Linking', 'removeAssetInTrashFromList', 'getAssoc');

        return $baseQuery;

    }//end removeAssetInTrashFromListQuery()


    /**
     * Replaces context.
     *
     * @param string $oldContext The context to get values from.
     * @param string $newContext The context to override.
     * @param string $assetid    Asset id to use.
     *
     * @since  4.0.0
     * @return void
     * @throws ChannelException If context cannot be replaced.
     */
    public static function replaceContextRemoveLinks($oldContext, $newContext, $assetid)
    {
        Channels::includeSystem('Context');
        $originalContext = Context::setCurrentContext($oldContext, TRUE);

        try {
            DAL::beginTransaction();

            // Get the parent links of old context.
            $oldParents  = DAL::getAssoc(Linking::getParents($assetid, 1, 1), 0);
            $oldChildren = DAL::getAssoc(Linking::getImmediateChildren($assetid), 0);
            $showInMenu  = array();

            foreach ($oldParents as $oldParentid) {
                $showInMenu[$oldParentid] = Linking::getLinkShowMenu($oldParentid, $assetid);
            }

            self::$_oldContextParents = array(
                                         'parents'    => $oldParents,
                                         'children'   => $oldChildren,
                                         'showInMenu' => $showInMenu,
                                        );

            // Get the new parents.
            Context::setCurrentContext($newContext, TRUE);
            $newParents = DAL::getAssoc(Linking::getParents($assetid, 1, 1), 0);

            $parentsToRemove = array_diff($newParents, $oldParents);

            // Note that we are still in the new context so all deleteLink calls will
            // effect the new context.
            foreach ($parentsToRemove as $parentid) {
                Linking::deleteLink($parentid, $assetid);
            }

            Context::setCurrentContext($originalContext, TRUE);

            DAL::commit();
        } catch (ChannelException $e) {
            Context::setCurrentContext($originalContext, TRUE);
            DAL::rollBack();
            throw new ChannelException($e->getMessage());
        }//end try

    }//end replaceContextRemoveLinks()


    /**
     * Replaces context.
     *
     * @param string $oldContext The context to get values from.
     * @param string $newContext The context to override.
     * @param string $assetid    Asset id to use.
     *
     * @since  4.0.0
     * @return void
     * @throws ChannelException If context cannot be replaced.
     */
    public static function replaceContextCreateLinks($oldContext, $newContext, $assetid)
    {
        Channels::includeSystem('Context');
        $originalContext = Context::setCurrentContext($oldContext, TRUE);

        try {
            DAL::beginTransaction();
            // Create links.
            // Get the parent links of old context.
            $oldParents = self::$_oldContextParents['parents'];
            $children   = self::$_oldContextParents['children'];
            $showInMenu = self::$_oldContextParents['showInMenu'];

            self::$_oldContextParents = array();

            // Get the new parents.
            Context::setCurrentContext($newContext, TRUE);
            $newParents = DAL::getAssoc(Linking::getParents($assetid, 1, 1), 0);

            // Update sort orders.
            $childCount = count($children);
            for ($i = 1; $i < $childCount; $i++) {
                Linking::updateSortOrders($assetid, $children[$i], $assetid, ($i + 1));
            }

            $parentsToLink = array_diff($oldParents, $newParents);

            foreach ($parentsToLink as $parentid) {
                Linking::createLink($parentid, $assetid);

                if (array_key_exists($parentid, $showInMenu) === TRUE) {
                    // Set the show in menu setting for this new link.
                    Linking::updateLinkShowMenu($parentid, $assetid, $showInMenu[$parentid]);
                }
            }

            // Update existing link showInMenu settings.
            foreach ($newParents as $parentid) {
                if (array_key_exists($parentid, $showInMenu) === TRUE) {
                    Linking::updateLinkShowMenu($parentid, $assetid, $showInMenu[$parentid]);
                }
            }

            // Remove data from old Context.
            Context::setCurrentContext($oldContext, TRUE);

            $path = '%/'.rtrim(Context::getAssetid($assetid), '0').'/';
            Channels::addToBasket('path', $path);

            DAL::executeQuery('Linking', 'contextCleanAssetLink');
            DAL::executeQuery('Linking', 'contextCleanAssetLinkTree');

            Context::setCurrentContext($originalContext, TRUE);
            DAL::commit();
        } catch (ChannelException $e) {
            Context::setCurrentContext($originalContext, TRUE);
            DAL::rollBack();
            throw new ChannelException($e->getMessage());
        }//end try

    }//end replaceContextCreateLinks()


    /**
     * Removes the context data for the specified context and asset.
     *
     * @param string $context The context to remove.
     * @param string $assetid Asset id to use.
     *
     * @since  4.0.0
     * @return void
     * @throws ChannelException If context data cannot be removed.
     */
    public static function removeContextData($context, $assetid=NULL)
    {
        Channels::includeSystem('Context');
        if (Context::isMasterContext($context) !== TRUE) {
            try {
                $contextAssetid = Context::getAssetid($assetid, $context, TRUE);
                $path = '%/'.rtrim($contextAssetid, '0').'/';
                Channels::addToBasket('path', $path);
                Channels::addToBasket('contextAssetid', $contextAssetid);

                DAL::beginTransaction();
                DAL::executeQuery('Linking', 'removeLinkContextData');
                DAL::executeQuery('Linking', 'removeLinkTreeContextData');
                DAL::commit();
            } catch (ChannelException $e) {
                DAL::rollBack();
                throw new ChannelException($e);
            }
        }

    }//end removeContextData()


    /**
     * Removes the entire context data for the specified context range.
     *
     * @param string $context Context string that could be used in LIKE queries.
     *                        E.g. _____01__ .
     *
     * @since  4.0.0
     * @return void
     * @throws ChannelException If context data cannot be removed.
     */
    public static function removeAllContextData($context)
    {
        Channels::includeSystem('Context');
        if (Context::isMasterContext($context) !== TRUE) {
            try {
                DAL::beginTransaction();
                $query = DAL::getQuery('Linking', 'removeAllLinkTreeContextData');
                DAL::executeQuery($query);
                $query = DAL::getQuery('Linking', 'removeAllLinkContextData');
                DAL::executeQuery($query);
                DAL::commit();
            } catch (DALException $e) {
                DAL::rollBack();
                throw new ChannelException($e);
            }
        }

    }//end removeAllContextData()


    /**
     * Returns getParents Query object.
     *
     * @param string $childid  The asset ID of the asset to link to.
     * @param string $minLevel The minimum level to get parent for.
     * @param string $maxLevel The maximum level to get parent for.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getParents($childid, $minLevel, $maxLevel)
    {
        if ($minLevel === 1 && $maxLevel === 1) {
            Channels::includeSystem('Context');
            $childid = Context::getAssetid($childid, NULL, TRUE);
            Channels::modifyBasket('childid', $childid);

            $baseQuery = DAL::getDALQuery(
                'Linking',
                'getImmediateParents',
                'executeQuery'
            );

            return $baseQuery;
        }

        Channels::includeSystem('Context');
        $childid = Context::getAssetid($childid, NULL, TRUE);
        Channels::modifyBasket('childid', $childid);

        $baseQuery = DAL::getDALQuery('Linking', 'getParents', 'Assoc');
        return $baseQuery;

    }//end getParents()


    /**
     * Returns getAssetids Query object.
     *
     * This method turns assetids array into a query object,
     * this object can later be binded to another query object.
     *
     * @param string $assetids The assetids to get Query object for.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getAssetids($assetids)
    {
        $baseQuery = DAL::getDALQuery('Linking', 'getAssetids', 'Assoc');
        return $baseQuery;

    }//end getAssetids()


    /**
     * Returns getChildren Query object.
     *
     * @param string $parentid The asset ID of the asset to link the new asset underneath.
     * @param string $minLevel The minimum level to get the children.
     * @param string $maxLevel The maximum level to get the children.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getChildren($parentid, $minLevel=0, $maxLevel=0)
    {
        Channels::includeSystem('Context');
        $parentid = Context::getAssetid($parentid, NULL, TRUE);
        Channels::modifyBasket('parentid', $parentid);

        $baseQuery = DAL::getDALQuery('Linking', 'getChildren', 'Assoc');
        return $baseQuery;

    }//end getChildren()


    /**
     * Returns batchGetChildren Query object (not implemented).
     *
     * @param integer $parentid The asset ID of the asset to link the new asset
     *                          underneath.
     *
     * @since  4.0.0
     * @return object
     */
    public static function batchGetChildren($parentid)
    {
        $baseQuery = DAL::getDALQuery('Linking', 'batchGetChildren', 'All');
        return $baseQuery;

    }//end batchGetChildren()


    /**
     * Returns getAssetChildPath Query object.
     *
     * @param string $childid The asset ID of the asset to link to.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getAssetChildPath($childid)
    {
        $baseQuery = DAL::getDALQuery('Linking', 'getAssetChildPath', 'Assoc');
        return $baseQuery;

    }//end getAssetChildPath()


    /**
     * Returns getChildrenByLevel Query object.
     *
     * @param string $path  The path to child asset.
     * @param string $level The level to get children.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getChildrenByLevel($path, $level)
    {
        $baseQuery = DAL::getDALQuery('Linking', 'getChildrenByLevel', 'Assoc');
        return $baseQuery;

    }//end getChildrenByLevel()


    /**
     * Retrieves an array of information about the children of this asset.
     *
     * The information will contain an entry for level underneath this asset
     * and a count of assets that are linked at that level.
     * This method works when asset has more than 1 asset_link_tree entry,
     * as we select the first tree, use pattern matching to find out all kids
     * under this tree, and then use depth to arrange result into level array.
     *
     * @param integer $assetid The asset to get information underneath.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getChildLevelInfo($assetid)
    {
        Channels::includeSystem('Linking');
        $path = DAL::getAssoc(Linking::getAssetChildPath($assetid));
        if (empty($path) === TRUE) {
            return array();
        }

        // Get the first element in the Association, although the query will
        // only return one row.
        $path      = current($path);
        $childPath = $path['childpath'];
        $depth     = ($path['depth'] + 1);
        $info      = Linking::getChildrenByLevel($childPath, $depth);
        $levelInfo = DAL::getAssoc($info);
        return $levelInfo;

    }//end getChildLevelInfo()


    /**
     * Returns the lineages for the specified asset.
     *
     * @param string $assetid The assetid.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getAssetLineages($assetid)
    {
        Channels::includeSystem('Context');
        $owner = Context::getContexts();
        Channels::addToBasket('owner', $owner);

        $baseQuery = DAL::getDALQuery(
            'Linking',
            'getAssetLineages',
            'Assoc'
        );
        $result    = DAL::getAssoc($baseQuery);
        $lineages  = array();

        if (empty($result) === TRUE) {
            // Failed to get the lineage.
            return $lineages;
        }

        // When a path is inactive it is added to this array so that a context below
        // cannot re-activate it.
        $inactivePaths = array();

        foreach ($result as $path) {
            // Get the parentids from the link path.
            $parentids = explode('/', trim($path['path'], '/'));

            // Convert all ids to master.
            foreach ($parentids as $index => $parentid) {
                $parentids[$index] = Context::getMasterid($parentid);
            }

            // Re-construct the lineage.
            $linPath = '/'.implode('/', $parentids).'/';

            if ($path['active'] !== TRUE) {
                // This lineage is not active.
                $inactivePaths[$linPath] = 1;
            } else if (isset($inactivePaths[$linPath]) === FALSE
                && isset($lineages[$linPath]) === FALSE
            ) {
                // This lineage is active and was not deactivated by a child context.
                $lineages[$linPath] = $parentids;
            }
        }//end foreach

        return $lineages;

    }//end getAssetLineages()


    /**
     * Removes the specified asset record from asset_link_tree table.
     *
     * @param integer $assetid Asset id.
     *
     * @since  4.0.0
     * @return void
     */
    public static function removeAssetFromLinkTree($assetid)
    {
        Dal::executeQuery('Linking', 'removeAssetFromLinkTree');

    }//end removeAssetFromLinkTree()


    /**
     * Returns DAL query to get asset link path.
     *
     * By default it limits the number of result to one.
     *
     * @param integer $assetid Asset id.
     * @param integer $limit   The limit of number of result rows.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getAssetPath($assetid, $limit=1)
    {
        $baseQuery = Dal::getDALQuery(
            'Linking',
            'getAssetPath',
            'Assoc'
        );

        return $baseQuery;

    }//end getAssetPath()


    /**
     * Returns DAL query to filter assets that are children of given parent path.
     *
     * @param mixed $parentPath Parent asset's link path. Typically useful with
     *                          getAssetPath() query.
     * @param mixed $assetids   List of assetids to filter.
     *
     * @since  4.0.0
     * @return object
     */
    public static function filterChildOf($parentPath, $assetids)
    {
        $baseQuery = Dal::getDALQuery(
            'Linking',
            'isChildOf',
            'Assoc'
        );

        return $baseQuery;

    }//end filterChildOf()


    /**
     * Returns DAL query to determine whether the asset belongs to parentid.
     *
     * @param mixed $assetid  Assetid of the asset to determine.
     * @param mixed $parentid Assetid of the asset belongs under.
     *
     * @since  4.0.0
     * @return object
     */
    public static function isChildOf($assetid, $parentid)
    {
        $baseQuery = Dal::getDALQuery(
            'Linking',
            'isChildOfByParentid',
            'Assoc'
        );

        return $baseQuery;

    }//end isChildOf()


}//end class

?>
