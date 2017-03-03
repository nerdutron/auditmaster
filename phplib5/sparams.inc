<?php
/*
Copyright: MU Systems Pty Ltd, 2009

You must define SPARAMS_DB in your config files which must contain the following table:

CREATE TABLE secure_parameters (
  hash VARCHAR( 8 ) NOT NULL,
  id INT( 11 ) NOT NULL,
  recorded DATETIME NOT NULL,
  parameters TEXT NOT NULL,
  PRIMARY KEY(hash,id)
) TYPE = MYISAM ;

*/

require_once(PHPLIB_DIR."database.inc");
require_once(PHPLIB_DIR."tools.inc");
require_once(PHPLIB_DIR."sql.inc");

class sparams
{
	function CreateHash()
	{
		$HASH = CreateUniqueKey(8);
		$Query = "SELECT * FROM secure_parameters WHERE hash = '".$HASH."'";
			if (DBGet(SPARAMS_DB, $Query))
				return(sparams::CreateHash());
		return($HASH);
	}

	function Put($Parameters, $UserID = 0)
	{
		$HASH = sparams::CreateHash();
		$INQ = new DBInsert;
		$INQ->Save("hash", $HASH);
		$INQ->Save("id", sql::integer($UserID));
		$INQ->Save("recorded", date("Y-m-d H:i:s"));
		$INQ->Save("parameters", serialize($Parameters));
		$INQ->Run(SPARAMS_DB, "secure_parameters", "INSERT");
		$INQ->EndQUery();
		return($HASH);
	}

	function Get($HASH, $UserID = 0)
	{
		$Query  = "";
		$Query .= "SELECT parameters ";
		$Query .= "FROM secure_parameters ";
		$Query .= "WHERE hash = '".sql::text($HASH)."' ";
		$Query .= "AND id = ".sql::integer($UserID)." ";
		$Parameters = DBGet(SPARAMS_DB, $Query);
		if (trim($Parameters))
			return(unserialize($Parameters));
	}

	function Clean()
	{
		$Query  = "";
		$Query .= "DELETE FROM secure_parameters ";
		$Query .= "WHERE recorded < '".date("Y-m-d H:i:s", strtotime("8 hours ago"))."' ";
		DB_DoQuery(SPARAMS_DB, $Query);
	}
}

?>
