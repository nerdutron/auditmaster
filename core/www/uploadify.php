<?php
/*
Uploadify v2.1.0
Release Date: August 24, 2009

Copyright (c) 2009 Ronnie Garcia, Travis Nickels

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

require_once("../local.inc");
require_once(ToIncludes("audit")."file.inc");

$Debug = false;

if (!empty($_FILES)) {
	$tempFile = $_FILES['Filedata']['tmp_name'];
	
	$AuditID = $_REQUEST["AuditID"];
	$OperatorID = $_REQUEST["OperatorID"];

	if ($Debug)
	{
		$FP = fopen("output.log", "w");
		fwrite($FP, "Audit $AuditID - Op $OperatorID - File - $tempFile\n");
	}

	$File = new file($OperatorID);

	// Set params
	$Data = array();
	$Data["file_data"] = file_get_contents($tempFile);
	$Data["file_name"] = $_FILES["Filedata"]["name"];
	$Data["file_size"] = filesize($tempFile);

	if ($Debug)
		fwrite($FP, print_r($Data,true));

	// Save
	$File->SaveFile($Data);

	$File->SaveLink($AuditID);

	if ($Debug)
	{
		fwrite($FP, "Done\n\n");
		fclose($FP);
	}

	// Be polite and clean up the temp file
	unlink($tempFile);

	echo "1";
}

?>
