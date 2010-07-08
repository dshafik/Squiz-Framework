--
-- Syncs the asset_link_tree table upon an INSERT to the asset_link table.
--
-- @param FLOAT newChildid  The new child to insert.
-- @param FLOAT newParentid The new parent to insert.
--
-- @return VOID.
--
CREATE OR REPLACE FUNCTION syncTreeInsert(newChildid DOUBLE PRECISION, newParentid DOUBLE PRECISION, newActive BOOLEAN) RETURNS VOID AS $$
DECLARE
    childPath    VARCHAR;
    parentMaster INTEGER;
    _owner       VARCHAR;
BEGIN

    parentMaster = getMasterAssetid(newParentid);
    _owner       = getContext(newParentid);

    -- Get the path of the child so that we can append any children that
    -- it has under its new location. We can't put this into the insert
    -- statement because PostgreSQL cannot use an index scan on LIKE queries
    -- unless the pattern is a plan-time constant.
    SELECT INTO childPath path
    FROM asset_link_tree
    WHERE assetid = newChildid
    LIMIT 1;

    IF NOT FOUND THEN
        -- If there is not instance of the child into the tree, then we can
        -- just insert this child straight into the parent locations
        INSERT INTO asset_link_tree
        (
            SELECT newChildid, regexp_replace(asset_link_tree.path, E'\\.\\d+', '', 'g') || getContextAssetid(assetid, _owner) || '/', pathDepth(path), asset_link_tree.active, asset_link_tree.owner
            FROM asset_link_tree
            WHERE assetid = parentMaster
        );

        -- Now, there might be child link in asset_link table which needs to be
        -- imported to asset_link_tree table.
        IF newParentid <> 0 THEN
            PERFORM syncLinkTreeChildren(newChildid, _owner, newActive);
        END IF;
    ELSE
    	childPath = regexp_replace(childPath, E'\\.\\d+', '', 'g');

        -- The child already exists in the tree, so insert it under all the
        -- locations of the parent, and insert all its children under this new
        -- location.
        INSERT INTO asset_link_tree
        (
            SELECT DISTINCT newChildid, regexp_replace(l1.path, E'\\.\\d+', '', 'g') || getContextAssetid(l1.assetid, _owner) || '/', pathDepth(l1.path), newActive, _owner
            FROM asset_link_tree l1
            WHERE l1.assetid = parentMaster
            AND NOT EXISTS(
                SELECT 1 FROM asset_link_tree l2
                WHERE l2.assetid = newChildid
                AND l2.path = regexp_replace(l1.path, E'\\.\\d+', '', 'g') || getContextAssetid(l1.assetid, _owner) || '/'
                AND l2.owner = _owner
            )

            UNION ALL

            SELECT t1.assetid, t2.path || t2.assetid || t1.path,
                   (pathDepth(t2.path || t2.assetid || t1.path) - 1), newActive, _owner
            FROM
            (
                SELECT replace(substring(t3.path, length(childPath)), CAST(newChildid AS TEXT), CAST(getContextAssetid(newChildid, _owner) AS TEXT)) AS path, assetid
                FROM asset_link_tree t3
                WHERE t3.path LIKE childPath || newChildid || '/%'
                AND t3.assetid <> newChildid
            ) t1, asset_link_tree t2
            WHERE t2.assetid = parentMaster
        );

        RETURN;
    END IF;

    -- If we couldn't insert into the tree, then we must be inserting at
    -- the root level. Ensure that the parentid is 0.
    IF NOT FOUND THEN
        IF newParentid <> 0 THEN
            RAISE EXCEPTION 'Could not find parent path for asset %',
             newParentid;
        END IF;
        INSERT INTO asset_link_tree VALUES (newChildid, '/', 1, true, '00000000');
    END IF;

END
$$ LANGUAGE plpgsql STRICT VOLATILE;


--
-- Syncs the asset_link_tree table when a insert is made to the asset_link table.
--
-- @return TRIGGER
--
CREATE OR REPLACE FUNCTION syncTreeInsert() RETURNS TRIGGER AS $$
DECLARE
BEGIN

    IF floor(NEW.sort_order) = NEW.sort_order THEN
        PERFORM updateSortOrder(NEW.parentid, NEW.childid, NEW.sort_order, 'insert');
        PERFORM syncTreeInsert(NEW.childid, NEW.parentid, NEW.active);
    END IF;

    RETURN NEW;
END
$$ LANGUAGE plpgsql VOLATILE;

DROP TRIGGER IF EXISTS syncTreeInsertTrigger ON asset_link;

CREATE TRIGGER syncTreeInsertTrigger
AFTER INSERT
ON asset_link
FOR EACH ROW
EXECUTE PROCEDURE syncTreeInsert();
