CREATE OR REPLACE FUNCTION syncLinkTreeChildren(pid FLOAT, linkOwner TEXT, linkActive BOOLEAN) RETURNS VOID AS $$
DECLARE
    rec RECORD;
    c INT;
BEGIN
    FOR rec IN
        SELECT childid, p.path, p.depth
        FROM asset_link, (SELECT (path || pid || '/') as path, (depth + 1) as depth
                          FROM asset_link_tree WHERE assetid = pid
                         ) AS p
        WHERE parentid = pid
    LOOP
        -- If link exists then do nothing.
        SELECT INTO c count(*)
        FROM asset_link_tree
        WHERE assetid = rec.childid AND path = rec.path;

        IF c = 0 THEN
            INSERT INTO asset_link_tree
            VALUES (rec.childid, rec.path, rec.depth, linkActive, linkOwner);

            PERFORM syncLinkTreeChildren(rec.childid, linkOwner, linkActive);
        END IF;

    END LOOP;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;
