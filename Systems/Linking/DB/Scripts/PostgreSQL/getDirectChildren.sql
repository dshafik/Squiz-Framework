CREATE OR REPLACE FUNCTION getDirectChildren(parentid TEXT, OUT childid DOUBLE PRECISION, OUT sort_order DOUBLE PRECISION) RETURNS SETOF record AS $$
DECLARE
    rec RECORD;
    childids TEXT;
BEGIN
    childids = '|';
    FOR rec IN SELECT a.childid, a.sort_order, a.active
               FROM asset_link AS a
               WHERE a.parentid IN (SELECT * FROM getContextAssetids(parentid))
               ORDER BY a.parentid DESC
    LOOP
        -- Check if we processed this childid.
        IF position('|' || rec.childid || '|' IN childids) = 0 THEN
            IF rec.active = true THEN
                childid    := rec.childid;
                sort_order := rec.sort_order;

                -- If the link is active then return it.
                RETURN NEXT;
            END IF;

            childids = childids || rec.childid || '|';
        END IF;
    END LOOP;

    RETURN;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;


CREATE OR REPLACE FUNCTION getDirectChildren(parentid DOUBLE PRECISION, OUT childid DOUBLE PRECISION, OUT sort_order DOUBLE PRECISION) RETURNS SETOF record AS $$
DECLARE
    rec RECORD;
    childids TEXT;
BEGIN
    childids = '|';
    FOR rec IN SELECT a.childid, a.sort_order, a.active
               FROM asset_link AS a
               WHERE a.parentid IN (SELECT * FROM getContextAssetids(parentid))
               ORDER BY a.parentid DESC
    LOOP
        -- Check if we processed this childid.
        IF position('|' || rec.childid || '|' IN childids) = 0 THEN
            IF rec.active = true THEN
                childid    := rec.childid;
                sort_order := rec.sort_order;

                -- If the link is active then return it.
                RETURN NEXT;
            END IF;

            childids = childids || rec.childid || '|';
        END IF;
    END LOOP;

    RETURN;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;
