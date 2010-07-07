--
-- Returns the context assetids for the specified assetid.
--
-- @param FLOAT|TEXT assetid The assetid.
--
-- @return SETOF TEXT.
--
CREATE OR REPLACE FUNCTION getContext(assetid ANYELEMENT) RETURNS TEXT AS $$
DECLARE
    tmp TEXT[];
BEGIN
    -- Cast the FLOAT asset to TEXT so that we can use string functions.
    tmp = string_to_array(CAST(assetid AS TEXT), '.');

    IF tmp[2] IS NULL OR tmp[2] = '' THEN
        RETURN '00000000';
    END IF;

    RETURN rpad(tmp[2], 8, '0');
END
$$ LANGUAGE plpgsql STRICT VOLATILE;
