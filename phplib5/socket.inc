<?php
//
// socket.inc - Various socket handling functions
//
// Copyright 1999 MU Systems Pty Ltd
//
//  Author: Scott McClintock
//  Date:   30/10/1999
//
// Change Log:
// --------------
// Changed PostPage text sent to work - Craig Sargent   30/5/2001

//
// Post a string as a page to a web browser
//

function PostPage($WebAddress, $URL, $Data)
{
	$Sock = fsockopen($WebAddress, 80, $ErrorNr, $ErrorString);

	if(!$Sock)  // There has been a problem
	{
		echo "<BR>".$ErrorString." (".$ErrorNr.")<BR>\n";
		echo "Error Opening Socket to $WebAddress<BR>\n";
		return;
	}

	// Changed by craig as this line didn't work.
	// GDTAG - investigate why this works sometimes, and line below works
	// on other times....
	$POSTING  = "POST http://".$WebAddress.$URL." HTTP/1.0\n";
	//  $POSTING  = "POST ".$URL." HTTP/1.0\n";
	$POSTING .= "content-type: application/x-www-form-urlencoded\n";
	$POSTING .= "content-length: ".strlen($Data)."\n";
	$POSTING .= "Referer: www.abr.com.au\n";
	$POSTING .= "\n";
	$POSTING .= $Data;

	fputs($Sock, $POSTING);

	while(!feof($Sock))
	{
		$Reply[] = fgets($Sock, 128);
	}
	fclose($Sock);
	//reset($Reply);
	//while(list($index, $Line) = each($Reply)) echo $Line;

	return $Reply;
}

function GetPage($WebAddress, $URL)
{
	$Sock = fsockopen($WebAddress, 80, $ErrorNr, $ErrorString);

	if(!$Sock)  // There has been a problem
	{
		echo "<BR>".$ErrorString." (".$ErrorNr.")<BR>\n";
		return;
	}

	$POSTING  = "GET http://".$WebAddress.$URL." HTTP/1.0\n";
	$POSTING .= "\n";
	fputs($Sock, $POSTING);

	while(!feof($Sock))
	{
		$Reply[] = fgets($Sock, 128);
	}
	fclose($Sock);
	//reset($Reply);
	//while(list($index, $Line) = each($Reply)) echo $Line;

	return $Reply;
}
?>
