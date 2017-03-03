<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

// callback.inc
// callback popup system
// written:	15.01.2004
// author:	Tim Cromwell

require_once(PHPLIB_DIR."browser.inc");

// begin class
class CallBack
{
	/**** PUBLIC FUNCTIONS ****/
	
	// CallBack - class constructor
	function CallBack()
	{
		// set passed param
		$Vars = func_get_args();

		if (sizeof($Vars) < 2)
			exit("Error - invalid number of arguments passed.");
		
		$this->Mode = $Vars[0];
		$this->FormName = $Vars[1];

		// depending on mode - write javascript
		switch ($this->Mode)
		{
			case "call": 
				// create javascript
				$this->CreatePopupJavascript();
				break;

			case "back":
				// create javascript
				$this->CreateCallBackJavascript();
				break;
		}
	}


	// Activate - function to process
	function Activate()
	{
		// set passed param
		$Vars = func_get_args();

		switch ($this->Mode)
		{
			case "call":
				if (sizeof($Vars) < 3)
					exit ("Inadequate number of parameters given for call link");
				
				// set vars

				// the URL to display in the popup window
				$URL = $Vars[0];

				// the name of the form field to be updated when the callback returns.
				$Field = $Vars[1];

				// the id of the <span> tag to by updated when the callback returns.
				$SpanID = $Vars[2];

				// the width of the popup window
				$Width = $Vars[3];

				// the height of the popup window
				$Height = $Vars[4];

				// the label displayed on the link to the popup window
				$Label = $Vars[5];
				return ($this->CreateInputFields($URL, $Field, $SpanID, $Width, $Height, $Label));
				/*
				$Field = $Vars[0];
				$SpanID = $Vars[1];
				$URL = $Vars[2];
				$WindowOptions = $Vars[3];
				$Launcher = $Vars[4];
				$CurrVars = $Vars[5];
				return ($this->CreateInputFields($Field, $SpanID, $URL, $WindowOptions, $Launcher, $CurrVars));
				*/
				
				break;

			case "back":
				// create callback link - check that adequate vars passed
				if (sizeof($Vars) < 5)
					exit ("Inadequate number of parameters given for callback link");

				// the name of the form field containing the value to update
				$Field = $Vars[0];

				// the id of the <span> tag containing the label
				$SpanID = $Vars[1];

				// the text displayed on the link to the callback popup
				$LinkText = $Vars[2];

				// the new value of the field identified by $Field
				$DataID = $Vars[3];

				// the new text displayed in the <span> identified by $SpanID
				$DataValue = $Vars[4];

				// ??? 
				$NoQuit = $Vars[5];
				
				// ???
				$FormSubmit = $Vars[6];

				// create link
				return ($this->CreateCallBackLink($Field, $SpanID, $DataID, $DataValue, $LinkText, $NoQuit, $FormSubmit));
				break;
		}
	}
	
	
	/**** PRIVATE FUNCTIONS ****/
	
	// CreatePopupJavascript - function to create the popup window javascript
	function CreatePopupJavascript()
	{
		echo "<script language=\"JavaScript1.2\">\n";
		echo "function PopupCallBack(url, windowname, options)\n";
		echo "{\n";
		echo "\twindow.open(url, windowname, options);\n";
		echo "}\n";
		echo "</script>\n";
	}

	// CreateCallBackJavascript - function to create the call back javascript
	function CreateCallBackJavascript()
	{
		echo "<script language=\"JavaScript1.2\">\n";
		echo "function writeContent(id,text)\n";
		echo "{\n";
		if (BrowserCheck("mozilla", 4, 4.9))
		{
			echo "\topener.document.layers[id].document.open();\n";
			echo "\topener.document.layers[id].document.write(text);\n";
			echo "\topener.document.layers[id].document.close();\n";
		}
		else
		{
			echo "\topener.document.getElementById(id).innerHTML = text;\n";
		}
		
		echo "}\n\n";

		echo "function writeFormContent(formname, fieldname, fieldvalue)\n";
		echo "{\n";
		echo "\topener.document.forms[formname].elements[fieldname].value = fieldvalue;\n";
		echo "}\n";
		echo "</script>\n";
	}

	// CreateInputFields - function to create the appropriate input fields
//	function CreateInputFields($Field, $SpanID, $URL, $WindowOptions = "", $Launch = "Lookup Item", $CurrVars = "")
	function CreateInputFields($URL, $Field, $SpanID, $Width=400, $Height=300, $Label="Find")
	{
		// determine name and/or value of field
		if (is_array($Field))
		{
			$FieldKey = array_keys($Field);
			$FieldName = $FieldKey[0];
			$FieldValue = $Field[$FieldName];
		}
		else
			$FieldName = $Field;

		// determine name and/or value of span
		if (is_array($SpanID))
		{
			$SpanKey = array_keys($SpanID);
			$SpanName = $SpanKey[0];
			$SpanValue = $SpanID[$SpanName];
		}
		else
			$SpanName = $SpanID;
		
		// set window options)
		$WindowOptions = "width=$Width, height=$Height, location=no, menubar=no, toolbar=no, scrollbars=yes, resizable=yes";
		
		// create popup link
		if (strstr($URL, "?"))
			$FirstJoin = "&";
		else
			$FirstJoin = "?";
			
		$URL .= $FirstJoin."CallBackData[Form]=".$this->FormName;
		$URL .= "&CallBackData[Field]=".$FieldName;
		$URL .= "&CallBackData[SpanID]=".$SpanName;

		//$IFields = "<input type=\"hidden\" name=\"".$FieldName."\" value=\"".$FieldValue."\">\n";
		$IFields = Field::GetHidden($FieldName, $FieldValue, "id=\"call_back_".$SpanName."\"");
		$IFields .= "<span id=\"".$SpanName."\">".$SpanValue."</span>&nbsp;&nbsp;";

		// FIX: strip out odd characters: fixes a bug in IE, which doesn't like square brackets
		// in window names (which is annoying when you're passing something like BSB[ID] to this
		// function)
		$WindowName = str_replace(array("[", "]", "(", ")", "{", "}"), "", $FieldName);
		$this->CallJavaScript = "PopupCallBack('".$URL."', '".$WindowName."', '".$WindowOptions."');";
		$IFields .= "[<a href=\"javascript: ".$this->CallJavaScript."\">".$Label."</a>]\n";
		return ($IFields);
	}

	// CreateCallBackLink - function to make the call back link required to exit the window and return values
	function CreateCallBackLink($Field, $SpanID, $DataID, $DataValue, $LinkText, $NoQuit="", $FormSubmit="")
	{
		if ($NoQuit)
			$Link = "<a href=\"#\"";
		else
			$Link = "<a href=\"javascript: self.close();\"";
			
		$Link .= " onclick=\"writeContent('".$SpanID."', '".$DataValue."');";
		$Link .= " writeFormContent('".$this->FormName."', '".$Field."', '".$DataID."');";
		
		if ($FormSubmit)
			$Link .= " opener.document['".$this->FormName."'].submit();";
		
		$Link .= "\">".$LinkText."</a>";
		return ($Link);
	}
}
?>
