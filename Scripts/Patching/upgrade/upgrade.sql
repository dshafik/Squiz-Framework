-------------------
-- PATCH_10231_X --
-------------------

BEGIN;
ALTER TABLE publishing ADD COLUMN scheduled_datetime TIMESTAMP WITH TIME ZONE DEFAULT NULL;

TRUNCATE cache;
ALTER TABLE cache DROP CONSTRAINT cache_pk RESTRICT;
ALTER TABLE cache DROP COLUMN url;
ALTER TABLE cache ADD COLUMN url VARCHAR(2000);
ALTER TABLE cache ALTER COLUMN url SET NOT NULL;
CREATE INDEX cache_idx1 ON cache(url);
ALTER TABLE cache DROP COLUMN content;
ALTER TABLE cache ADD COLUMN content TEXT;
ALTER TABLE cache ALTER COLUMN content SET NOT NULL;
ALTER TABLE cache ALTER COLUMN expiry SET NOT NULL;
ALTER TABLE cache ALTER COLUMN time_taken SET NOT NULL;
ALTER TABLE cache ADD COLUMN owner VARCHAR(8);
ALTER TABLE cache ALTER COLUMN owner SET NOT NULL;
ALTER TABLE cache ADD CONSTRAINT cache_pk PRIMARY KEY (assetid, owner, url);
CREATE INDEX cache_idx3 ON cache(owner);
ALTER TABLE cache_stats DROP COLUMN size;
ALTER TABLE cache_stats DROP COLUMN keyword;
ALTER TABLE cache_stats ADD COLUMN owner VARCHAR(8);
UPDATE cache_stats SET owner = '00000000';
ALTER TABLE cache_stats ALTER COLUMN owner SET NOT NULL;
ALTER TABLE cache_stats DROP CONSTRAINT cache_stats_pk RESTRICT;
ALTER TABLE cache_stats ADD CONSTRAINT cache_stats_pk PRIMARY KEY (date, url, assetid, owner);
CREATE INDEX cache_stats_idx3 ON cache_stats(owner);
ALTER TABLE cache_stats_daily DROP COLUMN size;
ALTER TABLE cache_stats_daily DROP COLUMN keyword;


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
            AND NOT EXISTS(
                SELECT 1 FROM asset_link_tree t4
                WHERE t4.assetid = t1.assetid
                AND t4.path = t2.path || t2.assetid || t1.path
                AND t4.owner = _owner
            )
        );

        RETURN;
    END IF;

    -- If we couldn't insert into the tree, then we must be inserting at the root level.
    IF NOT FOUND THEN
        IF newParentid <> 0 THEN
            -- Our parent needs to be inserted at the root level
            -- and we need to be inserted under the parent.
            INSERT INTO asset_link_tree VALUES (newParentid, '/', 1, true, _owner);
            INSERT INTO asset_link_tree VALUES (newChildid, '/' || newParentid || '/', 2, true, _owner);
        ELSE
            -- Inserting at the root level.
            INSERT INTO asset_link_tree VALUES (newChildid, '/', 1, true, _owner);
        END IF;
    END IF;

END
$$ LANGUAGE plpgsql STRICT VOLATILE;


CREATE OR REPLACE FUNCTION updateLookupPath() RETURNS TRIGGER AS $$
DECLARE
    old_url   lookup.url%TYPE;
    old_path  lookup.path%TYPE;
    new_url   lookup.url%TYPE;
    new_urlid lookup.urlid%TYPE;
BEGIN

    -- Select the old path so that we can use LIKE clause to update children.
    FOR old_url, old_path, new_urlid IN SELECT url, path, urlid FROM lookup
                   WHERE assetid = NEW.assetid
                   AND owner = getContext(NEW.assetid)
                   AND active = true
    LOOP
        new_url := regexp_replace(old_url, '(/' || old_path || ')$', '/' || NEW.path);

        -- A lookup entry to disable parent context lookups may exist.
        -- This would happen if path is changed from abc to abc1 in 01 context and
        -- then changed back to abc. The entry for abc false needs to be removed to
        -- prevent dupe key errors.
        DELETE FROM lookup
        WHERE (url = new_url OR url LIKE new_url || '/%')
        AND urlid = new_urlid
        AND owner = getContext(NEW.assetid)
        AND active = false;

        UPDATE lookup
        SET url = new_url,
        path = NEW.path
        WHERE url = old_url
        AND urlid = new_urlid
        AND owner = getContext(NEW.assetid)
        AND active = true;

        UPDATE lookup
        SET url = regexp_replace(url, '^' || old_url, new_url)
        WHERE url LIKE old_url || '/%'
        AND urlid = new_urlid
        AND owner = getContext(NEW.assetid)
        AND active = true;

    END LOOP;

    RETURN OLD;
END
$$ LANGUAGE PLPGSQL STRICT VOLATILE;

DROP TRIGGER IF EXISTS updateLookupPathTrig ON lookup_path;

CREATE TRIGGER updateLookupPathTrig
AFTER UPDATE ON lookup_path
FOR EACH ROW
EXECUTE PROCEDURE updateLookupPath();


CREATE TABLE analytics_test (
    testid     INTEGER NOT NULL,
    url        VARCHAR(2000) NOT NULL,
    variations VARCHAR(100) NOT NULL,
    CONSTRAINT analytics_test_pk PRIMARY KEY(testid),
    CONSTRAINT analytics_test_url_uni UNIQUE(url)
);

CREATE INDEX analytics_test_url_idx ON analytics_test(url);
CREATE SEQUENCE analytics_test_seq;

COMMIT;

