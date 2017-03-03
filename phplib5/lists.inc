<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

class Lists
{
	function __construct($Define = "", $Default = "")
	{
		$this->Types = array();
		$this->Data = array();
		$this->Bullets = array();
		$this->Bullets["1"] = "decimal";
		$this->Bullets["d"] = "disc";
		$this->Bullets["D"] = "disc";
		$this->Bullets["c"] = "circle";
		$this->Bullets["C"] = "circle";
		$this->Bullets["s"] = "square";
		$this->Bullets["S"] = "square";
		$this->Bullets["a"] = "lower-alpha";
		$this->Bullets["A"] = "upper-alpha";
		$this->Bullets["i"] = "lower-roman";
		$this->Bullets["I"] = "upper-roman";
		$this->Valid = array_keys($this->Bullets);

		if ($Define !== "")
			$this->Define($Define);

		if ($this->DefineVerify($Default))
			$this->DefaultType = $Default;
		else
			$this->DefaultType = "d";
	}

	function DefineVerify($String)
	{
		if (in_array($String, $this->Valid))
			return(1);
		if (substr($String, 0, 4) == "url(")
			return(1);
		return(0);
	}

	function Define($Define)
	{
		if (is_array($Define))
		{
			$Valid = 0;
			foreach ($Define as $Item)
				if ($this->DefineVerify($Item))
					$Valid++;
			if (sizeof($Define) == $Valid)
				$this->Types = $Define;
		}
		else
			if ($this->DefineVerify($Define))
				$this->Types = $Define;
	}

	function AddData($Data, $Recursed = 0)
	{
		if (is_array($Data))
		{
			if ($Recursed)
			{
				$Types = $this->Types;
				if (is_array($Types))
				{
					array_shift($Types);
					if (sizeof($Types) == 0)
						$Types = $this->DefaultType;
				}
				$L = new Lists($Types, $this->DefaultType);
				$L->AddData($Data);
				$this->Data[] = $L;
			}
			else
			{
				foreach ($Data as $Item)
					$this->AddData($Item, 1);
			}
		}
		elseif (strtolower(get_class($Data)) == "lists")
		{
			$this->Data[] = $Data;
		}
		else
		{
			$this->Data[] = $Data;
		}
	}

	function Body()
	{
		$List = "";
		foreach ($this->Data as $Item)
			if (strtolower(get_class($Item)) == "lists")
				$List .= $Item->Get();
			else
				$List .= "<li>".$Item."</li>\n";
		return ($List);
	}

	function Get($Extra = "")
	{
		if (is_array($this->Types))
			$Type = $this->Types[array_shift(array_keys($this->Types))];
		else
			$Type = $this->Types;
		if ($Type == "")
			$Type = $this->DefaultType;

		if (in_array($Type, array("1", "a", "A", "i", "I")))
			$Tag = "ol";
		else
			$Tag = "ul";

		if (trim($Extra))
			$Tag .= " ".trim($Extra);

		if (substr($Type, 0, 4) == "url(")
			$Style = " style=\"list-style-image: ".$Type.";\"";
		else
			$Style = " style=\"list-style-type: ".$this->Bullets[$Type]."; list-style-image: none;\"";

		$List = "";
		$List .= "<".$Tag.$Style.">\n";
		$List .= $this->Body();
		$List .= "</".$Tag.">\n";
		return ($List);
	}

	function Show($Extra = "")
	{
		echo $this->Get($Extra);
	}

	function Debug()
	{
		echo "<h2>\$this->Types;</h2>\n";
		PrintArrayContents($this->Types);

		echo "<h2>\$this->Data;</h2>\n";
		PrintArrayContents($this->Data);

		echo "<h2>\$this->Bullets;</h2>\n";
		PrintArrayContents($this->Bullets);

		echo "<h2>\$this->Valid;</h2>\n";
		PrintArrayContents($this->Valid);
	}
}

?>
