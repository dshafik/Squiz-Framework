--
-- Returns the context assetids for the specified assetid.
--
-- @param FLOAT|TEXT assetid The assetid.
--
-- @return SETOF TEXT.
--
CREATE OR REPLACE FUNCTION getParents(aChildid DOUBLE PRECISION) RETURNS SETOF DOUBLE PRECISION AS $$
DECLARE
    rec       RECORD;
    processed TEXT;
    inactives TEXT;
    path      TEXT;
BEGIN
    processed := '';
    inactives := '';
    FOR rec IN
        SELECT * FROM asset_link_tree
        WHERE assetid = getMasterAssetid(aChildid)
        AND owner IN(SELECT * FROM getContexts(aChildid))
        ORDER BY owner DESC
    LOOP
        -- Remove all the contexts from the path.
        path = '|' || regexp_replace(rec.path, E'\\.\\d+', '', 'g') || '|';
        if rec.active = TRUE AND strpos(processed, path) = 0 AND strpos(inactives, path) = 0 THEN
            -- It was not processed before, so add it to processed string.
            processed = processed || path;
        ELSE
            inactives = inactives || path;
        END IF;
    END LOOP;

    processed = replace(processed, '|', '');
    IF processed = '' THEN
        RETURN;
    END IF;

    RETURN QUERY SELECT DISTINCT CAST(res as DOUBLE PRECISION)
                 FROM regexp_split_to_table(processed, '/') as res
                 WHERE res <> '';
END
$$ LANGUAGE plpgsql STRICT VOLATILE;


CREATE OR REPLACE FUNCTION getParents(aChildid TEXT) RETURNS SETOF DOUBLE PRECISION AS $$
DECLARE
    rec       RECORD;
    processed TEXT;
    inactives TEXT;
    path      TEXT;
BEGIN
        processed := '';
        inactives := '';
        FOR rec IN
            SELECT * FROM asset_link_tree
            WHERE assetid = getMasterAssetid(aChildid)
            AND owner IN(SELECT * FROM getContexts(aChildid))
            ORDER BY owner DESC
        LOOP
            -- Remove all the contexts from the path.
            path = '|' || regexp_replace(rec.path, E'\\.\\d+', '', 'g') || '|';
            if rec.active = TRUE AND strpos(processed, path) = 0 AND strpos(inactives, path) = 0 THEN
                -- It was not processed before, so add it to processed string.
                processed = processed || path;
            ELSE
                inactives = inactives || path;
            END IF;
        END LOOP;

        processed = replace(processed, '|', '');
        IF processed = '' THEN
            RETURN;
        END IF;

        RETURN QUERY SELECT DISTINCT CAST(res as DOUBLE PRECISION)
                     FROM regexp_split_to_table(processed, '/') as res
                     WHERE res <> '';
END
$$ LANGUAGE plpgsql STRICT VOLATILE;
