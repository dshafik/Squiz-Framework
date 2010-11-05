-------------------
-- PATCH_10231_X --
-------------------

BEGIN;
ALTER TABLE publishing ADD COLUMN scheduled_datetime TIMESTAMP WITH TIME ZONE DEFAULT NULL;

TRUNCATE cache;
ALTER TABLE cache DROP CONSTRAINT cache_pk RESTRICT;
ALTER TABLE cache DROP COLUMN url;
ALTER TABLE cache ADD COLUMN url VARCHAR(2000);
ALTER TABLE cache ALTER COLUMN url SET NOT NULL;
CREATE INDEX cache_idx1 ON cache(url);
ALTER TABLE cache DROP COLUMN content;
ALTER TABLE cache ADD COLUMN content TEXT;
ALTER TABLE cache ALTER COLUMN content SET NOT NULL;
ALTER TABLE cache ALTER COLUMN expiry SET NOT NULL;
ALTER TABLE cache ALTER COLUMN time_taken SET NOT NULL;
ALTER TABLE cache ADD COLUMN owner VARCHAR(8);
ALTER TABLE cache ALTER COLUMN owner SET NOT NULL;
ALTER TABLE cache ADD CONSTRAINT cache_pk PRIMARY KEY (assetid, owner, url);
CREATE INDEX cache_idx3 ON cache(owner);
ALTER TABLE cache_stats DROP COLUMN size;
ALTER TABLE cache_stats DROP COLUMN keyword;
ALTER TABLE cache_stats ADD COLUMN owner VARCHAR(8);
UPDATE cache_stats SET owner = '00000000';
ALTER TABLE cache_stats ALTER COLUMN owner SET NOT NULL;
ALTER TABLE cache_stats DROP CONSTRAINT cache_stats_pk RESTRICT;
ALTER TABLE cache_stats ADD CONSTRAINT cache_stats_pk PRIMARY KEY (date, url, assetid, owner);
CREATE INDEX cache_stats_idx3 ON cache_stats(owner);
ALTER TABLE cache_stats_daily DROP COLUMN size;
ALTER TABLE cache_stats_daily DROP COLUMN keyword;

COMMIT;
