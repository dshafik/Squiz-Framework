CREATE OR REPLACE FUNCTION split_text_array(IN rec TEXT[], IN delim VARCHAR ) RETURNS setof FLOAT AS $$
DECLARE
    r TEXT[];
BEGIN
    IF rec <> '{}' THEN
        FOR j IN array_lower(rec, 1) .. array_upper(rec, 1)
        LOOP
            r = string_to_array(rec[j], delim);
            FOR i IN array_lower(r, 1) .. array_upper(r, 1)
            LOOP
                IF r[i] <> '' THEN
                    RETURN NEXT r[i];
                END IF;
            END LOOP;
        END LOOP;
    END IF;
END
$$ LANGUAGE PLPGSQL STRICT IMMUTABLE;
