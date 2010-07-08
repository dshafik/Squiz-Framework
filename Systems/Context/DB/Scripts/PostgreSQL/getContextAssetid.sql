--
-- Returns the context assetids for the specified assetid.
--
-- @param FLOAT|TEXT assetid The assetid.
--
-- @return SETOF TEXT.
--
CREATE OR REPLACE FUNCTION getContextAssetid(assetid ANYELEMENT, context TEXT, ignoreContext BOOL) RETURNS DOUBLE PRECISION AS $$
DECLARE
    tmp TEXT[];
BEGIN

    -- If the assetid is 0 then do nothing.
    IF assetid = 0 THEN
        return assetid;
    END IF;

    -- Cast the FLOAT asset to TEXT so that we can use string functions.
    tmp = string_to_array(CAST(assetid AS TEXT), '.');

    IF ignoreContext = FALSE AND tmp[2] IS NOT NULL THEN
        -- Assetid has context already, return it as it is.
        RETURN CAST(assetid AS DOUBLE PRECISION);
    END IF;

    IF context <> '' THEN
        IF context <> '0' OR context <> '00000000' THEN
            RETURN CAST((tmp[1] || '.' || context) AS DOUBLE PRECISION);
        END IF;
    END IF;

    -- Done.
    RETURN assetid;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;


CREATE OR REPLACE FUNCTION getContextAssetid(assetid ANYELEMENT, context TEXT) RETURNS DOUBLE PRECISION AS $$
DECLARE
BEGIN
    RETURN getContextAssetid(assetid, context, FALSE);
END
$$ LANGUAGE plpgsql STRICT VOLATILE;
