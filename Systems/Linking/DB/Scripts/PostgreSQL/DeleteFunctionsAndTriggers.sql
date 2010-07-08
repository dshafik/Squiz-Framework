--
-- Syncs the asset_link_tree table upon DELETE to the asset_link table.
-- Note: As of rev. 8xxx, this only happens when asset is purged from trash.
--
-- @param FLOAT oldChildid  The childid that we are deleting.
-- @param FLOAT oldParentid The paretid that we are deleting.
--
-- @return VOID
--
CREATE OR REPLACE FUNCTION syncTreeDelete(oldChildid FLOAT, oldParentid FLOAT) RETURNS VOID AS $$
DECLARE
    rec RECORD;
BEGIN

    IF oldParentid <> 0 THEN
        FOR rec IN SELECT path || oldParentid || '/' AS path
            FROM asset_link_tree
            WHERE assetid = oldParentid
        LOOP
            DELETE FROM asset_link_tree
            WHERE path LIKE rec.path || oldChildid || '/%'
            OR (assetid = oldChildid AND path = rec.path);
        END LOOP;
    ELSE
        -- Deleting a project, path is different.
        FOR rec IN SELECT path AS path
            FROM asset_link_tree
            WHERE assetid = oldChildid
        LOOP
            DELETE FROM asset_link_tree
            WHERE path LIKE rec.path || oldChildid || '/%'
            OR (assetid = oldChildid AND path = rec.path);
        END LOOP;

    END IF;


END
$$ LANGUAGE PLPGSQL STRICT VOLATILE;

--
-- Syncs the asset_link_tree table when a delete is issued on the asset_link table
--
-- @return Trigger
--
CREATE OR REPLACE FUNCTION syncTreeDelete() RETURNS TRIGGER AS $$
DECLARE
BEGIN

    PERFORM syncTreeDelete(OLD.childid, OLD.parentid);
    RETURN OLD;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;


DROP TRIGGER IF EXISTS syncTreeDeleteTrigger ON asset_link;


CREATE TRIGGER syncTreeDeleteTrigger
AFTER DELETE
ON asset_link
FOR EACH ROW
EXECUTE PROCEDURE syncTreeDelete();
