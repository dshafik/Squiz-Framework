CREATE OR REPLACE FUNCTION getChildren(parentid ANYELEMENT, minDepth INT, maxDepth INT, OUT assetid DOUBLE PRECISION, OUT depth DOUBLE PRECISION) RETURNS SETOF record AS $$
DECLARE
    rec RECORD;
    _pMaster  INT;
    processed TEXT;
    inactives TEXT;
    _path     TEXT;
    _parentPath TEXT;
    _parentDepth INT;
BEGIN
    _pMaster := getMasterAssetid(parentid);
    processed := '';
    inactives := '';

    SELECT INTO _parentPath, _parentDepth
        asset_link_tree.path || _pMaster || '/', asset_link_tree.depth
    FROM
        asset_link_tree
    WHERE
    (
        asset_link_tree.assetid = _pMaster
        AND asset_link_tree.owner IN(SELECT * FROM getContexts(parentid))
    )
    ORDER BY asset_link_tree.owner  DESC
    LIMIT 1 OFFSET 0;

    _parentPath := regexp_replace(_parentPath, E'\\.\\d+', '', 'g');

    FOR rec IN
        SELECT
            asset_link_tree.assetid, regexp_replace(asset_link_tree.path, E'\\.\\d+', '', 'g') as path, asset_link_tree.active, asset_link_tree.depth
        FROM asset_link_tree
        WHERE
        (
            regexp_replace(asset_link_tree.path, E'\\.\\d+', '', 'g') LIKE _parentPath || '%'
            -- depth filter.
            AND (minDepth = 0 OR asset_link_tree.depth >= (_parentDepth + minDepth))
            AND (maxDepth = 0 OR asset_link_tree.depth <= (_parentDepth + maxDepth))
            AND asset_link_tree.owner IN(SELECT * FROM getContexts(parentid))
        )
        ORDER BY owner DESC
    LOOP
        _path = '|' || rec.path || '/' || rec.assetid || '|';

        if rec.active = TRUE AND strpos(processed, _path) = 0 AND strpos(inactives, _path) = 0 THEN
            -- It was not processed before, so add it to processed string.
            processed = processed || _path;
            assetid := rec.assetid;
            depth := rec.depth;
            RETURN NEXT;
        ELSE
            inactives = inactives || _path;
        END IF;
    END LOOP;

    RETURN;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;


CREATE OR REPLACE FUNCTION getChildren(parentid TEXT, minDepth INT, maxDepth INT, OUT assetid DOUBLE PRECISION, OUT depth DOUBLE PRECISION) RETURNS SETOF record AS $$
DECLARE
    rec RECORD;
    _pMaster  INT;
    processed TEXT;
    inactives TEXT;
    _path     TEXT;
    _parentPath TEXT;
    _parentDepth INT;
BEGIN
    _pMaster := getMasterAssetid(parentid);
    processed := '';
    inactives := '';

    SELECT INTO _parentPath, _parentDepth
        asset_link_tree.path || _pMaster || '/', asset_link_tree.depth
    FROM
        asset_link_tree
    WHERE
    (
        asset_link_tree.assetid = _pMaster
        AND asset_link_tree.owner IN(SELECT * FROM getContexts(parentid))
    )
    ORDER BY asset_link_tree.owner  DESC
    LIMIT 1 OFFSET 0;

    _parentPath := regexp_replace(_parentPath, E'\\.\\d+', '', 'g');

    FOR rec IN
        SELECT
            asset_link_tree.assetid, regexp_replace(asset_link_tree.path, E'\\.\\d+', '', 'g') as path, asset_link_tree.active, asset_link_tree.depth
        FROM asset_link_tree
        WHERE
        (
            regexp_replace(asset_link_tree.path, E'\\.\\d+', '', 'g') LIKE _parentPath || '%'
            -- depth filter.
            AND (minDepth = 0 OR asset_link_tree.depth >= (_parentDepth + minDepth))
            AND (maxDepth = 0 OR asset_link_tree.depth <= (_parentDepth + maxDepth))
            AND asset_link_tree.owner IN(SELECT * FROM getContexts(parentid))
        )
        ORDER BY owner DESC
    LOOP
        _path = '|' || rec.path || '/' || rec.assetid || '|';

        if rec.active = TRUE AND strpos(processed, _path) = 0 AND strpos(inactives, _path) = 0 THEN
            -- It was not processed before, so add it to processed string.
            processed = processed || _path;
            assetid := rec.assetid;
            depth := rec.depth;
            RETURN NEXT;
        ELSE
            inactives = inactives || _path;
        END IF;
    END LOOP;

    RETURN;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;
