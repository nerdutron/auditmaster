<?php
// zip.inc
//
// Creates genuine ZIP files...
//
// Copyright MU Systems Pty Ltd 2002
// Copyright Australian Business Research 2002
//
// Author: Murray Shields	Written: 28/08/2002
//
// MODIFICATION HISTORY
// ====================
//

class ZIP
{
	function __construct()
	{
		$this->TMP = "/tmp/";
		$this->Reset();
	}

	function Dir($Dir)
	{
		$this->TMP = $Dir;
	}

	function Reset()
	{
		$this->Data = array();
		$this->File = 0;
	}

	function AddFile($Data, $Name)
	{
		$this->Data[$this->File]["data"] = $Data;
		$this->Data[$this->File]["name"] = $Name;
		$this->File++;
	}

	function Get()
	{
		$Key = $this->CreateKey();
		$Zip = $this->TMP."zip_".$Key.".zip";
		$Dir = $this->TMP."dir_".$Key."/";
		mkdir($Dir);

		for ($i=0;$i<$this->File;$i++)
		{
			$SaveFile = fopen($Dir.$this->Data[$i]["name"],"w");
			fputs($SaveFile, $this->Data[$i]["data"]);
			fclose($SaveFile);
		}

		exec ("zip -mj ".$Zip." ".$Dir."*");
		$ZIPData = file_get_contents($Zip);
		unlink($Zip);
		rmdir($Dir);

		return ($ZIPData);
	}

	function CreateKey()
	{
		$L = 8;
		$salt = "abchefghjkmnpqrstuvwxyz0123456789";
		srand((double)microtime()*1000000);
		$i = 0;
		while ($i <= ($L-1))
		{
			$num = rand() % 33;
			$tmp = substr($salt, $num, 1);
			$str = $str . $tmp;
			$i++;
		}
		return ($str);
	}
}

?>
