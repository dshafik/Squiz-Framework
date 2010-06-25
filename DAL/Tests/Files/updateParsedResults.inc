<?php
// Serialized results.
$results = array();

$results['parsed'] = array(
                      'a:1:{s:6:"UPDATE";a:4:{s:5:"table";s:10:"link_table";s:6:"FIELDS";a:2:{i:0;s:6:"linkid";i:1;s:8:"parentid";}s:6:"VALUES";a:2:{s:8:"parentid";s:3:"123";s:7:"childid";s:3:"234";}s:5:"WHERE";a:1:{s:3:"AND";a:2:{i:0;a:3:{s:7:"compare";a:2:{s:5:"table";s:10:"link_table";s:6:"column";s:6:"linkid";}s:4:"type";s:5:"equal";s:2:"to";s:1:"1";}i:1;a:1:{s:2:"IN";a:3:{s:5:"table";s:8:"my_table";s:6:"column";s:6:"my_col";s:10:"CONDITIONS";a:3:{s:6:"SELECT";a:1:{s:6:"FIELDS";a:1:{i:0;a:2:{s:5:"table";s:6:"table2";s:6:"column";s:4:"col2";}}}s:4:"FROM";a:1:{i:0;s:10:"link_table";}s:5:"WHERE";a:1:{s:3:"AND";a:2:{i:0;a:3:{s:7:"compare";a:2:{s:5:"table";s:6:"table2";s:6:"column";s:4:"col2";}s:4:"type";s:5:"equal";s:2:"to";s:5:"value";}i:1;a:1:{s:2:"IN";a:3:{s:5:"table";s:11:"asset_table";s:6:"column";s:9:"type_code";s:10:"CONDITIONS";a:2:{i:0;s:13:"page_standard";i:1;s:6:"folder";}}}}}}}}}}}}',
                     );

// For each database type and query there sould be a converted SQL string (base64 encoded).
$results['converted']['Mysql'] = array(
                                  'VVBEQVRFIGxpbmtfdGFibGUKU0VUCiAgICBwYXJlbnRpZCA9IDEyMywKICAgIGNoaWxkaWQgPSAyMzQKV0hFUkUgKAogICAgbGlua190YWJsZS5saW5raWQgPSAxCiAgICBBTkQgbXlfdGFibGUubXlfY29sIElOKAogICAgICAgIFNFTEVDVAogICAgICAgICAgICB0YWJsZTIuY29sMgogICAgICAgIEZST00KICAgICAgICAgICAgbGlua190YWJsZQogICAgICAgIFdIRVJFCiAgICAgICAgKAogICAgICAgICAgICB0YWJsZTIuY29sMiA9IHZhbHVlCiAgICAgICAgICAgIEFORCBhc3NldF90YWJsZS50eXBlX2NvZGUgSU4ocGFnZV9zdGFuZGFyZCwgZm9sZGVyKQogICAgICAgICkKICAgICkKKQo=',
                                 );

$results['converted']['Postgres'] = array(
                                     'VVBEQVRFIGxpbmtfdGFibGUKU0VUCiAgICBwYXJlbnRpZCA9IDEyMywKICAgIGNoaWxkaWQgPSAyMzQKV0hFUkUgKAogICAgbGlua190YWJsZS5saW5raWQgPSAxCiAgICBBTkQgbXlfdGFibGUubXlfY29sIElOKAogICAgICAgIFNFTEVDVAogICAgICAgICAgICB0YWJsZTIuY29sMgogICAgICAgIEZST00KICAgICAgICAgICAgbGlua190YWJsZQogICAgICAgIFdIRVJFCiAgICAgICAgKAogICAgICAgICAgICB0YWJsZTIuY29sMiA9IHZhbHVlCiAgICAgICAgICAgIEFORCBhc3NldF90YWJsZS50eXBlX2NvZGUgSU4ocGFnZV9zdGFuZGFyZCwgZm9sZGVyKQogICAgICAgICkKICAgICkKKQo=',
                                    );

$results['converted']['Oracle'] = array(
                                   'VVBEQVRFIGxpbmtfdGFibGUKU0VUCiAgICBwYXJlbnRpZCA9IDEyMywKICAgIGNoaWxkaWQgPSAyMzQKV0hFUkUgKAogICAgbGlua190YWJsZS5saW5raWQgPSAxCiAgICBBTkQgbXlfdGFibGUubXlfY29sIElOKAogICAgICAgIFNFTEVDVAogICAgICAgICAgICB0YWJsZTIuY29sMgogICAgICAgIEZST00KICAgICAgICAgICAgbGlua190YWJsZQogICAgICAgIFdIRVJFCiAgICAgICAgKAogICAgICAgICAgICB0YWJsZTIuY29sMiA9IHZhbHVlCiAgICAgICAgICAgIEFORCBhc3NldF90YWJsZS50eXBlX2NvZGUgSU4ocGFnZV9zdGFuZGFyZCwgZm9sZGVyKQogICAgICAgICkKICAgICkKKQo=',
                                  );

$results['converted']['Db2'] = array(
                                'VVBEQVRFIGxpbmtfdGFibGUKU0VUCiAgICBwYXJlbnRpZCA9IDEyMywKICAgIGNoaWxkaWQgPSAyMzQKV0hFUkUgKAogICAgbGlua190YWJsZS5saW5raWQgPSAxCiAgICBBTkQgbXlfdGFibGUubXlfY29sIElOKAogICAgICAgIFNFTEVDVAogICAgICAgICAgICB0YWJsZTIuY29sMgogICAgICAgIEZST00KICAgICAgICAgICAgbGlua190YWJsZQogICAgICAgIFdIRVJFCiAgICAgICAgKAogICAgICAgICAgICB0YWJsZTIuY29sMiA9IHZhbHVlCiAgICAgICAgICAgIEFORCBhc3NldF90YWJsZS50eXBlX2NvZGUgSU4ocGFnZV9zdGFuZGFyZCwgZm9sZGVyKQogICAgICAgICkKICAgICkKKQo=',
                               );

$results['converted']['Mssql'] = array(
                                  'VVBEQVRFIGxpbmtfdGFibGUKU0VUCiAgICBwYXJlbnRpZCA9IDEyMywKICAgIGNoaWxkaWQgPSAyMzQKV0hFUkUgKAogICAgbGlua190YWJsZS5saW5raWQgPSAxCiAgICBBTkQgbXlfdGFibGUubXlfY29sIElOKAogICAgICAgIFNFTEVDVAogICAgICAgICAgICB0YWJsZTIuY29sMgogICAgICAgIEZST00KICAgICAgICAgICAgbGlua190YWJsZQogICAgICAgIFdIRVJFCiAgICAgICAgKAogICAgICAgICAgICB0YWJsZTIuY29sMiA9IHZhbHVlCiAgICAgICAgICAgIEFORCBhc3NldF90YWJsZS50eXBlX2NvZGUgSU4ocGFnZV9zdGFuZGFyZCwgZm9sZGVyKQogICAgICAgICkKICAgICkKKQo=',
                                 );

?>
