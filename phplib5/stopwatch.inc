<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

class StopWatch
{
	function __construct()
	{
		$this->Start = $this->GetMicroSeconds();
		$this->Split = $this->Start;
	}

	function GetMicroSeconds()
	{
		$Parts = explode(" ", microtime());
		$MT = $Parts[0] + $Parts[1];
		return ($MT);
	}

	function GetTime($Duration)
	{
		$Duration = round($Duration * 1000) / 1000;
		$TimeString = "";
		$Parts = explode(".", $Duration);
		$Seconds = $Parts[0];
		$MicroSeconds = $Parts[1];

		$MicroSeconds = $MicroSeconds."000";
		$Tenths = substr($MicroSeconds,0,1);
		$Hundredths = substr($MicroSeconds,1,1);
		$Thousandths = substr($MicroSeconds,2,1);

		if ($Thousandths > 4)
			$Hundredths++;
		if ($Hundredths > 9)
		{
			$Tenths++;
			$Hundredths = 0;
		}
		if ($Tenths > 9)
		{
			$Seconds++;
			$Tenths = 0;
		}

		$Days = floor($Seconds / 86400);
		$Seconds = $Seconds - ($Days * 86400); 

		if ($Days)
			$TimeString .= $Days." days ";
		$TimeString .= date ("H:i:s", mktime (0,0,$Seconds,1,1,2000));
		$TimeString .= ".".$Tenths.$Hundredths;

		return($TimeString);
	}

	function GetSplitDuration()
	{
		$this->Old = $this->Split;
		$this->Split = $this->GetMicroSeconds();
		return ($this->Split - $this->Old);
	}

	function GetSplit()
	{
		return ($this->GetTime($this->GetSplitDuration()));
	}

	function Split($Message = "")
	{
		if ($Message)
		{
			echo "<div style=\"border: 1px solid #000000; padding: 10px; font-size: 0.8em;\">";
			echo "<b>";
			echo $this->GetSplit();
			echo ": </b>";
			echo $Message;
			echo "</div>";
		}
		else
		{
			echo "\n\n";
			echo "Split Time: ".$this->GetSplit();
			echo "\n\n";
		}
	}

	function GetStopDuration()
	{
		return ($this->GetMicroSeconds() - $this->Start);
	}

	function GetStop()
	{
		return ($this->GetTime($this->GetStopDuration()));
	}

	function Stop($Message = "")
	{
		if ($Message)
		{
			echo "<div style=\"border: 1px solid #000000; padding: 10px; font-size: 0.8em;\">";
			echo "<b>";
			echo $this->GetStop();
			echo ": </b>";
			echo $Message;
			echo "</div>";
		}
		else
		{
			echo "\n\n";
			echo "Total Time: ".$this->GetStop();
			echo "\n\n";
		}
	}
}

?>
