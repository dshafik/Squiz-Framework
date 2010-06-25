<?php
// Serialized results.
$results = array();

$results['parsed'] = array(
                      'a:1:{s:6:"DELETE";a:2:{s:4:"from";s:10:"link_table";s:5:"WHERE";a:1:{s:3:"AND";a:2:{i:0;a:3:{s:7:"compare";a:2:{s:5:"table";s:10:"link_table";s:6:"column";s:6:"linkid";}s:4:"type";s:5:"equal";s:2:"to";s:1:"1";}i:1;a:1:{s:2:"IN";a:3:{s:5:"table";s:10:"link_table";s:6:"column";s:8:"parentid";s:10:"CONDITIONS";a:3:{s:6:"SELECT";a:1:{s:6:"FIELDS";a:1:{i:0;a:2:{s:5:"table";s:10:"link_cache";s:6:"column";s:8:"parentid";}}}s:4:"FROM";a:1:{i:0;s:10:"link_table";}s:5:"WHERE";a:1:{s:3:"AND";a:1:{i:0;a:3:{s:7:"compare";a:2:{s:5:"table";s:10:"link_cache";s:6:"column";s:7:"childid";}s:4:"type";s:5:"equal";s:2:"to";s:3:"234";}}}}}}}}}}',
                     );

// For each database type and query there sould be a converted SQL string (base64 encoded).
$results['converted']['Mysql'] = array(
                                  'REVMRVRFIEZST00gbGlua190YWJsZQpXSEVSRSgKICAgIGxpbmtfdGFibGUubGlua2lkID0gMQogICAgQU5EIGxpbmtfdGFibGUucGFyZW50aWQgSU4oCiAgICAgICAgU0VMRUNUCiAgICAgICAgICAgIGxpbmtfY2FjaGUucGFyZW50aWQKICAgICAgICBGUk9NCiAgICAgICAgICAgIGxpbmtfdGFibGUKICAgICAgICBXSEVSRQogICAgICAgICgKICAgICAgICAgICAgbGlua19jYWNoZS5jaGlsZGlkID0gMjM0CiAgICAgICAgKQogICAgKQopCg==',
                                 );

$results['converted']['Postgres'] = array(
                                     'REVMRVRFIEZST00gbGlua190YWJsZQpXSEVSRSgKICAgIGxpbmtfdGFibGUubGlua2lkID0gMQogICAgQU5EIGxpbmtfdGFibGUucGFyZW50aWQgSU4oCiAgICAgICAgU0VMRUNUCiAgICAgICAgICAgIGxpbmtfY2FjaGUucGFyZW50aWQKICAgICAgICBGUk9NCiAgICAgICAgICAgIGxpbmtfdGFibGUKICAgICAgICBXSEVSRQogICAgICAgICgKICAgICAgICAgICAgbGlua19jYWNoZS5jaGlsZGlkID0gMjM0CiAgICAgICAgKQogICAgKQopCg==',
                                    );

$results['converted']['Oracle'] = array(
                                   'REVMRVRFIEZST00gbGlua190YWJsZQpXSEVSRSgKICAgIGxpbmtfdGFibGUubGlua2lkID0gMQogICAgQU5EIGxpbmtfdGFibGUucGFyZW50aWQgSU4oCiAgICAgICAgU0VMRUNUCiAgICAgICAgICAgIGxpbmtfY2FjaGUucGFyZW50aWQKICAgICAgICBGUk9NCiAgICAgICAgICAgIGxpbmtfdGFibGUKICAgICAgICBXSEVSRQogICAgICAgICgKICAgICAgICAgICAgbGlua19jYWNoZS5jaGlsZGlkID0gMjM0CiAgICAgICAgKQogICAgKQopCg==',
                                  );

$results['converted']['Db2'] = array(
                                'REVMRVRFIEZST00gbGlua190YWJsZQpXSEVSRSgKICAgIGxpbmtfdGFibGUubGlua2lkID0gMQogICAgQU5EIGxpbmtfdGFibGUucGFyZW50aWQgSU4oCiAgICAgICAgU0VMRUNUCiAgICAgICAgICAgIGxpbmtfY2FjaGUucGFyZW50aWQKICAgICAgICBGUk9NCiAgICAgICAgICAgIGxpbmtfdGFibGUKICAgICAgICBXSEVSRQogICAgICAgICgKICAgICAgICAgICAgbGlua19jYWNoZS5jaGlsZGlkID0gMjM0CiAgICAgICAgKQogICAgKQopCg==',
                               );

$results['converted']['Mssql'] = array(
                                  'REVMRVRFIEZST00gbGlua190YWJsZQpXSEVSRSgKICAgIGxpbmtfdGFibGUubGlua2lkID0gMQogICAgQU5EIGxpbmtfdGFibGUucGFyZW50aWQgSU4oCiAgICAgICAgU0VMRUNUCiAgICAgICAgICAgIGxpbmtfY2FjaGUucGFyZW50aWQKICAgICAgICBGUk9NCiAgICAgICAgICAgIGxpbmtfdGFibGUKICAgICAgICBXSEVSRQogICAgICAgICgKICAgICAgICAgICAgbGlua19jYWNoZS5jaGlsZGlkID0gMjM0CiAgICAgICAgKQogICAgKQopCg==',
                                 );

?>
