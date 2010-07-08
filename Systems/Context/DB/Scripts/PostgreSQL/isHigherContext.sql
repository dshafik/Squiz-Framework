CREATE OR REPLACE FUNCTION isHigherContext(context TEXT, compareTo TEXT) RETURNS BOOLEAN AS $$
DECLARE
    tmp TEXT;
BEGIN

    IF context = compareTo THEN
        RETURN FALSE;
    END IF;

    IF compareTo = '00000000' THEN
        RETURN TRUE;
    END IF;

    tmp := regexp_replace(compareTo, '^(00)+', '');

    IF regexp_replace(context, tmp, '') <> context THEN
        RETURN TRUE;
    END IF;

    RETURN FALSE;

END
$$ LANGUAGE plpgsql STRICT VOLATILE;
