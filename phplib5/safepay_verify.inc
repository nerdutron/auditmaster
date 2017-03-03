<?php

class safepay_verify
{
	function CheckDigits($CardNr)
	{
		$CardNr = "$CardNr";

		$Len = strlen($CardNr);

		if($Len < 14)
			return 0;

		$Card = strrev($CardNr);

		$FF = 1;
		for($i=0;$i<$Len;$i++)
		{
			$Val = $Card[$i] * $FF;
			if($Val > 9)
			{
				$ValEnd = 0;
				$ValStr = "$Val";
				$ValLen = strlen($ValStr);
				for($j=0;$j<$ValLen;$j++)
				{
					$ValEnd += $ValStr[$j];
				}
				$Val = $ValEnd;
			}

			$Total += $Val;

			$FF++;
			if($FF > 2)
				$FF = 1;

		}

		while($Total >= 10)
			$Total -= 10;
		if($Total == 0)
			return(array());
		return(array("ERROR - Please provide a valid Credit Card Number."));
	}

	function CheckExpiry($Expiry)
	{
		$Errors = array();

		if (strlen($Expiry) == 5)
		{
			$CardM = substr($Expiry, 0, 2) * 1;
			$CardY = substr($Expiry, 3, 2) * 1;
		}
		elseif (strlen($Expiry) == 4)
		{
			$CardM = substr($Expiry, 0, 2) * 1;
			$CardY = substr($Expiry, 2, 2) * 1;
		}
		else
		{
			$Errors[] = "ERROR - Expiry Date not supplied.";
			return($Errors);
		}

		if(!is_numeric($CardY) || !is_numeric($CardM))
			$Errors[] = "ERROR - Please provide a valid Expiry Date.";
		elseif($CardM < 1 || $CardM > 12)
			$Errors[] = "ERROR - Please provide a valid Expiry Date.";
		elseif($CardY == date("y") && $CardM < date("m"))
			$Errors[] = "ERROR - Credit Card has Expired.";
		elseif($CardY < date("y"))
			$Errors[] = "ERROR - Credit Card has Expired.";
		elseif($CardY > (date("y") + 10))
			$Errors[] = "ERROR - Please provide a valid Expiry Date.";

		return($Errors);
	}

	function CheckType($Type, $Number)
	{
		$Errors = array();

		$Cards = array();
		switch ($Type)
		{
			case "AE":
				$Cards[34]  = 15; // American Express
				$Cards[37]  = 15; // American Express
				break;

			case "DC":
				$Cards[300] = 14; // Diners Club Carte Blanche
				$Cards[301] = 14; // Diners Club Carte Blanche
				$Cards[302] = 14; // Diners Club Carte Blanche
				$Cards[303] = 14; // Diners Club Carte Blanche
				$Cards[304] = 14; // Diners Club Carte Blanche
				$Cards[305] = 14; // Diners Club Carte Blanche
				$Cards[36]  = 14; // Diners Club International
				$Cards[55]  = 16; // Diners Club US & Canada
				break;

			case "MC":
				$Cards[51]  = 16; // MasterCard
				$Cards[52]  = 16; // MasterCard
				$Cards[53]  = 16; // MasterCard
				$Cards[54]  = 16; // MasterCard
				$Cards[55]  = 16; // MasterCard
				break;

			case "VC":
				$Cards[4]   = 16; // Visa
				break;

			default:
				return(array("ERROR - Invalid Credit Card Type."));
				break;
		}

		// Credit Card Length Validation
		if (!is_numeric($Number))
			$Errors[] = "ERROR - Please provide a numeric Credit Card Number.";
		else
		{
			$Matched = 0;
			foreach ($Cards as $Prefix => $Length)
			{
				$CardPrefix = substr($Number, 0, strlen($Prefix));
				if ($CardPrefix == $Prefix)
				{
					$Matched = 1;
					if (strlen($Number) <> $Length)
						$Errors[] = "ERROR - Please provide a correct ".$Length." digit Card Number.";
				}
			}
			if (!$Matched)
				$Errors[] = "ERROR - Credit Card prefix does not match the Credit Card type.";
		}

		return($Errors);
	}

	function CheckName($Name)
	{
		if (!$Name)
			return(array("ERROR - Please provide a valid Name for the Credit Card."));
		else
			return(array());
	}

	function CompleteCheck($CardName, $CardNumber, $CardType, $CardExpiry)
	{
		$Errors = array();

		// Credit Card Type Validation
		$Errors = array_merge($Errors, safepay_verify::CheckType($CardType, $CardNumber));

		// Use Check Digits on the Card Number
		if (!sizeof($Errors))
			$Errors = array_merge($Errors, safepay_verify::CheckDigits($CardNumber));

		// Credit Card Name validation
		$Errors = array_merge($Errors, safepay_verify::CheckName($CardName));

		// Credit Card Expiry Validation
		$Errors = array_merge($Errors, safepay_verify::CheckExpiry($CardExpiry));

		return ($Errors);
	}
}

?>
