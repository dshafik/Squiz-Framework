--
-- Drops specified trigger if exists.
--
-- @param VARCHAR triggerName Name of the trigger to drop.
-- @param VARCHAR tableName   Name of the table that trigger belongs to.
--
-- @return VOID
--

CREATE OR REPLACE FUNCTION dropTriggerIfExists(triggerName VARCHAR, tableName VARCHAR) RETURNS VOID AS $$
DECLARE
BEGIN

    PERFORM relname FROM pg_class INNER JOIN pg_trigger ON (tgrelid = relfilenode) WHERE tgname = LOWER(triggerName) AND relname=LOWER(tableName);

    IF FOUND THEN
        EXECUTE 'DROP TRIGGER ' || triggerName || ' ON ' || tableName;
    END IF;

END
$$ LANGUAGE PLPGSQL STRICT VOLATILE;
