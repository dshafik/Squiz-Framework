CREATE OR REPLACE FUNCTION get_parent_assetid(path VARCHAR) RETURNS FLOAT AS $$
DECLARE
    paths VARCHAR[];
BEGIN
    paths = string_to_array(path, '/');
    IF paths[array_upper(paths, 1) - 1] = '' THEN
        RETURN 0;
    END IF;

    RETURN paths[array_upper(paths, 1) - 1];
END
$$ language PLPGSQL STRICT VOLATILE;
