--
-- Syncs the asset_link_tree table when a delete is issued on the asset_link table
--
-- @return Trigger
--
CREATE OR REPLACE FUNCTION syncTreeUpdate() RETURNS TRIGGER AS $$
DECLARE
    rec RECORD;
BEGIN
    -- If the old and new values are the same, then we are just updating the sort
    -- order so we don't need to modify the tree. So only modify the tree if
    -- they are not the same.
    IF NEW.parentid <> OLD.parentid THEN
        -- We need to insert first because we use the tree to find all the children
        -- under childid so that we can propagate its children to the new location,
        -- then we can delete it from its old location.
        PERFORM syncTreeInsert(NEW.childid, NEW.parentid, NEW.active);
        PERFORM syncTreeDelete(OLD.childid, OLD.parentid);
        PERFORM updateSortOrder(NEW.parentid, NEW.childid, NEW.sort_order, 'update');
    ELSIF OLD.active <> NEW.active THEN
        FOR rec IN SELECT path || NEW.parentid || '/' AS path, path || getMasterAssetid(NEW.parentid) || '/' AS path2
            FROM asset_link_tree
            WHERE assetid = getMasterAssetid(NEW.parentid)
        LOOP
            UPDATE asset_link_tree
            SET active = NEW.active
            WHERE path LIKE rec.path || NEW.childid || '/%'
            OR path LIKE rec.path2 || getContextAssetid(NEW.childid, getContext(NEW.parentid), TRUE) || '/%'
            OR (assetid = NEW.childid AND path = rec.path);
        END LOOP;
    END IF;

    RETURN NEW;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;

DROP TRIGGER IF EXISTS syncTreeUpdateTrigger ON asset_link;

CREATE TRIGGER syncTreeUpdateTrigger
AFTER UPDATE
ON asset_link
FOR EACH ROW
EXECUTE PROCEDURE syncTreeUpdate();
