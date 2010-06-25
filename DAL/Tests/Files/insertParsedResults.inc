<?php
// Serialized results.
$results = array();

$results['parsed'] = array(
                      'a:1:{s:6:"INSERT";a:3:{s:4:"into";s:10:"link_table";s:6:"FIELDS";a:2:{i:0;s:6:"linkid";i:1;s:7:"childid";}s:6:"VALUES";a:2:{s:6:"linkid";s:1:"1";s:7:"childid";s:3:"234";}}}',
                      'a:1:{s:6:"INSERT";a:3:{s:4:"into";s:10:"link_table";s:6:"FIELDS";a:4:{i:0;s:6:"linkid";i:1;s:8:"parentid";i:2;s:7:"childid";i:3;s:9:"link_type";}s:6:"VALUES";a:3:{s:6:"SELECT";a:1:{s:6:"FIELDS";a:4:{i:0;a:2:{s:5:"table";s:10:"link_cache";s:6:"column";s:6:"linkid";}i:1;a:2:{s:5:"table";s:10:"link_cache";s:6:"column";s:8:"parentid";}i:2;a:2:{s:5:"table";s:10:"link_cache";s:6:"column";s:7:"childid";}i:3;s:1:"1";}}s:4:"FROM";a:1:{i:0;s:10:"link_cache";}s:5:"WHERE";a:1:{s:3:"AND";a:1:{i:0;a:1:{s:2:"IN";a:3:{s:5:"table";s:10:"link_cache";s:6:"column";s:6:"linkid";s:10:"CONDITIONS";a:3:{s:6:"SELECT";a:1:{s:6:"FIELDS";a:1:{i:0;a:2:{s:5:"table";s:9:"link_tree";s:6:"column";s:6:"linkid";}}}s:4:"FROM";a:1:{i:0;s:10:"link_cache";}s:5:"WHERE";a:1:{s:3:"AND";a:1:{i:0;a:3:{s:7:"compare";a:2:{s:5:"table";s:9:"link_tree";s:6:"column";s:6:"treeid";}s:4:"type";s:4:"like";s:2:"to";s:4:"000%";}}}}}}}}}}}',
                     );

// For each database type and query there sould be a converted SQL string (base64 encoded).
$results['converted']['Mysql'] = array(
                                  'SU5TRVJUIElOVE8gbGlua190YWJsZShsaW5raWQsIGNoaWxkaWQpIApWQUxVRVMgKDEsIDIzNCkg',
                                  'SU5TRVJUIElOVE8gbGlua190YWJsZShsaW5raWQsIHBhcmVudGlkLCBjaGlsZGlkLCBsaW5rX3R5cGUpIAooU0VMRUNUCiAgICBsaW5rX2NhY2hlLmxpbmtpZCwgbGlua19jYWNoZS5wYXJlbnRpZCwgbGlua19jYWNoZS5jaGlsZGlkLCAxCkZST00KICAgIGxpbmtfY2FjaGUKV0hFUkUKKAogICAgbGlua19jYWNoZS5saW5raWQgSU4oCiAgICAgICAgU0VMRUNUCiAgICAgICAgICAgIGxpbmtfdHJlZS5saW5raWQKICAgICAgICBGUk9NCiAgICAgICAgICAgIGxpbmtfY2FjaGUKICAgICAgICBXSEVSRQogICAgICAgICgKICAgICAgICAgICAgbGlua190cmVlLnRyZWVpZCBMSUtFIDAwMCUKICAgICAgICApCiAgICApCikKKQ==',
                                 );

$results['converted']['Postgres'] = array(
                                     'SU5TRVJUIElOVE8gbGlua190YWJsZShsaW5raWQsIGNoaWxkaWQpIApWQUxVRVMgKDEsIDIzNCkg',
                                     'SU5TRVJUIElOVE8gbGlua190YWJsZShsaW5raWQsIHBhcmVudGlkLCBjaGlsZGlkLCBsaW5rX3R5cGUpIAooU0VMRUNUCiAgICBsaW5rX2NhY2hlLmxpbmtpZCwgbGlua19jYWNoZS5wYXJlbnRpZCwgbGlua19jYWNoZS5jaGlsZGlkLCAxCkZST00KICAgIGxpbmtfY2FjaGUKV0hFUkUKKAogICAgbGlua19jYWNoZS5saW5raWQgSU4oCiAgICAgICAgU0VMRUNUCiAgICAgICAgICAgIGxpbmtfdHJlZS5saW5raWQKICAgICAgICBGUk9NCiAgICAgICAgICAgIGxpbmtfY2FjaGUKICAgICAgICBXSEVSRQogICAgICAgICgKICAgICAgICAgICAgbGlua190cmVlLnRyZWVpZCBMSUtFIDAwMCUKICAgICAgICApCiAgICApCikKKQ==',
                                    );

