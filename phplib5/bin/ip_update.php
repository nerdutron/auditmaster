#!/usr/local/bin/php -C
<?php
######################################################################
##                                                                  ##
## 1. Download from: http://www.hostip.info/dl/index.html           ##
##                                                                  ##
## 2. Extract and load in to WEB database                           ##
##                                                                  ##
## 3. CD to /src/dev/phplib5/bin/                                   ##
##                                                                  ##
## 4. run "php ip_update.php"                                       ##
##                                                                  ##
######################################################################

$Parts = explode("/", $_SERVER["PWD"]);
array_pop($Parts);
define("PHPLIB_DIR", implode("/", $Parts)."/");

echo "\n\n";
set_time_limit(0);
include('definitions.inc');
require_once(PHPLIB_DIR."database.inc");

$Q = new DBQuery;
$T = new DBQuery;

$Query = "DROP TABLE IF EXISTS cityByCountry";
DB_DoQuery(WEB_DB, $Query);

if (DBGet(WEB_DB, "SELECT COUNT(*) FROM countries"))
{
	$Query = "TRUNCATE ip_countries";
	DB_DoQuery(WEB_DB, $Query);
	$Query = "INSERT INTO ip_countries SELECT * FROM countries";
	DB_DoQuery(WEB_DB, $Query);
	$Query = "DROP TABLE IF EXISTS countries";
	DB_DoQuery(WEB_DB, $Query);
	$Query = "TRUNCATE ip_list";
	DB_DoQuery(WEB_DB, $Query);

	$T->Run(WEB_DB, "show tables");
	while ($T->Next())
	{
		$Tables = $T->GetArray();
		$Table = array_shift($Tables);
		if (substr($Table, 0, 4) == "ip4_")
		{
			echo "\nProcessing ".$Table."...";
			$Insert = array();
			$A = array_pop(explode("_", $Table));
			$Q->Run(WEB_DB, "SELECT b,c,country FROM ".$Table);
			while ($Q->Next())
			{
				$B = $Q->GetField("b");
				$C = $Q->GetField("c");
				$Row  = "";
				$Row .= "(";
				$Row .= (16777216) * $A + (65536 * $B) + (256 * $C);
				$Row .= ",";
				$Row .= $Q->GetField("country");
				$Row .= ")";
				$Insert[] = $Row;
				if (sizeof($Insert) >= 500)
				{
					$Query = "REPLACE INTO ip_list VALUES\n ".implode(",\n", $Insert);
					DB_DoQuery(WEB_DB, $Query);
					$Insert = array();
				}
			}
			if (sizeof($Insert))
			{
				$Query = "REPLACE INTO ip_list VALUES\n ".implode(",\n", $Insert);
				DB_DoQuery(WEB_DB, $Query);
			}
			
			$Query = "DROP TABLE IF EXISTS ".$Table;
			DB_DoQuery(WEB_DB, $Query);
			$Q->EndQuery();
		}
	}
	$T->EndQuery();
}

echo "\n\n";

######################################################################
##                                                                  ##
## 1. Download from: http://www.hostip.info/dl/index.html           ##
##                                                                  ##
## 2. Extract and load in to WEB database                           ##
##                                                                  ##
## 3. CD to /src/dev/phplib5/bin/                                   ##
##                                                                  ##
## 4. run "php ip_update.php"                                       ##
##                                                                  ##
######################################################################
?>
