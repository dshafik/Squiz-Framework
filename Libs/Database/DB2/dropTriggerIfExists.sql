DROP PROCEDURE dropTriggerIfExists
--$$
CREATE PROCEDURE dropTriggerIfExists(IN trigName VARCHAR(18))
BEGIN
    DECLARE trigCount INTEGER default 0;
    DECLARE stmt VARCHAR(50);

    SELECT count(*) INTO trigCount
    FROM syscat.triggers t
    WHERE t.trigname = trigName;

    IF trigCount <> 0 THEN
        SET stmt = 'DROP TRIGGER ' || trigName;
        EXECUTE IMMEDIATE stmt;
    END IF;
END

