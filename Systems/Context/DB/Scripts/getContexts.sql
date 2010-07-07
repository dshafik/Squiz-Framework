--
-- Returns the context assetids for the specified assetid.
--
-- @param FLOAT|TEXT assetid The assetid.
--
-- @return SETOF TEXT.
--
CREATE OR REPLACE FUNCTION getContexts(assetid ANYELEMENT) RETURNS setof TEXT AS $$
DECLARE
    assetids TEXT;
    strAssetid TEXT;
    tmp TEXT[];
    j INT;
BEGIN
    -- Cast the FLOAT asset to TEXT so that we can use string functions.
    tmp = string_to_array(CAST(assetid AS TEXT), '.');

    -- Return the current assetid;
    assetids = assetid;
    IF tmp[2] IS NOT NULL THEN
        tmp[2] = rpad(tmp[2], 8, '0');

        RETURN NEXT tmp[2];

        j = 3;
        FOR i IN 1 .. ((length(tmp[2])/2) - 1)
        LOOP
            strAssetid = assetids;
            assetids   = rtrim(lpad('', (j - 1), '0') || substring(tmp[2] from j), '0');
            -- If the prev assetid is not the same as the new one then return it as a row.
            IF assetids <> strAssetid THEN
                IF assetids = '' THEN
                    RETURN NEXT '00000000';
                END IF;

                RETURN NEXT assetids;
            END IF;
            j = j + 2;
        END LOOP;
    END IF;

    RETURN NEXT '00000000';

    -- Done.
    RETURN;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;
