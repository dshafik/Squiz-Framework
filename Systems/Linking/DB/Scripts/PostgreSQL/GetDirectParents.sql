CREATE OR REPLACE FUNCTION getDirectParents(aChildid DOUBLE PRECISION) RETURNS SETOF DOUBLE PRECISION AS $$
DECLARE
    rec       RECORD;
    processed TEXT;
    inactives TEXT;
    path      TEXT;
    tmp       TEXT;
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
        tmp  = regexp_replace(rec.path, E'\\.\\d+', '', 'g');
        path = '|' || tmp || '|';
        if rec.active = TRUE AND strpos(processed, path) = 0 AND strpos(inactives, path) = 0 THEN
            -- It was not processed before, so add it to processed string.
            processed = processed || path;
            tmp = regexp_replace(rtrim(tmp, '/'), '^.*/', '', 'g');
            IF tmp = '' THEN
                RETURN NEXT 0;
            ELSE
                RETURN NEXT tmp;
            END IF;
        ELSE
            inactives = inactives || path;
        END IF;
    END LOOP;

    RETURN;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;
