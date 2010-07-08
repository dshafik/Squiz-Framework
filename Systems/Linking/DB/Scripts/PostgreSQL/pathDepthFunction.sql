--
-- Returns the depth of the specified path.
--
-- @param VARCHAR path The path to return the depth for.
--
-- @return INTEGER The depth of the path.
--
CREATE OR REPLACE FUNCTION pathDepth(path VARCHAR) RETURNS FLOAT AS $$
DECLARE
BEGIN
    RETURN (length(path) - length(replace(path, '/', '')) + 1);
END
$$ LANGUAGE plpgsql STRICT IMMUTABLE;