$results['converted']['Oracle'] = array(
                                   'SU5TRVJUIElOVE8gbGlua190YWJsZShsaW5raWQsIGNoaWxkaWQpIApWQUxVRVMgKDEsIDIzNCkg',
                                   'SU5TRVJUIElOVE8gbGlua190YWJsZShsaW5raWQsIHBhcmVudGlkLCBjaGlsZGlkLCBsaW5rX3R5cGUpIAooU0VMRUNUCiAgICBsaW5rX2NhY2hlLmxpbmtpZCwgbGlua19jYWNoZS5wYXJlbnRpZCwgbGlua19jYWNoZS5jaGlsZGlkLCAxCkZST00KICAgIGxpbmtfY2FjaGUKV0hFUkUKKAogICAgbGlua19jYWNoZS5saW5raWQgSU4oCiAgICAgICAgU0VMRUNUCiAgICAgICAgICAgIGxpbmtfdHJlZS5saW5raWQKICAgICAgICBGUk9NCiAgICAgICAgICAgIGxpbmtfY2FjaGUKICAgICAgICBXSEVSRQogICAgICAgICgKICAgICAgICAgICAgbGlua190cmVlLnRyZWVpZCBMSUtFIDAwMCUKICAgICAgICApCiAgICApCikKKQ==',
                                  );

$results['converted']['Db2'] = array(
                                'SU5TRVJUIElOVE8gbGlua190YWJsZShsaW5raWQsIGNoaWxkaWQpIApWQUxVRVMgKDEsIDIzNCkg',
                                'SU5TRVJUIElOVE8gbGlua190YWJsZShsaW5raWQsIHBhcmVudGlkLCBjaGlsZGlkLCBsaW5rX3R5cGUpIAooU0VMRUNUCiAgICBsaW5rX2NhY2hlLmxpbmtpZCwgbGlua19jYWNoZS5wYXJlbnRpZCwgbGlua19jYWNoZS5jaGlsZGlkLCAxCkZST00KICAgIGxpbmtfY2FjaGUKV0hFUkUKKAogICAgbGlua19jYWNoZS5saW5raWQgSU4oCiAgICAgICAgU0VMRUNUCiAgICAgICAgICAgIGxpbmtfdHJlZS5saW5raWQKICAgICAgICBGUk9NCiAgICAgICAgICAgIGxpbmtfY2FjaGUKICAgICAgICBXSEVSRQogICAgICAgICgKICAgICAgICAgICAgTE9DQVRFKDAwMCwgbGlua190cmVlLnRyZWVpZCkgPD4gMAogICAgICAgICkKICAgICkKKQop',
                               );

$results['converted']['Mssql'] = array(
                                  'SU5TRVJUIElOVE8gbGlua190YWJsZShsaW5raWQsIGNoaWxkaWQpIApWQUxVRVMgKDEsIDIzNCkg',
                                  'SU5TRVJUIElOVE8gbGlua190YWJsZShsaW5raWQsIHBhcmVudGlkLCBjaGlsZGlkLCBsaW5rX3R5cGUpIApTRUxFQ1QKICAgIGxpbmtfY2FjaGUubGlua2lkLCBsaW5rX2NhY2hlLnBhcmVudGlkLCBsaW5rX2NhY2hlLmNoaWxkaWQsIDEKRlJPTQogICAgbGlua19jYWNoZQpXSEVSRQooCiAgICBsaW5rX2NhY2hlLmxpbmtpZCBJTigKICAgICAgICBTRUxFQ1QKICAgICAgICAgICAgbGlua190cmVlLmxpbmtpZAogICAgICAgIEZST00KICAgICAgICAgICAgbGlua19jYWNoZQogICAgICAgIFdIRVJFCiAgICAgICAgKAogICAgICAgICAgICBsaW5rX3RyZWUudHJlZWlkIExJS0UgMDAwJQogICAgICAgICkKICAgICkKKQo=',
                                 );

?>
