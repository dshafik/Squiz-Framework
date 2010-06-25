DROP FUNCTION IF EXISTS get_parent_info(TEXT[]);

DROP TYPE IF EXISTS parent_info;

CREATE TYPE parent_info AS (path VARCHAR, parentid DOUBLE PRECISION, depth INTEGER);

CREATE OR REPLACE FUNCTION get_parent_info(IN rec TEXT[]) RETURNS setof parent_info AS $$
DECLARE
   r TEXT[];
   pinfo parent_info;
   depth integer := 1;
BEGIN
   IF array_lower(rec, 1) IS NULL THEN
       RETURN;
   END IF;
   FOR j IN array_lower(rec, 1) .. array_upper(rec, 1)
   LOOP
       r = string_to_array(rec[j], '/');
       FOR i IN array_lower(r, 1) .. array_upper(r, 1)
       LOOP
           IF r[i] <> '' THEN
               pinfo.depth := depth;
               pinfo.path  := rec[j];
               pinfo.parentid := r[i];
               RETURN NEXT pinfo;
               depth := depth + 1;
           END IF;
       END LOOP;
       depth := 1;
   END LOOP;
END
$$ LANGUAGE PLPGSQL STRICT IMMUTABLE;
