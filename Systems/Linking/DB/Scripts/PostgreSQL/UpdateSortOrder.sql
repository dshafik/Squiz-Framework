CREATE OR REPLACE FUNCTION updateSortOrder(parentid FLOAT, childid FLOAT, sortOrder FLOAT, upType TEXT) RETURNS BOOLEAN AS $$
DECLARE
    oldSortOrder   FLOAT;
    newSortOrder   FLOAT;
    sortOrderCount INT;
    context        TEXT;
BEGIN
    newSortOrder := sortOrder;

    IF upType = 'update' THEN
        -- IF a context is updating sort_orders then it needs to copy its parent
        -- context's sort_orders so it can override them.
        context := getContext(parentid);
        IF context <> '00000000' THEN
            PERFORM copyParentSortOrders(parentid, childid);
            newSortOrder := CAST(getContextAssetid(getMasterAssetid(newSortOrder), context) AS DOUBLE PRECISION);
        END IF;
    END IF;

    SELECT INTO oldSortOrder a.sort_order
    FROM asset_link AS a
    WHERE a.childid = childid AND a.parentid = parentid;

    IF newSortOrder <= 0 THEN
        -- Link needs to move to the end.
        SELECT INTO newSortOrder MAX(sort_order)
        FROM asset_link
        WHERE asset_link.parentid = parentid;
    END IF;

    -- First update the sort order of the link.
    IF oldSortOrder <> newSortOrder THEN
        UPDATE asset_link
        SET sort_order = newSortOrder
        WHERE asset_link.childid = childid AND asset_link.parentid = parentid;
    END IF;

    IF floor(oldSortOrder) = floor(newSortOrder) THEN
        -- Sort order is the same so just update everything after it.
        -- This might be the case when a new link is being inserted at position x.
        sortOrderCount := 0;
        SELECT INTO sortOrderCount count(sort_order)
        FROM asset_link
        WHERE asset_link.parentid = parentid
        AND asset_link.sort_order = newSortOrder
        AND asset_link.childid <> childid
        LIMIT 1;

        IF sortOrderCount >= 1 THEN
            UPDATE asset_link
            SET sort_order = (sort_order + 1)
            WHERE asset_link.parentid = parentid
            AND asset_link.sort_order >= newSortOrder
            AND asset_link.childid <> childid;
        END IF;

        RETURN TRUE;
    END IF;

    -- Now update the rest of the sort orders under this parent.
    IF oldSortOrder > newSortOrder THEN
        -- Link sort order is moving DOWN.
        UPDATE asset_link
        SET sort_order = (sort_order + 1)
        WHERE asset_link.parentid = parentid
        AND asset_link.sort_order < oldSortOrder
        AND asset_link.sort_order >= newSortOrder
        AND asset_link.childid <> childid;
    ELSE
        -- Link sort order is moving UP.
        UPDATE asset_link
        SET sort_order = (sort_order - 1)
        WHERE asset_link.parentid = parentid
        AND asset_link.sort_order > oldSortOrder
        AND asset_link.sort_order <= newSortOrder
        AND asset_link.childid <> childid;
    END IF;

    RETURN TRUE;

END
$$ LANGUAGE plpgsql STRICT VOLATILE;

-- This function is called when sort order is being updated for a context.
-- It copies the parent contexts child sort orders so that it can override them.
-- Orders are copies so that when the parent context order is changed the child
-- context is not affected.
CREATE OR REPLACE FUNCTION copyParentSortOrders(assetid FLOAT, childid FLOAT) RETURNS VOID AS $$
DECLARE
    context TEXT;
    pid FLOAT;
    rec RECORD;
    newSortOrder INT;
BEGIN
    INSERT INTO asset_link (
        SELECT DISTINCT ON (a.childid) assetid, a.childid, a.sort_order, a.show_menu, a.active, a.userid, a.last_updated
        FROM asset_link AS a
        WHERE a.parentid IN (SELECT * FROM getContextAssetids(assetid))
        -- Prevent dupe key errors..
        AND 0 = (SELECT count(*) FROM asset_link a2 WHERE a2.parentid = assetid AND a2.childid = a.childid)
        ORDER BY a.childid, a.parentid DESC
    );

    -- Reset sort orders to prevent dupes..
    context := getContext(assetid);
    newSortOrder := 1;
    FOR rec IN
        SELECT a.childid
        FROM asset_link AS a WHERE a.parentid = assetid ORDER BY a.sort_order ASC
    LOOP
        UPDATE asset_link
        SET sort_order = CAST(getContextAssetid(newSortOrder, context) AS DOUBLE PRECISION)
        WHERE asset_link.childid = rec.childid
            AND asset_link.parentid = assetid;

        newSortOrder := newSortOrder + 1;
    END LOOP;

    RETURN;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;
