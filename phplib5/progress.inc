<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

class progress
{
	function __construct($Values, $Estimate = 0)
	{
		$this->Values = $Values;
		$this->Unit = $this->Values / 100;
		$this->Estimate = $Estimate * 1;
		$this->i = 0;
		$this->c = 0;
		$this->bar = "";
		if ($this->Unit >= 1)
		{
			echo "\n";
			echo "\n                                                                                                   1";
			echo "\n         1         2         3         4         5         6         7         8         9         0";
			echo "\n1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			echo "\n";
		}
		$this->Start = mktime();
	}

	function increment()
	{
		if ($this->Unit >= 1)
		{
			$this->i++;
			$this->c++;
			if ($this->i >= $this->Unit)
			{
				$this->bar .= "#";
				$this->i = 0;
				echo "\r";
				echo str_pad($this->bar, 100, "_");
				if ($this->Estimate)
				{
					echo " ";
					$Spent = mktime() - $this->Start;
					echo $this->time($Spent);
					if ($Spent)
					{
						echo " / ";
						echo $this->time($Spent / $this->c * $this->Values);
					}
				}
			}
		}
	}

	function time($Seconds)
	{
		$Days = floor(round($Seconds) / 86400);
		$Seconds = round($Seconds) - ($Days * 86400); 
		if ($Days)
			$TimeString .= $Days." days ";
		$TimeString .= date ("H:i:s", mktime (0,0,$Seconds,1,1,2000));
		return($TimeString);
	}
}

?>
