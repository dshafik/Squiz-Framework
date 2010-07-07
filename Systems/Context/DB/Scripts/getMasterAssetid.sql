--
-- Returns the context assetids for the specified assetid.
--
-- @param FLOAT|TEXT assetid The assetid.
--
-- @return SETOF TEXT.
--
CREATE OR REPLACE FUNCTION getMasterAssetid(assetid ANYELEMENT) RETURNS INTEGER AS $$
DECLARE
    tmp DOUBLE PRECISION[];
BEGIN
    -- Cast the FLOAT asset to TEXT so that we can use string functions.
    tmp = string_to_array(CAST(assetid AS TEXT), '.');
    RETURN tmp[1];
END
$$ LANGUAGE plpgsql STRICT VOLATILE;
