CREATE OR REPLACE FUNCTION array_to_set(IN r ANYARRAY) RETURNS setof VARCHAR AS $$
DECLARE
BEGIN
   FOR i IN array_lower(r, 1) .. array_upper(r, 1)
   LOOP
        RETURN NEXT r[i];
   END LOOP;

END;
$$ language plpgsql STRICT immutable;
