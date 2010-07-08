--
-- Returns the context assetids for the specified assetid.
--
-- @param DOUBLE PRECISION|TEXT assetid The assetid.
--
-- @return SETOF TEXT.
--
CREATE OR REPLACE FUNCTION getContextAssetids(assetid DOUBLE PRECISION) RETURNS setof DOUBLE PRECISION AS $$
DECLARE
    assetids TEXT;
    strAssetid TEXT;
    tmp TEXT[];
    j INT;
BEGIN
    -- Cast the DOUBLE PRECISION asset to TEXT so that we can use string functions.
    tmp = string_to_array(CAST(assetid AS TEXT), '.');

    -- Return the current assetid;
    assetids = assetid;
    RETURN NEXT CAST(assetids AS DOUBLE PRECISION);

    IF tmp[2] IS NOT NULL THEN
        j = 3;
        FOR i IN 1 .. ((length(tmp[2])/2) - 1)
        LOOP
            strAssetid = assetids;
            assetids   = tmp[1] || '.' || lpad('', (j - 1), '0') || substring(tmp[2] from j);

            -- If the prev assetid is not the same as the new one then return it as a row.
            IF assetids <> strAssetid THEN
                RETURN NEXT CAST(assetids AS DOUBLE PRECISION);
            END IF;
            j = j + 2;
        END LOOP;

        -- Return the base assetid.
        RETURN NEXT CAST(tmp[1] AS DOUBLE PRECISION);
    END IF;

    -- Done.
    RETURN;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;

CREATE OR REPLACE FUNCTION getContextAssetids(assetid TEXT) RETURNS setof DOUBLE PRECISION AS $$
DECLARE
    assetids TEXT;
    strAssetid TEXT;
    tmp TEXT[];
    j INT;
BEGIN
    -- Cast the DOUBLE PRECISION asset to TEXT so that we can use string functions.
    tmp = string_to_array(CAST(assetid AS TEXT), '.');

    -- Return the current assetid;
    assetids = assetid;
    RETURN NEXT CAST(assetids AS DOUBLE PRECISION);

    IF tmp[2] IS NOT NULL THEN
        j = 3;
        FOR i IN 1 .. ((length(tmp[2])/2) - 1)
        LOOP
            strAssetid = assetids;
            assetids   = tmp[1] || '.' || lpad('', (j - 1), '0') || substring(tmp[2] from j);

            -- If the prev assetid is not the same as the new one then return it as a row.
            IF assetids <> strAssetid THEN
                RETURN NEXT CAST(assetids AS DOUBLE PRECISION);
            END IF;
            j = j + 2;
        END LOOP;

        -- Return the base assetid.
        RETURN NEXT CAST(tmp[1] AS DOUBLE PRECISION);
    END IF;

    -- Done.
    RETURN;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;


CREATE OR REPLACE FUNCTION getContextAssetids(assetid DOUBLE PRECISION[]) RETURNS setof DOUBLE PRECISION AS $$
DECLARE
    assetids TEXT;
    strAssetid TEXT;
    tmp TEXT[];
    j INT;
BEGIN

    FOR i IN array_lower(assetid, 1) .. array_upper(assetid, 1)
    LOOP
        -- Cast the DOUBLE PRECISION asset to TEXT so that we can use string functions.
        tmp = string_to_array(CAST(assetid[i] AS TEXT), '.');

        -- Return the current assetid;
        assetids = assetid[i];
        RETURN NEXT CAST(assetids AS DOUBLE PRECISION);

        IF tmp[2] IS NOT NULL THEN
            j = 3;
            FOR i IN 1 .. ((length(tmp[2])/2) - 1)
            LOOP
                strAssetid = assetids;
                assetids   = tmp[1] || '.' || lpad('', (j - 1), '0') || substring(tmp[2] from j);

                -- If the prev assetid is not the same as the new one then return it as a row.
                IF assetids <> strAssetid THEN
                    RETURN NEXT CAST(assetids AS DOUBLE PRECISION);
                END IF;
                j = j + 2;
            END LOOP;

            -- Return the base assetid.
            RETURN NEXT CAST(tmp[1] AS DOUBLE PRECISION);
        END IF;

    END LOOP;

    -- Done.
    RETURN;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;

-- Given assetid and context, it will return the assetid with context lineage.
CREATE OR REPLACE FUNCTION getContextAssetids(assetid DOUBLE PRECISION, context TEXT) RETURNS setof DOUBLE PRECISION AS $$
DECLARE
    assetids TEXT;
    strAssetid TEXT;
    tmp TEXT[];
    j INT;
BEGIN
    -- Cast the DOUBLE PRECISION asset to TEXT so that we can use string functions.
    tmp := string_to_array(CAST(assetid AS TEXT), '.');
    assetids := '';

    j = 1;
    FOR i IN 1 .. ((length(context) / 2) - 1)
    LOOP
        strAssetid = assetids;
        assetids   = tmp[1] || '.' || lpad('', (j - 1), '0') || substring(context from j);

        -- If the prev assetid is not the same as the new one then return it as a row.
        IF assetids <> strAssetid THEN
            RETURN NEXT CAST(assetids AS DOUBLE PRECISION);
        END IF;
        j = j + 2;
    END LOOP;

    -- Return the base assetid.
    RETURN NEXT CAST(tmp[1] AS DOUBLE PRECISION);

    -- Done.
    RETURN;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;


-- Given assetid and context, it will return the assetid with context lineage.
CREATE OR REPLACE FUNCTION getContextAssetids(assetid DOUBLE PRECISION[], context TEXT) RETURNS setof DOUBLE PRECISION AS $$
DECLARE
    assetids TEXT;
    strAssetid TEXT;
    tmp TEXT[];
    j INT;
BEGIN
    FOR k IN array_lower(assetid, 1) .. array_upper(assetid, 1)
    LOOP
        -- Cast the DOUBLE PRECISION asset to TEXT so that we can use string functions.
        tmp := string_to_array(CAST(assetid[k] AS TEXT), '.');
        assetids := '';

        j = 1;
        FOR i IN 1 .. ((length(context) / 2) - 1)
        LOOP
            strAssetid = assetids;
            assetids   = tmp[1] || '.' || lpad('', (j - 1), '0') || substring(context from j);

            -- If the prev assetid is not the same as the new one then return it as a row.
            IF assetids <> strAssetid THEN
                RETURN NEXT CAST(assetids AS DOUBLE PRECISION);
            END IF;
            j = j + 2;
        END LOOP;

        -- Return the base assetid.
        RETURN NEXT CAST(tmp[1] AS DOUBLE PRECISION);
    END LOOP;

    -- Done.
    RETURN;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;
