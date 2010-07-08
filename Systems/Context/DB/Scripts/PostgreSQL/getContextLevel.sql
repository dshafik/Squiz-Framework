CREATE OR REPLACE FUNCTION getContextLevel(assetid FLOAT) RETURNS INT AS $$
DECLARE
    lvl     INT;
    length  INT;
    context TEXT;
BEGIN
    context := getContext(assetid);

    IF context = '00000000' THEN
        return 0;
    END IF;

    lvl    := 0;
    length := char_length(context);
    IF length = 0 THEN
        lvl := 0;
    ELSEIF length <= 2 THEN
        lvl := 4;
    ELSEIF length <= 4 THEN
        lvl := 3;
    ELSEIF length <= 6 THEN
        lvl := 2;
    ELSEIF length <= 8 THEN
        lvl := 1;
    END IF;

    RETURN lvl;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;


CREATE OR REPLACE FUNCTION getContextRange(masterid FLOAT, contextLvl anyelement) RETURNS anyarray AS $$
DECLARE
    range FLOAT[];
BEGIN

    IF contextLvl = 0 THEN
        range[1] = masterid;
        range[2] = masterid;
    ELSEIF contextLvl = 1 THEN
        range[1] = masterid + 0.00000001;
        range[2] = masterid + 0.00000099;
    ELSEIF contextLvl = 2 THEN
        range[1] = masterid + 0.000001;
        range[2] = masterid + 0.000099;
    ELSEIF contextLvl = 3 THEN
        range[1] = masterid + 0.0001;
        range[2] = masterid + 0.0099;
    ELSEIF contextLvl = 4 THEN
        range[1] = masterid + 0.01;
        range[2] = masterid + 0.99;
    END IF;

    RETURN range;
END
$$ LANGUAGE plpgsql STRICT VOLATILE;

