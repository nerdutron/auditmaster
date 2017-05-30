<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

class variable
{
	function __construct($Mode = "")
	{
		$this->SetVariableDecodeMode($Mode);
		if (!in_array($Mode, array("PLAIN", "SECURE")))
			if (defined("VARIABLE_DECODE_DEFAULT_MODE"))
				if(in_array(VARIABLE_DECODE_DEFAULT_MODE, array("PLAIN", "SECURE")))
					$this->SetVariableDecodeMode(VARIABLE_DECODE_DEFAULT_MODE);
		$this->VAR_DO = "";
		$this->VAR_CACHE = array_merge($_REQUEST, $_FILES, $_SERVER);
	}

	function SetVariableDecodeMode($Mode)
	{
		if (strtoupper($Mode) == "SECURE")
			$this->VariableDecodeMode = "SECURE";
		else
			$this->VariableDecodeMode = "PLAIN";
	}

	function GetItem($Name)
	{
		return($_REQUEST[$Name]);
	}

	function GetInput($ARGS)
	{
		if (is_array($this->VAR_DO))
			$Data = $this->VAR_DO;
		else
			$Data = $ARGS;
		$this->VAR_DO = "";
		return($Data);
	}

	// COOKIE functions

	function GetCookie($Name)
	{
		return($_COOKIE[$Name]);
	}

	// PUT values

	function DoPutV($Keys, $Val)
	{
		$Target = &$this->VAR_CACHE;
		foreach ($Keys as $Key)
			if ($Key == "[]")
				$Target = &$Target[];
			else
				$Target = &$Target[$Key];
		if (sizeof($Keys))
			$Target = $Val;
	}

	function PutV()
	{
		$Keys = func_get_args();
		$Val = array_pop($Keys);
		$this->DoPutV($Keys, $Val);
	}

	// PLAIN versions - no interference

	function PlainGetV()
	{
		$Val = $this->VAR_CACHE;
		foreach ($this->GetInput(func_get_args()) as $Key)
			$Val = $Val[$Key];
		return ($Val);
	}

	function PlainMakeV()
	{
		$Keys = $this->GetInput(func_get_args());
		$Var = array_pop($Keys);
		if (sizeof($Keys))
		{
			$Val = &$this->VAR_CACHE;
			foreach ($Keys as $Key)
				if ($Key == "[]")
					$Val = &$Val[];
				else
					$Val = &$Val[$Key];
			$this->$Var = &$Val;
		}
		else
			$this->$Var = &$this->VAR_CACHE[$Var];
	}

	function PlainMakeL()
	{
		$Data = $this->GetInput(func_get_args());
		foreach($Data as $Var)
			$this->PlainMakeV($Var);
	}

	function PlainCopyV()
	{
		$Keys = $this->GetInput(func_get_args());
		$Var = array_pop($Keys);
		if (sizeof($Keys))
		{
			$Val = &$this->VAR_CACHE;
			foreach ($Keys as $Key)
				if ($Key == "[]")
					$Val = &$Val[];
				else
					$Val = &$Val[$Key];
			$this->$Var = $Val;
		}
		else
			$this->$Var = $this->VAR_CACHE[$Var];
	}

	function PlainCopyL()
	{
		foreach($this->GetInput(func_get_args()) as $Var)
			$this->PlainCopyV($Var);
	}

	// HTML STRIP versions

	function DoStrip($Data)
	{
		if(is_array($Data))
		{
			foreach ($Data as $Key => $Val)
				$Data[$Key] = $this->DoStrip($Val);
		}
		else
			$Data = strip_tags($Data);
		return($Data);
	}

	function StripGetV()
	{
		$Val = $this->VAR_CACHE;
		foreach ($this->GetInput(func_get_args()) as $Key)
			$Val = $Val[$Key];
		return ($this->DoStrip($Val));
	}

	function StripMakeV()
	{
		$Keys = $this->GetInput(func_get_args());
		$Var = array_pop($Keys);
		if (sizeof($Keys))
		{
			$Val = &$this->VAR_CACHE;
			foreach ($Keys as $Key)
				if ($Key == "[]")
					$Val = &$Val[];
				else
					$Val = &$Val[$Key];
			$this->$Var = &$Val;
		}
		else
			$this->$Var = &$this->VAR_CACHE[$Var];
		$this->$Var = $this->DoStrip($this->$Var);
	}

	function StripMakeL()
	{
		foreach($this->GetInput(func_get_args()) as $Var)
			$this->StripMakeV($Var);
	}

	function StripCopyV()
	{
		$Keys = $this->GetInput(func_get_args());
		$Var = array_pop($Keys);
		if (sizeof($Keys))
		{
			$Val = &$this->VAR_CACHE;
			foreach ($Keys as $Key)
				if ($Key == "[]")
					$Val = &$Val[];
				else
					$Val = &$Val[$Key];
			$this->$Var = $Val;
		}
		else
			$this->$Var = $this->VAR_CACHE[$Var];
		$this->$Var = $this->DoStrip($this->$Var);
	}

	function StripCopyL()
	{
		foreach($this->GetInput(func_get_args()) as $Var)
			$this->CopyV($Var);
	}

	// these are the original and switching functions 

	function GetV()
	{
		$this->VAR_DO = func_get_args();
		if ($this->VariableDecodeMode == "SECURE")
			return($this->StripGetV());
		else
			return($this->PlainGetV());
	}

	function MakeV()
	{
		$this->VAR_DO = func_get_args();
		if ($this->VariableDecodeMode == "SECURE")
			$this->StripMakeV();
		else
			$this->PlainMakeV();
	}

	function MakeL()
	{
		$this->VAR_DO = func_get_args();
		if ($this->VariableDecodeMode == "SECURE")
			$this->StripMakeL();
		else
			$this->PlainMakeL();
	}

	function CopyV()
	{
		$this->VAR_DO = func_get_args();
		if ($this->VariableDecodeMode == "SECURE")
			$this->StripCopyV();
		else
			$this->PlainCopyV();
	}

	function CopyL()
	{
		$this->VAR_DO = func_get_args();
		if ($this->VariableDecodeMode == "SECURE")
			$this->StripCopyL();
		else
			$this->PlainCopyL();
	}
}

?>
